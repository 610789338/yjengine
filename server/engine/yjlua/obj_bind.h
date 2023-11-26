#pragma once

#include "../gvalue.h"

#include "lua/src/lua.hpp"
#include "yj_method.h"

class YjObj;

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


extern bool pre_call_lua(YjObj* obj, GString method_name);
extern void real_call_lua(int8_t arg_nums);

template<class... T>
void call_lua(YjObj* obj, GString method_name, const T&... args) {
    auto l = YjLua::get_state();
    int32_t old_top = lua_gettop(l);
    if (pre_call_lua(obj, method_name)) {
        real_call_lua(lua_push_args(l, args...));
    }
    lua_settop(l, old_top);
}

#define CALL_LUA(rpc_name, ...) call_lua(this, rpc_name, ##__VA_ARGS__)


template<class TObj>
struct YjMethod0_NORET : public YjMethodBase {
    typedef void(TObj::*CBType)();
    YjMethod0_NORET(CBType _cb) : cb(_cb) {}
    CBType cb;

    void decode() {}
    int8_t exec(void* _this) { cb(); return 0; }
};

template<class TObj, class T1>
struct YjMethod1_NORET : public YjMethodBase {
    typedef void(TObj::*CBType)(T1);
    YjMethod1_NORET(CBType _cb) : cb(_cb) {}
    CBType cb;
    RMCVR(T1) t1;

    void decode() {
        t1 = lua_arg_read<RMCVR(T1)>(this);
    }
    int8_t exec(void* _this) {
        (((TObj*)_this)->*cb)(t1);
        return 0;
    }
};

template<class TObj, class T1, class T2>
struct YjMethod2_NORET : public YjMethodBase {
    typedef void(TObj::*CBType)(T1, T2);
    YjMethod2_NORET(CBType _cb) : cb(_cb) {}
    CBType cb;
    RMCVR(T1) t1;
    RMCVR(T2) t2;

    void decode() {
        t1 = lua_arg_read<RMCVR(T1)>(this);
        t2 = lua_arg_read<RMCVR(T2)>(this);
    }

    int8_t exec(void* _this) {
        (((TObj*)_this)->*cb)(t1, t2);
        return 0;
    }
};

template<class TObj, class T1, class T2, class T3>
struct YjMethod3_NORET : public YjMethodBase {
    typedef void(TObj::*CBType)(T1, T2, T3);
    YjMethod3_NORET(CBType _cb) : cb(_cb) {}
    CBType cb;
    RMCVR(T1) t1;
    RMCVR(T2) t2;
    RMCVR(T3) t3;

    void decode() {
        t1 = lua_arg_read<RMCVR(T1)>(this);
        t2 = lua_arg_read<RMCVR(T2)>(this);
        t3 = lua_arg_read<RMCVR(T3)>(this);
    }

    int8_t exec(void* _this) {
        (((TObj*)_this)->*cb)(t1, t2, t3);
        return 0;
    }
};

template<class TObj, class T1, class T2, class T3, class T4>
struct YjMethod4_NORET : public YjMethodBase {
    typedef void(TObj::*CBType)(T1, T2, T3, T4);
    YjMethod4_NORET(CBType _cb) : cb(_cb) {}
    CBType cb;
    RMCVR(T1) t1;
    RMCVR(T2) t2;
    RMCVR(T3) t3;
    RMCVR(T4) t4;

    void decode() {
        t1 = lua_arg_read<RMCVR(T1)>(this);
        t2 = lua_arg_read<RMCVR(T2)>(this);
        t3 = lua_arg_read<RMCVR(T3)>(this);
        t4 = lua_arg_read<RMCVR(T4)>(this);
    }

    int8_t exec(void* _this) {
        (((TObj*)_this)->*cb)(t1, t2, t3, t4);
        return 0;
    }
};

template<class TObj, class T1, class T2, class T3, class T4, class T5>
struct YjMethod5_NORET : public YjMethodBase {
    typedef void(TObj::*CBType)(T1, T2, T3, T4, T5);
    YjMethod5_NORET(CBType _cb) : cb(_cb) {}
    CBType cb;
    RMCVR(T1) t1;
    RMCVR(T2) t2;
    RMCVR(T3) t3;
    RMCVR(T4) t4;
    RMCVR(T5) t5;

    void decode() {
        t1 = lua_arg_read<RMCVR(T1)>(this);
        t2 = lua_arg_read<RMCVR(T2)>(this);
        t3 = lua_arg_read<RMCVR(T3)>(this);
        t4 = lua_arg_read<RMCVR(T4)>(this);
        t5 = lua_arg_read<RMCVR(T5)>(this);
    }

