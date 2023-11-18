#pragma once

#include "../gvalue.h"

#include "lua/src/lua.hpp"
#include "yj_method.h"
#include "entity_bind.h"

class Entity;
struct YjMethodBase;

class YjLua {
public:
    YjLua();
    ~YjLua();

    lua_State* get_state() { return l; }

    void do_str(const GString& str);
    void do_file(const GString& filename);

public:
    void yj_method_bind(const GString& rpc_name, void(*cb)()) {
        YjMethodBase* method = new YjMethod0_NORET(cb);
        add_yj_method(rpc_name, method);
    }

    template<class T1>
    void yj_method_bind(const GString& method_name, void(*cb)(T1)) {
        YjMethodBase* method = new YjMethod1_NORET<T1>(cb);
        add_yj_method(method_name, method);
    }

    template<class T1, class T2>
    void yj_method_bind(const GString& method_name, void(*cb)(T1, T2)) {
        YjMethodBase* method = new YjMethod2_NORET<T1, T2>(cb);
        add_yj_method(method_name, method);
    }

    template<class T1, class T2, class T3>
    void yj_method_bind(const GString& method_name, void(*cb)(T1, T2, T3)) {
        YjMethodBase* method = new YjMethod3_NORET<T1, T2, T3>(cb);
        add_yj_method(method_name, method);
    }

    template<class T1, class T2, class T3, class T4>
    void yj_method_bind(const GString& method_name, void(*cb)(T1, T2, T3, T4)) {
        YjMethodBase* method = new YjMethod4_NORET<T1, T2, T3, T4>(cb);
        add_yj_method(method_name, method);
    }

    template<class T1, class T2, class T3, class T4, class T5>
    void yj_method_bind(const GString& method_name, void(*cb)(T1, T2, T3, T4, T5)) {
        YjMethodBase* method = new YjMethod5_NORET<T1, T2, T3, T4, T5>(cb);
        add_yj_method(method_name, method);
    }

    template<class T1, class T2, class T3, class T4, class T5, class T6>
    void yj_method_bind(const GString& method_name, void(*cb)(T1, T2, T3, T4, T5, T6)) {
        YjMethodBase* method = new YjMethod6_NORET<T1, T2, T3, T4, T5, T6>(cb);
        add_yj_method(method_name, method);
    }

    template<class T1, class T2, class T3, class T4, class T5, class T6, class T7>
    void yj_method_bind(const GString& method_name, void(*cb)(T1, T2, T3, T4, T5, T6, T7)) {
        YjMethodBase* method = new YjMethod7_NORET<T1, T2, T3, T4, T5, T6, T7>(cb);
        add_yj_method(method_name, method);
    }

    template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
    void yj_method_bind(const GString& method_name, void(*cb)(T1, T2, T3, T4, T5, T6, T7, T8)) {
        YjMethodBase* method = new YjMethod8_NORET<T1, T2, T3, T4, T5, T6, T7, T8>(cb);
        add_yj_method(method_name, method);
    }

    template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
    void yj_method_bind(const GString& method_name, void(*cb)(T1, T2, T3, T4, T5, T6, T7, T8, T9)) {
        YjMethodBase* method = new YjMethod9_NORET<T1, T2, T3, T4, T5, T6, T7, T8, T9>(cb);
        add_yj_method(method_name, method);
    }

    template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10>
    void yj_method_bind(const GString& method_name, void(*cb)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10)) {
        YjMethodBase* method = new YjMethod10_NORET<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10>(cb);
        add_yj_method(method_name, method);
    }

    // -------------------------------------------------------------------

    template<class RT>
    void yj_method_bind(const GString& method_name, RT(*cb)()) {
        YjMethodBase* method = new YjMethod0<RT>(cb);
        add_yj_method(method_name, method);
    }

    template<class T1, class RT>
    void yj_method_bind(const GString& method_name, RT(*cb)(T1)) {
        YjMethodBase* method = new YjMethod1<T1, RT>(cb);
        add_yj_method(method_name, method);
    }

    template<class T1, class T2, class RT>
    void yj_method_bind(const GString& method_name, RT(*cb)(T1, T2)) {
        YjMethodBase* method = new YjMethod2<T1, T2, RT>(cb);
        add_yj_method(method_name, method);
    }

    template<class T1, class T2, class T3, class RT>
    void yj_method_bind(const GString& method_name, void(*cb)(T1, T2, T3)) {
        YjMethodBase* method = new YjMethod3<T1, T2, T3>(cb);
        add_yj_method(method_name, method);
    }

    template<class T1, class T2, class T3, class T4, class RT>
    void yj_method_bind(const GString& method_name, RT(*cb)(T1, T2, T3, T4)) {
        YjMethodBase* method = new YjMethod4<T1, T2, T3, T4>(cb);
        add_yj_method(method_name, method);
    }

    template<class T1, class T2, class T3, class T4, class T5, class RT>
    void yj_method_bind(const GString& method_name, RT(*cb)(T1, T2, T3, T4, T5)) {
        YjMethodBase* method = new YjMethod5<T1, T2, T3, T4, T5>(cb);
        add_yj_method(method_name, method);
    }

    template<class T1, class T2, class T3, class T4, class T5, class T6, class RT>
    void yj_method_bind(const GString& method_name, RT(*cb)(T1, T2, T3, T4, T5, T6)) {
        YjMethodBase* method = new YjMethod6<T1, T2, T3, T4, T5, T6>(cb);
        add_yj_method(method_name, method);
    }

    template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class RT>
    void yj_method_bind(const GString& method_name, RT(*cb)(T1, T2, T3, T4, T5, T6, T7)) {
        YjMethodBase* method = new YjMethod7<T1, T2, T3, T4, T5, T6, T7>(cb);
        add_yj_method(method_name, method);
    }

    template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class RT>
    void yj_method_bind(const GString& method_name, RT(*cb)(T1, T2, T3, T4, T5, T6, T7, T8)) {
        YjMethodBase* method = new YjMethod8<T1, T2, T3, T4, T5, T6, T7, T8>(cb);
        add_yj_method(method_name, method);
    }

    template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class RT>
    void yj_method_bind(const GString& method_name, RT(*cb)(T1, T2, T3, T4, T5, T6, T7, T8, T9)) {
        YjMethodBase* method = new YjMethod9<T1, T2, T3, T4, T5, T6, T7, T8, T9>(cb);
        add_yj_method(method_name, method);
    }

    template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class RT>
    void yj_method_bind(const GString& method_name, RT(*cb)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10)) {
        YjMethodBase* method = new YjMethod10<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10>(cb);
        add_yj_method(method_name, method);
    }

    void add_yj_method(const GString& rpc_name, YjMethodBase* method);
    YjMethodBase* find_yj_method(const GString& rpc_name);

    // class bind
    ClassDesc* get_class_desc(Entity* entity);
    bool class_bind(Entity* entity);
    bool entity_bind(ClassDesc* class_desc, Entity* entity);

private:
    lua_State* l = nullptr;
    unordered_map<GString, YjMethodBase*> m_yj_methods;

    unordered_map<GString, ClassDesc*> m_class_binds;
};

extern YjLua g_yjlua;

#define YJ_METHOD(method) g_yjlua.yj_method_bind(#method, method);

extern void notify_entity_create(Entity* entity);
