#include "../yjobj.h"

#include "obj_bind.h"
#include "yjlua.h"

int8_t lua_push_args(lua_State* l) {
    return 0;
}

extern int32_t error_handler(lua_State *l);
bool pre_call_lua(YjObj* obj, GString method_name) {
    ClassDescBase* class_desc = obj->get_class_desc();

    auto l = YjLua::get_state();
    lua_pushcfunction(l, error_handler);
    lua_rawgeti(l, LUA_REGISTRYINDEX, class_desc->class_ref);
    lua_getfield(l, -1, method_name.c_str());
    if (lua_type(l, -1) != LUA_TFUNCTION) {
        ERROR_LOG("%s has no lua funciont named %s\n", obj->get_name().c_str(), method_name.c_str());
        return false;
    }

    lua_rawgeti(l, LUA_REGISTRYINDEX, obj->obj_ref);

    return true;
}

void real_call_lua(int8_t arg_nums) {
    auto l = YjLua::get_state();
    lua_pcall(l, 1 + arg_nums, LUA_MULTRET, -arg_nums - 4);
}

extern int32_t class_index(lua_State *l);
void notify_yjobj_create(YjObj* obj) {
    auto l = YjLua::get_state();

    lua_newtable(l); // lua_obj
    lua_pushstring(l, "Object");
    lua_pushlightuserdata(l, obj);
    lua_rawset(l, -3); // lua_obj.Object = obj

    lua_newtable(l); // metatable
    lua_pushstring(l, "__index");
    lua_pushcfunction(l, class_index);
    lua_rawset(l, -3);

    lua_setmetatable(l, -2); // lua_obj.metatable = {__index = class_index}

    obj->obj_ref = luaL_ref(l, LUA_REGISTRYINDEX);
}
