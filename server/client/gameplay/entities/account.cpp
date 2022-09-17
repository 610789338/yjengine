#include "account.h"
#include "common/prop_def/account_prop_def.h"


GENERATE_ENTITY_OUT(ClientAccount)

void ClientAccount::regist_components() {
}

void ClientAccount::rpc_method_define() {
    RPC_METHOD(RpcType::CLIENT, msg_from_base, GString());
    RPC_METHOD(RpcType::CLIENT, msg_from_cell, GString());
    RPC_METHOD(RpcType::CLIENT, prop_sync_compare, GBin());
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

void ClientAccount::prop_sync_compare(const GValue& v) {
    
    Encoder encoder;
    serialize2client(encoder, true);
    encoder.write_end();

    auto server_len = v.as_bin().size;
    auto client_len = encoder.get_offset();
    ASSERT_LOG(server_len == client_len, "server_len.%d != client_len.%d\n", server_len, client_len);

    ASSERT_LOG(memcmp(v.as_bin().buf, encoder.get_buf(), v.as_bin().size) == 0, "server buf != client buf\n");

    INFO_LOG("prop syc compare %d.%d\n", server_len, client_len);
}

void ClientAccount::on_ready() {
    INFO_LOG("ClientAccount on_ready\n");

    base.call("msg_from_client", "hello, i am client");
    cell.call("msg_from_client", "hello, i am client");

    base.call("component_rpc_test", "hello, my cute component");
}

void ClientAccount::on_prop_sync_from_server() {
    //avatar_data_print(get_prop("avatar_datas"));
}
