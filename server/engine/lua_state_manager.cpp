#include "lua_state_manager.h"

std::shared_ptr<LuaStateManager> g_luas_mgr = nullptr;

void lua_state_mgr_init() {
    g_luas_mgr = make_shared<LuaStateManager>();
}

void LuaStateManager::do_str(const GString& str) {
    luaL_dostring(l, str.c_str());
    //lua_pcall(l, 0, 0, 0);
}
