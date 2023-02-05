#include <vector>

#include "entity.h"
#include "log.h"
#include "rpc_manager.h"
#include "entity_component_manager.h"
#include "entity_property_manager.h"


void Entity::tick() {

    if (is_ready) {
        propertys_sync2client();
    }

    timer_tick();  // 处理当前帧创建的立即触发的timer
}

void Entity::on_create(const GDict& create_data) {
    ready_check_timerid = REGIST_TIMER_INNER(10, 0, false, "ready_check_timer", &Entity::ready_check_timer);
}

void Entity::ready_check_timer() {
    // 10s内没有ready就会触发自毁流程
    // base/cell自毁时会通知对方一起自毁
    if (!is_ready) {
        destroy_self();
    }
}

void Entity::destroy_self() {
    need_destroy = true;
}

void Entity::release_component() {
    for (auto iter = components.begin(); iter != components.end(); ++iter) {
        delete iter->second;
    }
    components.clear();
}

void Entity::release_property() {
    for (auto iter = propertys.begin(); iter != propertys.end(); ++iter) {
        delete iter->second;
    }
    propertys.clear();
}

void Entity::release_timer() {
    vector<TimerID> _timer_ids;
    for (auto iter = timer_ids.begin(); iter != timer_ids.end(); ++iter) {
        if (iter->second->m_cb_name == "migrate_check_timer") {
            continue;
        }
        _timer_ids.push_back(iter->first);
    }

    for (auto iter = _timer_ids.begin(); iter != _timer_ids.end(); ++iter) {
        CANCEL_TIMER(*iter);
    }
}

void Entity::propertys_sync2client(bool force_all) {
    // do nothing
}

double Entity::get_db_save_interval() {
    return 10.0;
}

void Entity::serialize_all(Encoder& encoder) {
    for (auto iter = propertys.begin(); iter != propertys.end(); ++iter) {
        encoder.write_uint16(prop_str2int(iter->first));
        iter->second->serialize_all(encoder);
    }
}

void Entity::serialize_db(Encoder& encoder) {
    for (auto iter = propertys.begin(); iter != propertys.end(); ++iter) {

        if (!iter->second->need_sync2db()) {
            continue;
        }

        encoder.write_uint16(prop_str2int(iter->first));
        iter->second->serialize_all(encoder);
    }
}

void Entity::serialize_client(Encoder& encoder, bool force_all) {

    if (force_all) {
        for (auto iter = propertys.begin(); iter != propertys.end(); ++iter) {

            if (!iter->second->need_sync2client()) {
                continue;
            }

            encoder.write_uint16(prop_str2int(iter->first));
            iter->second->serialize_all(encoder);
        }
    }
    else {
        for (auto iter = dirty_propertys.begin(); iter != dirty_propertys.end(); ++iter) {

            if (!iter->second->need_sync2client()) {
                continue;
            }

            if (!iter->second->is_dirty() && !iter->second->is_all_dirty()) {
                continue;
            }

            encoder.write_uint16(prop_str2int(iter->first));
            iter->second->serialize(encoder);
        }
    }

    dirty_propertys.clear();
}

void Entity::give_propertys(unordered_map<GString, EntityPropertyBase*>& other_propertys) {
    propertys.clear();
    for (auto iter = other_propertys.begin(); iter != other_propertys.end(); ++iter) {
        auto prop = iter->second->create_self();
        prop->set_prop_type(iter->second->get_prop_type());
        prop->set_first_level();
        prop->set_parent((EntityPropertyBase*)this);
        propertys[iter->first] = prop;
        propertys_turn[prop] = iter->first;
    }
}

void Entity::ready() {
    if (is_ready) {
        return;
    }

    is_ready = true;
    propertys_sync2client(true);

    if (need_create_save_timer()) {
        create_dbsave_timer();
    }

    if (ready_check_timerid != 0) {
        CANCEL_TIMER(ready_check_timerid);
        ready_check_timerid = 0;
    }

    on_ready();

    for (auto iter = components.begin(); iter != components.end(); ++iter) {
        iter->second->on_ready();
    }
}

void Entity::propertys_unserialize(Decoder& decoder) {
    while (!decoder.is_finish()) {
        auto idx = decoder.read_int16();
        const GString& prop_name = prop_int2str(idx);
        propertys.at(prop_name)->unserialize(decoder);
    }
}

