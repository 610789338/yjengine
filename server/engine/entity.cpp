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

    if (create_data.find("init_data") != create_data.end()) {
        auto& init_data = create_data.at("init_data").as_dict();
        if (is_base_entity() && init_data.find("base_data") != init_data.end()) {
            apply_init_data(init_data.at("base_data").as_dict());
        }
        else if (is_cell_entity() && init_data.find("cell_data") != init_data.end()) {
            apply_init_data(init_data.at("cell_data").as_dict());
        }
    }

    ready_check_timerid = REGIST_TIMER_INNER(10, 0, false, "ready_check_timer", &Entity::ready_check_timer);
    create_heart_beat_timer();
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

void Entity::release_event() {
    for (auto iter = events.begin(); iter != events.end(); ++iter) {
        auto event_bases = iter->second;
        for (auto iter_inner = event_bases.begin(); iter_inner != event_bases.end(); ++iter_inner) {
            delete *iter_inner;
        }
    }

    events.clear();
}

void Entity::propertys_sync2client(bool force_all) {
    // do nothing
}

double Entity::get_db_save_interval() {
    return 10.0;
}

void Entity::serialize_all(Encoder& encoder, bool need_clean_dirty) {
    for (auto iter = propertys.begin(); iter != propertys.end(); ++iter) {
        encoder.write_uint16(prop_str2int(iter->first));
        iter->second->serialize_all(encoder, need_clean_dirty);
    }
}

void Entity::serialize_db(Encoder& encoder, bool need_clean_dirty) {
    for (auto iter = propertys.begin(); iter != propertys.end(); ++iter) {

        if (!iter->second->need_sync2db()) {
            continue;
        }

        encoder.write_uint16(prop_str2int(iter->first));
        iter->second->serialize_all(encoder, need_clean_dirty);
    }
}

void Entity::serialize_client(Encoder& encoder, bool need_clean_dirty) {
    for (auto iter = dirty_propertys.begin(); iter != dirty_propertys.end(); ++iter) {

        if (!iter->second->need_sync2client()) {
            continue;
        }

        if (!iter->second->is_dirty() && !iter->second->is_all_dirty()) {
            continue;
        }

        encoder.write_uint16(prop_str2int(iter->first));
        iter->second->serialize(encoder, need_clean_dirty);
    }

    if(need_clean_dirty) dirty_propertys.clear();
}

