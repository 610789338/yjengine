#ifdef WIN32
#include <direct.h>
#else
#include <unistd.h>
#endif

#include "../entity.h"
#include "yjlua.h"


static int32_t func_pre_call(lua_State *l);
static int32_t error_handler(lua_State *l);

YjLua::YjLua() {
    l = luaL_newstate();
    luaL_openlibs(l);

    lua_register(l, "func_pre_call", func_pre_call);
    do_str("setmetatable(_G, {__index = func_pre_call})");

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
    do_str(add_script_path.c_str());
    do_str("require(\"main\")");
}

YjLua::~YjLua() {
    for (auto iter = m_lua_methods.begin(); iter != m_lua_methods.end(); ++iter) {
        delete iter->second;
    }
    m_lua_methods.clear();

    for (auto iter = m_class_binds.begin(); iter != m_class_binds.end(); ++iter) {
        delete iter->second;
    }
    m_class_binds.clear();

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

ClassDesc* YjLua::get_class_desc(Entity* entity) {
    GString entity_name = entity->get_full_entity_name();
    auto iter = m_class_binds.find(entity_name);
    if (iter != m_class_binds.end()) {
        return iter->second;
    }
    return nullptr;
}

bool YjLua::class_bind(Entity* entity) {
    GString entity_name = entity->get_full_entity_name();
    auto iter = m_class_binds.find(entity_name);
    if (iter != m_class_binds.end()) {
        return true;
    }

    GString module_name = entity->get_lua_module_name();
    if (module_name.empty()) {
        ERROR_LOG("entity class(%s) lua module empty\n", entity_name.c_str(), module_name.c_str());
        return false;
    }

    ClassDesc* class_desc = new ClassDesc;
    class_desc->class_name = entity_name;
    class_desc->lua_module_name = module_name;

    lua_pushcfunction(l, error_handler);
    lua_getglobal(l, "require");
    lua_pushstring(l, module_name.c_str());
    int32_t code = lua_pcall(l, 1, LUA_MULTRET, -3);
    if (code != LUA_OK) {
        ERROR_LOG("entity class(%s) require(%s) error\n", entity_name.c_str(), module_name.c_str());
        return false;
    }

    lua_pop(l, 1);
    class_desc->method_bind(l, entity);

    // pop error_handler
    lua_pop(l, 1);

    m_class_binds.insert(make_pair(entity_name, class_desc));

    return true;
}

bool YjLua::entity_bind(ClassDesc* class_desc, Entity* entity) {
    lua_newtable(l);
    lua_pushstring(l, "Object");
    lua_pushlightuserdata(l, entity);
    lua_rawset(l, -3);

    entity->entity_ref = luaL_ref(l, LUA_REGISTRYINDEX);

    //lua_rawgeti(l, LUA_REGISTRYINDEX, entity->entity_ref);
    //lua_rawgeti(l, LUA_REGISTRYINDEX, class_desc->class_ref);

    return true;
}

YjLua g_yjlua;

static GString lua_func_name;
static int32_t func_call(lua_State *l);
static int32_t func_pre_call(lua_State *l) {
    int n = lua_gettop(l);

    ASSERT_LOG(n == 2, "func call arg num.%d error\n", n);

    if (!lua_isstring(l, 2)) {
        ERROR_LOG("func name error");
    }

    lua_func_name = lua_tostring(l, 2);
    lua_pushcfunction(l, func_call);
    return 1;
}

static int32_t func_call(lua_State *l) {
    auto lua_method = g_yjlua.find_lua_function(lua_func_name);
    lua_method->l = l;
    lua_method->lua_arg_idx = 1;
    lua_method->decode();
    return lua_method->exec();
}

static int32_t error_handler(lua_State *l) {
    const char* msg = lua_tostring(l, -1);
    int32_t type = lua_type(l, -1);
    if (type == LUA_TSTRING) {
        luaL_traceback(l, l, msg, 1);
        msg = lua_tostring(l, -1);
    }
    lua_pop(l, 1);

    ERROR_LOG("%s\n", msg);
    return 0;
}

void notify_entity_create(Entity* entity) {
    ClassDesc* class_desc = g_yjlua.get_class_desc(entity);
    if (class_desc == nullptr) {
        if (!g_yjlua.class_bind(entity)) {
            return;
        }
        class_desc = g_yjlua.get_class_desc(entity);
    }

    g_yjlua.entity_bind(class_desc, entity);
}

void notify_entity_call(Entity* entity, GString method_name) {
    ClassDesc* class_desc = g_yjlua.get_class_desc(entity);
    if (class_desc == nullptr) {
        return;
    }

    auto iter = class_desc->lua_methods.find(method_name);
    if (iter == class_desc->lua_methods.end()) {
        return;
    }

    auto l = g_yjlua.get_state();
    lua_pushcfunction(l, error_handler);

    lua_rawgeti(l, LUA_REGISTRYINDEX, class_desc->class_ref);
    lua_getfield(l, -1, method_name.c_str());
    lua_rawgeti(l, LUA_REGISTRYINDEX, entity->entity_ref);
    lua_pushstring(l, "hahaha");
    lua_pcall(l, 2, LUA_MULTRET, -5);
}
