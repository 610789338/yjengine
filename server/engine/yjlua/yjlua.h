#pragma once

#include "../gvalue.h"

#ifdef USE_LUAJIT
#include "luajit/src/lua.hpp"
#else
#include "lua/src/lua.hpp"
#endif

#include "yj_method.h"
#include "class_bind.h"
#include "obj_bind.h"


class YjLua {
public:
    static lua_State* get_state();
    static void do_str(const GString& str);
    static void do_file(const GString& filename);

public:
    YjLua();
    ~YjLua();

public:
    void yjg_method_bind(const GString& rpc_name, void(*cb)()) {
        YjMethodBase* method = new YjGMethod0_NORET(cb);
        add_yjg_method(rpc_name, method);
    }

    template<class T1>
    void yjg_method_bind(const GString& method_name, void(*cb)(T1)) {
        YjMethodBase* method = new YjGMethod1_NORET<T1>(cb);
        add_yjg_method(method_name, method);
    }

    template<class T1, class T2>
    void yjg_method_bind(const GString& method_name, void(*cb)(T1, T2)) {
        YjMethodBase* method = new YjGMethod2_NORET<T1, T2>(cb);
        add_yjg_method(method_name, method);
    }

    template<class T1, class T2, class T3>
    void yjg_method_bind(const GString& method_name, void(*cb)(T1, T2, T3)) {
        YjMethodBase* method = new YjGMethod3_NORET<T1, T2, T3>(cb);
        add_yjg_method(method_name, method);
    }

    template<class T1, class T2, class T3, class T4>
    void yjg_method_bind(const GString& method_name, void(*cb)(T1, T2, T3, T4)) {
        YjMethodBase* method = new YjGMethod4_NORET<T1, T2, T3, T4>(cb);
        add_yjg_method(method_name, method);
    }

    template<class T1, class T2, class T3, class T4, class T5>
    void yjg_method_bind(const GString& method_name, void(*cb)(T1, T2, T3, T4, T5)) {
        YjMethodBase* method = new YjGMethod5_NORET<T1, T2, T3, T4, T5>(cb);
        add_yjg_method(method_name, method);
    }

    template<class T1, class T2, class T3, class T4, class T5, class T6>
    void yjg_method_bind(const GString& method_name, void(*cb)(T1, T2, T3, T4, T5, T6)) {
        YjMethodBase* method = new YjGMethod6_NORET<T1, T2, T3, T4, T5, T6>(cb);
        add_yjg_method(method_name, method);
    }

    template<class T1, class T2, class T3, class T4, class T5, class T6, class T7>
    void yjg_method_bind(const GString& method_name, void(*cb)(T1, T2, T3, T4, T5, T6, T7)) {
        YjMethodBase* method = new YjGMethod7_NORET<T1, T2, T3, T4, T5, T6, T7>(cb);
        add_yjg_method(method_name, method);
    }

    template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
    void yjg_method_bind(const GString& method_name, void(*cb)(T1, T2, T3, T4, T5, T6, T7, T8)) {
        YjMethodBase* method = new YjGMethod8_NORET<T1, T2, T3, T4, T5, T6, T7, T8>(cb);
        add_yjg_method(method_name, method);
    }

    template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
    void yjg_method_bind(const GString& method_name, void(*cb)(T1, T2, T3, T4, T5, T6, T7, T8, T9)) {
        YjMethodBase* method = new YjGMethod9_NORET<T1, T2, T3, T4, T5, T6, T7, T8, T9>(cb);
        add_yjg_method(method_name, method);
    }

    template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10>
    void yjg_method_bind(const GString& method_name, void(*cb)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10)) {
        YjMethodBase* method = new YjGMethod10_NORET<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10>(cb);
        add_yjg_method(method_name, method);
    }

    // -------------------------------------------------------------------

    template<class RT>
    void yjg_method_bind(const GString& method_name, RT(*cb)()) {
        YjMethodBase* method = new YjGMethod0<RT>(cb);
        add_yjg_method(method_name, method);
    }

    template<class T1, class RT>
    void yjg_method_bind(const GString& method_name, RT(*cb)(T1)) {
        YjMethodBase* method = new YjGMethod1<T1, RT>(cb);
        add_yjg_method(method_name, method);
    }

    template<class T1, class T2, class RT>
    void yjg_method_bind(const GString& method_name, RT(*cb)(T1, T2)) {
        YjMethodBase* method = new YjGMethod2<T1, T2, RT>(cb);
        add_yjg_method(method_name, method);
    }

    template<class T1, class T2, class T3, class RT>
    void yjg_method_bind(const GString& method_name, void(*cb)(T1, T2, T3)) {
        YjMethodBase* method = new YjGMethod3<T1, T2, T3>(cb);
        add_yjg_method(method_name, method);
    }

    template<class T1, class T2, class T3, class T4, class RT>
    void yjg_method_bind(const GString& method_name, RT(*cb)(T1, T2, T3, T4)) {
        YjMethodBase* method = new YjGMethod4<T1, T2, T3, T4>(cb);
        add_yjg_method(method_name, method);
    }

    template<class T1, class T2, class T3, class T4, class T5, class RT>
    void yjg_method_bind(const GString& method_name, RT(*cb)(T1, T2, T3, T4, T5)) {
        YjMethodBase* method = new YjGMethod5<T1, T2, T3, T4, T5>(cb);
        add_yjg_method(method_name, method);
    }

    template<class T1, class T2, class T3, class T4, class T5, class T6, class RT>
    void yjg_method_bind(const GString& method_name, RT(*cb)(T1, T2, T3, T4, T5, T6)) {
        YjMethodBase* method = new YjGMethod6<T1, T2, T3, T4, T5, T6>(cb);
        add_yjg_method(method_name, method);
    }

    template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class RT>
    void yjg_method_bind(const GString& method_name, RT(*cb)(T1, T2, T3, T4, T5, T6, T7)) {
        YjMethodBase* method = new YjGMethod7<T1, T2, T3, T4, T5, T6, T7>(cb);
        add_yjg_method(method_name, method);
    }

    template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class RT>
    void yjg_method_bind(const GString& method_name, RT(*cb)(T1, T2, T3, T4, T5, T6, T7, T8)) {
        YjMethodBase* method = new YjGMethod8<T1, T2, T3, T4, T5, T6, T7, T8>(cb);
        add_yjg_method(method_name, method);
    }

    template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class RT>
    void yjg_method_bind(const GString& method_name, RT(*cb)(T1, T2, T3, T4, T5, T6, T7, T8, T9)) {
        YjMethodBase* method = new YjGMethod9<T1, T2, T3, T4, T5, T6, T7, T8, T9>(cb);
        add_yjg_method(method_name, method);
    }

    template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class RT>
    void yjg_method_bind(const GString& method_name, RT(*cb)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10)) {
        YjMethodBase* method = new YjGMethod10<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10>(cb);
        add_yjg_method(method_name, method);
    }

    void add_yjg_method(const GString& rpc_name, YjMethodBase* method);
    YjMethodBase* find_yjg_method(const GString& rpc_name);

private:
    unordered_map<GString, YjMethodBase*> m_yjg_methods;
};

extern YjLua g_yjlua;

#define YJ_GMETHOD(method) g_yjlua.yjg_method_bind(#method, method);
