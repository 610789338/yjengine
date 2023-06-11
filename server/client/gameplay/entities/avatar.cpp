#include "avatar.h"


GENERATE_ENTITY_OUT(ClientAvatar)

void ClientAvatar::on_ready() {
    INFO_LOG("ClientAvatar on_ready\n");

    //migrate_timer = REGIST_TIMER(0, 0.1, true, avatar_migrate_timer);
    //migrate_print_timer = REGIST_TIMER(0, 1, true, avatar_migrate_print_timer);

    REGIST_TIMER(0, ini_get_float("Utils", "rpc_timer_interval", 5.0), true, client_rpc_timer);
}

void ClientAvatar::msg_from_base(const GString& msg) {
    INFO_LOG("[client] msg.%s from base\n", msg.c_str()); 
}

void ClientAvatar::msg_from_cell(const GString& msg) {
    INFO_LOG("[client] msg.%s from cell\n", msg.c_str()); 
}

void ClientAvatar::prop_sync_compare(const GBin& v) {

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

    INFO_LOG("prop sync compare %d.%d\n", server_len, client_len);
}

void ClientAvatar::on_prop_sync_from_server() {
#ifndef __PROP_SYNC_TEST__
    avatar_datas_print(get_prop("avatar_datas"));
#endif
}

void ClientAvatar::avatar_migrate_timer() {
    static int32_t max_call = 1000;
    if (max_call <= 0) {
        return;
    }

    base.call("add_migrate_int_from_client");
    cell.call("add_migrate_int_from_client");
    --max_call;
}

void ClientAvatar::avatar_migrate_print_timer() {
    INFO_LOG("avatar_migrate_print_timer %d\n", get_prop("migrate_int")->as_int32());
}

void ClientAvatar::client_rpc_timer() {
    base.call("msg_from_client", "hello, i am client");
    cell.call("msg_from_client", "hello, i am client");
    //base.call("byte_swap_test", "hello, i am client", int16_t(-1), int32_t(-1), int64_t(-1), float(888.888), uint16_t(1), uint32_t(1), uint64_t(1), double(987654321.987654321987654321));
}
