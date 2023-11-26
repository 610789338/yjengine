#pragma once

#include "../gvalue.h"
#include "lua/src/lua.hpp"

class YjMethodBase;

class PropDesc {
};

class ClassDescBase {
public:
    ClassDescBase(const GString& _class_name) : class_name(_class_name) {}
    virtual ~ClassDescBase();

    void add_yj_method(const GString& method_name, YjMethodBase* method);
    YjMethodBase* find_yj_method(const GString& method_name);

    bool bind(const GString& module_name);
    void prop_bind();
    void method_bind();

public:
    GString class_name;
    GString lua_module_name;
    lua_Integer class_ref;
    unordered_map<GString, PropDesc*> yj_props;
    unordered_map<GString, YjMethodBase*> yj_methods;
};

template<class ObjClassType>
class ClassDesc : public ClassDescBase {
public:
    ClassDesc() = delete;
    ClassDesc(GString entity_name) : ClassDescBase(entity_name) {}
    ~ClassDesc() {}

public:
    template<class TObj>
    void yj_method_bind(const GString& method_name, void(TObj::*cb)()) {
        YjMethodBase* method = new YjMethod0_NORET<TObj>(cb);
        add_yj_method(method_name, method);
    }

    template<class TObj, class T1>
    void yj_method_bind(const GString& method_name, void(TObj::*cb)(T1)) {
        YjMethodBase* method = new YjMethod1_NORET<TObj, T1>(cb);
        add_yj_method(method_name, method);
    }

    template<class TObj, class T1, class T2>
    void yj_method_bind(const GString& method_name, void(TObj::*cb)(T1, T2)) {
        YjMethodBase* method = new YjMethod2_NORET<TObj, T1, T2>(cb);
        add_yj_method(method_name, method);
    }

    template<class TObj, class T1, class T2, class T3>
    void yj_method_bind(const GString& method_name, void(TObj::*cb)(T1, T2, T3)) {
        YjMethodBase* method = new YjMethod3_NORET<TObj, T1, T2, T3>(cb);
        add_yj_method(method_name, method);
    }

    template<class TObj, class T1, class T2, class T3, class T4>
    void yj_method_bind(const GString& method_name, void(TObj::*cb)(T1, T2, T3, T4)) {
        YjMethodBase* method = new YjMethod4_NORET<TObj, T1, T2, T3, T4>(cb);
        add_yj_method(method_name, method);
    }

    template<class TObj, class T1, class T2, class T3, class T4, class T5>
    void yj_method_bind(const GString& method_name, void(TObj::*cb)(T1, T2, T3, T4, T5)) {
        YjMethodBase* method = new YjMethod5_NORET<TObj, T1, T2, T3, T4, T5>(cb);
        add_yj_method(method_name, method);
    }

    template<class TObj, class T1, class T2, class T3, class T4, class T5, class T6>
    void yj_method_bind(const GString& method_name, void(TObj::*cb)(T1, T2, T3, T4, T5, T6)) {
        YjMethodBase* method = new YjMethod6_NORET<TObj, T1, T2, T3, T4, T5, T6>(cb);
        add_yj_method(method_name, method);
    }

    template<class TObj, class T1, class T2, class T3, class T4, class T5, class T6, class T7>
    void yj_method_bind(const GString& method_name, void(TObj::*cb)(T1, T2, T3, T4, T5, T6, T7)) {
        YjMethodBase* method = new YjMethod7_NORET<TObj, T1, T2, T3, T4, T5, T6, T7>(cb);
        add_yj_method(method_name, method);
    }

    template<class TObj, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
    void yj_method_bind(const GString& method_name, void(TObj::*cb)(T1, T2, T3, T4, T5, T6, T7, T8)) {
        YjMethodBase* method = new YjMethod8_NORET<TObj, T1, T2, T3, T4, T5, T6, T7, T8>(cb);
        add_yj_method(method_name, method);
    }

    template<class TObj, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
    void yj_method_bind(const GString& method_name, void(TObj::*cb)(T1, T2, T3, T4, T5, T6, T7, T8, T9)) {
        YjMethodBase* method = new YjMethod9_NORET<TObj, T1, T2, T3, T4, T5, T6, T7, T8, T9>(cb);
        add_yj_method(method_name, method);
    }

