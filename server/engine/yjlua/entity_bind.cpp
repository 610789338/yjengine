#include "../entity.h"

#include "lua/src/lua.hpp"
#include "entity_bind.h"
#include "yjlua.h"


void ClassDesc::method_bind(lua_State* l, Entity* entity) {
    if (lua_type(l, -1) != LUA_TTABLE) {
        ERROR_LOG("entity class(%s) lua module(%s) must return table\n", class_name.c_str(), lua_module_name.c_str());
        return;
    }

    RpcManagerBase* rpc_mgr = entity->get_rpc_mgr();
    auto& rpc_methods = rpc_mgr->get_rpc_methods();

    lua_pushnil(l);
    while (lua_next(l, -2)) {
        if (lua_type(l, -1) != LUA_TFUNCTION) {
            lua_pop(l, 1);
            continue;
        }

        GString lua_method_name(lua_tostring(l, -2));
        auto iter = rpc_methods.find(lua_method_name);
        if (iter != rpc_methods.end()) {
            lua_methods.insert(make_pair(lua_method_name, iter->second));
        }
        lua_pop(l, 1);
    }

    class_ref = luaL_ref(l, LUA_REGISTRYINDEX);
}