    int8_t exec(void* _this) {
        (((TObj*)_this)->*cb)(t1, t2, t3, t4, t5);
        return 0;
    }
};

template<class TObj, class T1, class T2, class T3, class T4, class T5, class T6>
struct YjMethod6_NORET : public YjMethodBase {
    typedef void(TObj::*CBType)(T1, T2, T3, T4, T5, T6);
    YjMethod6_NORET(CBType _cb) : cb(_cb) {}
    CBType cb;
    RMCVR(T1) t1;
    RMCVR(T2) t2;
    RMCVR(T3) t3;
    RMCVR(T4) t4;
    RMCVR(T5) t5;
    RMCVR(T6) t6;

    void decode() {
        t1 = lua_arg_read<RMCVR(T1)>(this);
        t2 = lua_arg_read<RMCVR(T2)>(this);
        t3 = lua_arg_read<RMCVR(T3)>(this);
        t4 = lua_arg_read<RMCVR(T4)>(this);
        t5 = lua_arg_read<RMCVR(T5)>(this);
        t6 = lua_arg_read<RMCVR(T6)>(this);
    }

    int8_t exec(void* _this) {
        (((TObj*)_this)->*cb)(t1, t2, t3, t4, t5, t6);
        return 0;
    }
};

template<class TObj, class T1, class T2, class T3, class T4, class T5, class T6, class T7>
struct YjMethod7_NORET : public YjMethodBase {
    typedef void(TObj::*CBType)(T1, T2, T3, T4, T5, T6, T7);
    YjMethod7_NORET(CBType _cb) : cb(_cb) {}
    CBType cb;
    RMCVR(T1) t1;
    RMCVR(T2) t2;
    RMCVR(T3) t3;
    RMCVR(T4) t4;
    RMCVR(T5) t5;
    RMCVR(T6) t6;
    RMCVR(T7) t7;

    void decode() {
        t1 = lua_arg_read<RMCVR(T1)>(this);
        t2 = lua_arg_read<RMCVR(T2)>(this);
        t3 = lua_arg_read<RMCVR(T3)>(this);
        t4 = lua_arg_read<RMCVR(T4)>(this);
        t5 = lua_arg_read<RMCVR(T5)>(this);
        t6 = lua_arg_read<RMCVR(T6)>(this);
        t7 = lua_arg_read<RMCVR(T7)>(this);
    }

    int8_t exec(void* _this) {
        (((TObj*)_this)->*cb)(t1, t2, t3, t4, t5, t6, t7);
        return 0;
    }
};

template<class TObj, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
struct YjMethod8_NORET : public YjMethodBase {
    typedef void(TObj::*CBType)(T1, T2, T3, T4, T5, T6, T7, T8);
    YjMethod8_NORET(CBType _cb) : cb(_cb) {}
    CBType cb;
    RMCVR(T1) t1;
    RMCVR(T2) t2;
    RMCVR(T3) t3;
    RMCVR(T4) t4;
    RMCVR(T5) t5;
    RMCVR(T6) t6;
    RMCVR(T7) t7;
    RMCVR(T8) t8;

    void decode() {
        t1 = lua_arg_read<RMCVR(T1)>(this);
        t2 = lua_arg_read<RMCVR(T2)>(this);
        t3 = lua_arg_read<RMCVR(T3)>(this);
        t4 = lua_arg_read<RMCVR(T4)>(this);
        t5 = lua_arg_read<RMCVR(T5)>(this);
        t6 = lua_arg_read<RMCVR(T6)>(this);
        t7 = lua_arg_read<RMCVR(T7)>(this);
        t8 = lua_arg_read<RMCVR(T8)>(this);
    }

    int8_t exec(void* _this) {
        (((TObj*)_this)->*cb)(t1, t2, t3, t4, t5, t6, t7, t8);
        return 0;
    }
};

template<class TObj, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
struct YjMethod9_NORET : public YjMethodBase {
    typedef void(TObj::*CBType)(T1, T2, T3, T4, T5, T6, T7, T8, T9);
    YjMethod9_NORET(CBType _cb) : cb(_cb) {}
    CBType cb;
    RMCVR(T1) t1;
    RMCVR(T2) t2;
    RMCVR(T3) t3;
    RMCVR(T4) t4;
    RMCVR(T5) t5;
    RMCVR(T6) t6;
    RMCVR(T7) t7;
    RMCVR(T8) t8;
    RMCVR(T9) t9;

