#include <vector>

#include "entity.h"
#include "log.h"
#include "rpc_manager.h"
#include "entity_component_manager.h"
#include "entity_property_manager.h"


void Entity::tick() {
    if (!is_ready) {
        return;
    }

    propertys_sync2client();
    timer_tick();  // 处理当前帧创建的立即触发的timer
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
        _timer_ids.push_back(iter->first);
    }

    for (auto iter = _timer_ids.begin(); iter != _timer_ids.end(); ++iter) {
        CANCELL_TIMER(*iter);
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
    is_ready = true;
    propertys_sync2client(true);

    if (need_create_save_timer()) {
        create_dbsave_timer();
    }
    on_ready();
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
            get_timer_manager()->timer_callback(timer);
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
    ready();
}

void BaseEntityWithCell::on_create(const GDict& create_data) {
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
    client.set_entity_and_addr(uuid, create_data.at("client_addr").as_string());
    client.set_gate_addr(create_data.at("gate_addr").as_string());
    create_cell(create_data);
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
    Entity::ready();
    cell.call("ready");
}

void BaseEntityWithCellAndClient::on_reconnect_fromclient(const GString& client_addr, const GString& gate_addr) {
    // kick old client
    kick_client();

    // recover client mailbox
    client.set_entity_and_addr(client.get_entity_uuid(), client_addr);
    client.set_gate_addr(gate_addr);
    cell.call("on_reconnect_fromclient", client_addr, gate_addr);
}

void BaseEntityWithCellAndClient::on_client_reconnect_success() {
    propertys_sync2client(true);
    cell.call("on_client_reconnect_success");
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
}

void BaseEntityWithCellAndClient::new_cell_migrate_in(const GString& new_cell_addr) {
    BaseEntityWithCell::new_cell_migrate_in(new_cell_addr);
}

void CellEntity::begin_migrate(const GString& new_addr) {
    if (new_addr == get_listen_addr()) {
        // new cell addr == old cell addr
        WARN_LOG("ignore migrate to local game\n");
        return;
    }

    if (is_migrating) {
        return;
    }

    if (!is_ready) {
        return;
    }

    is_migrating = true;

    new_cell_addr = new_addr;
    base.call("migrate_req_from_cell"); 
}

void CellEntity::migrate_reqack_from_base(bool is_ok) { 
    is_reqack_from_base = true; 
    real_begin_migrate(); 
}

void CellEntity::real_begin_migrate() { 
    if (!is_reqack_from_base) {
        return;
    }

    migrate_entity();
    is_reqack_from_base = false;
}

void CellEntity::migrate_entity() {
    Encoder encoder;
    serialize_all(encoder);
    encoder.write_end();

    GDict create_data;
    on_migrate_out(create_data);

    auto session = g_session_mgr.get_rand_session();
    REMOTE_RPC_CALL(session, "entity_property_migrate_from_oldcell",
        /*new cell addr*/new_cell_addr,
        /*old cell addr*/get_listen_addr(),
        /*entity class name*/class_name,
        /*create data*/create_data,
        /*entity property*/GBin(encoder.get_buf(), encoder.get_offset()));
}

void CellEntity::on_migrate_out(GDict& create_data) {
    ASSERT(false);
}

void CellEntity::on_migrate_in(const GDict& create_data) {
    ASSERT(false);
}

void CellEntity::on_new_cell_migrate_finish() {
    destroy_self();
}

void CellEntity::destroy_self() {
    destroy_local_cell_entity(uuid);
}

void CellEntityWithClient::on_create(const GDict& create_data) {
    base.set_entity_and_addr(uuid, create_data.at("base_addr").as_string());
    client.set_entity_and_addr(uuid, create_data.at("client_addr").as_string());
    client.set_gate_addr(create_data.at("gate_addr").as_string());

    base.call("on_cell_create", server->get_listen_addr());
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
        WARN_LOG("ignore migrate to local game\n");
        return;
    }

    if (is_migrating) {
        return;
    }

    if (!is_ready) {
        return;
    }

    is_migrating = true;

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
    if (!is_reqack_from_base || !is_reqack_from_client) {
        return;
    }

    migrate_entity();
    is_reqack_from_base = false;
    is_reqack_from_client = false;
}