bool Entity::need_create_save_timer() {
    if (get_entity_type() == EntityType::EntityType_BaseWithCellAndClient) {
        return true;
    }
    return false;
}

void Entity::timer_tick() {

    auto now = nowms_timestamp();

    vector<TimerBase*> timer_fired;
    for (auto iter = timers.begin(); iter != timers.end(); ++iter) {
        const auto& timer = *iter;
        if (timer->m_expiration <= now) {
            ++timer->m_fire_num;
            timer->callback();
            timer_fired.push_back(timer);
        }
        else {
            break;
        }
    }

    for (auto iter = timer_fired.begin(); iter != timer_fired.end(); ++iter) {
        const auto& timer = *iter;

        remove_timer(timer);

        if (timer->m_repeat) {
            timer->m_expiration = timer->m_start_time + int64_t(timer->m_fire_num * timer->m_interval * 1000);
            add_timer(timer);
        }
    }
}

void Entity::cancel_timer(TimerID timer_id) {
    auto iter = timer_ids.find(timer_id);
    if (iter == timer_ids.end())
        return;

    TimerBase* timer = iter->second;
    remove_timer(timer);
    delete timer;
}

void Entity::add_timer(TimerBase* timer) {
    timers.insert(timer);
    timer_ids.insert(make_pair(timer->m_id, timer));
}

void Entity::remove_timer(TimerBase* timer) {
    timers.erase(timer);
    timer_ids.erase(timer->m_id);
}

void BaseEntity::on_create(const GDict& create_data) {
    Entity::on_create(create_data);

    ready();
}

void BaseEntityWithCell::on_create(const GDict& create_data) {
    Entity::on_create(create_data);

    create_cell(create_data);
}

void BaseEntityWithCell::create_cell(const GDict& create_data) {
    
    // game -> gate -> game
    auto const& session = g_session_mgr.get_rand_session();
    REMOTE_RPC_CALL(session, "create_cell_entity", class_name, 
        /*entity uuid*/ uuid,
        /*base addr*/ session->get_local_addr(),
        /*gate addr*/ create_data.at("gate_addr").as_string(),
        /*client addr*/ create_data.at("client_addr").as_string(),
        /*cell bin*/ create_data.at("cell_bin").as_bin());
}

void BaseEntityWithCell::on_cell_create(const GString& cell_addr) {
    cell.set_entity_and_addr(uuid, cell_addr);
    ready();
}

void BaseEntityWithCell::migrate_req_from_cell() {
    cell.call("migrate_reqack_from_base", true);
    cell.start_cache_rpc();
}

void BaseEntityWithCell::new_cell_migrate_in(const GString& new_cell_addr) {
    cell.set_entity_and_addr(cell.get_entity_uuid(), new_cell_addr);
    cell.stop_cache_rpc();
}

void BaseEntityWithCellAndClient::on_create(const GDict& create_data) {
    Entity::on_create(create_data);

    client.set_entity_and_addr(uuid, create_data.at("client_addr").as_string());
    client.set_gate_addr(create_data.at("gate_addr").as_string());
    create_cell(create_data);
}

void BaseEntityWithCellAndClient::ready_check_timer() {
    if (!is_ready) {
        destroy_self();
        cell.call("destroy_self");
        client.call("destroy_self");
    }
}

void BaseEntityWithCellAndClient::on_cell_create(const GString& cell_addr) {
    cell.set_entity_and_addr(uuid, cell_addr);
    create_client();
}

void BaseEntityWithCellAndClient::create_client() {
    // game -> gate -> client
    auto gate = g_session_mgr.get_gate(client.get_gate_addr());
    REMOTE_RPC_CALL(gate, "create_client_entity", client.get_addr(), class_name,
        /*entity uuid*/ uuid,
        /*base addr*/ gate->get_local_addr(),
        /*cell addr*/ cell.get_addr()
    );
}

void BaseEntityWithCellAndClient::ready() {
    // ready通知意味base/cell/client entity都创建好了
    if (is_ready) {
        return;
    }

    Entity::ready();
    cell.call("ready");
}

void BaseEntityWithCellAndClient::destroy_self() {
    Entity::destroy_self();
}

