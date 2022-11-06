#pragma once

#include "gvalue.h"
#include "rpc_manager.h"
#include "entity.h"

struct EntityPropertyBase;

template<class TEntityComp>
struct EntityCompRpcMethod0 : public RpcMethodBase {
    typedef void(TEntityComp::*CBType)();
    EntityCompRpcMethod0(CBType _cb) : cb(_cb) {}
    CBType cb;

    void decode(Decoder& decoder) {}
    void exec(void* _this) { (((TEntityComp*)_this)->*cb)(); }
    RpcMethodBase* create_self() { return new EntityCompRpcMethod0(cb); }
};

template<class TEntityComp, class T1>
struct EntityCompRpcMethod1 : public RpcMethodBase {
    typedef void(TEntityComp::*CBType)(T1);
    EntityCompRpcMethod1(CBType _cb) : cb(_cb) {}
    CBType cb;
    RMCVR(T1) t1;

    void decode(Decoder& decoder) {
        t1 = decoder.read<RMCVR(T1)>();
    }
    void exec(void* _this) { (((TEntityComp*)_this)->*cb)(t1); }
    RpcMethodBase* create_self() { return new EntityCompRpcMethod1(cb); }
};

template<class TEntityComp, class T1, class T2>
struct EntityCompRpcMethod2 : public RpcMethodBase {
    typedef void(TEntityComp::*CBType)(T1, T2);
    EntityCompRpcMethod2(CBType _cb) : cb(_cb) {}
    CBType cb;
    RMCVR(T1) t1;
    RMCVR(T2) t2;

    void decode(Decoder& decoder) {
        t1 = decoder.read<RMCVR(T1)>();
        t2 = decoder.read<RMCVR(T2)>();
    }
    void exec(void* _this) { (((TEntityComp*)_this)->*cb)(t1, t2); }
    RpcMethodBase* create_self() { return new EntityCompRpcMethod2(cb); }
};

template<class TEntityComp, class T1, class T2, class T3>
struct EntityCompRpcMethod3 : public RpcMethodBase {
    typedef void(TEntityComp::*CBType)(T1, T2, T3);
    EntityCompRpcMethod3(CBType _cb) : cb(_cb) {}
    CBType cb;
    RMCVR(T1) t1;
    RMCVR(T2) t2;
    RMCVR(T3) t3;

    void decode(Decoder& decoder) {
        t1 = decoder.read<RMCVR(T1)>();
        t2 = decoder.read<RMCVR(T2)>();
        t3 = decoder.read<RMCVR(T3)>();
    }
    void exec(void* _this) { (((TEntityComp*)_this)->*cb)(t1, t2, t3); }
    RpcMethodBase* create_self() { return new EntityCompRpcMethod3(cb); }
};

template<class TEntityComp, class T1, class T2, class T3, class T4>
struct EntityCompRpcMethod4 : public RpcMethodBase {
    typedef void(TEntityComp::*CBType)(T1, T2, T3, T4);
    EntityCompRpcMethod4(CBType _cb) : cb(_cb) {}
    CBType cb;
    RMCVR(T1) t1;
    RMCVR(T2) t2;
    RMCVR(T3) t3;
    RMCVR(T4) t4;

    void decode(Decoder& decoder) {
        t1 = decoder.read<RMCVR(T1)>();
        t2 = decoder.read<RMCVR(T2)>();
        t3 = decoder.read<RMCVR(T3)>();
        t4 = decoder.read<RMCVR(T4)>();
    }
    void exec(void* _this) { (((TEntityComp*)_this)->*cb)(t1, t2, t3, t4); }
    RpcMethodBase* create_self() { return new EntityCompRpcMethod4(cb); }
};

template<class TEntityComp, class T1, class T2, class T3, class T4, class T5>
struct EntityCompRpcMethod5 : public RpcMethodBase {
    typedef void(TEntityComp::*CBType)(T1, T2, T3, T4, T5);
    EntityCompRpcMethod5(CBType _cb) : cb(_cb) {}
    CBType cb;
    RMCVR(T1) t1;
    RMCVR(T2) t2;
    RMCVR(T3) t3;
    RMCVR(T4) t4;
    RMCVR(T5) t5;

    void decode(Decoder& decoder) {
        t1 = decoder.read<RMCVR(T1)>();
        t2 = decoder.read<RMCVR(T2)>();
        t3 = decoder.read<RMCVR(T3)>();
        t4 = decoder.read<RMCVR(T4)>();
        t5 = decoder.read<RMCVR(T5)>();
    }
    void exec(void* _this) { (((TEntityComp*)_this)->*cb)(t1, t2, t3, t4, t5); }
    RpcMethodBase* create_self() { return new EntityCompRpcMethod5(cb); }
};


class EntityComponentBase {
public:
    EntityComponentBase() {}
    virtual ~EntityComponentBase();
    virtual EntityComponentBase* create_self(Entity* owner) = 0;

    void set_name(GString _name) { name = _name; }
    GString& get_name() { return name; }

    void set_owner(Entity* _owner) { owner = _owner; }
    Entity* get_owner() { return owner; }

    virtual void rpc_call(bool from_client, const GString& rpc_name, RpcMethodBase* rpc_method) = 0;
    EntityPropertyBase* get_prop(const GString& prop_name) const;

protected:
    GString name;
    Entity* owner;
};


class ComponentManagerBase {
public:
    virtual void rpc_call(Entity* entity, bool from_client, const GString& rpc_name, RpcMethodBase* rpc_method) = 0;
};

extern GArray* get_local_entity_rpc_names();

// entity component manager
template<class EntityClassType>
class EntityComponentManager : public ComponentManagerBase {

public:
    EntityComponentManager() {
        rpc_mgr = &(EntityClassType::rpc_manager);
        EntityClassType::regist_components();
    }