void Entity::serialize_all_client(Encoder& encoder, bool need_clean_dirty) {
    for (auto iter = propertys.begin(); iter != propertys.end(); ++iter) {

        if (!iter->second->need_sync2client()) {
            continue;
        }

        encoder.write_uint16(prop_str2int(iter->first));
        iter->second->serialize_all(encoder, need_clean_dirty);
    }

    if (need_clean_dirty) dirty_propertys.clear();
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

    if (need_create_disater_backup_timer()) {
        create_disater_backup_timer();
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

bool Entity::is_base_entity() {
    if (get_entity_type() == EntityType::EntityType_BaseWithCellAndClient || 
        get_entity_type() == EntityType::EntityType_BaseWithClient || 
        get_entity_type() == EntityType::EntityType_BaseWithCell || 
        get_entity_type() == EntityType::EntityType_Base) {
        return true;
    }
    return false;
}

bool Entity::is_cell_entity() {
    if (get_entity_type() == EntityType::EntityType_CellWithClient ||
        get_entity_type() == EntityType::EntityType_Cell) {
        return true;
    }
    return false;
}

bool Entity::need_create_save_timer() {
    if (get_entity_type() == EntityType::EntityType_BaseWithCellAndClient) {
        return true;
    }
    return false;
}

bool Entity::need_create_disater_backup_timer() {
    if (get_entity_type() == EntityType::EntityType_BaseWithCellAndClient || 
        get_entity_type() == EntityType::EntityType_BaseWithCell) {
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
    size_t erase_num = timers.erase(timer);
    timer_ids.erase(timer->m_id);

    if (timer->m_cb_name == "base_rpc_timer") {
        int a = 10;
    }

    if (erase_num > 1) {
        WARN_LOG("@@@@@@@@@@@@@@@@ erase_num.%d\n", erase_num);
    }
}

void Entity::add_event(EventBase* event) {
    auto iter = events.find(event->m_event_name);
    if (iter == events.end()) {
        events.insert(make_pair(event->m_event_name, vector<EventBase*>()));
        iter = events.find(event->m_event_name);
    }

    iter->second.push_back(event);
}

void BaseEntity::on_create(const GDict& create_data) {
    Entity::on_create(create_data);

    ready();
}

MailBox BaseEntity::get_self_mailbox() {
    MailBox mailbox;
    mailbox.set_entity_and_addr(uuid, get_listen_addr());
    mailbox.set_base();
    return mailbox;
}

void BaseEntityWithCell::on_create(const GDict& create_data) {
    Entity::on_create(create_data);

    create_cell(create_data);
}

void BaseEntityWithCell::ready_check_timer() {
    if (!is_ready) {
        destroy_self();
        cell.call("destroy_self");
    }
}

void BaseEntityWithCell::create_cell(const GDict& create_data) {

    // game -> gate -> game
    auto const& session = g_session_mgr.get_rand_session();
    REMOTE_RPC_CALL(session, "create_cell_entity", entity_name, 
        /*entity uuid*/ uuid,
        /*base addr*/ session->get_local_addr(),
        /*gate addr*/ create_data.at("gate_addr").as_string(),
        /*client addr*/ GString(),
        /*cell bin*/ create_data.at("cell_bin").as_bin());
}

void BaseEntityWithCell::on_cell_create(const GString& cell_addr) {
    cell.set_entity_and_addr(uuid, cell_addr);
    ready();
}

void BaseEntityWithCell::ready() {
    // ready通知意味base/cell entity都创建好了
    if (is_ready) {
        return;
    }

    Entity::ready();
    cell.call("ready");
}

void BaseEntityWithCell::destroy_self() {
    Entity::destroy_self();
}

void BaseEntityWithCell::create_heart_beat_timer() {
    heart_beat_timerid = REGIST_TIMER_INNER(0, 45, true, "heart_beat_timer", &BaseEntityWithCell::heart_beat_timer);
}

void BaseEntityWithCell::heart_beat_timer() {
    cell.call("respect_from_base");

    auto now = now_timestamp();
    if (last_respect_from_cell == 0) {
        last_respect_from_cell = now;
    }

    if (now - last_respect_from_cell > 60) {
        WARN_LOG("%s lose touch with cell\n", uuid.c_str());
        destroy_self();
    }
}

void BaseEntityWithCell::respect_from_cell() {
    last_respect_from_cell = now_timestamp();
}

void BaseEntityWithCell::real_time_to_save() {
    // rpc to cell
    Encoder base_db;
    serialize_db(base_db);
    base_db.write_end();

    GBin base_bin;
    base_db.move_to_bin(base_bin);
    cell.call("cell_real_time_to_save", uuid, base_bin);
}

void BaseEntityWithCell::real_time_to_disaster_backup() {
    Encoder self_all_db;
    serialize_all(self_all_db);
    self_all_db.write_end();
    self_all_db.move_to_bin(disaster_backup_of_self);
    //disaster_backup_of_self = std::move(GBin(self_all_db.get_buf(), self_all_db.get_offset()));
    packet_migrate_data(disaster_backup_of_self_migrate_data);

    cell.call("cell_disaster_backup", disaster_backup_of_self, disaster_backup_of_self_migrate_data);
}

void BaseEntityWithCell::migrate_req_from_cell() {
    cell.call("migrate_reqack_from_base", true);
    cell.start_cache_rpc();
}

void BaseEntityWithCell::new_cell_migrate_in(const GString& new_cell_addr) {
    cell.set_entity_and_addr(cell.get_entity_uuid(), new_cell_addr);
    cell.stop_cache_rpc();

    real_time_to_disaster_backup();
}

void BaseEntityWithCell::packet_migrate_data(GDict& migrate_data) {
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

    migrate_events.clear();
    for (auto iter = events.begin(); iter != events.end(); ++iter) {
        auto event_bases = iter->second;
        for (auto iter_inner = event_bases.begin(); iter_inner != event_bases.end(); ++iter_inner) {
            Encoder encoder;
            encoder.write_string((*iter_inner)->m_event_name);
            encoder.write_string((*iter_inner)->m_component_name);
            encoder.write_end();
            migrate_events.push_back(GBin(encoder.get_buf(), encoder.get_offset()));
        }
    }
    migrate_data.insert(make_pair("events", migrate_events));
    migrate_data.insert(make_pair("next_timer_id", next_timer_id));
}

void BaseEntityWithCell::unpacket_migrate_data(const GDict& migrate_data) {
    release_timer();
    auto _timers = migrate_data.at("timers").as_dict();
    for (auto iter = _timers.begin(); iter != _timers.end(); ++iter) {
        RESTORE_TIMER(iter->first, iter->second.as_bin());
    }
    next_timer_id = migrate_data.at("next_timer_id").as_int32();

    release_event();
    auto _events = migrate_data.at("events").as_array();
    for (auto iter = _events.begin(); iter != _events.end(); ++iter) {
        RESTORE_EVENT((*iter).as_bin());
    }
}

void BaseEntityWithCell::base_disaster_backup(const GBin& cell_all_db, const GDict& cell_migrate_data) {
    disaster_backup_of_cell = std::move(cell_all_db);
    disaster_backup_of_cell_migrate_data = cell_migrate_data;
}

void BaseEntityWithCell::on_game_disappear(const GString& game_addr) {
    if (cell.get_addr() != game_addr) {
        return;
    }

    if (disaster_backup_of_cell.size == 0 || disaster_backup_of_cell_migrate_data.size() == 0) {
        ERROR_LOG("try backup cell fail %d %d\n", int32_t(disaster_backup_of_cell.size), int32_t(disaster_backup_of_cell_migrate_data.size()));
        return;
    }

    // cell recover by disaster backup
    auto session = g_session_mgr.get_rand_session();
    REMOTE_RPC_CALL(session, "cell_recover_by_disaster_backup",
        /*base addr*/get_listen_addr(),
        /*client addr*/GString(),
        /*client gate addr*/GString(),
        /*entity name*/entity_name,
        /*uuid*/uuid,
        /*disaster backup of cell*/disaster_backup_of_cell,
        /*disaster backup of cell migrate data*/disaster_backup_of_cell_migrate_data);
}

void BaseEntityWithCell::recover_by_disaster_backup(const GString& cell_addr, const GString& client_addr, const GString& client_gate_addr) {
    cell.set_entity_and_addr(uuid, cell_addr);

    cell.call("base_recover_by_disaster_backup_success", get_listen_addr());

    is_ready = true;

    real_time_to_disaster_backup();
}

void BaseEntityWithCell::cell_recover_by_disaster_backup_success(const GString& cell_addr) {
    cell.set_entity_and_addr(uuid, cell_addr);
    cell.clear_cache_rpc();

    // sync recover self
    Decoder decoder(disaster_backup_of_self.buf, disaster_backup_of_self.size);
    decoder.skip_head_len();
    propertys_unserialize(decoder);

    unpacket_migrate_data(disaster_backup_of_self_migrate_data);

    real_time_to_disaster_backup();
}

void BaseEntityWithClient::on_create(const GDict& create_data) {
    Entity::on_create(create_data);

    client.set_entity_and_addr(uuid, create_data.at("client_addr").as_string());
    client.set_gate_addr(create_data.at("gate_addr").as_string());

    create_client();
}

void BaseEntityWithClient::ready_check_timer() {
    if (!is_ready) {
        destroy_self();
        client.call("destroy_self");
    }
}

void BaseEntityWithClient::create_client() {
    // game -> gate -> client
    auto gate = g_session_mgr.get_gate(client.get_gate_addr());
    REMOTE_RPC_CALL(gate, "create_client_entity", client.get_addr(), entity_name,
        /*entity uuid*/ uuid,
        /*base addr*/ gate->get_local_addr(),
        /*cell addr*/ ""
    );
}

void BaseEntityWithClient::ready() {
    // ready通知意味base/client entity都创建好了
    if (is_ready) {
        return;
    }

    Entity::ready();
    client.call("ready");
}

void BaseEntityWithClient::destroy_self() {
    Entity::destroy_self();
}

void BaseEntityWithClient::on_reconnect_fromclient(const GString& client_addr, const GString& gate_addr) {
    INFO_LOG("[base] entity.%s reconnect from client\n", client.get_entity_uuid().c_str());

    // kick old client
    kick_client();

    // recover client mailbox
    client.set_entity_and_addr(client.get_entity_uuid(), client_addr);
    client.set_gate_addr(gate_addr);

    // create new client
    auto gate = g_session_mgr.get_gate(gate_addr);
    REMOTE_RPC_CALL(gate, "create_client_entity_onreconnect", client.get_addr(), entity_name,
        /*uuid*/ uuid,
        /*base addr*/ get_listen_addr(),
        /*cell addr*/ ""
    );
}

void BaseEntityWithClient::on_client_reconnect_success() {
    // 对于服务端，登陆会收到ready通知，断线重连会收到on_client_reconnect_success通知
    // 对于客户端，无论是登陆还是断线重连，只有ready通知，也就是说客户端对断线重连是无感的
    if (!is_ready) {
        // 如果是登陆流程中途客户端断线，重连上来后重新走ready通知
        ready();
    }
    else {
        propertys_sync2client(true);
        client.call("ready");
    }
}

void BaseEntityWithClient::propertys_sync2client(bool force_all) {
    Encoder encoder;
    if (force_all) {
        serialize_all_client(encoder, true);
    }
    else {
        serialize_client(encoder, true);
    }
    encoder.write_end();

    if (encoder.anything()) {
        //byte_print(encoder.get_buf(), encoder.get_offset()); // TODO delete
        client.call("prop_sync_from_base", GBin(encoder.get_buf(), encoder.get_offset()));
    }
}

void BaseEntityWithClient::kick_client() {
    client.call("on_kick");
}

void BaseEntityWithClient::real_time_to_save() {
    Encoder base_db;
    serialize_db(base_db);
    base_db.write_end();

    GBin base_bin;
    base_db.move_to_bin(base_bin);

    Encoder db;
    db.write_bin(base_bin);
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

void BaseEntityWithCellAndClient::create_cell(const GDict& create_data) {
    // game -> gate -> game
    auto const& session = g_session_mgr.get_rand_session();
    REMOTE_RPC_CALL(session, "create_cell_entity", entity_name,
        /*entity uuid*/ uuid,
        /*base addr*/ session->get_local_addr(),
        /*gate addr*/ create_data.at("gate_addr").as_string(),
        /*client addr*/ create_data.at("client_addr").as_string(),
        /*cell bin*/ create_data.at("cell_bin").as_bin());
}

void BaseEntityWithCellAndClient::on_cell_create(const GString& cell_addr) {
    cell.set_entity_and_addr(uuid, cell_addr);
    create_client();
}

void BaseEntityWithCellAndClient::create_client() {
    // game -> gate -> client
    auto gate = g_session_mgr.get_gate(client.get_gate_addr());
    REMOTE_RPC_CALL(gate, "create_client_entity", client.get_addr(), entity_name,
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
    if (force_all) {
        serialize_all_client(encoder, true);
    }
    else {
        serialize_client(encoder, true);
    }
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
    BaseEntityWithCell::real_time_to_save();
}

void BaseEntityWithCellAndClient::new_cell_migrate_in(const GString& new_cell_addr) {
    BaseEntityWithCell::new_cell_migrate_in(new_cell_addr);
}

void BaseEntityWithCellAndClient::real_time_to_disaster_backup() {
    BaseEntityWithCell::real_time_to_disaster_backup();
}

void BaseEntityWithCellAndClient::packet_migrate_data(GDict& migrate_data) {
    BaseEntityWithCell::packet_migrate_data(migrate_data);
}

void BaseEntityWithCellAndClient::unpacket_migrate_data(const GDict& migrate_data) {
    BaseEntityWithCell::unpacket_migrate_data(migrate_data);
}

void BaseEntityWithCellAndClient::base_disaster_backup(const GBin& cell_all_db, const GDict& cell_migrate_data) {
    BaseEntityWithCell::base_disaster_backup(cell_all_db, cell_migrate_data);
}

void BaseEntityWithCellAndClient::on_game_disappear(const GString& game_addr) {
    if (cell.get_addr() != game_addr) {
        return;
    }

    if (disaster_backup_of_cell.size == 0 || disaster_backup_of_cell_migrate_data.size() == 0) {
        ERROR_LOG("try backup cell fail %s %s\n", disaster_backup_of_cell.size, disaster_backup_of_cell_migrate_data.size());
        return;
    }

    // cell recover by disaster backup
    auto session = g_session_mgr.get_rand_session();
    REMOTE_RPC_CALL(session, "cell_recover_by_disaster_backup",
        /*base addr*/get_listen_addr(),
        /*client addr*/client.get_addr(),
        /*client gate addr*/client.get_gate_addr(),
        /*entity name*/entity_name,
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

    real_time_to_disaster_backup();
}

void BaseEntityWithCellAndClient::cell_recover_by_disaster_backup_success(const GString& cell_addr) {
    cell.set_entity_and_addr(uuid, cell_addr);
    cell.clear_cache_rpc();

    // sync recover self
    Decoder decoder(disaster_backup_of_self.buf, disaster_backup_of_self.size);
    decoder.skip_head_len();
    propertys_unserialize(decoder);
    propertys_sync2client(true);

    unpacket_migrate_data(disaster_backup_of_self_migrate_data);

    real_time_to_disaster_backup();
}

void CellEntity::on_create(const GDict& create_data) {
    Entity::on_create(create_data);

    base.set_entity_and_addr(uuid, create_data.at("base_addr").as_string());

    base.call("on_cell_create", server->get_listen_addr());
}

void CellEntity::ready_check_timer() {
    if (!is_ready) {
        destroy_self();
        base.call("destroy_self");
    }
}

void CellEntity::create_heart_beat_timer() {
    heart_beat_timerid = REGIST_TIMER_INNER(0, 45, true, "heart_beat_timer", &CellEntity::heart_beat_timer);
}

void CellEntity::heart_beat_timer() {
    base.call("respect_from_cell");

    if (last_respect_from_base == 0) {
        last_respect_from_base = now_timestamp();
    }

    if (now_timestamp() - last_respect_from_base > 60) {
        WARN_LOG("%s lose touch with base\n", uuid.c_str());
        destroy_self();
    }
}

void CellEntity::respect_from_base() {
    last_respect_from_base = now_timestamp();
}

void CellEntity::destroy_self() {
    Entity::destroy_self();
}

void CellEntity::begin_migrate(const GString& new_addr) {
    //GString new_addr(IPPORT_STRING(ini_get_string("MigrateAddr", "ip"), ini_get_int("MigrateAddr", "port")));

    if (new_addr == get_listen_addr()) {
        // new cell addr == old cell addr
        WARN_LOG("ignore migrate, new addr(%s) equal to local addr(%s)\n", new_addr.c_str(), get_listen_addr().c_str());
        return;
    }

    //if (new_addr == base.get_addr()) {
    //    // new cell addr == base addr
    //    WARN_LOG("ignore migrate, new addr(%s) equal to base addr(%s)\n", new_addr.c_str(), base.get_addr().c_str());
    //    return;
    //}

    if (!is_ready) {
        WARN_LOG("migrate error: entity.%s is not ready\n", uuid.c_str());
        return;
    }

    if (migrate_state > MigrateState::Migrate_None) {
        WARN_LOG("entity.%s migrate state.%d invalid\n", uuid.c_str(), migrate_state);
        return;
    }

    migrate_state = MigrateState::Migrate_Prepare;

    migrate_check_timer_id = REGIST_TIMER_INNER(10, 0, false, "migrate_check_timer", &CellEntity::migrate_check_timer);

    new_cell_addr = new_addr;
    base.call("migrate_req_from_cell");
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
        /*entity name*/entity_name,
        /*create data*/migrate_data,
        /*entity property*/GBin(encoder.get_buf(), encoder.get_offset()));
}

void CellEntity::on_migrate_out(GDict& migrate_data) {
    //INFO_LOG("entity.%s on_migrate_out\n", uuid.c_str());
    packet_migrate_data(migrate_data);
    release_timer();
    release_event();
}

void CellEntity::packet_migrate_data(GDict& migrate_data) {
    migrate_data.clear();

    migrate_data.insert(make_pair("entity_uuid", uuid));
    migrate_data.insert(make_pair("base_addr", base.get_addr()));

    migrate_timers.clear();
    for (auto iter = timers.begin(); iter != timers.end(); ++iter) {
        TimerBase* timer = *iter;
        Encoder encoder;
        timer->serialize(encoder);
        encoder.write_end();
        migrate_timers.insert(make_pair(timer->m_cb_name, GBin(encoder.get_buf(), encoder.get_offset())));
    }
    migrate_data.insert(make_pair("timers", migrate_timers));

    migrate_events.clear();
    for (auto iter = events.begin(); iter != events.end(); ++iter) {
        auto event_bases = iter->second;
        for (auto iter_inner = event_bases.begin(); iter_inner != event_bases.end(); ++iter_inner) {
            Encoder encoder;
            encoder.write_string((*iter_inner)->m_event_name);
            encoder.write_string((*iter_inner)->m_component_name);
            encoder.write_end();
            migrate_events.push_back(GBin(encoder.get_buf(), encoder.get_offset()));
        }
    }
    migrate_data.insert(make_pair("events", migrate_events));
    migrate_data.insert(make_pair("next_timer_id", next_timer_id));
    migrate_data.insert(make_pair("last_respect_from_base", last_respect_from_base));
}

void CellEntity::on_migrate_in(const GDict& migrate_data) {
    unpacket_migrate_data(migrate_data);

    base.call("new_cell_migrate_in", get_listen_addr());
    is_ready = true;

    send_event("on_migrate_in");
}

void CellEntity::unpacket_migrate_data(const GDict& migrate_data) {
    base.set_entity_and_addr(uuid, migrate_data.at("base_addr").as_string());

    release_timer();
    auto _timers = migrate_data.at("timers").as_dict();
    for (auto iter = _timers.begin(); iter != _timers.end(); ++iter) {
        RESTORE_TIMER(iter->first, iter->second.as_bin());
    }
    next_timer_id = migrate_data.at("next_timer_id").as_int32();

    release_event();
    auto _events = migrate_data.at("events").as_array();
    for (auto iter = _events.begin(); iter != _events.end(); ++iter) {
        RESTORE_EVENT((*iter).as_bin());
    }

    last_respect_from_base = migrate_data.at("last_respect_from_base").as_int64();
}

void CellEntity::on_new_cell_migrate_finish() {
    destroy_self();
}

void CellEntity::migrate_check_timer() {
    migrate_failed_rollback("timeout");
}

void CellEntity::migrate_failed_rollback(const GString& reason) {
    WARN_LOG("CellEntity::migrate failed state.%d reason.%s\n", migrate_state, reason.c_str());

    if (migrate_state == MigrateState::Migrate_Prepare) {
        migrate_state = MigrateState::Migrate_None;

        base.call("new_cell_migrate_in", get_listen_addr());
    }
    else if (migrate_state == MigrateState::Migrate_RealMigrate) {
        migrate_state = MigrateState::Migrate_None;

        for (auto iter = migrate_timers.begin(); iter != migrate_timers.end(); ++iter) {
            RESTORE_TIMER(iter->first, iter->second.as_bin());
        }

        base.call("new_cell_migrate_in", get_listen_addr());

        // notify new cell destroy
        CellMailBox new_cell;
        new_cell.set_entity_and_addr(uuid, new_cell_addr);
        new_cell.set_owner(this);
        new_cell.call("on_new_cell_migrate_finish");
    }
}

void CellEntity::cell_real_time_to_save(const GString& base_uuid, const GBin& base_bin) {
    Encoder cell_db;
    serialize_db(cell_db);
    cell_db.write_end();

    GBin cell_bin;
    cell_db.move_to_bin(cell_bin);

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

void CellEntity::cell_disaster_backup(const GBin& base_all_db, const GDict& base_migrate_data) {
    disaster_backup_of_base = std::move(base_all_db);
    disaster_backup_of_base_migrate_data = base_migrate_data;

    Encoder self_all_db;
    serialize_all(self_all_db);
    self_all_db.write_end();
    self_all_db.move_to_bin(disaster_backup_of_self);
    //disaster_backup_of_self = std::move(GBin(self_all_db.get_buf(), self_all_db.get_offset()));
    packet_migrate_data(disaster_backup_of_self_migrate_data);

    base.call("base_disaster_backup", disaster_backup_of_self, disaster_backup_of_self_migrate_data);
}

void CellEntity::on_game_disappear(const GString& game_addr) {
    if (base.get_addr() != game_addr) {
        return;
    }

    if (migrate_check_timer_id != 0) {
        CANCEL_TIMER(migrate_check_timer_id);
        ready_check_timerid = 0;
        migrate_failed_rollback("on_game_disappear");
    }

    if (disaster_backup_of_base.size == 0 || disaster_backup_of_base_migrate_data.size() == 0) {
        ERROR_LOG("try backup base fail %d %d\n", int32_t(disaster_backup_of_base.size), int32_t(disaster_backup_of_base_migrate_data.size()));
        return;
    }

    // base recover by disaster backup
    auto session = g_session_mgr.get_rand_session();
    REMOTE_RPC_CALL(session, "base_recover_by_disaster_backup",
        /*base addr*/get_listen_addr(),
        /*client addr*/GString(),
        /*client gate addr*/GString(),
        /*entity name*/entity_name,
        /*uuid*/uuid,
        /*disaster backup of base*/disaster_backup_of_base,
        /*disaster backup of base migrate data*/disaster_backup_of_base_migrate_data
    );
}

void CellEntity::recover_by_disaster_backup(const GString& base_addr, const GString& client_addr, const GString& client_gate_addr) {
    base.set_entity_and_addr(uuid, base_addr);

    base.call("cell_recover_by_disaster_backup_success", get_listen_addr());

    is_ready = true;
}

void CellEntity::base_recover_by_disaster_backup_success(const GString& base_addr) {
    // sync recover self
    Decoder decoder(disaster_backup_of_self.buf, disaster_backup_of_self.size);
    decoder.skip_head_len();
    propertys_unserialize(decoder);

    unpacket_migrate_data(disaster_backup_of_self_migrate_data);

    // set base addr after recover self
    base.set_entity_and_addr(uuid, base_addr);
}

MailBox CellEntity::get_self_mailbox() {
    MailBox mailbox;
    mailbox.set_entity_and_addr(base.get_entity_uuid(), base.get_addr());
    mailbox.set_flag(base.get_flag());
    mailbox.set_b2c();
    return mailbox;
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

void CellEntityWithClient::create_heart_beat_timer() {
    CellEntity::create_heart_beat_timer();
}

void CellEntityWithClient::heart_beat_timer() {
    CellEntity::heart_beat_timer();
}

void CellEntityWithClient::respect_from_base() {
    CellEntity::respect_from_base();
}

void CellEntityWithClient::on_reconnect_fromclient(const GString& client_addr, const GString& gate_addr) {
    // recover client mailbox
    client.set_entity_and_addr(uuid, client_addr);
    client.set_gate_addr(gate_addr);

    // create new client
    auto gate = g_session_mgr.get_gate(gate_addr);
    REMOTE_RPC_CALL(gate, "create_client_entity_onreconnect", client.get_addr(), entity_name,
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
    if (force_all) {
        serialize_all_client(encoder, true);
    }
    else {
        serialize_client(encoder, true);
    }
    encoder.write_end();

    if (encoder.anything()) {
        client.call("prop_sync_from_cell", GBin(encoder.get_buf(), encoder.get_offset()));
    }
}

void CellEntityWithClient::begin_migrate(const GString& new_addr) {
    //GString new_addr(IPPORT_STRING(ini_get_string("MigrateAddr", "ip"), ini_get_int("MigrateAddr", "port")));

    if (new_addr == get_listen_addr()) {
        // new cell addr == old cell addr
        WARN_LOG("ignore migrate, new addr(%s) equal to local addr(%s)\n", new_addr.c_str(), get_listen_addr().c_str());
        return;
    }

    //if (new_addr == base.get_addr()) {
    //    // new cell addr == base addr
    //    WARN_LOG("ignore migrate, new addr(%s) equal to base addr(%s)\n", new_addr.c_str(), base.get_addr().c_str());
    //    return;
    //}

    //INFO_LOG("entity.%s migrate %s to %s\n", uuid.c_str(), get_listen_addr().c_str(), new_addr.c_str());

    if (!is_ready) {
        WARN_LOG("migrate error: entity.%s is not ready\n", uuid.c_str());
        return;
    }

    if (migrate_state > MigrateState::Migrate_None) {
        WARN_LOG("entity.%s migrate state.%d invalid\n", uuid.c_str(), migrate_state);
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
    CellEntity::on_migrate_out(migrate_data);
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

    migrate_events.clear();
    for (auto iter = events.begin(); iter != events.end(); ++iter) {
        auto event_bases = iter->second;
        for (auto iter_inner = event_bases.begin(); iter_inner != event_bases.end(); ++iter_inner) {
            Encoder encoder;
            encoder.write_string((*iter_inner)->m_event_name);
            encoder.write_string((*iter_inner)->m_component_name);
            encoder.write_end();
            migrate_events.push_back(GBin(encoder.get_buf(), encoder.get_offset()));
        }
    }
    migrate_data.insert(make_pair("events", migrate_events));
    migrate_data.insert(make_pair("next_timer_id", next_timer_id));
    migrate_data.insert(make_pair("last_respect_from_base", last_respect_from_base));
}

void CellEntityWithClient::on_migrate_in(const GDict& migrate_data) {
    unpacket_migrate_data(migrate_data);

    base.call("new_cell_migrate_in", get_listen_addr());
    client.call("new_cell_migrate_in", get_listen_addr());
    is_ready = true;

    // move to timer restore
    //create_heart_beat_timer();

    send_event("on_migrate_in");
}

void CellEntityWithClient::unpacket_migrate_data(const GDict& migrate_data) {
    base.set_entity_and_addr(uuid, migrate_data.at("base_addr").as_string());
    client.set_entity_and_addr(uuid, migrate_data.at("client_addr").as_string());
    client.set_gate_addr(migrate_data.at("gate_addr").as_string());

    release_timer();
    auto _timers = migrate_data.at("timers").as_dict();
    for (auto iter = _timers.begin(); iter != _timers.end(); ++iter) {
        RESTORE_TIMER(iter->first, iter->second.as_bin());
    }
    next_timer_id = migrate_data.at("next_timer_id").as_int32();

    release_event();
    auto _events = migrate_data.at("events").as_array();
    for (auto iter = _events.begin(); iter != _events.end(); ++iter) {
        RESTORE_EVENT((*iter).as_bin());
    }

    last_respect_from_base = migrate_data.at("last_respect_from_base").as_int64();
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
        new_cell.set_entity_and_addr(uuid, new_cell_addr);
        new_cell.set_owner(this);
        new_cell.call("on_new_cell_migrate_finish");
    }
}

void CellEntityWithClient::cell_real_time_to_save(const GString& base_uuid, const GBin& base_bin) {
    CellEntity::cell_real_time_to_save(base_uuid, base_bin);
}

void CellEntityWithClient::cell_disaster_backup(const GBin& base_all_db, const GDict& base_migrate_data) {
    CellEntity::cell_disaster_backup(base_all_db, base_migrate_data);
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

    if (disaster_backup_of_base.size == 0 || disaster_backup_of_base_migrate_data.size() == 0) {
        ERROR_LOG("try backup base fail %d %d\n", int32_t(disaster_backup_of_base.size == 0), int32_t(disaster_backup_of_base_migrate_data.size()));
        return;
    }

    // base recover by disaster backup
    auto session = g_session_mgr.get_rand_session();
    REMOTE_RPC_CALL(session, "base_recover_by_disaster_backup",
        /*base addr*/get_listen_addr(),
        /*client addr*/client.get_addr(),
        /*client gate addr*/client.get_gate_addr(),
        /*entity name*/entity_name,
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

    // move to timer restore
    //create_heart_beat_timer();
}

void CellEntityWithClient::base_recover_by_disaster_backup_success(const GString& base_addr) {
    // sync recover self
    Decoder decoder(disaster_backup_of_self.buf, disaster_backup_of_self.size);
    decoder.skip_head_len();
    propertys_unserialize(decoder);
    propertys_sync2client(true);

    unpacket_migrate_data(disaster_backup_of_self_migrate_data);

    // set base addr after recover self
    base.set_entity_and_addr(uuid, base_addr);
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

boost::shared_mutex g_base_entity_mutex;
boost::shared_mutex g_cell_entity_mutex;
boost::shared_mutex g_client_entity_mutex;

inline void thread_safe_add_base_entity(Entity* entity) {
    unique_lock<boost::shared_mutex> lock(g_base_entity_mutex);
    g_base_entities.insert(make_pair(entity->uuid, entity));
}

inline void thread_safe_remove_base_entity(GString entity_uuid) {
    unique_lock<boost::shared_mutex> lock(g_base_entity_mutex);
    g_base_entities.erase(entity_uuid);
}

inline Entity* thread_safe_get_base_entity(GString entity_uuid) {
    shared_lock<boost::shared_mutex> lock(g_base_entity_mutex);
    auto iter = g_base_entities.find(entity_uuid);
    return iter != g_base_entities.end() ? iter->second : nullptr;
}

inline RpcManagerBase* thread_safe_get_base_entity_rpcmgr(GString entity_uuid) {
    shared_lock<boost::shared_mutex> lock(g_base_entity_mutex);
    auto iter = g_base_entities.find(entity_uuid);
    return iter != g_base_entities.end() ? iter->second->rpc_mgr : nullptr;
}

inline void thread_safe_add_cell_entity(Entity* entity) {
    unique_lock<boost::shared_mutex> lock(g_cell_entity_mutex);
    g_cell_entities.insert(make_pair(entity->uuid, entity));
}

inline void thread_safe_remove_cell_entity(GString entity_uuid) {
    unique_lock<boost::shared_mutex> lock(g_cell_entity_mutex);
    g_cell_entities.erase(entity_uuid);
}

inline Entity* thread_safe_get_cell_entity(GString entity_uuid) {
    shared_lock<boost::shared_mutex> lock(g_cell_entity_mutex);
    auto iter = g_cell_entities.find(entity_uuid);
    return iter != g_cell_entities.end() ? iter->second : nullptr;
}

inline RpcManagerBase* thread_safe_get_cell_entity_rpcmgr(GString entity_uuid) {
    shared_lock<boost::shared_mutex> lock(g_cell_entity_mutex);
    auto iter = g_cell_entities.find(entity_uuid);
    return iter != g_cell_entities.end() ? iter->second->rpc_mgr : nullptr;
}

inline void thread_safe_add_client_entity(Entity* entity) {
    unique_lock<boost::shared_mutex> lock(g_client_entity_mutex);
    g_client_entities.insert(make_pair(entity->uuid, entity));
}

inline void thread_safe_remove_client_entity(GString entity_uuid) {
    unique_lock<boost::shared_mutex> lock(g_client_entity_mutex);
    g_client_entities.erase(entity_uuid);
}

inline Entity* thread_safe_get_client_entity(GString entity_uuid) {
    shared_lock<boost::shared_mutex> lock(g_client_entity_mutex);
    auto iter = g_client_entities.find(entity_uuid);
    return iter != g_client_entities.end() ? iter->second : nullptr;
}

inline RpcManagerBase* thread_safe_get_client_entity_rpcmgr(GString entity_uuid) {
    shared_lock<boost::shared_mutex> lock(g_client_entity_mutex);
    auto iter = g_client_entities.find(entity_uuid);
    return iter != g_client_entities.end() ? iter->second->rpc_mgr : nullptr;
}

typedef unordered_map<GString, function<Entity*()>> EntityCreatorMap;
EntityCreatorMap* get_entity_creator_map() {
    static EntityCreatorMap _entity_creator;
    return &_entity_creator;
}

void regist_entity_creator(const GString& entity_name, const function<Entity*()>& creator) {
    get_entity_creator_map()->insert(make_pair(entity_name, creator));
}

function<Entity*()> get_entity_creator(const GString& entity_name) {
    auto iter = get_entity_creator_map()->find(entity_name);
    if (iter == get_entity_creator_map()->end()) {
        return nullptr;
    }

    return iter->second;
}

Entity* create_local_base_entity(const GString& entity_name, const GString& entity_uuid) {
    function<Entity*()> creator = get_entity_creator("Base" + entity_name);
    if (!creator) {
        ERROR_LOG("base entity type(%s) error\n", entity_name.c_str());
        return nullptr;
    }

    Entity* entity = creator();
    entity->uuid = entity_uuid;
    entity->entity_name = entity_name;

    auto old_entity = thread_safe_get_base_entity(entity->uuid);
    if (old_entity != nullptr) {
        WARN_LOG("lose base entity.%s unexcept\n", entity->uuid.c_str());
        delete old_entity;
        //g_base_entities.erase(iter);
        thread_safe_remove_base_entity(entity_uuid);
    }

    //g_base_entities.insert(make_pair(entity->uuid, entity));
    thread_safe_add_base_entity(entity);

    DEBUG_LOG("create_local_base_entity %s.%s\n", entity_name.c_str(), entity_uuid.c_str());

    return entity;
}

Entity* create_local_cell_entity(const GString& entity_name, const GString& entity_uuid) {
    function<Entity*()> creator = get_entity_creator("Cell" + entity_name);
    if (!creator) {
        ERROR_LOG("cell entity type(%s) error\n", entity_name.c_str());
        return nullptr;
    }

    Entity* entity = creator();
    entity->uuid = entity_uuid;
    entity->entity_name = entity_name;

    auto old_entity = thread_safe_get_cell_entity(entity->uuid);
    if (old_entity != nullptr) {
        WARN_LOG("lose cell entity.%s unexcept\n", entity->uuid.c_str());
        delete old_entity;
        //g_cell_entities.erase(iter);
        thread_safe_remove_cell_entity(entity_uuid);
    }

    //g_cell_entities.insert(make_pair(entity->uuid, entity));
    thread_safe_add_cell_entity(entity);

    DEBUG_LOG("create_local_cell_entity %s.%s\n", entity_name.c_str(), entity_uuid.c_str());

    return entity;
}

Entity* create_local_client_entity(const GString& entity_name, const GString& entity_uuid) {
    function<Entity*()> creator = get_entity_creator("Client" + entity_name);
    if (!creator) {
        ERROR_LOG("client entity type(%s) error\n", entity_name.c_str());
        return nullptr;
    }

    Entity* entity = creator();
    entity->uuid = entity_uuid;
    entity->entity_name = entity_name;

    auto old_entity = thread_safe_get_client_entity(entity->uuid);
    if (old_entity != nullptr) {
        WARN_LOG("lose client entity.%s unexcept\n", entity->uuid.c_str());
        delete old_entity;
        //g_client_entities.erase(iter);
        thread_safe_remove_client_entity(entity_uuid);
    }

    //g_client_entities.insert(make_pair(entity->uuid, entity));
    thread_safe_add_client_entity(entity);

    DEBUG_LOG("create_local_client_entity %s.%s\n", entity_name.c_str(), entity_uuid.c_str());

    return entity;
}

void destroy_local_base_entity(const GString& entity_uuid) {
    auto old_entity = thread_safe_get_base_entity(entity_uuid);
    if (old_entity == nullptr) {
        return;
    }

    old_entity->on_destroy();
    delete old_entity;
    //g_base_entities.erase(iter);
    thread_safe_remove_base_entity(entity_uuid);
}

void destroy_local_cell_entity(const GString& entity_uuid) {
    auto old_entity = thread_safe_get_cell_entity(entity_uuid);
    if (old_entity == nullptr) {
        return;
    }

    old_entity->on_destroy();
    delete old_entity;
    //g_cell_entities.erase(iter);
    thread_safe_remove_cell_entity(entity_uuid);
}

void destroy_local_client_entity(const GString& entity_uuid) {
    auto old_entity = thread_safe_get_client_entity(entity_uuid);
    if (old_entity == nullptr) {
        return;
    }

    old_entity->on_destroy();
    delete old_entity;
    //g_client_entities.erase(iter);
    thread_safe_remove_client_entity(entity_uuid);
}

RpcManagerBase* get_entity_rpc_mgr(Entity* entity) {
    return entity->get_rpc_mgr();
}

bool is_base_only(int8_t entity_type) {
    if ((EntityType)entity_type == EntityType::EntityType_Base) {
        return true;
    }
    return false;
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
