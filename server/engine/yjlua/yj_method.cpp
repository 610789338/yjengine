#include "yj_method.h"
#include "yjlua.h"

template<> bool lua_arg_read<bool>(YjMethodBase* yj_method) {
    return (bool)lua_tointeger(yj_method->l, yj_method->lua_arg_idx++);
}

template<> int8_t lua_arg_read<int8_t>(YjMethodBase* yj_method) {
    return (int8_t)lua_tointeger(yj_method->l, yj_method->lua_arg_idx++);
}

template<> int16_t lua_arg_read<int16_t>(YjMethodBase* yj_method) {
    return (int16_t)lua_tointeger(yj_method->l, yj_method->lua_arg_idx++);
}
template<> int32_t lua_arg_read<int32_t>(YjMethodBase* yj_method) {
    return (int32_t)lua_tointeger(yj_method->l, yj_method->lua_arg_idx++);
}

template<> int64_t lua_arg_read<int64_t>(YjMethodBase* yj_method) {
    return (int64_t)lua_tointeger(yj_method->l, yj_method->lua_arg_idx++);
}

template<> uint8_t lua_arg_read<uint8_t>(YjMethodBase* yj_method) {
    return (uint8_t)lua_tointeger(yj_method->l, yj_method->lua_arg_idx++);
}

template<> uint16_t lua_arg_read<uint16_t>(YjMethodBase* yj_method) {
    return (uint16_t)lua_tointeger(yj_method->l, yj_method->lua_arg_idx++);
}

template<> uint32_t lua_arg_read<uint32_t>(YjMethodBase* yj_method) {
    return (uint32_t)lua_tointeger(yj_method->l, yj_method->lua_arg_idx++);
}

template<> uint64_t lua_arg_read<uint64_t>(YjMethodBase* yj_method) {
    return (uint64_t)lua_tointeger(yj_method->l, yj_method->lua_arg_idx++);
}

template<> float lua_arg_read<float>(YjMethodBase* yj_method) {
    return (float)lua_tonumber(yj_method->l, yj_method->lua_arg_idx++);
}

template<> double lua_arg_read<double>(YjMethodBase* yj_method) {
    return (double)lua_tonumber(yj_method->l, yj_method->lua_arg_idx++);
}

template<> GString lua_arg_read<GString>(YjMethodBase* yj_method) {
    return GString(lua_tostring(yj_method->l, yj_method->lua_arg_idx++));
}

template<> void lua_push<bool>(lua_State* l, bool ret) {
    lua_pushboolean(l, ret);
}

template<> void lua_push<int8_t>(lua_State* l, int8_t ret) {
    lua_pushinteger(l, ret);
}

template<> void lua_push<int16_t>(lua_State* l, int16_t ret) {
    lua_pushinteger(l, ret);
}

template<> void lua_push<int32_t>(lua_State* l, int32_t ret) {
    lua_pushinteger(l, ret);
}

template<> void lua_push<int64_t>(lua_State* l, int64_t ret) {
    lua_pushinteger(l, ret);
}

template<> void lua_push<uint8_t>(lua_State* l, uint8_t ret) {
    lua_pushinteger(l, ret);
}

template<> void lua_push<uint16_t>(lua_State* l, uint16_t ret) {
    lua_pushinteger(l, ret);
}

template<> void lua_push<uint32_t>(lua_State* l, uint32_t ret) {
    lua_pushinteger(l, ret);
}

template<> void lua_push<uint64_t>(lua_State* l, uint64_t ret) {
    lua_pushinteger(l, ret);
}

template<> void lua_push<float>(lua_State* l, float ret) {
    lua_pushnumber(l, ret);
}

template<> void lua_push<double>(lua_State* l, double ret) {
    lua_pushnumber(l, ret);
}

template<> void lua_push<GString>(lua_State* l, GString ret) {
    lua_pushstring(l, ret.c_str());
}