void CellEntityWithClient::on_migrate_out(GDict& create_data) {
    create_data.insert(make_pair("entity_uuid", uuid));
    create_data.insert(make_pair("base_addr", base.get_addr()));
    create_data.insert(make_pair("client_addr", client.get_addr()));
    create_data.insert(make_pair("gate_addr", client.get_gate_addr()));

    //INFO_LOG("entity.%s on_migrate_out\n", uuid.c_str());s

    GDict _timers;
    for (auto iter = timers.begin(); iter != timers.end(); ++iter) {
        TimerBase* timer = *iter;
        Encoder encoder;
        timer->serialize(encoder);
        encoder.write_end();
        _timers.insert(make_pair(timer->m_cb_name, GBin(encoder.get_buf(), encoder.get_offset()) ));
    }
    create_data.insert(make_pair("timers", _timers));
    create_data.insert(make_pair("next_timer_id", next_timer_id));
    release_timer();
}

void CellEntityWithClient::on_migrate_in(const GDict& create_data) {
    base.set_entity_and_addr(uuid, create_data.at("base_addr").as_string());
    client.set_entity_and_addr(uuid, create_data.at("client_addr").as_string());
    client.set_gate_addr(create_data.at("gate_addr").as_string());

    //INFO_LOG("entity.%s on_migrate_in\n", uuid.c_str());

    auto _timers = create_data.at("timers").as_dict();
    for (auto iter = _timers.begin(); iter != _timers.end(); ++iter) {
        RESTORE_TIMER(iter->first, iter->second.as_bin());
    }
    next_timer_id = create_data.at("next_timer_id").as_int32();

    base.call("new_cell_migrate_in", get_listen_addr());
    client.call("new_cell_migrate_in", get_listen_addr());

    is_ready = true;
}

void CellEntityWithClient::on_new_cell_migrate_finish() {
    destroy_self();
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


void ClientEntity::on_create(const GDict& create_data) {
    base.set_entity_and_addr(uuid, create_data.at("base_addr").as_string());
    cell.set_entity_and_addr(uuid, create_data.at("cell_addr").as_string());

    base.call("ready");
}

void ClientEntity::on_reconnect_success(const GDict& create_data) {
    base.set_entity_and_addr(uuid, create_data.at("base_addr").as_string());
    cell.set_entity_and_addr(uuid, create_data.at("cell_addr").as_string());

    base.call("on_client_reconnect_success");
}

void ClientEntity::prop_sync_from_base(const GBin& v) {
    Decoder decoder(v.buf, v.size);
    decoder.read_int16(); // skip pkg len offset
    propertys_unserialize(decoder);
    on_prop_sync_from_server();
}

void ClientEntity::prop_sync_from_cell(const GBin& v) {
    Decoder decoder(v.buf, v.size);
    decoder.read_int16(); // skip pkg len offset
    propertys_unserialize(decoder);
    on_prop_sync_from_server();
}

void ClientEntity::on_kick() {
    INFO_LOG("on kick\n");
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

Entity* create_local_base_entity(const GString& entity_class_name, const GString& entity_uuid) {\
    function<Entity*()> creator = get_entity_creator("Base" + entity_class_name);
    if (!creator) {
        ERROR_LOG("base entity type(%s) error\n", entity_class_name.c_str());
        return nullptr;
    }

    Entity* entity = creator();
    entity->uuid = entity_uuid;
    entity->class_name = entity_class_name;
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
    for (auto iter = g_base_entities.begin(); iter != g_base_entities.end(); ++iter) {
        iter->second->tick();
    }

    for (auto iter = g_cell_entities.begin(); iter != g_cell_entities.end(); ++iter) {
        iter->second->tick();
    }

    for (auto iter = g_client_entities.begin(); iter != g_client_entities.end(); ++iter) {
        iter->second->tick();
    }
}
