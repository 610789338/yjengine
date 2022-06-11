#include "entity.h"


void BaseEntityWithCell::on_create(const GDict& create_data) {
    create_cell();
}

void BaseEntityWithCell::on_destroy() {

}

void BaseEntityWithCell::create_cell() {
    // game -> gate -> game
}

void BaseEntityWithCell::on_cell_create() {

}

void BaseEntityWithClient::on_create(const GDict& create_data) {
    create_client();
}

void BaseEntityWithClient::on_destroy() {

}

void BaseEntityWithClient::on_client_create() {

}

void BaseEntityWithClient::create_client() {
    // game -> gate -> client
}

void BaseEntityWithCellAndClient::on_create(const GDict& create_data) {
    create_cell();
}

void BaseEntityWithCellAndClient::on_destroy() {

}

void BaseEntityWithCellAndClient::on_cell_create() {
    create_client();
}

void BaseEntityWithCellAndClient::on_client_create() {

}

unordered_map<GString, Entity*> g_entities;
unordered_map<GString, function<Entity*()>> g_entity_creator;

void regist_entity_creator(const GString& entity, const function<Entity*()>& creator) {
    g_entity_creator.insert(make_pair(entity, creator));
}

function<Entity*()> get_entity_creator(const GString& entity_type) {
    auto iter = g_entity_creator.find(entity_type);
    if (iter == g_entity_creator.end()) {
        return nullptr;
    }

    return iter->second;
}

Entity* create_entity(const GString& entity_type, const GDict& create_data) {
    function<Entity*()> creator = get_entity_creator(entity_type);
    if (!creator) {
        ERROR_LOG("entity type(%s) error\n", entity_type.c_str());
        return nullptr;
    }

    Entity* entity = creator();
    entity->id = gen_uuid();
    entity->on_create(create_data);
    g_entities.insert(make_pair(entity->id, entity));

    return entity;
}
