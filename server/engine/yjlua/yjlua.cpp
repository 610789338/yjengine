#ifdef WIN32
#include <direct.h>
#else
#include <unistd.h>
#endif

#include "../entity.h"
#include "yjlua.h"


static int32_t func_pre_call(lua_State *l);
lua_State* YjLua::get_state() {
    static lua_State* l = nullptr;
    static bool lua_state_init = false;
    if (!lua_state_init) {
        l = luaL_newstate();
        luaL_openlibs(l);

        lua_register(l, "func_pre_call", func_pre_call);
        luaL_dostring(l, "setmetatable(_G, {__index = func_pre_call})");

        char path[512] = { 0 };
        getcwd(path, 512);
        GString script_path = GString(path) + "\\..\\..\\script\\?.lua";
        while (script_path.find("\\") != GString::npos) {
            script_path.replace(script_path.find("\\"), 1, "&");
        }
        while (script_path.find("&") != GString::npos) {
            script_path.replace(script_path.find("&"), 1, "\\\\");
        }
        INFO_LOG("lua script_path.%s\n", script_path.c_str());

        GString add_script_path = GString("package.path=package.path..\";") + script_path + GString("\"");
        luaL_dostring(l, add_script_path.c_str());
        luaL_dostring(l, "require(\"main\")");

        lua_state_init = true;
    }
    return l;
}

void YjLua::do_str(const GString& str) {
    auto l = YjLua::get_state();
    luaL_dostring(l, str.c_str());
}

void YjLua::do_file(const GString& filename) {
    auto l = YjLua::get_state();
    luaL_dofile(l, filename.c_str());
}

YjLua::YjLua() {
}

YjLua::~YjLua() {
    for (auto iter = m_yjg_methods.begin(); iter != m_yjg_methods.end(); ++iter) {
        delete iter->second;
    }
    m_yjg_methods.clear();

    auto l = YjLua::get_state();
    lua_close(l);
}

void YjLua::add_yjg_method(const GString& rpc_name, YjMethodBase* method) {
    ASSERT_LOG(m_yjg_methods.find(rpc_name) == m_yjg_methods.end(), "lua method(%s) already exist\n", rpc_name.c_str());
    m_yjg_methods.emplace(rpc_name, method);
}

YjMethodBase* YjLua::find_yjg_method(const GString& rpc_name) {
    auto iter = m_yjg_methods.find(rpc_name);
    if (iter == m_yjg_methods.end()) {
        return nullptr;
    }
    return iter->second;
}

YjLua g_yjlua;

static GString lua_func_name;
static int32_t func_call(lua_State *l);
static int32_t func_pre_call(lua_State *l) {
    int n = lua_gettop(l);

    ASSERT_LOG(n == 2, "func call arg num.%d error\n", n);

    if (!lua_isstring(l, 2)) {
        ERROR_LOG("func name error");
        return 0;
    }

    lua_func_name = lua_tostring(l, -1);
    lua_pushcfunction(l, func_call);
    return 1;
}

static int32_t func_call(lua_State *l) {
    auto yjg_method = g_yjlua.find_yjg_method(lua_func_name);
    yjg_method->l = l;
    yjg_method->lua_arg_idx = 1;
    yjg_method->decode();
    return yjg_method->exec();
}
