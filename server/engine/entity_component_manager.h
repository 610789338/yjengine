#pragma once

#include "gvalue.h"
#include "rpc_manager.h"

class Entity;

class EntityComponentBase {
public:
    EntityComponentBase() = delete;
    EntityComponentBase(GString component_name, Entity* _owner) : name(component_name), owner(_owner) {}
    virtual ~EntityComponentBase();
    virtual EntityComponentBase* create_self(Entity* owner) = 0;
    GString& get_name() { return name; }
    Entity* get_owner() { return owner; }
    virtual void rpc_call(bool from_client, const GString& rpc_name, const GArray& params) = 0;

private:
    GString name;
    Entity* owner;
};

template<class TEntityComp, class... T>
struct EntityCompRpcMethod : public RpcMethodBase {
    typedef void(TEntityComp::*CBType)(T... args);
    CBType cb;

    GString get_comp_name() { return component_name; }
    GString component_name = "";
};

// entity component manager
template<class EntityClassType>
class EntityComponentManager : public RpcManagerBase {

public:
    EntityComponentManager() {
        EntityClassType::regist_components();
    }

    void component_regist(EntityComponentBase* component) {

        if (components.find(component->get_name()) != components.end()) {
            WARN_LOG("component.%s exist\n", component->get_name().c_str());
            return;
        }

        components[component->get_name()] = component;
    }

    void generate_entity_components(Entity* owner) {
        owner->components.clear();
        for (auto iter = components.begin(); iter != components.end(); ++iter) {
            owner->components[iter->first] = iter->second->create_self(owner);
        }
    }

    template<class TEntityComp, class... T, class... T2>
    void entity_comp_rpc_regist(enum RpcType entity_rpc_type, const GString& rpc_name, void(TEntityComp::*cb)(T...), T2... args) {
        ASSERT_LOG(sizeof...(T) == sizeof...(args), "rpc(%s) formal param size(%zu) != real param size(%zu)\n", rpc_name.c_str(), sizeof...(T), sizeof...(args));

        EntityCompRpcMethod<TEntityComp, T...>* method = new EntityCompRpcMethod<TEntityComp, T...>;
        method->component_name = TEntityComp::get_name();
        method->cb = cb;
        method->type = entity_rpc_type;
        method->rpc_real_params_parse(args...);

        RpcFormalParamsCheck<T...>();
        add_rpc_method(rpc_name, method);

        if (g_local_entity_rpc_names == nullptr)
            g_local_entity_rpc_names = new GArray;
        g_local_entity_rpc_names->push_back(rpc_name);
    }

    template<class TEntityComp, class... T>
    void entity_comp_rpc_regist(enum RpcType entity_rpc_type, const GString& rpc_name, void(TEntityComp::*cb)(T...)) {
        EntityCompRpcMethod<TEntityComp, T...>* method = new EntityCompRpcMethod<TEntityComp, T...>;
        method->component_name = TEntityComp::get_name();
        method->cb = cb;
        method->type = entity_rpc_type;
        add_rpc_method(rpc_name, method);

        if (g_local_entity_rpc_names == nullptr)
            g_local_entity_rpc_names = new GArray;
        g_local_entity_rpc_names->push_back(rpc_name);
    }

    void rpc_call(bool from_client, const GString& rpc_name, const GArray& params) {

        RpcMethodBase* method = find_rpc_method(rpc_name);
        const auto& comp_name = method->get_comp_name();

        auto iter = components.find(comp_name);
        if (iter == components.end()) {
            return;
        }

        iter->second->rpc_call(from_client, rpc_name, params);
    }

    template<class TEntityComp, class... T>
    void entity_comp_rpc_call(TEntityComp* comp, const GString& rpc_name, T... args) {

        RpcMethodBase* method = find_rpc_method(rpc_name);
        if (sizeof...(args) != method->m_params_t.size()) {
            ERROR_LOG("entity comp rpc.%s args num.%zd error, must be %zd\n", rpc_name.c_str(), sizeof...(args), method->m_params_t.size());
            return;
        }

        EntityCompRpcMethod<TEntityComp, T...>* entity_comp_method = (EntityCompRpcMethod<TEntityComp, T...>*)method;
        auto _cb = entity_comp_method->cb;
        (comp->*_cb)(args...);
    }