void BaseEntityWithCellAndClient::on_reconnect_fromclient(const GString& client_addr, const GString& gate_addr) {
    INFO_LOG("[base] entity.%s reconnect from client\n", client.get_entity_uuid().c_str());

    // kick old client
    kick_client();

    // recover client mailbox
    client.set_entity_and_addr(client.get_entity_uuid(), client_addr);
    client.set_gate_addr(gate_addr);
    cell.call("on_reconnect_fromclient", client_addr, gate_addr);
}

void BaseEntityWithCellAndClient::on_client_reconnect_success() {
    // 对于服务端，登陆会收到ready通知，断线重连会收到on_client_reconnect_success通知
    // 对于客户端，无论是登陆还是断线重连，只有ready通知，也就是说客户端对断线重连是无感的
    if (!is_ready) {
        // 如果是登陆流程中途客户端断线，重连上来后重新走ready通知
        ready();
    }
    else {
        propertys_sync2client(true);
        cell.call("on_client_reconnect_success");
    }
}

void BaseEntityWithCellAndClient::propertys_sync2client(bool force_all) {

    Encoder encoder;
    serialize_client(encoder, force_all);
    encoder.write_end();

    if (encoder.anything()) {
        //byte_print(encoder.get_buf(), encoder.get_offset()); // TODO delete
        client.call("prop_sync_from_base", GBin(encoder.get_buf(), encoder.get_offset()));
    }
}

void BaseEntityWithCellAndClient::kick_client() {
    client.call("on_kick");
}

void BaseEntityWithCellAndClient::real_time_to_save() {
    // rpc to cell
    Encoder base_db;
    serialize_db(base_db);
    base_db.write_end();
    cell.call("cell_real_time_to_save", uuid, GBin(base_db.get_buf(), base_db.get_offset()));

    time_to_disaster_backup();
}

void BaseEntityWithCellAndClient::new_cell_migrate_in(const GString& new_cell_addr) {
    BaseEntityWithCell::new_cell_migrate_in(new_cell_addr);
}

void BaseEntityWithCellAndClient::time_to_disaster_backup() {

    Encoder base_all_db;
    serialize_all(base_all_db);
    base_all_db.write_end();

    GDict base_migrate_data;
    packet_migrate_data(base_migrate_data);
    cell.call("cell_disaster_backup", GBin(base_all_db.get_buf(), base_all_db.get_offset()), base_migrate_data);
}

void BaseEntityWithCellAndClient::packet_migrate_data(GDict& migrate_data) {
    migrate_data.clear();

    migrate_timers.clear();
    for (auto iter = timers.begin(); iter != timers.end(); ++iter) {
        TimerBase* timer = *iter;
        Encoder encoder;
        timer->serialize(encoder);
        encoder.write_end();
        migrate_timers.insert(make_pair(timer->m_cb_name, GBin(encoder.get_buf(), encoder.get_offset())));
    }
    migrate_data.insert(make_pair("timers", migrate_timers));
    migrate_data.insert(make_pair("next_timer_id", next_timer_id));
}

void BaseEntityWithCellAndClient::unpacket_migrate_data(const GDict& migrate_data) {
    auto _timers = migrate_data.at("timers").as_dict();
    for (auto iter = _timers.begin(); iter != _timers.end(); ++iter) {
        RESTORE_TIMER(iter->first, iter->second.as_bin());
    }
    next_timer_id = migrate_data.at("next_timer_id").as_int32();
}

void BaseEntityWithCellAndClient::base_disaster_backup(const GBin& cell_all_db, const GDict& cell_migrate_data) {
    Encoder self_all_db;
    serialize_all(self_all_db);
    self_all_db.write_end();
    disaster_backup_of_self = std::move(GBin(self_all_db.get_buf(), self_all_db.get_offset()));
    packet_migrate_data(disaster_backup_of_self_migrate_data);

    disaster_backup_of_cell = std::move(cell_all_db);
    disaster_backup_of_cell_migrate_data = cell_migrate_data;
}

