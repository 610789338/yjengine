#include "engine/utils.h"

#include "account.h"


GENERATE_ENTITY_OUT(BaseAccount)


void BaseAccount::on_ready() {

    INFO_LOG("BaseAccount on_ready\n");

    //test_timer = REGIST_TIMER(5, 60, true, account_timer_test, "1 minutes");
    //prop_timer = REGIST_TIMER(0, 1, true, account_timer_prop_sync);
    migrate_timer = REGIST_TIMER(0, 20, true, account_migrate_timer);
    REGIST_TIMER(0, ini_get_float("Utils", "rpc_timer_interval", 5.0), true, base_rpc_timer);

    //REGIST_EVENT("event_test", entity_event_test);
    //REGIST_TIMER(5, 0, false, account_event_timer);

#ifdef __PROP_SYNC_TEST__
    property_test();
#endif
}

void BaseAccount::entity_event_test(const GString& msg) {
    INFO_LOG("[base] entity_event_test.%s \n", msg.c_str());
}

void BaseAccount::account_event_timer() {
    SEND_EVENT("event_test", "hahahaha");
}

void BaseAccount::base_rpc_timer() {
    cell.call("msg_from_base", "hello, i am base");
    client.call("msg_from_base", "hello, i am base");
}

void BaseAccount::msg_from_cell(const GString& msg) {
    INFO_LOG("[base] msg.%s from cell\n", msg.c_str()); 
}

void BaseAccount::msg_from_client(const GString& msg) {
    INFO_LOG("[base] msg.%s from client\n", msg.c_str()); 
}

void BaseAccount::account_timer_test(const GString& arg1) {
    INFO_LOG("[Base] this is timer test arg1.%s\n", arg1.c_str());
}

void BaseAccount::account_migrate_timer() {
    GArray addrs;
    addrs.push_back(get_listen_addr());
    addrs.push_back(IPPORT_STRING(ini_get_string("MigrateAddr", "ip"), ini_get_int("MigrateAddr", "port")));

    if (cell.get_addr() == addrs[0].as_string()) {
        cell.call("begin_migrate", addrs[1].as_string());
    }
    else {
        cell.call("begin_migrate", addrs[0].as_string());
    }
}

void BaseAccount::add_migrate_int_from_client() {
    cell.call("add_migrate_int_from_base");
}

GENERATE_ENTITY_OUT(CellAccount)

void CellAccount::on_ready() {

    INFO_LOG("CellAccount on_ready\n");

    REGIST_TIMER(0, ini_get_float("Utils", "rpc_timer_interval", 5.0), true, cell_rpc_timer);

    //get_prop("test_timer")->update(REGIST_TIMER(0, 0, false, account_timer_test, "arg1"));
}

void CellAccount::msg_from_base(const GString& msg) {
    INFO_LOG("[cell] msg.%s from base\n", msg.c_str());
}

void CellAccount::msg_from_client(const GString& msg) {
    INFO_LOG("[cell] msg.%s from client\n", msg.c_str());
}

void CellAccount::add_migrate_int_from_base() {
    auto old_migrate_int = get_prop("migrate_int")->as_int32();
    get_prop("migrate_int")->update(old_migrate_int + 1);

    INFO_LOG("add_migrate_int_from_base %d\n", get_prop("migrate_int")->as_int32());
}

void CellAccount::add_migrate_int_from_client() {
    auto old_migrate_int = get_prop("migrate_int")->as_int32();
    get_prop("migrate_int")->update(old_migrate_int + 1);

    INFO_LOG("add_migrate_int_from_client %d\n", get_prop("migrate_int")->as_int32());
}

void CellAccount::account_timer_test(const GString& arg1) {
    INFO_LOG("[Cell] @@@@@@ this is timer test arg1.%s next_timer_id.%d\n", arg1.c_str(), get_prop("test_timer")->as_int32());

    get_prop("test_timer")->update(REGIST_TIMER(3, 0, false, account_timer_test, "arg1"));
}

void CellAccount::cell_rpc_timer() {
    base.call("msg_from_cell", "hello, i am cell");
    client.call("msg_from_cell", "hello, i am cell");
}
