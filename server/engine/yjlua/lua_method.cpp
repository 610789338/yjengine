#include "lua_method.h"
#include "yjlua.h"

template<> bool lua_arg_read<bool>(LuaMethodBase* lua_method) {
    return (bool)lua_tointeger(lua_method->l, lua_method->lua_arg_idx++);
}

template<> int8_t lua_arg_read<int8_t>(LuaMethodBase* lua_method) {
    return (int8_t)lua_tointeger(lua_method->l, lua_method->lua_arg_idx++);
}

template<> int16_t lua_arg_read<int16_t>(LuaMethodBase* lua_method) {
    return (int16_t)lua_tointeger(lua_method->l, lua_method->lua_arg_idx++);
}
template<> int32_t lua_arg_read<int32_t>(LuaMethodBase* lua_method) {
    return (int32_t)lua_tointeger(lua_method->l, lua_method->lua_arg_idx++);
}

template<> int64_t lua_arg_read<int64_t>(LuaMethodBase* lua_method) {
    return (int64_t)lua_tointeger(lua_method->l, lua_method->lua_arg_idx++);
}

template<> uint8_t lua_arg_read<uint8_t>(LuaMethodBase* lua_method) {
    return (uint8_t)lua_tointeger(lua_method->l, lua_method->lua_arg_idx++);
}

template<> uint16_t lua_arg_read<uint16_t>(LuaMethodBase* lua_method) {
    return (uint16_t)lua_tointeger(lua_method->l, lua_method->lua_arg_idx++);
}

template<> uint32_t lua_arg_read<uint32_t>(LuaMethodBase* lua_method) {
    return (uint32_t)lua_tointeger(lua_method->l, lua_method->lua_arg_idx++);
}

template<> uint64_t lua_arg_read<uint64_t>(LuaMethodBase* lua_method) {
    return (uint64_t)lua_tointeger(lua_method->l, lua_method->lua_arg_idx++);
}

template<> float lua_arg_read<float>(LuaMethodBase* lua_method) {
    return (float)lua_tonumber(lua_method->l, lua_method->lua_arg_idx++);
}

template<> double lua_arg_read<double>(LuaMethodBase* lua_method) {
    return (double)lua_tonumber(lua_method->l, lua_method->lua_arg_idx++);
}

template<> GString lua_arg_read<GString>(LuaMethodBase* lua_method) {
    return GString(lua_tostring(lua_method->l, lua_method->lua_arg_idx++));
}

template<> void push_lua_ret<bool>(LuaMethodBase* lua_method, bool ret) {
    lua_pushboolean(lua_method->l, ret);
}

template<> void push_lua_ret<int8_t>(LuaMethodBase* lua_method, int8_t ret) {
    lua_pushinteger(lua_method->l, ret);
}

template<> void push_lua_ret<int16_t>(LuaMethodBase* lua_method, int16_t ret) {
    lua_pushinteger(lua_method->l, ret);
}

template<> void push_lua_ret<int32_t>(LuaMethodBase* lua_method, int32_t ret) {
    lua_pushinteger(lua_method->l, ret);
}

template<> void push_lua_ret<int64_t>(LuaMethodBase* lua_method, int64_t ret) {
    lua_pushinteger(lua_method->l, ret);
}

template<> void push_lua_ret<uint8_t>(LuaMethodBase* lua_method, uint8_t ret) {
    lua_pushinteger(lua_method->l, ret);
}

template<> void push_lua_ret<uint16_t>(LuaMethodBase* lua_method, uint16_t ret) {
    lua_pushinteger(lua_method->l, ret);
}

template<> void push_lua_ret<uint32_t>(LuaMethodBase* lua_method, uint32_t ret) {
    lua_pushinteger(lua_method->l, ret);
}

template<> void push_lua_ret<uint64_t>(LuaMethodBase* lua_method, uint64_t ret) {
    lua_pushinteger(lua_method->l, ret);
}

template<> void push_lua_ret<float>(LuaMethodBase* lua_method, float ret) {
    lua_pushnumber(lua_method->l, ret);
}

template<> void push_lua_ret<double>(LuaMethodBase* lua_method, double ret) {
    lua_pushnumber(lua_method->l, ret);
}

template<> void push_lua_ret<GString>(LuaMethodBase* lua_method, GString ret) {
    lua_pushstring(lua_method->l, ret.c_str());
}