void BaseEntityWithCellAndClient::on_game_disappear(const GString& game_addr) {
    if (cell.get_addr() != game_addr) {
        return;
    }

    // cell recover by disaster backup
    auto session = g_session_mgr.get_rand_session();
    REMOTE_RPC_CALL(session, "cell_recover_by_disaster_backup",
        /*base addr*/get_listen_addr(),
        /*client addr*/client.get_addr(),
        /*client gate addr*/client.get_gate_addr(),
        /*entity class name*/class_name,
        /*uuid*/uuid,
        /*disaster backup of cell*/disaster_backup_of_cell,
        /*disaster backup of cell migrate data*/disaster_backup_of_cell_migrate_data);
}

void BaseEntityWithCellAndClient::recover_by_disaster_backup(const GString& cell_addr, const GString& client_addr, const GString& client_gate_addr) {
    cell.set_entity_and_addr(uuid, cell_addr);
    client.set_entity_and_addr(uuid, client_addr);
    client.set_gate_addr(client_gate_addr);

    cell.call("base_recover_by_disaster_backup_success", get_listen_addr());
    client.call("base_recover_by_disaster_backup_success", get_listen_addr());

    is_ready = true;
    propertys_sync2client(true);

    if (need_create_save_timer()) {
        create_dbsave_timer();
    }
}

void BaseEntityWithCellAndClient::cell_recover_by_disaster_backup_success(const GString& cell_addr) {
    cell.set_entity_and_addr(uuid, cell_addr);
    cell.clear_cache_rpc();

    // sync recover self
    Decoder decoder(disaster_backup_of_self.buf, disaster_backup_of_self.size);
    decoder.skip_head_len();
    propertys_unserialize(decoder);
    propertys_sync2client(true);

    time_to_disaster_backup();
}

void CellEntity::begin_migrate(const GString& new_addr) {
    //if (new_addr == get_listen_addr()) {
    //    // new cell addr == old cell addr
    //    WARN_LOG("ignore migrate to local addr\n");
    //    return;
    //}

    //if (new_addr == base.get_addr()) {
    //    // new cell addr == base addr
    //    WARN_LOG("ignore migrate to base addr\n");
    //    return;
    //}

    //if (!is_ready) {
    //    return;
    //}

    //if (migrate_state > MigrateState::Migrate_None) {
    //    return;
    //}

    //migrate_state = MigrateState::Migrate_Prepare;

    //new_cell_addr = new_addr;
    //base.call("migrate_req_from_cell"); 
}

void CellEntity::migrate_reqack_from_base(bool is_ok) { 
    is_reqack_from_base = true; 
    real_begin_migrate(); 
}

void CellEntity::real_begin_migrate() { 
    if (!is_reqack_from_base || migrate_state != MigrateState::Migrate_Prepare) {
        return;
    }

    migrate_state = MigrateState::Migrate_RealMigrate;

    migrate_entity();
    is_reqack_from_base = false;
}

void CellEntity::migrate_entity() {
    Encoder encoder;
    serialize_all(encoder);
    encoder.write_end();

    GDict migrate_data;
    on_migrate_out(migrate_data);

    auto session = g_session_mgr.get_rand_session();
    REMOTE_RPC_CALL(session, "entity_property_migrate_from_oldcell",
        /*new cell addr*/new_cell_addr,
        /*old cell addr*/get_listen_addr(),
        /*entity class name*/class_name,
        /*create data*/migrate_data,
        /*entity property*/GBin(encoder.get_buf(), encoder.get_offset()));
}

void CellEntity::on_new_cell_migrate_finish() {
    destroy_self();
}

void CellEntityWithClient::on_create(const GDict& create_data) {
    Entity::on_create(create_data);

    base.set_entity_and_addr(uuid, create_data.at("base_addr").as_string());
    client.set_entity_and_addr(uuid, create_data.at("client_addr").as_string());
    client.set_gate_addr(create_data.at("gate_addr").as_string());

    base.call("on_cell_create", server->get_listen_addr());
}

void CellEntityWithClient::ready_check_timer() {
    if (!is_ready) {
        destroy_self();
        base.call("destroy_self");
        client.call("destroy_self");
    }
}

void CellEntityWithClient::ready() {
    Entity::ready();
    client.call("ready");
}

void CellEntityWithClient::on_reconnect_fromclient(const GString& client_addr, const GString& gate_addr) {
    // recover client mailbox
    client.set_entity_and_addr(uuid, client_addr);
    client.set_gate_addr(gate_addr);

    // create new client
    auto gate = g_session_mgr.get_gate(gate_addr);
    REMOTE_RPC_CALL(gate, "create_client_entity_onreconnect", client.get_addr(), class_name,
        /*uuid*/ uuid,
        /*base addr*/ base.get_addr(),
        /*cell addr*/ gate->get_local_addr()
    );
}

