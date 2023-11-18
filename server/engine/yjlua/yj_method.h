#pragma once

#include "lua/src/lua.hpp"

#include "../gvalue.h"

struct YjMethodBase;

template<class T> T lua_arg_read(YjMethodBase* yj_method) { 
    return (T)lua_touserdata(yj_method->l, yj_method->lua_arg_idx++);
}

template<> bool     lua_arg_read<bool>      (YjMethodBase* yj_method);
template<> int8_t   lua_arg_read<int8_t>    (YjMethodBase* yj_method);
template<> int16_t  lua_arg_read<int16_t>   (YjMethodBase* yj_method);
template<> int32_t  lua_arg_read<int32_t>   (YjMethodBase* yj_method);
template<> int64_t  lua_arg_read<int64_t>   (YjMethodBase* yj_method);
template<> uint8_t  lua_arg_read<uint8_t>   (YjMethodBase* yj_method);
template<> uint16_t lua_arg_read<uint16_t>  (YjMethodBase* yj_method);
template<> uint32_t lua_arg_read<uint32_t>  (YjMethodBase* yj_method);
template<> uint64_t lua_arg_read<uint64_t>  (YjMethodBase* yj_method);
template<> float    lua_arg_read<float>     (YjMethodBase* yj_method);
template<> double   lua_arg_read<double>    (YjMethodBase* yj_method);
template<> GString  lua_arg_read<GString>   (YjMethodBase* yj_method);


template<class T> void lua_push(lua_State* l, T ret) {
    lua_pushlightuserdata(l, (void*)ret);
}

template<> void lua_push<bool>      (lua_State* l, bool ret);
template<> void lua_push<int8_t>    (lua_State* l, int8_t ret);
template<> void lua_push<int16_t>   (lua_State* l, int16_t ret);
template<> void lua_push<int32_t>   (lua_State* l, int32_t ret);
template<> void lua_push<int64_t>   (lua_State* l, int64_t ret);
template<> void lua_push<uint8_t>   (lua_State* l, uint8_t ret);
template<> void lua_push<uint16_t>  (lua_State* l, uint16_t ret);
template<> void lua_push<uint32_t>  (lua_State* l, uint32_t ret);
template<> void lua_push<uint64_t>  (lua_State* l, uint64_t ret);
template<> void lua_push<float>     (lua_State* l, float ret);
template<> void lua_push<double>    (lua_State* l, double ret);
template<> void lua_push<GString>(lua_State* l, GString ret);

#define LUA_PUSH_RET(l, ret) lua_push(l, ret)

// lua method imp
struct YjMethodBase {
    YjMethodBase() {}

    virtual void decode() { ASSERT(false); }
    virtual int8_t exec() { ASSERT(false); return 0; }

    lua_State* l = nullptr;
    int32_t lua_arg_idx = 1;
};

struct YjMethod0_NORET : public YjMethodBase {
    typedef void(*CBType)();
    YjMethod0_NORET(CBType _cb) : cb(_cb) {}
    CBType cb;

    void decode() {}
    int8_t exec() { cb(); return 0; }
};

template<class T1>
struct YjMethod1_NORET : public YjMethodBase {
    typedef void(*CBType)(T1);
    YjMethod1_NORET(CBType _cb) : cb(_cb) {}
    CBType cb;
    RMCVR(T1) t1;

    void decode() {
        t1 = lua_arg_read<RMCVR(T1)>(this);
    }
    int8_t exec() {
        cb(t1);
        return 0;
    }
};

template<class T1, class T2>
struct YjMethod2_NORET : public YjMethodBase {
    typedef void(*CBType)(T1, T2);
    YjMethod2_NORET(CBType _cb) : cb(_cb) {}
    CBType cb;
    RMCVR(T1) t1;
    RMCVR(T2) t2;

    void decode() {
        t1 = lua_arg_read<RMCVR(T1)>(this);
        t2 = lua_arg_read<RMCVR(T2)>(this);
    }

    int8_t exec() {
        cb(t1, t2);
        return 0;
    }
};

template<class T1, class T2, class T3>
struct YjMethod3_NORET : public YjMethodBase {
    typedef void(*CBType)(T1, T2, T3);
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

    int8_t exec() {
        cb(t1, t2, t3);
        return 0;
    }
};

template<class T1, class T2, class T3, class T4>
struct YjMethod4_NORET : public YjMethodBase {
    typedef void(*CBType)(T1, T2, T3, T4);
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

    int8_t exec() {
        cb(t1, t2, t3, t4);
        return 0;
    }
};

template<class T1, class T2, class T3, class T4, class T5>
struct YjMethod5_NORET : public YjMethodBase {
    typedef void(*CBType)(T1, T2, T3, T4, T5);
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

    int8_t exec() {
        cb(t1, t2, t3, t4, t5);
        return 0;
    }
};

template<class T1, class T2, class T3, class T4, class T5, class T6>
struct YjMethod6_NORET : public YjMethodBase {
    typedef void(*CBType)(T1, T2, T3, T4, T5, T6);
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

    int8_t exec() {
        cb(t1, t2, t3, t4, t5, t6);
        return 0;
    }
};

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7>
struct YjMethod7_NORET : public YjMethodBase {
    typedef void(*CBType)(T1, T2, T3, T4, T5, T6, T7);
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

