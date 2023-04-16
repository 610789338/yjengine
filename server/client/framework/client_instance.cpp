#include "engine/utils.h"
#include "engine/log.h"

#include "client_instance.h"

extern void connect_gate();

GENERATE_ENTITY_OUT(ClientGameInstance);

void ClientGameInstance::on_gate_disappear(const GString& gate_addr) {
    connect_gate();
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
    g_client_instance->ready();
}

void assist_thread() {
    while (true) {
        log_queue_tick();
        boost::this_thread::sleep(boost::posix_time::milliseconds(1));
    }
}

void assist_thread_start() {
    boost::thread t(assist_thread);
}
