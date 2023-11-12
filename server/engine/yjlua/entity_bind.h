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
    unordered_map<GString, RpcMethodBase*> lua_methods;
};