    template<class TObj, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10>
    void yj_method_bind(const GString& method_name, void(TObj::*cb)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10)) {
        YjMethodBase* method = new YjMethod10_NORET<TObj, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10>(cb);
        add_yj_method(method_name, method);
    }

    // -------------------------------------------------------------------

    template<class TObj, class RT>
    void yj_method_bind(const GString& method_name, RT(TObj::*cb)()) {
        YjMethodBase* method = new YjMethod0<TObj, RT>(cb);
        add_yj_method(method_name, method);
    }

    template<class TObj, class T1, class RT>
    void yj_method_bind(const GString& method_name, RT(TObj::*cb)(T1)) {
        YjMethodBase* method = new YjMethod1<TObj, T1, RT>(cb);
        add_yj_method(method_name, method);
    }

    template<class TObj, class T1, class T2, class RT>
    void yj_method_bind(const GString& method_name, RT(TObj::*cb)(T1, T2)) {
        YjMethodBase* method = new YjMethod2<TObj, T1, T2, RT>(cb);
        add_yj_method(method_name, method);
    }

    template<class TObj, class T1, class T2, class T3, class RT>
    void yj_method_bind(const GString& method_name, void(TObj::*cb)(T1, T2, T3)) {
        YjMethodBase* method = new YjMethod3<TObj, T1, T2, T3>(cb);
        add_yj_method(method_name, method);
    }

    template<class TObj, class T1, class T2, class T3, class T4, class RT>
    void yj_method_bind(const GString& method_name, RT(TObj::*cb)(T1, T2, T3, T4)) {
        YjMethodBase* method = new YjMethod4<TObj, T1, T2, T3, T4>(cb);
        add_yj_method(method_name, method);
    }

    template<class TObj, class T1, class T2, class T3, class T4, class T5, class RT>
    void yj_method_bind(const GString& method_name, RT(TObj::*cb)(T1, T2, T3, T4, T5)) {
        YjMethodBase* method = new YjMethod5<TObj, T1, T2, T3, T4, T5>(cb);
        add_yj_method(method_name, method);
    }

    template<class TObj, class T1, class T2, class T3, class T4, class T5, class T6, class RT>
    void yj_method_bind(const GString& method_name, RT(TObj::*cb)(T1, T2, T3, T4, T5, T6)) {
        YjMethodBase* method = new YjMethod6<TObj, T1, T2, T3, T4, T5, T6>(cb);
        add_yj_method(method_name, method);
    }

    template<class TObj, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class RT>
    void yj_method_bind(const GString& method_name, RT(TObj::*cb)(T1, T2, T3, T4, T5, T6, T7)) {
        YjMethodBase* method = new YjMethod7<TObj, T1, T2, T3, T4, T5, T6, T7>(cb);
        add_yj_method(method_name, method);
    }

    template<class TObj, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class RT>
    void yj_method_bind(const GString& method_name, RT(TObj::*cb)(T1, T2, T3, T4, T5, T6, T7, T8)) {
        YjMethodBase* method = new YjMethod8<TObj, T1, T2, T3, T4, T5, T6, T7, T8>(cb);
        add_yj_method(method_name, method);
    }

    template<class TObj, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class RT>
    void yj_method_bind(const GString& method_name, RT(TObj::*cb)(T1, T2, T3, T4, T5, T6, T7, T8, T9)) {
        YjMethodBase* method = new YjMethod9<TObj, T1, T2, T3, T4, T5, T6, T7, T8, T9>(cb);
        add_yj_method(method_name, method);
    }

    template<class TObj, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class RT>
    void yj_method_bind(const GString& method_name, RT(TObj::*cb)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10)) {
        YjMethodBase* method = new YjMethod10<TObj, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10>(cb);
        add_yj_method(method_name, method);
    }

    ObjClassType* tclass;
};

#define YJ_METHOD(method) class_desc.yj_method_bind(#method, &RMP(decltype(class_desc.tclass))::method)