    int8_t exec() {
        cb(t1, t2, t3, t4, t5, t6, t7);
        return 0;
    }
};

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
struct YjMethod8_NORET : public YjMethodBase {
    typedef void(*CBType)(T1, T2, T3, T4, T5, T6, T7, T8);
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

    int8_t exec() {
        cb(t1, t2, t3, t4, t5, t6, t7, t8);
        return 0;
    }
};

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
struct YjMethod9_NORET : public YjMethodBase {
    typedef void(*CBType)(T1, T2, T3, T4, T5, T6, T7, T8, T9);
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

    int8_t exec() {
        cb(t1, t2, t3, t4, t5, t6, t7, t8, t9);
        return 0;
    }
};

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10>
struct YjMethod10_NORET : public YjMethodBase {
    typedef void(*CBType)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10);
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

    int8_t exec() {
        cb(t1, t2, t3, t4, t5, t6, t7, t8, t9, t10);
        return 0;
    }
};


// -------------------------------------------------------------------------

template<class RT>
struct YjMethod0 : public YjMethodBase {
    typedef RT(*CBType)();
    YjMethod0(CBType _cb) : cb(_cb) {}
    CBType cb;

    void decode() {}
    int8_t exec() {
        RMCVR(RT) ret = cb();
        LUA_PUSH_RET(l, ret);
        return 1;
    }
};

template<class T1, class RT>
struct YjMethod1 : public YjMethodBase {
    typedef RT(*CBType)(T1);
    YjMethod1(CBType _cb) : cb(_cb) {}
    CBType cb;
    RMCVR(T1) t1;

    void decode() {
        t1 = lua_arg_read<RMCVR(T1)>(this);
    }
    int8_t exec() { 
        RMCVR(RT) ret = cb(t1);
        LUA_PUSH_RET(l, ret);
        return 1;
    }
};

template<class T1, class T2, class RT>
struct YjMethod2 : public YjMethodBase {
    typedef RT(*CBType)(T1, T2);
    YjMethod2(CBType _cb) : cb(_cb) {}
    CBType cb;
    RMCVR(T1) t1;
    RMCVR(T2) t2;

    void decode() {
        t1 = lua_arg_read<RMCVR(T1)>(this);
        t2 = lua_arg_read<RMCVR(T2)>(this);
    }

    int8_t exec() {
        RMCVR(RT) ret = cb(t1, t2);
        LUA_PUSH_RET(l, ret);
        return 1;
    }
};

template<class T1, class T2, class T3, class RT>
struct YjMethod3 : public YjMethodBase {
    typedef RT(*CBType)(T1, T2, T3);
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

    int8_t exec() {
        RMCVR(RT) ret = cb(t1, t2, t3);
        LUA_PUSH_RET(l, ret);
        return 1;
    }
};

template<class T1, class T2, class T3, class T4, class RT>
struct YjMethod4 : public YjMethodBase {
    typedef RT(*CBType)(T1, T2, T3, T4);
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

    int8_t exec() {
        RMCVR(RT) ret = cb(t1, t2, t3, t4);
        LUA_PUSH_RET(l, ret);
        return 1;
    }
};

template<class T1, class T2, class T3, class T4, class T5, class RT>
struct YjMethod5 : public YjMethodBase {
    typedef RT(*CBType)(T1, T2, T3, T4, T5);
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

    int8_t exec() {
        RMCVR(RT) ret = cb(t1, t2, t3, t4, t5);
        LUA_PUSH_RET(l, ret);
        return 1;
    }
};

template<class T1, class T2, class T3, class T4, class T5, class T6, class RT>
struct YjMethod6 : public YjMethodBase {
    typedef RT(*CBType)(T1, T2, T3, T4, T5, T6);
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

    int8_t exec() {
        RMCVR(RT) ret = cb(t1, t2, t3, t4, t5, t6);
        LUA_PUSH_RET(l, ret);
        return 1;
    }
};

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class RT>
struct YjMethod7 : public YjMethodBase {
    typedef RT(*CBType)(T1, T2, T3, T4, T5, T6, T7);
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

    int8_t exec() {
        RMCVR(RT) ret = cb(t1, t2, t3, t4, t5, t6, t7);
        LUA_PUSH_RET(l, ret);
        return 1;
    }
};

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class RT>
struct YjMethod8 : public YjMethodBase {
    typedef RT(*CBType)(T1, T2, T3, T4, T5, T6, T7, T8);
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

    int8_t exec() {
        RMCVR(RT) ret = cb(t1, t2, t3, t4, t5, t6, t7, t8);
        LUA_PUSH_RET(l, ret);
        return 1;
    }
};

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class RT>
struct YjMethod9 : public YjMethodBase {
    typedef RT(*CBType)(T1, T2, T3, T4, T5, T6, T7, T8, T9);
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

    int8_t exec() {
        RMCVR(RT) ret = cb(t1, t2, t3, t4, t5, t6, t7, t8, t9);
        LUA_PUSH_RET(l, ret);
        return 1;
    }
};

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class RT>
struct YjMethod10 : public YjMethodBase {
    typedef RT(*CBType)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10);
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

    int8_t exec() {
        RMCVR(RT) ret = cb(t1, t2, t3, t4, t5, t6, t7, t8, t9, t10);
        LUA_PUSH_RET(l, ret);
        return 1;
    }
};
