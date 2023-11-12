#pragma once

#include "lua/src/lua.hpp"

#include "../gvalue.h"

struct LuaMethodBase;

template<class T> T lua_arg_read(LuaMethodBase* lua_method) { 
    return (T)lua_touserdata(lua_method->l, lua_method->lua_arg_idx++);
}

template<> bool     lua_arg_read<bool>      (LuaMethodBase* lua_method);
template<> int8_t   lua_arg_read<int8_t>    (LuaMethodBase* lua_method);
template<> int16_t  lua_arg_read<int16_t>   (LuaMethodBase* lua_method);
template<> int32_t  lua_arg_read<int32_t>   (LuaMethodBase* lua_method);
template<> int64_t  lua_arg_read<int64_t>   (LuaMethodBase* lua_method);
template<> uint8_t  lua_arg_read<uint8_t>   (LuaMethodBase* lua_method);
template<> uint16_t lua_arg_read<uint16_t>  (LuaMethodBase* lua_method);
template<> uint32_t lua_arg_read<uint32_t>  (LuaMethodBase* lua_method);
template<> uint64_t lua_arg_read<uint64_t>  (LuaMethodBase* lua_method);
template<> float    lua_arg_read<float>     (LuaMethodBase* lua_method);
template<> double   lua_arg_read<double>    (LuaMethodBase* lua_method);
template<> GString  lua_arg_read<GString>   (LuaMethodBase* lua_method);


template<class T> void push_lua_ret(LuaMethodBase* lua_method, T ret) {
    lua_pushlightuserdata(lua_method->l, (void*)ret);
}

template<> void push_lua_ret<bool>      (LuaMethodBase* lua_method, bool ret);
template<> void push_lua_ret<int8_t>    (LuaMethodBase* lua_method, int8_t ret);
template<> void push_lua_ret<int16_t>   (LuaMethodBase* lua_method, int16_t ret);
template<> void push_lua_ret<int32_t>   (LuaMethodBase* lua_method, int32_t ret);
template<> void push_lua_ret<int64_t>   (LuaMethodBase* lua_method, int64_t ret);
template<> void push_lua_ret<uint8_t>   (LuaMethodBase* lua_method, uint8_t ret);
template<> void push_lua_ret<uint16_t>  (LuaMethodBase* lua_method, uint16_t ret);
template<> void push_lua_ret<uint32_t>  (LuaMethodBase* lua_method, uint32_t ret);
template<> void push_lua_ret<uint64_t>  (LuaMethodBase* lua_method, uint64_t ret);
template<> void push_lua_ret<float>     (LuaMethodBase* lua_method, float ret);
template<> void push_lua_ret<double>    (LuaMethodBase* lua_method, double ret);
template<> void push_lua_ret<GString>   (LuaMethodBase* lua_method, GString ret);

// lua method imp
struct LuaMethodBase {
    LuaMethodBase() {}

    virtual void decode() { ASSERT(false); }
    virtual int8_t exec() { ASSERT(false); return 0; }

    lua_State* l = nullptr;
    int32_t lua_arg_idx = 1;
};

struct LuaMethod0_NORET : public LuaMethodBase {
    typedef void(*CBType)();
    LuaMethod0_NORET(CBType _cb) : cb(_cb) {}
    CBType cb;

    void decode() {}
    int8_t exec() { cb(); return 0; }
};

template<class T1>
struct LuaMethod1_NORET : public LuaMethodBase {
    typedef void(*CBType)(T1);
    LuaMethod1_NORET(CBType _cb) : cb(_cb) {}
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
struct LuaMethod2_NORET : public LuaMethodBase {
    typedef void(*CBType)(T1, T2);
    LuaMethod2_NORET(CBType _cb) : cb(_cb) {}
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
struct LuaMethod3_NORET : public LuaMethodBase {
    typedef void(*CBType)(T1, T2, T3);
    LuaMethod3_NORET(CBType _cb) : cb(_cb) {}
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
struct LuaMethod4_NORET : public LuaMethodBase {
    typedef void(*CBType)(T1, T2, T3, T4);
    LuaMethod4_NORET(CBType _cb) : cb(_cb) {}
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
struct LuaMethod5_NORET : public LuaMethodBase {
    typedef void(*CBType)(T1, T2, T3, T4, T5);
    LuaMethod5_NORET(CBType _cb) : cb(_cb) {}
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
struct LuaMethod6_NORET : public LuaMethodBase {
    typedef void(*CBType)(T1, T2, T3, T4, T5, T6);
    LuaMethod6_NORET(CBType _cb) : cb(_cb) {}
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
struct LuaMethod7_NORET : public LuaMethodBase {
    typedef void(*CBType)(T1, T2, T3, T4, T5, T6, T7);
    LuaMethod7_NORET(CBType _cb) : cb(_cb) {}
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
struct LuaMethod8_NORET : public LuaMethodBase {
    typedef void(*CBType)(T1, T2, T3, T4, T5, T6, T7, T8);
    LuaMethod8_NORET(CBType _cb) : cb(_cb) {}
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
struct LuaMethod9_NORET : public LuaMethodBase {
    typedef void(*CBType)(T1, T2, T3, T4, T5, T6, T7, T8, T9);
    LuaMethod9_NORET(CBType _cb) : cb(_cb) {}
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
struct LuaMethod10_NORET : public LuaMethodBase {
    typedef void(*CBType)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10);
    LuaMethod10_NORET(CBType _cb) : cb(_cb) {}
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
struct LuaMethod0 : public LuaMethodBase {
    typedef RT(*CBType)();
    LuaMethod0(CBType _cb) : cb(_cb) {}
    CBType cb;

