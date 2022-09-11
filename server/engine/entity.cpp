#include <vector>

#include "entity.h"
#include "entity_component_manager.h"


void Entity::tick() {
    propertys_sync2client();
    on_tick();
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

void Entity::propertys_sync2client(bool force_all) {
    // do nothing
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

void BaseEntity::on_create(const GDict& create_data) {
    on_ready();
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
    on_ready();
}

void BaseEntityWithClient::on_create(const GDict& create_data) {
    client.set_entity_and_addr("", create_data.at("client_addr").as_string());
    client.set_gate_addr(create_data.at("gate_addr").as_string());
}

void BaseEntityWithClient::create_client() {
    // game -> gate -> client
    auto gate = g_session_mgr.get_session(client.m_gate_addr);
    REMOTE_RPC_CALL(gate, "create_client_entity", client.get_addr(), client_class_name,
        /*base entity uuid*/ uuid,
        /*base addr*/ gate->get_local_addr(),
        /*cell entity uuid*/ GString(),
        /*cell addr*/ GString()
    );
}

void BaseEntityWithClient::on_client_create(const GValue& client_entity_uuid) {
    client.set_entity_and_addr(client_entity_uuid.as_string(), client.get_addr());
    on_ready();
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
    auto gate = g_session_mgr.get_session(client.m_gate_addr);
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

void CellEntityWithClient::on_create(const GDict& create_data) {
    base.set_entity_and_addr(create_data.at("base_entity_uuid").as_string(), create_data.at("base_addr").as_string());
    client.set_entity_and_addr("", create_data.at("client_addr").as_string());
    client.set_gate_addr(create_data.at("gate_addr").as_string());

    base.call("on_cell_create", uuid, server->get_listen_addr());
}

void CellEntityWithClient::on_client_create(const GValue& client_entity_uuid) {
    client.set_entity_and_addr(client_entity_uuid.as_string(), client.get_addr());

    base.call("on_ready");
    client.call("on_ready");
    on_ready();
}

void CellEntityWithClient::propertys_sync2client(bool force_all) {

    Encoder encoder;
    serialize2client(encoder, force_all);
    encoder.write_end();

    if (encoder.anything()) {
        client.call("prop_sync_from_cell", GBin(encoder.get_buf(), encoder.get_offset()));
    }
}

void ClientEntity::on_create(const GDict& create_data) {
    base.set_entity_and_addr(create_data.at("base_entity_uuid").as_string(), create_data.at("base_addr").as_string());
    cell.set_entity_and_addr(create_data.at("cell_entity_uuid").as_string(), create_data.at("cell_addr").as_string());

    base.call("on_client_create", uuid);
}

//extern void prop_read_for_test(EntityPropertyBase*);

void ClientEntity::prop_sync_from_base(const GValue& v) {
    Decoder decoder(v.as_bin().buf, v.as_bin().size);
    //byte_print(decoder.get_buf(), decoder.get_max_offset()); // TODO delete
    decoder.read_int16(); // skip pkg len offset
    unserialize_from_server(decoder);

    //prop_read_for_test(get_prop("avatar_datas"));
}

void ClientEntity::prop_sync_from_cell(const GValue& v) {
    Decoder decoder(v.as_bin().buf, v.as_bin().size);
    decoder.read_int16(); // skip pkg len offset
    unserialize_from_server(decoder);
}

void ClientEntity::unserialize_from_server(Decoder& decoder) {
    while (!decoder.is_finish()) {
        auto idx = decoder.read_int16();
        const GString& prop_name = prop_int2str(idx);
        propertys.at(prop_name)->unserialize(decoder);
    }
}

unordered_map<GString, Entity*> g_entities; // uuid -> entity
typedef unordered_map<GString, function<Entity*()>> CreatorMap;
CreatorMap* g_entity_creator = nullptr;

void regist_entity_creator(const GString& entity_class_name, const function<Entity*()>& creator) {
    if (g_entity_creator == nullptr)
        //g_entity_creator = make_shared<CreatorMap>();
        g_entity_creator = new CreatorMap;

    g_entity_creator->insert(make_pair(entity_class_name, creator));
}

function<Entity*()> get_entity_creator(const GString& entity_class_name) {
    auto iter = g_entity_creator->find(entity_class_name);
    if (iter == g_entity_creator->end()) {
        return nullptr;
    }

    return iter->second;
}

Entity* create_entity(const GString& entity_class_name, const GString& entity_uuid, const GDict& create_data) {
    function<Entity*()> creator = get_entity_creator(entity_class_name);
    if (!creator) {
        ERROR_LOG("entity type(%s) error\n", entity_class_name.c_str());
        return nullptr;
    }

    Entity* entity = creator();
    entity->uuid = entity_uuid;
    entity->class_name = entity_class_name;
    entity->on_create(create_data);
    g_entities.insert(make_pair(entity->uuid, entity));

    INFO_LOG("create_entity %s.%s\n", entity_class_name.c_str(), entity_uuid.c_str());

    return entity;
}

RpcManagerBase* get_entity_rpc_mgr(Entity* entity) {
    return entity->get_rpc_mgr();
}

void entity_tick() {
    for (auto iter = g_entities.begin(); iter != g_entities.end(); ++iter) {
        iter->second->tick();
    }
}
