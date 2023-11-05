#pragma once

#include "../gvalue.h"

#include "lua/src/lua.hpp"
#include "lua_method.h"

struct LuaMethodBase;

class YjLua {
public:
    YjLua();
    ~YjLua();

    void do_str(const GString& str);
    void do_file(const GString& filename);

public:
    void lua_method_bind(const GString& rpc_name, void(*cb)()) {
        LuaMethodBase* method = new LuaMethod0_NORET(cb);
        add_lua_function(rpc_name, method);
    }

    template<class T1, class T2>
    void lua_method_bind(const GString& method_name, void(*cb)(T1, T2)) {
        LuaMethodBase* method = new LuaMethod2_NORET<T1, T2>(cb);
        add_lua_function(method_name, method);
    }

    template<class T1, class T2, class T3>
    void lua_method_bind(const GString& method_name, void(*cb)(T1, T2, T3)) {
        LuaMethodBase* method = new LuaMethod3_NORET<T1, T2, T3>(cb);
        add_lua_function(method_name, method);
    }

    template<class T1, class T2, class T3, class T4>
    void lua_method_bind(const GString& method_name, void(*cb)(T1, T2, T3, T4)) {
        LuaMethodBase* method = new LuaMethod4_NORET<T1, T2, T3, T4>(cb);
        add_lua_function(method_name, method);
    }

    template<class T1, class T2, class T3, class T4, class T5>
    void lua_method_bind(const GString& method_name, void(*cb)(T1, T2, T3, T4, T5)) {
        LuaMethodBase* method = new LuaMethod5_NORET<T1, T2, T3, T4, T5>(cb);
        add_lua_function(method_name, method);
    }

    template<class T1, class T2, class T3, class T4, class T5, class T6>
    void lua_method_bind(const GString& method_name, void(*cb)(T1, T2, T3, T4, T5, T6)) {
        LuaMethodBase* method = new LuaMethod6_NORET<T1, T2, T3, T4, T5, T6>(cb);
        add_lua_function(method_name, method);
    }

    template<class T1, class T2, class T3, class T4, class T5, class T6, class T7>
    void lua_method_bind(const GString& method_name, void(*cb)(T1, T2, T3, T4, T5, T6, T7)) {
        LuaMethodBase* method = new LuaMethod7_NORET<T1, T2, T3, T4, T5, T6, T7>(cb);
        add_lua_function(method_name, method);
    }

    template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
    void lua_method_bind(const GString& method_name, void(*cb)(T1, T2, T3, T4, T5, T6, T7, T8)) {
        LuaMethodBase* method = new LuaMethod8_NORET<T1, T2, T3, T4, T5, T6, T7, T8>(cb);
        add_lua_function(method_name, method);
    }

    template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
    void lua_method_bind(const GString& method_name, void(*cb)(T1, T2, T3, T4, T5, T6, T7, T8, T9)) {
        LuaMethodBase* method = new LuaMethod9_NORET<T1, T2, T3, T4, T5, T6, T7, T8, T9>(cb);
        add_lua_function(method_name, method);
    }

    template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10>
    void lua_method_bind(const GString& method_name, void(*cb)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10)) {
        LuaMethodBase* method = new LuaMethod10_NORET<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10>(cb);
        add_lua_function(method_name, method);
    }

    // -------------------------------------------------------------------

    template<class RT>
    void lua_method_bind(const GString& method_name, RT(*cb)()) {
        LuaMethodBase* method = new LuaMethod0<RT>(cb);
        add_lua_function(method_name, method);
    }

    template<class T1, class RT>
    void lua_method_bind(const GString& method_name, RT(*cb)(T1)) {
        LuaMethodBase* method = new LuaMethod1<T1, RT>(cb);
        add_lua_function(method_name, method);
    }

    template<class T1, class T2, class RT>
    void lua_method_bind(const GString& method_name, RT(*cb)(T1, T2)) {
        LuaMethodBase* method = new LuaMethod2<T1, T2, RT>(cb);
        add_lua_function(method_name, method);
    }

    template<class T1, class T2, class T3, class RT>
    void lua_method_bind(const GString& method_name, void(*cb)(T1, T2, T3)) {
        LuaMethodBase* method = new LuaMethod3<T1, T2, T3>(cb);
        add_lua_function(method_name, method);
    }

    template<class T1, class T2, class T3, class T4, class RT>
    void lua_method_bind(const GString& method_name, RT(*cb)(T1, T2, T3, T4)) {
        LuaMethodBase* method = new LuaMethod4<T1, T2, T3, T4>(cb);
        add_lua_function(method_name, method);
    }

    template<class T1, class T2, class T3, class T4, class T5, class RT>
    void lua_method_bind(const GString& method_name, RT(*cb)(T1, T2, T3, T4, T5)) {
        LuaMethodBase* method = new LuaMethod5<T1, T2, T3, T4, T5>(cb);
        add_lua_function(method_name, method);
    }

    template<class T1, class T2, class T3, class T4, class T5, class T6, class RT>
    void lua_method_bind(const GString& method_name, RT(*cb)(T1, T2, T3, T4, T5, T6)) {
        LuaMethodBase* method = new LuaMethod6<T1, T2, T3, T4, T5, T6>(cb);
        add_lua_function(method_name, method);
    }

    template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class RT>
    void lua_method_bind(const GString& method_name, RT(*cb)(T1, T2, T3, T4, T5, T6, T7)) {
        LuaMethodBase* method = new LuaMethod7<T1, T2, T3, T4, T5, T6, T7>(cb);
        add_lua_function(method_name, method);
    }

    template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class RT>
    void lua_method_bind(const GString& method_name, RT(*cb)(T1, T2, T3, T4, T5, T6, T7, T8)) {
        LuaMethodBase* method = new LuaMethod8<T1, T2, T3, T4, T5, T6, T7, T8>(cb);
        add_lua_function(method_name, method);
    }

    template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class RT>
    void lua_method_bind(const GString& method_name, RT(*cb)(T1, T2, T3, T4, T5, T6, T7, T8, T9)) {
        LuaMethodBase* method = new LuaMethod9<T1, T2, T3, T4, T5, T6, T7, T8, T9>(cb);
        add_lua_function(method_name, method);
    }

    template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class RT>
    void lua_method_bind(const GString& method_name, RT(*cb)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10)) {
        LuaMethodBase* method = new LuaMethod10<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10>(cb);
        add_lua_function(method_name, method);
    }

    void add_lua_function(const GString& rpc_name, LuaMethodBase* method);
    LuaMethodBase* find_lua_function(const GString& rpc_name);

private:
    lua_State* l = nullptr;
    unordered_map<GString, LuaMethodBase*> m_lua_methods;
};

extern YjLua g_yjlua;

#define LUA_METHOD_BIND(method) g_yjlua.lua_method_bind(#method, method);