void CellEntityWithClient::on_client_reconnect_success() {
    propertys_sync2client(true);
    client.call("ready");
}

void CellEntityWithClient::destroy_self() {
    Entity::destroy_self();
}

void CellEntityWithClient::propertys_sync2client(bool force_all) {

    Encoder encoder;
    serialize_client(encoder, force_all);
    encoder.write_end();

    if (encoder.anything()) {
        client.call("prop_sync_from_cell", GBin(encoder.get_buf(), encoder.get_offset()));
    }
}

void CellEntityWithClient::begin_migrate(const GString& new_addr) {
    if (new_addr == get_listen_addr()) {
        // new cell addr == old cell addr
        WARN_LOG("ignore migrate to local addr\n");
        return;
    }

    if (new_addr == base.get_addr()) {
        // new cell addr == base addr
        WARN_LOG("ignore migrate to base addr\n");
        return;
    }

    if (!is_ready) {
        return;
    }

    if (migrate_state > MigrateState::Migrate_None) {
        return;
    }

    migrate_state = MigrateState::Migrate_Prepare;

    migrate_check_timer_id = REGIST_TIMER_INNER(10, 0, false, "migrate_check_timer", &CellEntityWithClient::migrate_check_timer);

    new_cell_addr = new_addr;
    base.call("migrate_req_from_cell");
    client.call("migrate_req_from_cell");
}

void CellEntityWithClient::migrate_reqack_from_base(bool is_ok) {
    is_reqack_from_base = true;
    real_begin_migrate();
}

void CellEntityWithClient::migrate_reqack_from_client(bool is_ok) { 
    is_reqack_from_client = true;
    real_begin_migrate();
}

void CellEntityWithClient::real_begin_migrate() {
    if (!is_reqack_from_base || !is_reqack_from_client || migrate_state != MigrateState::Migrate_Prepare) {
        return;
    }

    migrate_state = MigrateState::Migrate_RealMigrate;

    migrate_entity();
    is_reqack_from_base = false;
    is_reqack_from_client = false;
}

void CellEntityWithClient::on_migrate_out(GDict& migrate_data) {
    //INFO_LOG("entity.%s on_migrate_out\n", uuid.c_str());
    packet_migrate_data(migrate_data);
    release_timer();
}

void CellEntityWithClient::packet_migrate_data(GDict& migrate_data) {
    migrate_data.clear();

    migrate_data.insert(make_pair("entity_uuid", uuid));
    migrate_data.insert(make_pair("base_addr", base.get_addr()));
    migrate_data.insert(make_pair("client_addr", client.get_addr()));
    migrate_data.insert(make_pair("gate_addr", client.get_gate_addr()));

    migrate_timers.clear();
    for (auto iter = timers.begin(); iter != timers.end(); ++iter) {
        TimerBase* timer = *iter;
        Encoder encoder;
        timer->serialize(encoder);
        encoder.write_end();
        migrate_timers.insert(make_pair(timer->m_cb_name, GBin(encoder.get_buf(), encoder.get_offset())));
    }
    migrate_data.insert(make_pair("timers", migrate_timers));
    migrate_data.insert(make_pair("next_timer_id", next_timer_id));
}

void CellEntityWithClient::on_migrate_in(const GDict& migrate_data) {
    unpacket_migrate_data(migrate_data);

    base.call("new_cell_migrate_in", get_listen_addr());
    client.call("new_cell_migrate_in", get_listen_addr());
    is_ready = true;
}

void CellEntityWithClient::unpacket_migrate_data(const GDict& migrate_data) {
    base.set_entity_and_addr(uuid, migrate_data.at("base_addr").as_string());
    client.set_entity_and_addr(uuid, migrate_data.at("client_addr").as_string());
    client.set_gate_addr(migrate_data.at("gate_addr").as_string());

    auto _timers = migrate_data.at("timers").as_dict();
    for (auto iter = _timers.begin(); iter != _timers.end(); ++iter) {
        RESTORE_TIMER(iter->first, iter->second.as_bin());
    }
    next_timer_id = migrate_data.at("next_timer_id").as_int32();
}

