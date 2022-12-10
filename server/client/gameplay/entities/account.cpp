#include "account.h"


GENERATE_ENTITY_OUT(ClientAccount)

void ClientAccount::msg_from_base(const GString& msg) {
    INFO_LOG("[client] msg.%s from base\n", msg.c_str()); 
}

void ClientAccount::msg_from_cell(const GString& msg) {
    INFO_LOG("[client] msg.%s from cell\n", msg.c_str()); 
}

void ClientAccount::prop_sync_compare(const GBin& v) {
    
    Encoder encoder;
    serialize_client(encoder, true);
    encoder.write_end();

    auto server_len = v.size;
    auto client_len = encoder.get_offset();
    ASSERT_LOG(server_len == client_len, "server_len.%d != client_len.%d\n", server_len, client_len);

    if (memcmp(v.buf, encoder.get_buf(), server_len) != 0) {
        byte_print(v.buf, server_len);
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

    //migrate_timer = REGIST_TIMER(0, 0.1, true, account_migrate_timer);
    //migrate_print_timer = REGIST_TIMER(0, 1, true, account_migrate_print_timer);
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
