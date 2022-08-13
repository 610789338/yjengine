#include "account.h"

#include "../components/create_avatar_component.h"


GENERATE_ENTITY_OUT(BaseAccount)

void BaseAccount::regist_components() {
    REGIST_COMPONENT(BaseAccount, CreateAvatarComponent);
}

void BaseAccount::rpc_method_define() {
    RPC_METHOD(RpcType::SERVER_ONLY, msg_from_cell, GString());
    RPC_METHOD(RpcType::EXPOSED, msg_from_client, GString());
}

void BaseAccount::property_define() {
    PROPERTY(PropType::BASE_AND_CLIENT, "id", 1001);
    PROPERTY(PropType::BASE_AND_CLIENT, "name", "youjun");
}

void BaseAccount::on_ready() {

    INFO_LOG("BaseAccount on_ready\n");

    cell.call("msg_from_base", "hello, i am base");
    client.call("msg_from_base", "hello, i am base");

    test_timer = RETIST_TIMER(0, 60, true, &BaseAccount::account_timer_test, "60 seconds");
}

void BaseAccount::msg_from_cell(const GValue& msg) { 
    INFO_LOG("[base] msg.%s from cell\n", msg.as_string().c_str()); 
}

void BaseAccount::msg_from_client(const GValue& msg) { 
    INFO_LOG("[base] msg.%s from client\n", msg.as_string().c_str()); 
}

void BaseAccount::account_timer_test(const GValue& arg1) {
    INFO_LOG("this is timer test arg1.%s\n", arg1.as_string().c_str());
}


GENERATE_ENTITY_OUT(CellAccount)

void CellAccount::regist_components() {
}

void CellAccount::rpc_method_define() {
    RPC_METHOD(RpcType::SERVER_ONLY, msg_from_base, GString());
    RPC_METHOD(RpcType::EXPOSED, msg_from_client, GString());
}

void CellAccount::property_define() {
}

void CellAccount::msg_from_base(const GValue& msg) {
    INFO_LOG("[cell] msg.%s from base\n", msg.as_string().c_str()); 
}

void CellAccount::msg_from_client(const GValue& msg) {
    INFO_LOG("[cell] msg.%s from client\n", msg.as_string().c_str()); 
}

void CellAccount::on_ready() {

    INFO_LOG("CellAccount on_ready\n");

    base.call("msg_from_cell", "hello, i am cell");
    client.call("msg_from_cell", "hello, i am cell");
}