    void decode() {}
    int8_t exec() {
        RMCVR(RT) ret = cb();
        push_lua_ret<RMCVR(RT)>(this, ret);
        return 1;
    }
};

template<class T1, class RT>
struct LuaMethod1 : public LuaMethodBase {
    typedef RT(*CBType)(T1);
    LuaMethod1(CBType _cb) : cb(_cb) {}
    CBType cb;
    RMCVR(T1) t1;

    void decode() {
        t1 = lua_arg_read<RMCVR(T1)>(this);
    }
    int8_t exec() { 
        RMCVR(RT) ret = cb(t1);
        push_lua_ret<RMCVR(RT)>(this, ret);
        return 1;
    }
};

template<class T1, class T2, class RT>
struct LuaMethod2 : public LuaMethodBase {
    typedef RT(*CBType)(T1, T2);
    LuaMethod2(CBType _cb) : cb(_cb) {}
    CBType cb;
    RMCVR(T1) t1;
    RMCVR(T2) t2;

    void decode() {
        t1 = lua_arg_read<RMCVR(T1)>(this);
        t2 = lua_arg_read<RMCVR(T2)>(this);
    }

    int8_t exec() {
        RMCVR(RT) ret = cb(t1, t2);
        push_lua_ret<RMCVR(RT)>(this, ret);
        return 1;
    }
};

template<class T1, class T2, class T3, class RT>
struct LuaMethod3 : public LuaMethodBase {
    typedef RT(*CBType)(T1, T2, T3);
    LuaMethod3(CBType _cb) : cb(_cb) {}
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
        push_lua_ret<RMCVR(RT)>(this, ret);
        return 1;
    }
};

template<class T1, class T2, class T3, class T4, class RT>
struct LuaMethod4 : public LuaMethodBase {
    typedef RT(*CBType)(T1, T2, T3, T4);
    LuaMethod4(CBType _cb) : cb(_cb) {}
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
        push_lua_ret<RMCVR(RT)>(this, ret);
        return 1;
    }
};

template<class T1, class T2, class T3, class T4, class T5, class RT>
struct LuaMethod5 : public LuaMethodBase {
    typedef RT(*CBType)(T1, T2, T3, T4, T5);
    LuaMethod5(CBType _cb) : cb(_cb) {}
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
        push_lua_ret<RMCVR(RT)>(this, ret);
        return 1;
    }
};

template<class T1, class T2, class T3, class T4, class T5, class T6, class RT>
struct LuaMethod6 : public LuaMethodBase {
    typedef RT(*CBType)(T1, T2, T3, T4, T5, T6);
    LuaMethod6(CBType _cb) : cb(_cb) {}
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
        push_lua_ret<RMCVR(RT)>(this, ret);
        return 1;
    }
};

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class RT>
struct LuaMethod7 : public LuaMethodBase {
    typedef RT(*CBType)(T1, T2, T3, T4, T5, T6, T7);
    LuaMethod7(CBType _cb) : cb(_cb) {}
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
        push_lua_ret<RMCVR(RT)>(this, ret);
        return 1;
    }
};

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class RT>
struct LuaMethod8 : public LuaMethodBase {
    typedef RT(*CBType)(T1, T2, T3, T4, T5, T6, T7, T8);
    LuaMethod8(CBType _cb) : cb(_cb) {}
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
        push_lua_ret<RMCVR(RT)>(this, ret);
        return 1;
    }
};

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class RT>
struct LuaMethod9 : public LuaMethodBase {
    typedef RT(*CBType)(T1, T2, T3, T4, T5, T6, T7, T8, T9);
    LuaMethod9(CBType _cb) : cb(_cb) {}
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
        push_lua_ret<RMCVR(RT)>(this, ret);
        return 1;
    }
};

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class RT>
struct LuaMethod10 : public LuaMethodBase {
    typedef RT(*CBType)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10);
    LuaMethod10(CBType _cb) : cb(_cb) {}
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
        push_lua_ret<RMCVR(RT)>(this, ret);
        return 1;
    }
};
