#include "yjlua.h"


static int func_pre_call(lua_State *l);

YjLua::YjLua() {
    l = luaL_newstate();
    luaL_openlibs(l);

    lua_register(l, "func_pre_call", func_pre_call);
    do_str("setmetatable(_G, {__index = func_pre_call})");
}

YjLua::~YjLua() {
    for (auto iter = m_lua_methods.begin(); iter != m_lua_methods.end(); ++iter) {
        delete iter->second;
    }
    m_lua_methods.clear();

    lua_close(l);
}

void YjLua::do_str(const GString& str) {
    luaL_dostring(l, str.c_str());
}

void YjLua::do_file(const GString& filename) {
    luaL_dofile(l, filename.c_str());
}

void YjLua::add_lua_function(const GString& rpc_name, LuaMethodBase* method) {
    ASSERT_LOG(m_lua_methods.find(rpc_name) == m_lua_methods.end(), "lua method(%s) already exist\n", rpc_name.c_str());
    m_lua_methods.emplace(rpc_name, method);
}

LuaMethodBase* YjLua::find_lua_function(const GString& rpc_name) {
    auto iter = m_lua_methods.find(rpc_name);
    if (iter == m_lua_methods.end()) {
        return nullptr;
    }
    return iter->second;
}

YjLua g_yjlua;

static GString lua_func_name;
static int func_call(lua_State *l);
static int func_pre_call(lua_State *l) {
    int n = lua_gettop(l);

    ASSERT_LOG(n == 2, "func call arg num.%d error\n", n);

    if (!lua_isstring(l, 2)) {
        ERROR_LOG("func name error");
    }

    lua_func_name = lua_tostring(l, 2);
    lua_pushcfunction(l, func_call);
    return 1;
}

static int func_call(lua_State *l) {
    auto lua_method = g_yjlua.find_lua_function(lua_func_name);
    lua_method->l = l;
    lua_method->lua_arg_idx = 1;
    lua_method->decode();
    return lua_method->exec();
}