    GString rpc_name_decode(Decoder& decoder) { return ""; }
    void rpc_name_encode(Encoder& encoder, const GString& rpc_name) {}


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

    template<class TEntityComp>
    void entity_comp_rpc_regist(enum RpcType entity_rpc_type, const GString& rpc_name, void(TEntityComp::*cb)()) {
        RpcMethodBase* method = new EntityCompRpcMethod0<TEntityComp>(cb);
        method->component_name = TEntityComp::get_name();
        method->type = entity_rpc_type;
        rpc_mgr->add_rpc_method(rpc_name, method);
        get_local_entity_rpc_names()->push_back(rpc_name);
    }

    template<class TEntityComp, class T1>
    void entity_comp_rpc_regist(enum RpcType entity_rpc_type, const GString& rpc_name, void(TEntityComp::*cb)(T1)) {
        RpcMethodBase* method = new EntityCompRpcMethod1<TEntityComp, T1>(cb);
        method->component_name = TEntityComp::get_name();
        method->type = entity_rpc_type;
        rpc_mgr->add_rpc_method(rpc_name, method);
        get_local_entity_rpc_names()->push_back(rpc_name);
    }

    template<class TEntityComp, class T1, class T2>
    void entity_comp_rpc_regist(enum RpcType entity_rpc_type, const GString& rpc_name, void(TEntityComp::*cb)(T1, T2)) {
        RpcMethodBase* method = new EntityCompRpcMethod2<TEntityComp, T1, T2>(cb);
        method->component_name = TEntityComp::get_name();
        method->type = entity_rpc_type;
        rpc_mgr->add_rpc_method(rpc_name, method);
        get_local_entity_rpc_names()->push_back(rpc_name);
    }

    template<class TEntityComp, class T1, class T2, class T3>
    void entity_comp_rpc_regist(enum RpcType entity_rpc_type, const GString& rpc_name, void(TEntityComp::*cb)(T1, T2, T3)) {
        RpcMethodBase* method = new EntityCompRpcMethod3<TEntityComp, T1, T2, T3>(cb);
        method->component_name = TEntityComp::get_name();
        method->type = entity_rpc_type;
        rpc_mgr->add_rpc_method(rpc_name, method);
        get_local_entity_rpc_names()->push_back(rpc_name);
    }

    template<class TEntityComp, class T1, class T2, class T3, class T4>
    void entity_comp_rpc_regist(enum RpcType entity_rpc_type, const GString& rpc_name, void(TEntityComp::*cb)(T1, T2, T3, T4)) {
        RpcMethodBase* method = new EntityCompRpcMethod4<TEntityComp, T1, T2, T3, T4>(cb);
        method->component_name = TEntityComp::get_name();
        method->type = entity_rpc_type;
        rpc_mgr->add_rpc_method(rpc_name, method);
        get_local_entity_rpc_names()->push_back(rpc_name);
    }

    template<class TEntityComp, class T1, class T2, class T3, class T4, class T5>
    void entity_comp_rpc_regist(enum RpcType entity_rpc_type, const GString& rpc_name, void(TEntityComp::*cb)(T1, T2, T3, T4, T5)) {
        RpcMethodBase* method = new EntityCompRpcMethod5<TEntityComp, T1, T2, T3, T4, T5>(cb);
        method->component_name = TEntityComp::get_name();
        method->type = entity_rpc_type;
        rpc_mgr->add_rpc_method(rpc_name, method);
        get_local_entity_rpc_names()->push_back(rpc_name);
    }

    void rpc_call(Entity* entity, bool from_client, const GString& rpc_name, RpcMethodBase* rpc_method) {

        RpcMethodBase* method = rpc_mgr->find_rpc_method(rpc_name);
        const auto& comp_name = method->get_comp_name();

        auto iter = entity->components.find(comp_name);
        if (iter == entity->components.end()) {
            return;
        }

        iter->second->rpc_call(from_client, rpc_name, rpc_method);
    }

    unordered_map<GString, EntityComponentBase*> components;
    RpcManagerBase* rpc_mgr;
};

#define COMP_RPC_CALL_DEFINE(TCLASS) \
void rpc_call(bool from_client, const GString& rpc_name, RpcMethodBase* rpc_method) { \
\
    auto method = get_owner()->rpc_mgr->find_rpc_method(rpc_name); \
    if (!method) { \
        ERROR_LOG("comp entity rpc.%s not exist\n", rpc_name.c_str()); \
        return; \
    } \
\
    if (from_client && method->type != RpcType::EXPOSED) { \
        ERROR_LOG("comp entity rpc.%s can not call from client\n", rpc_name.c_str()); \
        return; \
    } \
    rpc_method->exec((void*)this); \
}


#define COMP_RPC_METHOD(rpc_type, rpc_name) TEntity::component_manager.entity_comp_rpc_regist(rpc_type, #rpc_name, &TEntityComp::rpc_name)


#define REGIST_COMPONENT(TEntity, TEntityComp) \
    TEntityComp* component = new TEntityComp; \
    component->set_name(TEntityComp::get_name()); \
    component->set_owner(nullptr); \
    component_manager.component_regist(component); \
    TEntityComp::rpc_method_define<TEntity, TEntityComp>();

#define GENERATE_COMPONENT_INNER(TEntityComp) \
public: \
    static GString get_name() { return #TEntityComp; } \
    EntityComponentBase* create_self(Entity* owner) { \
        TEntityComp* component = new TEntityComp; \
        component->set_name(TEntityComp::get_name()); \
        component->set_owner(owner); \
        return component; \
    } \
    COMP_RPC_CALL_DEFINE(TEntityComp)
