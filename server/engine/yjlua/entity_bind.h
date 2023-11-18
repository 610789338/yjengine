#pragma once

#include "../gvalue.h"
#include "../rpc_manager.h"
#include "lua/src/lua.hpp"

class Entity;

class ClassDesc {
public:
    ClassDesc() {}
    ~ClassDesc() {}

    void method_bind(lua_State* l, Entity* entity);

public:
    GString class_name;
    GString lua_module_name;
    lua_Integer class_ref;
    unordered_map<GString, RpcMethodBase*> yj_methods;
};


int8_t lua_push_args(lua_State* l);

template<class T1>
int8_t lua_push_args(lua_State* l, T1& t1) {
    lua_push(l, t1);
    return 1;
}

template<class T1, class T2>
int8_t lua_push_args(lua_State* l, T1& t1, T2& t2) {
    lua_push(l, t1);
    lua_push(l, t2);
    return 2;
}

template<class T1, class T2, class T3>
int8_t lua_push_args(lua_State* l, T1& t1, T2& t2, T3& t3) {
    lua_push(l, t1);
    lua_push(l, t2);
    lua_push(l, t3);
    return 3;
}

template<class T1, class T2, class T3, class T4>
int8_t lua_push_args(lua_State* l, T1& t1, T2& t2, T3& t3, T4& t4) {
    lua_push(l, t1);
    lua_push(l, t2);
    lua_push(l, t3);
    lua_push(l, t4);
    return 4;
}

template<class T1, class T2, class T3, class T4, class T5>
int8_t lua_push_args(lua_State* l, T1& t1, T2& t2, T3& t3, T4& t4, T5& t5) {
    lua_push(l, t1);
    lua_push(l, t2);
    lua_push(l, t3);
    lua_push(l, t4);
    lua_push(l, t5);
    return 5;
}

template<class T1, class T2, class T3, class T4, class T5, class T6>
int8_t lua_push_args(lua_State* l, T1& t1, T2& t2, T3& t3, T4& t4, T5& t5, T6& t6) {
    lua_push(l, t1);
    lua_push(l, t2);
    lua_push(l, t3);
    lua_push(l, t4);
    lua_push(l, t5);
    lua_push(l, t6);
    return 6;
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7>
int8_t lua_push_args(lua_State* l, T1& t1, T2& t2, T3& t3, T4& t4, T5& t5, T6& t6, T7& t7) {
    lua_push(l, t1);
    lua_push(l, t2);
    lua_push(l, t3);
    lua_push(l, t4);
    lua_push(l, t5);
    lua_push(l, t6);
    lua_push(l, t7);
    return 7;
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
int8_t lua_push_args(lua_State* l, T1& t1, T2& t2, T3& t3, T4& t4, T5& t5, T6& t6, T7& t7, T8& t8) {
    lua_push(l, t1);
    lua_push(l, t2);
    lua_push(l, t3);
    lua_push(l, t4);
    lua_push(l, t5);
    lua_push(l, t6);
    lua_push(l, t7);
    lua_push(l, t8);
    return 8;
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
int8_t lua_push_args(lua_State* l, T1& t1, T2& t2, T3& t3, T4& t4, T5& t5, T6& t6, T7& t7, T8& t8, T9& t9) {
    lua_push(l, t1);
    lua_push(l, t2);
    lua_push(l, t3);
    lua_push(l, t4);
    lua_push(l, t5);
    lua_push(l, t6);
    lua_push(l, t7);
    lua_push(l, t8);
    lua_push(l, t9);
    return 9;
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10>
int8_t lua_push_args(lua_State* l, T1& t1, T2& t2, T3& t3, T4& t4, T5& t5, T6& t6, T7& t7, T8& t8, T9& t9, T10& t10) {
    lua_push(l, t1);
    lua_push(l, t2);
    lua_push(l, t3);
    lua_push(l, t4);
    lua_push(l, t5);
    lua_push(l, t6);
    lua_push(l, t7);
    lua_push(l, t8);
    lua_push(l, t9);
    lua_push(l, t10);
    return 10;
}


extern int32_t error_handler(lua_State *l);
template<class... T>
void call_lua(Entity* entity, GString method_name, const T&... args) {
    ClassDesc* class_desc = g_yjlua.get_class_desc(entity);
    if (class_desc == nullptr) {
        return;
    }

    auto l = g_yjlua.get_state();
    int32_t old_top = lua_gettop(l);
    lua_pushcfunction(l, error_handler);

    lua_rawgeti(l, LUA_REGISTRYINDEX, class_desc->class_ref);
    lua_getfield(l, -1, method_name.c_str());
    if (lua_type(l, -1) != LUA_TFUNCTION) {
        ERROR_LOG("%s has no lua funciont named %s\n", entity->get_full_entity_name().c_str(), method_name.c_str());
        lua_pop(l, 3);
        return;
    }

    lua_rawgeti(l, LUA_REGISTRYINDEX, entity->entity_ref);
    int8_t arg_nums = lua_push_args(l, args...);
    lua_pcall(l, 2, LUA_MULTRET, -arg_nums-4);
    lua_settop(l, old_top);
}

#define CALL_LUA(rpc_name, ...) call_lua(this, rpc_name, ##__VA_ARGS__)
