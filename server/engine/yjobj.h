#pragma once

#include "gvalue.h"
#include "yjlua/lua/src/lua.hpp"

using namespace std;

class ClassDescBase;

class YjObj {
public:
    YjObj();
    virtual ~YjObj();

    // lua bind
    virtual ClassDescBase* get_class_desc() { ASSERT(false); return nullptr; }
    virtual GString get_class_name() { ASSERT(false); return ""; }

    lua_Integer obj_ref;
};