    void decode() {
        t1 = lua_arg_read<RMCVR(T1)>(this);
        t2 = lua_arg_read<RMCVR(T2)>(this);
        t3 = lua_arg_read<RMCVR(T3)>(this);
        t4 = lua_arg_read<RMCVR(T4)>(this);
        t5 = lua_arg_read<RMCVR(T5)>(this);
        t6 = lua_arg_read<RMCVR(T6)>(this);
        t7 = lua_arg_read<RMCVR(T7)>(this);
        t8 = lua_arg_read<RMCVR(T8)>(this);
        t9 = lua_arg_read<RMCVR(T9)>(this);
    }

    int8_t exec(void* _this) {
        (((TObj*)_this)->*cb)(t1, t2, t3, t4, t5, t6, t7, t8, t9);
        return 0;
    }
};

template<class TObj, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10>
struct YjMethod10_NORET : public YjMethodBase {
    typedef void(TObj::*CBType)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10);
    YjMethod10_NORET(CBType _cb) : cb(_cb) {}
    CBType cb;
    RMCVR(T1) t1;
    RMCVR(T2) t2;
    RMCVR(T3) t3;
    RMCVR(T4) t4;
    RMCVR(T5) t5;
    RMCVR(T6) t6;
    RMCVR(T7) t7;
    RMCVR(T8) t8;
    RMCVR(T9) t9;
    RMCVR(T10) t10;

    void decode() {
        t1 = lua_arg_read<RMCVR(T1)>(this);
        t2 = lua_arg_read<RMCVR(T2)>(this);
        t3 = lua_arg_read<RMCVR(T3)>(this);
        t4 = lua_arg_read<RMCVR(T4)>(this);
        t5 = lua_arg_read<RMCVR(T5)>(this);
        t6 = lua_arg_read<RMCVR(T6)>(this);
        t7 = lua_arg_read<RMCVR(T7)>(this);
        t8 = lua_arg_read<RMCVR(T8)>(this);
        t9 = lua_arg_read<RMCVR(T9)>(this);
        t10 = lua_arg_read<RMCVR(T10)>(this);
    }

    int8_t exec(void* _this) {
        (((TObj*)_this)->*cb)(t1, t2, t3, t4, t5, t6, t7, t8, t9, t10);
        return 0;
    }
};


// -------------------------------------------------------------------------

template<class TObj, class RT>
struct YjMethod0 : public YjMethodBase {
    typedef RT(TObj::*CBType)();
    YjMethod0(CBType _cb) : cb(_cb) {}
    CBType cb;

    void decode() {}
    int8_t exec(void* _this) {
        RMCVR(RT) ret = cb();
        LUA_PUSH_RET(l, ret);
        return 1;
    }
};

template<class TObj, class T1, class RT>
struct YjMethod1 : public YjMethodBase {
    typedef RT(TObj::*CBType)(T1);
    YjMethod1(CBType _cb) : cb(_cb) {}
    CBType cb;
    RMCVR(T1) t1;

    void decode() {
        t1 = lua_arg_read<RMCVR(T1)>(this);
    }
    int8_t exec(void* _this) {
        RMCVR(RT) ret = (((TObj*)_this)->*cb)(t1);
        LUA_PUSH_RET(l, ret);
        return 1;
    }
};

template<class TObj, class T1, class T2, class RT>
struct YjMethod2 : public YjMethodBase {
    typedef RT(TObj::*CBType)(T1, T2);
    YjMethod2(CBType _cb) : cb(_cb) {}
    CBType cb;
    RMCVR(T1) t1;
    RMCVR(T2) t2;

    void decode() {
        t1 = lua_arg_read<RMCVR(T1)>(this);
        t2 = lua_arg_read<RMCVR(T2)>(this);
    }

    int8_t exec(void* _this) {
        RMCVR(RT) ret = (((TObj*)_this)->*cb)(t1, t2);
        LUA_PUSH_RET(l, ret);
        return 1;
    }
};

template<class TObj, class T1, class T2, class T3, class RT>
struct YjMethod3 : public YjMethodBase {
    typedef RT(TObj::*CBType)(T1, T2, T3);
    YjMethod3(CBType _cb) : cb(_cb) {}
    CBType cb;
    RMCVR(T1) t1;
    RMCVR(T2) t2;
    RMCVR(T3) t3;

    void decode() {
        t1 = lua_arg_read<RMCVR(T1)>(this);
        t2 = lua_arg_read<RMCVR(T2)>(this);
        t3 = lua_arg_read<RMCVR(T3)>(this);
    }

