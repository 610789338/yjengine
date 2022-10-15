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
    timer_tick();  // ����ǰ֡����������������timer
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

void Entity::serialize_all(Encoder& encoder) {
    for (auto iter = propertys.begin(); iter != propertys.end(); ++iter) {
        encoder.write_uint16(prop_str2int(iter->first));
        iter->second->serialize_all(encoder);
    }
}

void Entity::serialize2client(Encoder& encoder, bool force_all) {

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
    on_ready();
}

void Entity::timer_tick() {

    auto now = nowms_timestamp();

    if (class_name == "BaseAccount") {
        INFO_LOG("Enity::timer_tick %u\n", timers.size());
    }

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
    REMOTE_RPC_CALL(session, "create_cell_entity", cell_class_name, 
        /*base entity addr*/ uuid,
        /*base addr*/ session->get_local_addr(),
        /*gate addr*/ create_data.at("gate_addr").as_string(),
        /*client addr*/ create_data.at("client_addr").as_string());
}

void BaseEntityWithCell::on_cell_create(const GValue& cell_entity_uuid, const GValue& cell_addr) {
    cell.set_entity_and_addr(cell_entity_uuid.as_string(), cell_addr.as_string());
    ready();
}

void BaseEntityWithCell::migrate_req_from_cell() {
    cell.call("migrate_reqack_from_base", true);
    cell.start_cache_rpc();
}

void BaseEntityWithCell::new_cell_migrate_in(const GValue& new_cell_addr) {
    cell.set_entity_and_addr(cell.get_entity_uuid(), new_cell_addr.as_string());
    cell.stop_cache_rpc();
}

void BaseEntityWithClient::on_create(const GDict& create_data) {
    client.set_entity_and_addr("", create_data.at("client_addr").as_string());
    client.set_gate_addr(create_data.at("gate_addr").as_string());
}

void BaseEntityWithClient::create_client() {
    // game -> gate -> client
    auto gate = g_session_mgr.get_gate(client.m_gate_addr);
    REMOTE_RPC_CALL(gate, "create_client_entity", client.get_addr(), client_class_name,
        /*base entity uuid*/ uuid,
        /*base addr*/ gate->get_local_addr(),
        /*cell entity uuid*/ GString(),
        /*cell addr*/ GString()
    );
}

void BaseEntityWithClient::on_client_create(const GValue& client_entity_uuid) {
    client.set_entity_and_addr(client_entity_uuid.as_string(), client.get_addr());
    client.call("ready");
    ready();
}

void BaseEntityWithClient::propertys_sync2client(bool force_all) {
    
    Encoder encoder;
    serialize2client(encoder, force_all);
    encoder.write_end();

    if (encoder.anything()) {
        //byte_print(encoder.get_buf(), encoder.get_offset()); // TODO delete
        client.call("prop_sync_from_base", GBin(encoder.get_buf(), encoder.get_offset()));
    }
}

void BaseEntityWithCellAndClient::on_create(const GDict& create_data) {
    client.set_entity_and_addr("", create_data.at("client_addr").as_string());
    client.set_gate_addr(create_data.at("gate_addr").as_string());
    create_cell(create_data);
}

void BaseEntityWithCellAndClient::on_cell_create(const GValue& cell_entity_uuid, const GValue& cell_addr) {
    cell.set_entity_and_addr(cell_entity_uuid.as_string(), cell_addr.as_string());
    create_client();
}

void BaseEntityWithCellAndClient::create_client() {
    // game -> gate -> client
    auto gate = g_session_mgr.get_gate(client.m_gate_addr);
    REMOTE_RPC_CALL(gate, "create_client_entity", client.get_addr(), client_class_name,
        /*base entity uuid*/ uuid,
        /*base addr*/ gate->get_local_addr(),
        /*cell entity uuid*/ cell.get_entity_uuid(),
        /*cell addr*/ cell.get_addr()
    );
}

void BaseEntityWithCellAndClient::on_client_create(const GValue& client_entity_uuid) {
    client.set_entity_and_addr(client_entity_uuid.as_string(), client.get_addr());
    cell.call("on_client_create", client_entity_uuid.as_string());
}

void BaseEntityWithCellAndClient::new_cell_migrate_in(const GValue& new_cell_addr) {
    BaseEntityWithCell::new_cell_migrate_in(new_cell_addr);
}

void CellEntity::propertys_unserialize(Decoder& decoder) {
    while (!decoder.is_finish()) {
        auto idx = decoder.read_int16();
        const GString& prop_name = prop_int2str(idx);
        propertys.at(prop_name)->unserialize(decoder);
    }
}