void CellEntityWithClient::on_new_cell_migrate_finish() {
    destroy_self();
}

void CellEntityWithClient::migrate_check_timer() {
    migrate_failed_rollback("timeout");
}

void CellEntityWithClient::migrate_failed_rollback(const GString& reason) {
    WARN_LOG("CellEntityWithClient::migrate failed state.%d reason.%s\n", migrate_state, reason.c_str());

    if (migrate_state == MigrateState::Migrate_Prepare) {
        migrate_state = MigrateState::Migrate_None;

        base.call("new_cell_migrate_in", get_listen_addr());
        client.call("new_cell_migrate_in", get_listen_addr());
    }
    else if (migrate_state == MigrateState::Migrate_RealMigrate) {
        migrate_state = MigrateState::Migrate_None;

        for (auto iter = migrate_timers.begin(); iter != migrate_timers.end(); ++iter) {
            RESTORE_TIMER(iter->first, iter->second.as_bin());
        }

        base.call("new_cell_migrate_in", get_listen_addr());
        client.call("new_cell_migrate_in", get_listen_addr());

        // notify new cell destroy
        CellMailBox new_cell;
        new_cell.set_side("server");
        new_cell.set_entity_and_addr(uuid, new_cell_addr);
        new_cell.set_owner(this);
        new_cell.call("on_new_cell_migrate_finish");
    }
}

void CellEntityWithClient::cell_real_time_to_save(const GString& base_uuid, const GBin& base_bin) {
    Encoder cell_db;
    serialize_db(cell_db);
    cell_db.write_end();

    GBin cell_bin(cell_db.get_buf(), cell_db.get_offset());

    Encoder db;
    db.write_bin(base_bin);
    db.write_bin(cell_bin);
    db.write_end();

    // TODO - move to child thread
    GString db_file_name = "./db/" + uuid + ".bin";
    auto fp = fopen(db_file_name.c_str(), "wb");
    if (fp == nullptr) {
        ERROR_LOG("save - open db file %s error\n", db_file_name.c_str());
        return;
    }
    fwrite(db.get_buf(), db.get_offset(), 1, fp);
    fclose(fp);
    return;
}

void CellEntityWithClient::cell_disaster_backup(const GBin& base_all_db, const GDict& base_migrate_data) {
    disaster_backup_of_base = std::move(base_all_db);
    disaster_backup_of_base_migrate_data = base_migrate_data;

    Encoder self_all_db;
    serialize_all(self_all_db);
    self_all_db.write_end();
    disaster_backup_of_self = std::move(GBin(self_all_db.get_buf(), self_all_db.get_offset()));
    packet_migrate_data(disaster_backup_of_self_migrate_data);

    base.call("base_disaster_backup", GBin(self_all_db.get_buf(), self_all_db.get_offset()), disaster_backup_of_self_migrate_data);
}

void CellEntityWithClient::on_game_disappear(const GString& game_addr) {
    if (base.get_addr() != game_addr) {
        return;
    }

    if (migrate_check_timer_id != 0) {
        CANCEL_TIMER(migrate_check_timer_id);
        ready_check_timerid = 0;
        migrate_failed_rollback("on_game_disappear");
    }

    // base recover by disaster backup
    auto session = g_session_mgr.get_rand_session();
    REMOTE_RPC_CALL(session, "base_recover_by_disaster_backup",
        /*base addr*/get_listen_addr(),
        /*client addr*/client.get_addr(),
        /*client gate addr*/client.get_gate_addr(),
        /*entity class name*/class_name,
        /*uuid*/uuid,
        /*disaster backup of base*/disaster_backup_of_base,
        /*disaster backup of base migrate data*/disaster_backup_of_base_migrate_data
        );
}

void CellEntityWithClient::recover_by_disaster_backup(const GString& base_addr, const GString& client_addr, const GString& client_gate_addr) {
    base.set_entity_and_addr(uuid, base_addr);
    client.set_entity_and_addr(uuid, client_addr);
    client.set_gate_addr(client_gate_addr);

    base.call("cell_recover_by_disaster_backup_success", get_listen_addr());
    client.call("cell_recover_by_disaster_backup_success", get_listen_addr());

    is_ready = true;
    propertys_sync2client(true);
}

