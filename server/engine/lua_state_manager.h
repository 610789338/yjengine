#pragma once

#include "thirdparty/lua/src/lua.hpp"
#include "gvalue.h"

class LuaStateManager {

public:
    ~LuaStateManager() {
        lua_close(l);
    }

    LuaStateManager() {
        l = luaL_newstate();
        luaL_openlibs(l);
    };

    void do_str(const GString& str);
    void do_file(const GString& filename);

private:
    lua_State* l = nullptr;
};
