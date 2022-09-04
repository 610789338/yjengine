#include "account.h"
#include "common/prop_def/account_prop_def.h"


GENERATE_ENTITY_OUT(ClientAccount)

void ClientAccount::regist_components() {
}

void ClientAccount::rpc_method_define() {
    RPC_METHOD(RpcType::CLIENT, msg_from_base, GString());
    RPC_METHOD(RpcType::CLIENT, msg_from_cell, GString());
}

void ClientAccount::property_define() {
    account_property_define<ClientAccount>();
}

void ClientAccount::msg_from_base(const GValue& msg) { 
    INFO_LOG("[client] msg.%s from base\n", msg.as_string().c_str()); 
}

void ClientAccount::msg_from_cell(const GValue& msg) { 
    INFO_LOG("[client] msg.%s from cell\n", msg.as_string().c_str()); 
}

void ClientAccount::on_ready() {
    INFO_LOG("ClientAccount on_ready\n");

    base.call("msg_from_client", "hello, i am client");
    cell.call("msg_from_client", "hello, i am client");

    base.call("component_rpc_test", "hello, my cute component");
}