void CellEntityWithClient::base_recover_by_disaster_backup_success(const GString& base_addr) {
    base.set_entity_and_addr(uuid, base_addr);

    // sync recover self
    Decoder decoder(disaster_backup_of_self.buf, disaster_backup_of_self.size);
    decoder.skip_head_len();
    propertys_unserialize(decoder);
    propertys_sync2client(true);
}

void ClientEntity::on_create(const GDict& create_data) {
    Entity::on_create(create_data);

    base.set_entity_and_addr(uuid, create_data.at("base_addr").as_string());
    cell.set_entity_and_addr(uuid, create_data.at("cell_addr").as_string());

    base.call("ready");
}

void ClientEntity::on_reconnect_success(const GDict& create_data) {
    base.set_entity_and_addr(uuid, create_data.at("base_addr").as_string());
    cell.set_entity_and_addr(uuid, create_data.at("cell_addr").as_string());

    base.call("on_client_reconnect_success");
}

void ClientEntity::ready() { 
    Entity::ready();
}

void ClientEntity::destroy_self() {
    Entity::destroy_self();
}

void ClientEntity::prop_sync_from_base(const GBin& v) {
    Decoder decoder(v.buf, v.size);
    decoder.skip_head_len();
    propertys_unserialize(decoder);
    on_prop_sync_from_server();
}

void ClientEntity::prop_sync_from_cell(const GBin& v) {
    Decoder decoder(v.buf, v.size);
    decoder.skip_head_len();
    propertys_unserialize(decoder);
    on_prop_sync_from_server();
}

void ClientEntity::on_kick() {
    INFO_LOG("on kick\n");
}

void ClientEntity::base_recover_by_disaster_backup_success(const GString& base_addr) {
    base.set_entity_and_addr(uuid, base_addr);
}

void ClientEntity::cell_recover_by_disaster_backup_success(const GString& cell_addr) {
    cell.set_entity_and_addr(uuid, cell_addr);
    cell.clear_cache_rpc();
}

void ClientEntity::migrate_req_from_cell() {
    cell.call("migrate_reqack_from_client", true);
    cell.start_cache_rpc();
}

void ClientEntity::new_cell_migrate_in(const GString& new_cell_addr) {
    cell.set_entity_and_addr(cell.get_entity_uuid(), new_cell_addr);
    cell.stop_cache_rpc();
}

unordered_map<GString, Entity*> g_base_entities; // uuid -> entity
unordered_map<GString, Entity*> g_cell_entities; // uuid -> entity
unordered_map<GString, Entity*> g_client_entities; // uuid -> entity
typedef unordered_map<GString, function<Entity*()>> EntityCreatorMap;
EntityCreatorMap* get_entity_creator_map() {
    static EntityCreatorMap _entity_creator;
    return &_entity_creator;
}

void regist_entity_creator(const GString& entity_class_name, const function<Entity*()>& creator) {
    get_entity_creator_map()->insert(make_pair(entity_class_name, creator));
}

function<Entity*()> get_entity_creator(const GString& entity_class_name) {
    auto iter = get_entity_creator_map()->find(entity_class_name);
    if (iter == get_entity_creator_map()->end()) {
        return nullptr;
    }

    return iter->second;
}

Entity* create_local_base_entity(const GString& entity_class_name, const GString& entity_uuid) {
    function<Entity*()> creator = get_entity_creator("Base" + entity_class_name);
    if (!creator) {
        ERROR_LOG("base entity type(%s) error\n", entity_class_name.c_str());
        return nullptr;
    }

    Entity* entity = creator();
    entity->uuid = entity_uuid;
    entity->class_name = entity_class_name;

    auto iter = g_base_entities.find(entity->uuid);
    if (iter != g_base_entities.end()) {
        WARN_LOG("lose base entity.%s unexcept\n", entity->uuid.c_str());
        delete iter->second;
        g_base_entities.erase(iter);
    }

    g_base_entities.insert(make_pair(entity->uuid, entity));

    DEBUG_LOG("create_local_base_entity %s.%s\n", entity_class_name.c_str(), entity_uuid.c_str());

    return entity;
}

