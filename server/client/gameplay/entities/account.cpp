#include "account.h"


GENERATE_ENTITY_OUT(ClientAccount)

void ClientAccount::on_ready() {
    INFO_LOG("ClientAccount on_ready\n");

    base.call("component_rpc_test", "hello, my cute component");

    //REGIST_TIMER(0, ini_get_float("Utils", "rpc_timer_interval", 5.0), true, client_rpc_timer);

    base.call("create_avatar");
}

void ClientAccount::msg_from_base(const GString& msg) {
    INFO_LOG("[client] msg.%s from base\n", msg.c_str()); 
}

void ClientAccount::client_rpc_timer() {
    base.call("msg_from_client", "hello, i am client");
}
