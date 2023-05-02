#include "engine/utils.h"

#include "monster.h"


GENERATE_ENTITY_OUT(BaseMonster)


void BaseMonster::on_ready() {

    INFO_LOG("BaseMonster on_ready\n");

    migrate_timer = REGIST_TIMER(0, 20, true, monster_migrate_timer);
    REGIST_TIMER(0, ini_get_float("Utils", "rpc_timer_interval", 5.0), true, base_rpc_timer);
}

void BaseMonster::base_rpc_timer() {
    cell.call("msg_from_base", "hello, i am base");
}

void BaseMonster::msg_from_cell(const GString& msg) {
    INFO_LOG("[base] msg.%s from cell\n", msg.c_str()); 
}

void BaseMonster::monster_migrate_timer() {
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

GENERATE_ENTITY_OUT(CellMonster)

void CellMonster::on_ready() {

    INFO_LOG("CellMonster on_ready\n");

    REGIST_TIMER(0, ini_get_float("Utils", "rpc_timer_interval", 5.0), true, cell_rpc_timer);
}

void CellMonster::msg_from_base(const GString& msg) {
    INFO_LOG("[cell] msg.%s from base\n", msg.c_str());
}

void CellMonster::cell_rpc_timer() {
    base.call("msg_from_cell", "hello, i am cell");
}