    int8_t exec(void* _this) {
        RMCVR(RT) ret = (((TObj*)_this)->*cb)(t1, t2, t3);
        LUA_PUSH_RET(l, ret);
        return 1;
    }
};

template<class TObj, class T1, class T2, class T3, class T4, class RT>
struct YjMethod4 : public YjMethodBase {
    typedef RT(TObj::*CBType)(T1, T2, T3, T4);
    YjMethod4(CBType _cb) : cb(_cb) {}
    CBType cb;
    RMCVR(T1) t1;
    RMCVR(T2) t2;
    RMCVR(T3) t3;
    RMCVR(T4) t4;

    void decode() {
        t1 = lua_arg_read<RMCVR(T1)>(this);
        t2 = lua_arg_read<RMCVR(T2)>(this);
        t3 = lua_arg_read<RMCVR(T3)>(this);
        t4 = lua_arg_read<RMCVR(T4)>(this);
    }

    int8_t exec(void* _this) {
        RMCVR(RT) ret = (((TObj*)_this)->*cb)(t1, t2, t3, t4);
        LUA_PUSH_RET(l, ret);
        return 1;
    }
};

template<class TObj, class T1, class T2, class T3, class T4, class T5, class RT>
struct YjMethod5 : public YjMethodBase {
    typedef RT(TObj::*CBType)(T1, T2, T3, T4, T5);
    YjMethod5(CBType _cb) : cb(_cb) {}
    CBType cb;
    RMCVR(T1) t1;
    RMCVR(T2) t2;
    RMCVR(T3) t3;
    RMCVR(T4) t4;
    RMCVR(T5) t5;

    void decode() {
        t1 = lua_arg_read<RMCVR(T1)>(this);
        t2 = lua_arg_read<RMCVR(T2)>(this);
        t3 = lua_arg_read<RMCVR(T3)>(this);
        t4 = lua_arg_read<RMCVR(T4)>(this);
        t5 = lua_arg_read<RMCVR(T5)>(this);
    }

    int8_t exec(void* _this) {
        RMCVR(RT) ret = (((TObj*)_this)->*cb)(t1, t2, t3, t4, t5);
        LUA_PUSH_RET(l, ret);
        return 1;
    }
};

template<class TObj, class T1, class T2, class T3, class T4, class T5, class T6, class RT>
struct YjMethod6 : public YjMethodBase {
    typedef RT(TObj::*CBType)(T1, T2, T3, T4, T5, T6);
    YjMethod6(CBType _cb) : cb(_cb) {}
    CBType cb;
    RMCVR(T1) t1;
    RMCVR(T2) t2;
    RMCVR(T3) t3;
    RMCVR(T4) t4;
    RMCVR(T5) t5;
    RMCVR(T6) t6;

    void decode() {
        t1 = lua_arg_read<RMCVR(T1)>(this);
        t2 = lua_arg_read<RMCVR(T2)>(this);
        t3 = lua_arg_read<RMCVR(T3)>(this);
        t4 = lua_arg_read<RMCVR(T4)>(this);
        t5 = lua_arg_read<RMCVR(T5)>(this);
        t6 = lua_arg_read<RMCVR(T6)>(this);
    }

    int8_t exec(void* _this) {
        RMCVR(RT) ret = (((TObj*)_this)->*cb)(t1, t2, t3, t4, t5, t6);
        LUA_PUSH_RET(l, ret);
        return 1;
    }
};

template<class TObj, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class RT>
struct YjMethod7 : public YjMethodBase {
    typedef RT(TObj::*CBType)(T1, T2, T3, T4, T5, T6, T7);
    YjMethod7(CBType _cb) : cb(_cb) {}
    CBType cb;
    RMCVR(T1) t1;
    RMCVR(T2) t2;
    RMCVR(T3) t3;
    RMCVR(T4) t4;
    RMCVR(T5) t5;
    RMCVR(T6) t6;
    RMCVR(T7) t7;

    void decode() {
        t1 = lua_arg_read<RMCVR(T1)>(this);
        t2 = lua_arg_read<RMCVR(T2)>(this);
        t3 = lua_arg_read<RMCVR(T3)>(this);
        t4 = lua_arg_read<RMCVR(T4)>(this);
        t5 = lua_arg_read<RMCVR(T5)>(this);
        t6 = lua_arg_read<RMCVR(T6)>(this);
        t7 = lua_arg_read<RMCVR(T7)>(this);
    }

    int8_t exec(void* _this) {
        RMCVR(RT) ret = (((TObj*)_this)->*cb)(t1, t2, t3, t4, t5, t6, t7);
        LUA_PUSH_RET(l, ret);
        return 1;
    }
};

