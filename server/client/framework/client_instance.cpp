#include "engine/utils.h"
#include "engine/log.h"

#include "client_instance.h"

extern void connect_gate();

GENERATE_ENTITY_OUT(ClientInstance);

void ClientInstance::on_gate_disappear(const GString& gate_addr) {
    connect_gate();
}

void ClientInstance::set_should_call_create(bool should_call_create) {
    m_should_call_create = should_call_create;
}

bool ClientInstance::get_should_call_create() {
    return m_should_call_create;
}

ClientInstance* g_client_instance = nullptr;

void create_client_instance() {
    g_client_instance = (ClientInstance*)create_local_base_entity("ClientInstance", gen_uuid());
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