Entity* create_local_cell_entity(const GString& entity_class_name, const GString& entity_uuid) {
    function<Entity*()> creator = get_entity_creator("Cell" + entity_class_name);
    if (!creator) {
        ERROR_LOG("cell entity type(%s) error\n", entity_class_name.c_str());
        return nullptr;
    }

    Entity* entity = creator();
    entity->uuid = entity_uuid;
    entity->class_name = entity_class_name;

    auto iter = g_cell_entities.find(entity->uuid);
    if (iter != g_cell_entities.end()) {
        WARN_LOG("lose cell entity.%s unexcept\n", entity->uuid.c_str());
        delete iter->second;
        g_cell_entities.erase(iter);
    }

    g_cell_entities.insert(make_pair(entity->uuid, entity));

    DEBUG_LOG("create_local_cell_entity %s.%s\n", entity_class_name.c_str(), entity_uuid.c_str());

    return entity;
}

Entity* create_local_client_entity(const GString& entity_class_name, const GString& entity_uuid) {
    function<Entity*()> creator = get_entity_creator("Client" + entity_class_name);
    if (!creator) {
        ERROR_LOG("client entity type(%s) error\n", entity_class_name.c_str());
        return nullptr;
    }

    Entity* entity = creator();
    entity->uuid = entity_uuid;
    entity->class_name = entity_class_name;

    auto iter = g_client_entities.find(entity->uuid);
    if (iter != g_client_entities.end()) {
        WARN_LOG("lose client entity.%s unexcept\n", entity->uuid.c_str());
        delete iter->second;
        g_client_entities.erase(iter);
    }

    g_client_entities.insert(make_pair(entity->uuid, entity));

    DEBUG_LOG("create_local_client_entity %s.%s\n", entity_class_name.c_str(), entity_uuid.c_str());

    return entity;
}

void destroy_local_base_entity(const GString& entity_uuid) {
    auto iter = g_base_entities.find(entity_uuid);
    if (iter == g_base_entities.end()) {
        return;
    }

    const auto& entity = iter->second;
    entity->on_destroy();
    delete entity;
    g_base_entities.erase(iter);
}

void destroy_local_cell_entity(const GString& entity_uuid) {
    auto iter = g_cell_entities.find(entity_uuid);
    if (iter == g_cell_entities.end()) {
        return;
    }

    const auto& entity = iter->second;
    entity->on_destroy();
    delete entity;
    g_cell_entities.erase(iter);
}

void destroy_local_client_entity(const GString& entity_uuid) {
    auto iter = g_client_entities.find(entity_uuid);
    if (iter == g_client_entities.end()) {
        return;
    }

    const auto& entity = iter->second;
    entity->on_destroy();
    delete entity;
    g_client_entities.erase(iter);
}

RpcManagerBase* get_entity_rpc_mgr(Entity* entity) {
    return entity->get_rpc_mgr();
}

void entity_tick() {
    GArray base_entitys_to_destroy;
    GArray cell_entitys_to_destroy;
    GArray client_entitys_to_destroy;
    for (auto iter = g_base_entities.begin(); iter != g_base_entities.end(); ++iter) {
        iter->second->tick();
        if (iter->second->need_destroy) {
            base_entitys_to_destroy.push_back(iter->second->uuid);
        }
    }

    for (auto iter = g_cell_entities.begin(); iter != g_cell_entities.end(); ++iter) {
        iter->second->tick();
        if (iter->second->need_destroy) {
            cell_entitys_to_destroy.push_back(iter->second->uuid);
        }
    }

    for (auto iter = g_client_entities.begin(); iter != g_client_entities.end(); ++iter) {
        iter->second->tick();
        if (iter->second->need_destroy) {
            client_entitys_to_destroy.push_back(iter->second->uuid);
        }
    }

    for (auto iter = base_entitys_to_destroy.begin(); iter != base_entitys_to_destroy.end(); ++iter) {
        destroy_local_base_entity(iter->as_string());
    }

    for (auto iter = cell_entitys_to_destroy.begin(); iter != cell_entitys_to_destroy.end(); ++iter) {
        destroy_local_cell_entity(iter->as_string());
    }

    for (auto iter = client_entitys_to_destroy.begin(); iter != client_entitys_to_destroy.end(); ++iter) {
        destroy_local_client_entity(iter->as_string());
    }
}