template<class TObj, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class RT>
struct YjMethod8 : public YjMethodBase {
    typedef RT(TObj::*CBType)(T1, T2, T3, T4, T5, T6, T7, T8);
    YjMethod8(CBType _cb) : cb(_cb) {}
    CBType cb;
    RMCVR(T1) t1;
    RMCVR(T2) t2;
    RMCVR(T3) t3;
    RMCVR(T4) t4;
    RMCVR(T5) t5;
    RMCVR(T6) t6;
    RMCVR(T7) t7;
    RMCVR(T8) t8;

    void decode() {
        t1 = lua_arg_read<RMCVR(T1)>(this);
        t2 = lua_arg_read<RMCVR(T2)>(this);
        t3 = lua_arg_read<RMCVR(T3)>(this);
        t4 = lua_arg_read<RMCVR(T4)>(this);
        t5 = lua_arg_read<RMCVR(T5)>(this);
        t6 = lua_arg_read<RMCVR(T6)>(this);
        t7 = lua_arg_read<RMCVR(T7)>(this);
        t8 = lua_arg_read<RMCVR(T8)>(this);
    }

    int8_t exec(void* _this) {
        RMCVR(RT) ret = (((TObj*)_this)->*cb)(t1, t2, t3, t4, t5, t6, t7, t8);
        LUA_PUSH_RET(l, ret);
        return 1;
    }
};

template<class TObj, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class RT>
struct YjMethod9 : public YjMethodBase {
    typedef RT(TObj::*CBType)(T1, T2, T3, T4, T5, T6, T7, T8, T9);
    YjMethod9(CBType _cb) : cb(_cb) {}
    CBType cb;
    RMCVR(T1) t1;
    RMCVR(T2) t2;
    RMCVR(T3) t3;
    RMCVR(T4) t4;
    RMCVR(T5) t5;
    RMCVR(T6) t6;
    RMCVR(T7) t7;
    RMCVR(T8) t8;
    RMCVR(T9) t9;

    void decode() {
        t1 = lua_arg_read<RMCVR(T1)>(this);
        t2 = lua_arg_read<RMCVR(T2)>(this);
        t3 = lua_arg_read<RMCVR(T3)>(this);
        t4 = lua_arg_read<RMCVR(T4)>(this);
        t5 = lua_arg_read<RMCVR(T5)>(this);
        t6 = lua_arg_read<RMCVR(T6)>(this);
        t7 = lua_arg_read<RMCVR(T7)>(this);
        t8 = lua_arg_read<RMCVR(T8)>(this);
        t9 = lua_arg_read<RMCVR(T9)>(this);
    }

    int8_t exec(void* _this) {
        RMCVR(RT) ret = (((TObj*)_this)->*cb)(t1, t2, t3, t4, t5, t6, t7, t8, t9);
        LUA_PUSH_RET(l, ret);
        return 1;
    }
};

template<class TObj, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class RT>
struct YjMethod10 : public YjMethodBase {
    typedef RT(TObj::*CBType)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10);
    YjMethod10(CBType _cb) : cb(_cb) {}
    CBType cb;
    RMCVR(T1) t1;
    RMCVR(T2) t2;
    RMCVR(T3) t3;
    RMCVR(T4) t4;
    RMCVR(T5) t5;
    RMCVR(T6) t6;
    RMCVR(T7) t7;
    RMCVR(T8) t8;
    RMCVR(T9) t9;
    RMCVR(T10) t10;

    void decode() {
        t1 = lua_arg_read<RMCVR(T1)>(this);
        t2 = lua_arg_read<RMCVR(T2)>(this);
        t3 = lua_arg_read<RMCVR(T3)>(this);
        t4 = lua_arg_read<RMCVR(T4)>(this);
        t5 = lua_arg_read<RMCVR(T5)>(this);
        t6 = lua_arg_read<RMCVR(T6)>(this);
        t7 = lua_arg_read<RMCVR(T7)>(this);
        t8 = lua_arg_read<RMCVR(T8)>(this);
        t9 = lua_arg_read<RMCVR(T9)>(this);
        t10 = lua_arg_read<RMCVR(T10)>(this);
    }

    int8_t exec(void* _this) {
        RMCVR(RT) ret = (((TObj*)_this)->*cb)(t1, t2, t3, t4, t5, t6, t7, t8, t9, t10);
        LUA_PUSH_RET(l, ret);
        return 1;
    }
};


extern void notify_yjobj_create(YjObj* obj);
