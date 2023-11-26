#include "engine/engine.h"

#include "avatar.h"


GENERATE_ENTITY_OUT(BaseAvatar)


//Entity* entity = nullptr;
//Entity* get_entity() {
//    return entity;
//}
//
//void pass_entity(Entity* entity) {
//    INFO_LOG("entity uuid %s\n", entity->uuid.c_str());
//}
//
//static int average(int32_t i1, int32_t i2) {
//    return (i1 + i2) / 2;
//}

void BaseAvatar::on_ready() {

    INFO_LOG("BaseAvatar on_ready\n");

    //test_timer = REGIST_TIMER(5, 60, true, avatar_timer_test, "1 minutes");
    //migrate_timer = REGIST_TIMER(0, 20, true, avatar_migrate_timer);
    //REGIST_TIMER(0, ini_get_float("Utils", "rpc_timer_interval", 5.0), true, base_rpc_timer);

    //YJ_GMETHOD(average);
    //YJ_GMETHOD(get_entity);
    //YJ_GMETHOD(pass_entity);

    //REGIST_TIMER(0, 1, true, lua_test_timer);
    CALL_LUA("on_ready");
}

void BaseAvatar::lua_test_timer() {
    //entity = this;
    //YjLua::do_str("print(string.format(\"average is %s\", average(7, 5)))");
    //YjLua::do_str("local entity = get_entity(); pass_entity(entity)");
}

void BaseAvatar::base_rpc_timer() {
    cell.call("msg_from_base", "hello, i am base");
    client.call("msg_from_base", "hello, i am base");
}

void BaseAvatar::msg_from_cell(const GString& msg) {
    INFO_LOG("[base] msg.%s from cell\n", msg.c_str()); 
    CALL_LUA("msg_from_cell", msg);
}

void BaseAvatar::msg_from_client(const GString& msg) {
    INFO_LOG("[base] msg.%s from client\n", msg.c_str());
}

void BaseAvatar::byte_swap_test(const GString& msg, int16_t arg1, int32_t arg2, int64_t arg3, float arg4, uint16_t arg5, uint32_t arg6, uint64_t arg7, double arg8) {
    INFO_LOG("[base] msg.%s from client arg1.%d arg2.%d arg3.%lld arg4.%f arg5.%u arg6.%u arg7.%llu arg8.%llf\n", 
        msg.c_str(), arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
}

void BaseAvatar::avatar_timer_test(const GString& arg1) {
    INFO_LOG("[Base] this is timer test arg1.%s\n", arg1.c_str());
}

void BaseAvatar::avatar_migrate_timer() {
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

void BaseAvatar::add_migrate_int_from_client() {
    cell.call("add_migrate_int_from_base");
}

int BaseAvatar::yj_method_test(int32_t i1, int32_t i2) {
    return (i1 + i2) / 2;
}

GENERATE_ENTITY_OUT(CellAvatar)

void CellAvatar::on_ready() {

    INFO_LOG("CellAvatar on_ready\n");

    REGIST_TIMER(0, ini_get_float("Utils", "rpc_timer_interval", 5.0), true, cell_rpc_timer);

    //get_prop("test_timer")->update(REGIST_TIMER(0, 0, false, avatar_timer_test, "arg1"));

    REGIST_EVENT(begin_migrate);
}

void CellAvatar::msg_from_base(const GString& msg) {
    INFO_LOG("[cell] msg.%s from base\n", msg.c_str());
}

void CellAvatar::msg_from_client(const GString& msg) {
    INFO_LOG("[cell] msg.%s from client\n", msg.c_str());
}

void CellAvatar::add_migrate_int_from_base() {
    auto old_migrate_int = get_prop("migrate_int")->as_int32();
    get_prop("migrate_int")->update(old_migrate_int + 1);

    INFO_LOG("add_migrate_int_from_base %d\n", get_prop("migrate_int")->as_int32());
}

void CellAvatar::add_migrate_int_from_client() {
    auto old_migrate_int = get_prop("migrate_int")->as_int32();
    get_prop("migrate_int")->update(old_migrate_int + 1);

    INFO_LOG("add_migrate_int_from_client %d\n", get_prop("migrate_int")->as_int32());
}

void CellAvatar::begin_migrate(const GString& new_addr) {
    CellEntityWithClient::begin_migrate(new_addr);
}

void CellAvatar::avatar_timer_test(const GString& arg1) {
    INFO_LOG("[Cell] @@@@@@ this is timer test arg1.%s next_timer_id.%d\n", arg1.c_str(), get_prop("test_timer")->as_int32());

    get_prop("test_timer")->update(REGIST_TIMER(3, 0, false, avatar_timer_test, "arg1"));
}

void CellAvatar::cell_rpc_timer() {
    base.call("msg_from_cell", "hello, i am cell");
    client.call("msg_from_cell", "hello, i am cell");
}
