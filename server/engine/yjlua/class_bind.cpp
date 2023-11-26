#include "../entity.h"

#include "class_bind.h"
#include "yjlua.h"


int32_t error_handler(lua_State *l) {
    const char* msg = lua_tostring(l, -1);
    int32_t type = lua_type(l, -1);
    if (type == LUA_TSTRING) {
        luaL_traceback(l, l, msg, 1);
        msg = lua_tostring(l, -1);
        lua_pop(l, 1);
    }
    lua_pop(l, 1);

    ERROR_LOG("%s\n", msg);
    return 0;
}

static GString cache_method_name;
static int32_t call_yj_method(lua_State *l);
int32_t class_index(lua_State *l) {
    int n = lua_gettop(l);

    ASSERT_LOG(n == 2, "func call arg num.%d error\n", n);

    if (!lua_isstring(l, -1)) {
        ERROR_LOG("func name error");
    }

    cache_method_name = lua_tostring(l, -1);
    lua_pushcfunction(l, call_yj_method);

    return 1;
}

static int32_t call_yj_method(lua_State *l) {
    lua_getfield(l, 1, "Object");
    void* yj_obj = lua_touserdata(l, -1);

    ClassDescBase* class_desc = ((YjObj*)yj_obj)->get_class_desc();
    auto yj_method = class_desc->find_yj_method(cache_method_name);
    yj_method->l = l;
    yj_method->lua_arg_idx = 2; // 1 is self
    yj_method->decode();
    lua_pop(l, 4);
    return yj_method->exec(yj_obj);
}

ClassDescBase::~ClassDescBase() {
    for (auto iter = yj_methods.begin(); iter != yj_methods.end(); ++iter) {
        delete iter->second;
    }
    yj_methods.clear();
}

void ClassDescBase::add_yj_method(const GString& rpc_name, YjMethodBase* method) {
    ASSERT_LOG(yj_methods.find(rpc_name) == yj_methods.end(), "lua method(%s) already exist\n", rpc_name.c_str());
    yj_methods.emplace(rpc_name, method);
}

YjMethodBase* ClassDescBase::find_yj_method(const GString& rpc_name) {
    auto iter = yj_methods.find(rpc_name);
    if (iter == yj_methods.end()) {
        return nullptr;
    }
    return iter->second;
}

bool ClassDescBase::bind(const GString& module_name) {
    lua_module_name = module_name;

    lua_State* l = YjLua::get_state();
    lua_pushcfunction(l, error_handler);
    lua_getglobal(l, "require");
    lua_pushstring(l, lua_module_name.c_str());
    int32_t code = lua_pcall(l, 1, LUA_MULTRET, -3);
    if (code != LUA_OK) {
        ERROR_LOG("entity class(%s) require(%s) error\n", class_name.c_str(), lua_module_name.c_str());
        return false;
    }

    lua_pop(l, 1);

    if (lua_type(l, -1) != LUA_TTABLE) {
        ERROR_LOG("entity class(%s) lua module(%s) must return table\n", class_name.c_str(), lua_module_name.c_str());
        return false;
    }

    class_ref = luaL_ref(l, LUA_REGISTRYINDEX);

    prop_bind();
    method_bind();

    // pop error_handler
    lua_pop(l, 1);

    return true;
}

void ClassDescBase::prop_bind() {

}

void ClassDescBase::method_bind() {
    //lua_State* l = YjLua::get_state();
    //lua_pushnil(l);
    //while (lua_next(l, -2)) {
    //    if (lua_type(l, -1) != LUA_TFUNCTION) {
    //        lua_pop(l, 1);
    //        continue;
    //    }

    //    GString yj_method_name(lua_tostring(l, -2));
    //    auto iter = yj_methods.find(yj_method_name);
    //    if (iter != yj_methods.end()) {
    //        yj_methods.insert(make_pair(yj_method_name, iter->second));
    //    }
    //    lua_pop(l, 1);
    //}
}