    unordered_map<GString, EntityComponentBase*> components;
};

#define COMP_RPC_CALL_DEFINE(TCLASS) \
void rpc_call(bool from_client, const GString& rpc_name, const GArray& params) { \
\
    auto method = get_owner()->get_comp_mgr()->find_rpc_method(rpc_name); \
    if (!method) { \
        ERROR_LOG("comp entity rpc.%s not exist\n", rpc_name.c_str()); \
        return; \
    } \
\
    if (from_client && method->type != RpcType::EXPOSED) { \
        ERROR_LOG("comp entity rpc.%s can not call from client\n", rpc_name.c_str()); \
        return; \
    } \
 \
    auto comp_mgr = (EntityComponentManager<TCLASS>*)get_comp_mgr(); \
    switch (params.size()) { \
    case 0: \
        comp_mgr->entity_comp_rpc_call(this, rpc_name); \
        break; \
    case 1: \
        comp_mgr->entity_comp_rpc_call(this, rpc_name, params[0]); \
        break; \
    case 2: \
        comp_mgr->entity_comp_rpc_call(this, rpc_name, params[0], params[1]); \
        break; \
    case 3: \
        comp_mgr->entity_comp_rpc_call(this, rpc_name, params[0], params[1], params[2]); \
        break; \
    case 4: \
        comp_mgr->entity_comp_rpc_call(this, rpc_name, params[0], params[1], params[2], params[3]); \
        break; \
    case 5: \
        comp_mgr->entity_comp_rpc_call(this, rpc_name, params[0], params[1], params[2], params[3], params[4]); \
        break; \
    case 6: \
        comp_mgr->entity_comp_rpc_call(this, rpc_name, params[0], params[1], params[2], params[3], params[4], params[5]); \
        break; \
    case 7: \
        comp_mgr->entity_comp_rpc_call(this, rpc_name, params[0], params[1], params[2], params[3], params[4], params[5], params[6]); \
        break; \
    case 8: \
        comp_mgr->entity_comp_rpc_call(this, rpc_name, params[0], params[1], params[2], params[3], params[4], params[5], params[6], params[7]); \
        break; \
    case 9: \
        comp_mgr->entity_comp_rpc_call(this, rpc_name, params[0], params[1], params[2], params[3], params[4], params[5], params[6], params[7], params[8]); \
        break; \
    case 10: \
        comp_mgr->entity_comp_rpc_call(this, rpc_name, params[0], params[1], params[2], params[3], params[4], params[5], params[6], params[7], params[8], params[9]); \
        break; \
    default: \
        break; \
    } \
}

#define COMP_RPC_METHOD(rpc_type, rpc_name, ...) TEntity::component_manager.regist_entity_comp_rpc(rpc_type, #rpc_name, &TEntityComp::rpc_name, __VA_ARGS__)

#define REGIST_COMPONENT(TEntity, TEntityComp) \
    component_manager.regist_component(new TEntityComp(TEntityComp::get_name(), nullptr)); \
    TEntityComp::rpc_method_define<TEntity, TEntityComp>(); \
    TEntityComp::property_define<TEntity, TEntityComp>();

#define GENERATE_COMPONENT_INNER(TEntityComp) \
public: \
    TEntityComp() = delete; \
    static GString get_name() { return #TEntityComp; } \
    template<class TEntity, class TEntityComp> \
    static void rpc_method_define(); \
    template<class TEntity, class TEntityComp> \
    static void property_define(); \
    EntityComponentBase* create_self(Entity* owner) { return new TEntityComp(TEntityComp::get_name(), owner); } \
    COMP_RPC_CALL_DEFINE(TEntityComp)
