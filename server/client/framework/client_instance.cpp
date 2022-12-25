#include "engine/utils.h"

#include "client_instance.h"


GENERATE_ENTITY_OUT(ClientGameInstance);

void ClientGameInstance::on_gate_disappear(const GString& gate_addr) {
    //m_controller->on_gate_disappear(gate_addr);
}

void ClientGameInstance::set_controller(Entity* entity) {
    m_controller = entity;
}

Entity* ClientGameInstance::get_controller() {
    return m_controller;
}

void ClientGameInstance::set_should_call_create(bool should_call_create) {
    m_should_call_create = should_call_create;
}

bool ClientGameInstance::get_should_call_create() {
    return m_should_call_create;
}

ClientGameInstance* g_client_instance = nullptr;

void create_client_instance() {
    g_client_instance = (ClientGameInstance*)create_local_client_entity("GameInstance", gen_uuid());
    g_client_instance->Entity::on_create(GDict());
}

void assist_thread_start() {
}