void CellEntity::begin_migrate(const GValue& new_addr) {
    if (new_addr.as_string() == get_listen_addr()) {
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

    new_cell_addr = new_addr.as_string();
    base.call("migrate_req_from_cell"); 
}

void CellEntity::migrate_reqack_from_base(const GValue& is_ok) { 
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
    destroy_entity(uuid);
}

void CellEntityWithClient::on_create(const GDict& create_data) {
    base.set_entity_and_addr(create_data.at("base_entity_uuid").as_string(), create_data.at("base_addr").as_string());
    client.set_entity_and_addr("", create_data.at("client_addr").as_string()); // client entity uuid will set on client create
    client.set_gate_addr(create_data.at("gate_addr").as_string());

    base.call("on_cell_create", uuid, server->get_listen_addr());
}

void CellEntityWithClient::on_client_create(const GValue& client_entity_uuid) {
    client.set_entity_and_addr(client_entity_uuid.as_string(), client.get_addr());

    base.call("ready");
    client.call("ready");
    ready();
}

void CellEntityWithClient::propertys_sync2client(bool force_all) {

    Encoder encoder;
    serialize2client(encoder, force_all);
    encoder.write_end();

    if (encoder.anything()) {
        client.call("prop_sync_from_cell", GBin(encoder.get_buf(), encoder.get_offset()));
    }
}

void CellEntityWithClient::begin_migrate(const GValue& new_addr) {
    if (new_addr.as_string() == get_listen_addr()) {
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

    new_cell_addr = new_addr.as_string();
    base.call("migrate_req_from_cell");
    client.call("migrate_req_from_cell");
}

void CellEntityWithClient::migrate_reqack_from_base(const GValue& is_ok) {
    is_reqack_from_base = true;
    real_begin_migrate();
}

void CellEntityWithClient::migrate_reqack_from_client(const GValue& is_ok) { 
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
    create_data.insert(make_pair("base_entity_uuid", base.get_entity_uuid()));
    create_data.insert(make_pair("cell_entity_uuid", uuid));
    create_data.insert(make_pair("client_entity_uuid", client.get_entity_uuid()));
    create_data.insert(make_pair("base_addr", base.get_addr()));
    create_data.insert(make_pair("gate_addr", client.get_gate_addr()));
    create_data.insert(make_pair("client_addr", client.get_addr()));

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
    base.set_entity_and_addr(create_data.at("base_entity_uuid").as_string(), create_data.at("base_addr").as_string());
    client.set_entity_and_addr(create_data.at("client_entity_uuid").as_string(), create_data.at("client_addr").as_string());
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


void ClientEntity::on_create(const GDict& create_data) {
    base.set_entity_and_addr(create_data.at("base_entity_uuid").as_string(), create_data.at("base_addr").as_string());
    cell.set_entity_and_addr(create_data.at("cell_entity_uuid").as_string(), create_data.at("cell_addr").as_string());

    base.call("on_client_create", uuid);
}

void ClientEntity::prop_sync_from_base(const GValue& v) {
    Decoder decoder(v.as_bin().buf, v.as_bin().size);
    decoder.read_int16(); // skip pkg len offset
    propertys_unserialize(decoder);
    on_prop_sync_from_server();
}

void ClientEntity::prop_sync_from_cell(const GValue& v) {
    Decoder decoder(v.as_bin().buf, v.as_bin().size);
    decoder.read_int16(); // skip pkg len offset
    propertys_unserialize(decoder);
    on_prop_sync_from_server();
}

void ClientEntity::propertys_unserialize(Decoder& decoder) {
    while (!decoder.is_finish()) {
        auto idx = decoder.read_int16();
        const GString& prop_name = prop_int2str(idx);
        propertys.at(prop_name)->unserialize(decoder);
    }
}

void ClientEntity::migrate_req_from_cell() {
    cell.call("migrate_reqack_from_client", true);
    cell.start_cache_rpc();
}

void ClientEntity::new_cell_migrate_in(const GValue& new_cell_addr) {
    cell.set_entity_and_addr(cell.get_entity_uuid(), new_cell_addr.as_string());
    cell.stop_cache_rpc();
}

unordered_map<GString, Entity*> g_entities; // uuid -> entity
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

Entity* create_entity(const GString& entity_class_name, const GString& entity_uuid) {
    function<Entity*()> creator = get_entity_creator(entity_class_name);
    if (!creator) {
        ERROR_LOG("entity type(%s) error\n", entity_class_name.c_str());
        return nullptr;
    }

    Entity* entity = creator();
    entity->uuid = entity_uuid;
    entity->class_name = entity_class_name;
    g_entities.insert(make_pair(entity->uuid, entity));

    DEBUG_LOG("create_entity %s.%s\n", entity_class_name.c_str(), entity_uuid.c_str());

    return entity;
}

void destroy_entity(const GString& entity_uuid) {
    auto iter = g_entities.find(entity_uuid);
    if (iter == g_entities.end()) {
        return;
    }

    const auto& entity = iter->second;
    entity->on_destroy();
    delete entity;
    g_entities.erase(iter);
}

RpcManagerBase* get_entity_rpc_mgr(Entity* entity) {
    return entity->get_rpc_mgr();
}

void entity_tick() {
    for (auto iter = g_entities.begin(); iter != g_entities.end(); ++iter) {
        iter->second->tick();
    }
}
