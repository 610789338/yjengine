#include "engine/utils.h"

#include "account.h"

extern void create_entity(const GString& entity_name, const GString& client_addr, const GString& gate_addr, const GString& entity_uuid, const GDict& init_data);

GENERATE_ENTITY_OUT(BaseAccount)

void BaseAccount::on_ready() {
    INFO_LOG("BaseAccount on_ready\n");

    //REGIST_TIMER(0, ini_get_float("Utils", "rpc_timer_interval", 5.0), true, base_rpc_timer);

    //REGIST_TIMER(0, 2, true, account_event_timer);
    REGIST_EVENT(event_test);
}

void BaseAccount::event_test(const GString& msg) {
    INFO_LOG("[base] event_test.%s \n", msg.c_str());
}

void BaseAccount::account_event_timer() {
    send_event("event_test", "hahahaha");
}

void BaseAccount::base_rpc_timer() {
    client.call("msg_from_base", "hello, i am base");
}

void BaseAccount::create_avatar() {
    create_entity("Avatar", client.get_addr(), client.get_gate_addr(), "", GDict());
}

void BaseAccount::create_monster() {
    create_entity("Monster", "", "", "", GDict());
}

void BaseAccount::msg_from_client(const GString& msg) {
    //INFO_LOG("[base] msg.%s from client\n", msg.c_str()); 
}   
