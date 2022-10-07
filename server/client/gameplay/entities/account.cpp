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

    if (memcmp(v.as_bin().buf, encoder.get_buf(), server_len) != 0) {
        byte_print(v.as_bin().buf, server_len);
        byte_print(encoder.get_buf(), client_len);
        ASSERT_LOG(false, "server buf != client buf\n");
    }

    INFO_LOG("prop syc compare %d.%d\n", server_len, client_len);
}

void ClientAccount::on_ready() {
    INFO_LOG("ClientAccount on_ready\n");

    base.call("msg_from_client", "hello, i am client");
    cell.call("msg_from_client", "hello, i am client");

    base.call("component_rpc_test", "hello, my cute component");

    //migrate_timer = REGIST_TIMER(0, 0.1, true, ClientAccount::account_migrate_timer);
    //migrate_print_timer = REGIST_TIMER(0, 1, true, ClientAccount::account_migrate_print_timer);
}

void ClientAccount::on_prop_sync_from_server() {
    avatar_datas_print(get_prop("avatar_datas"));
}

void ClientAccount::account_migrate_timer() {
    static int32_t max_call = 1000;
    if (max_call <= 0) {
        return;
    }

    base.call("add_migrate_int_from_client");
    cell.call("add_migrate_int_from_client");
    --max_call;
}

void ClientAccount::account_migrate_print_timer() {
    INFO_LOG("account_migrate_print_timer %d\n", get_prop("migrate_int")->as_int32());
}
