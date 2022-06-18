#include "entity.h"


void BaseEntityWithCell::on_create(const GDict& create_data) {
    create_cell(create_data);
}

void BaseEntityWithCell::on_destroy() {

}

void BaseEntityWithCell::create_cell(const GDict& create_data) {
    // game -> gate -> game
    auto const& session = g_session_mgr.get_rand_session();
    REMOTE_RPC_CALL(session, "create_cell_entity", cell_class_name, uuid, session->get_local_addr(), create_data.at("gate_addr").as_string(), create_data.at("client_addr").as_string());
}

void BaseEntityWithCell::on_cell_create(const GString& cell_entity_uuid, const GString& cell_addr) {
    cell.set_entity_and_addr(cell_entity_uuid, cell_addr);
}

void BaseEntityWithClient::on_create(const GDict& create_data) {
    client.set_entity_and_addr("", create_data.at("client_addr").as_string());
    client.set_gate_addr(create_data.at("gate_addr").as_string());
}

void BaseEntityWithClient::on_destroy() {

}

void BaseEntityWithClient::on_client_create() {

}

void BaseEntityWithClient::create_client() {
    // game -> gate -> client
}

void BaseEntityWithCellAndClient::on_create(const GDict& create_data) {
    this->BaseEntityWithCell::on_create(create_data);
    this->BaseEntityWithClient::on_create(create_data);
}

void BaseEntityWithCellAndClient::on_destroy() {

}

void BaseEntityWithCellAndClient::on_cell_create(const GString& cell_entity_uuid, const GString& cell_addr) {
    this->BaseEntityWithCell::on_cell_create(cell_entity_uuid, cell_addr);
}

void BaseEntityWithCellAndClient::on_client_create() {

}
void CellEntityWithClient::on_create(const GDict& create_data) {
    base.set_entity_and_addr(create_data.at("base_entity_uuid").as_string(), create_data.at("base_addr").as_string());
    client.set_entity_and_addr("", create_data.at("client_addr").as_string());
    client.set_gate_addr(create_data.at("gate_addr").as_string());

    base.call("on_cell_create", uuid, server->get_listen_addr());
}

void CellEntityWithClient::on_destroy() {

}

unordered_map<GString, Entity*> g_entities;
unordered_map<GString, function<Entity*()>> g_entity_creator;

void regist_entity_creator(const GString& entity_class_name, const function<Entity*()>& creator) {
    g_entity_creator.insert(make_pair(entity_class_name, creator));
}

function<Entity*()> get_entity_creator(const GString& entity_class_name) {
    auto iter = g_entity_creator.find(entity_class_name);
    if (iter == g_entity_creator.end()) {
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

    return entity;
}
