#pragma once

#include "gvalue.h"
#include "timer.h"
#include "entity_rpc_manager.h"
#include "event.h"
#include "mailbox.h"

struct EntityPropertyBase;
class EventManagerBase;

class EntityComponentBase {
public:
    EntityComponentBase() {}
    virtual ~EntityComponentBase();
    virtual EntityComponentBase* create_self(Entity* owner) = 0;
    virtual void on_ready() {}

    void set_name(GString _name) { name = _name; }
    GString& get_name() { return name; }

    void set_owner(Entity* _owner) { owner = _owner; }
    Entity* get_owner() { return owner; }

    virtual void rpc_call(bool from_client, const GString& rpc_name, RpcMethodBase* rpc_method) = 0;
    EntityPropertyBase* get_prop(const GString& prop_name) const;
    EntityPropertyBase& get_prop_ref(const GString& prop_name) const;

    // event
    virtual EventManagerBase* get_event_manager() { return nullptr; }
    bool is_comp() { return true; }

    template<class... Args>
    void send_event(const GString& event_name, Args... args) {
        get_owner()->get_event_manager()->send_event(get_owner(), event_name, args...);
    }

    // mailbox
    BaseMailBox& Base();
    CellMailBox& Cell();
    ClientMailBox& Client();

protected:
    GString name;
    Entity* owner;
};


class ComponentManagerBase {
public:
    ComponentManagerBase() {}
    virtual ~ComponentManagerBase() {}

    template<class TEntityComp>
    void entity_comp_rpc_regist(enum RpcType entity_rpc_type, const GString& rpc_name, void(TEntityComp::*cb)()) {
        RpcMethodBase* method = new EntityRpcMethod0<TEntityComp>(cb);
        method->component_name = TEntityComp::get_name();
        method->type = entity_rpc_type;
        get_rpc_mgr()->add_rpc_method(rpc_name, method);
        get_local_entity_rpc_names()->push_back(rpc_name);
    }

    template<class TEntityComp, class T1>
    void entity_comp_rpc_regist(enum RpcType entity_rpc_type, const GString& rpc_name, void(TEntityComp::*cb)(T1)) {
        RpcMethodBase* method = new EntityRpcMethod1<TEntityComp, T1>(cb);
        method->component_name = TEntityComp::get_name();
        method->type = entity_rpc_type;
        get_rpc_mgr()->add_rpc_method(rpc_name, method);
        get_local_entity_rpc_names()->push_back(rpc_name);
    }

    template<class TEntityComp, class T1, class T2>
    void entity_comp_rpc_regist(enum RpcType entity_rpc_type, const GString& rpc_name, void(TEntityComp::*cb)(T1, T2)) {
        RpcMethodBase* method = new EntityRpcMethod2<TEntityComp, T1, T2>(cb);
        method->component_name = TEntityComp::get_name();
        method->type = entity_rpc_type;
        get_rpc_mgr()->add_rpc_method(rpc_name, method);
        get_local_entity_rpc_names()->push_back(rpc_name);
    }

    template<class TEntityComp, class T1, class T2, class T3>
    void entity_comp_rpc_regist(enum RpcType entity_rpc_type, const GString& rpc_name, void(TEntityComp::*cb)(T1, T2, T3)) {
        RpcMethodBase* method = new EntityRpcMethod3<TEntityComp, T1, T2, T3>(cb);
        method->component_name = TEntityComp::get_name();
        method->type = entity_rpc_type;
        get_rpc_mgr()->add_rpc_method(rpc_name, method);
        get_local_entity_rpc_names()->push_back(rpc_name);
    }

    template<class TEntityComp, class T1, class T2, class T3, class T4>
    void entity_comp_rpc_regist(enum RpcType entity_rpc_type, const GString& rpc_name, void(TEntityComp::*cb)(T1, T2, T3, T4)) {
        RpcMethodBase* method = new EntityRpcMethod4<TEntityComp, T1, T2, T3, T4>(cb);
        method->component_name = TEntityComp::get_name();
        method->type = entity_rpc_type;
        get_rpc_mgr()->add_rpc_method(rpc_name, method);
        get_local_entity_rpc_names()->push_back(rpc_name);
    }

    template<class TEntityComp, class T1, class T2, class T3, class T4, class T5>
    void entity_comp_rpc_regist(enum RpcType entity_rpc_type, const GString& rpc_name, void(TEntityComp::*cb)(T1, T2, T3, T4, T5)) {
        RpcMethodBase* method = new EntityRpcMethod5<TEntityComp, T1, T2, T3, T4, T5>(cb);
        method->component_name = TEntityComp::get_name();
        method->type = entity_rpc_type;
        get_rpc_mgr()->add_rpc_method(rpc_name, method);
        get_local_entity_rpc_names()->push_back(rpc_name);
    }

    virtual void component_regist(EntityComponentBase* component);
    virtual void generate_entity_components(Entity* owner);
    virtual void rpc_call(Entity* entity, bool from_client, const GString& rpc_name, RpcMethodBase* rpc_method);
    virtual RpcManagerBase* get_rpc_mgr() { return nullptr; }

#define GENERATE_COMP_TIMER_BODY \
        int64_t start_time = nowms_timestamp() + int64_t(start * 1000); \
        timer->m_id = entity_comp->get_owner()->next_timer_id++; \
        timer->m_interval = (float)interval; \
        timer->m_repeat = repeat; \
        timer->m_start_time = start_time; \
        timer->m_expiration = start_time; \
        timer->m_cb_name = cb_name; \
        timer->m_component_name = TEntityComp::get_name();

    template<class TEntityComp>
    Timer0<TEntityComp>* regist_timer(TEntityComp* entity_comp, double start, double interval, bool repeat, const GString& cb_name, void(TEntityComp::*cb)()) {
        auto timer = new Timer0<TEntityComp>(entity_comp, cb);
        GENERATE_COMP_TIMER_BODY;
        entity_comp->get_owner()->add_timer(timer);
        return timer;
    }

    template<class TEntityComp, class T1>
    Timer1<TEntityComp, T1>* regist_timer(TEntityComp* entity_comp, double start, double interval, bool repeat, const GString& cb_name, void(TEntityComp::*cb)(T1)) {
        auto timer = new Timer1<TEntityComp, T1>(entity_comp, cb);
        GENERATE_COMP_TIMER_BODY;
        entity_comp->get_owner()->add_timer(timer);
        return timer;
    }

    template<class TEntityComp, class T1, class T2>
    Timer2<TEntityComp, T1, T2>* regist_timer(TEntityComp* entity_comp, double start, double interval, bool repeat, const GString& cb_name, void(TEntityComp::*cb)(T1, T2)) {
        auto timer = new Timer2<TEntityComp, T1, T2>(entity_comp, cb);
        GENERATE_COMP_TIMER_BODY;
        entity_comp->get_owner()->add_timer(timer);
        return timer;
    }

    template<class TEntityComp, class T1, class T2, class T3>
    Timer3<TEntityComp, T1, T2, T3>* regist_timer(TEntityComp* entity_comp, double start, double interval, bool repeat, const GString& cb_name, void(TEntityComp::*cb)(T1, T2, T3)) {
        auto timer = new Timer3<TEntityComp, T1, T2, T3>(entity_comp, cb);
        GENERATE_COMP_TIMER_BODY;
        entity_comp->get_owner()->add_timer(timer);
        return timer;
    }

    template<class TEntityComp, class T1, class T2, class T3, class T4>
    Timer4<TEntityComp, T1, T2, T3, T4>* regist_timer(TEntityComp* entity_comp, double start, double interval, bool repeat, const GString& cb_name, void(TEntityComp::*cb)(T1, T2, T3, T4)) {
        auto timer = new Timer4<TEntityComp, T1, T2, T3, T4>(entity_comp, cb);
        GENERATE_COMP_TIMER_BODY;
        entity_comp->get_owner()->add_timer(timer);
        return timer;
    }

    template<class TEntityComp, class T1, class T2, class T3, class T4, class T5>
    Timer5<TEntityComp, T1, T2, T3, T4, T5>* regist_timer(TEntityComp* entity_comp, double start, double interval, bool repeat, const GString& cb_name, void(TEntityComp::*cb)(T1, T2, T3, T4, T5)) {
        auto timer = new Timer5<TEntityComp, T1, T2, T3, T4, T5>(entity_comp, cb);
        GENERATE_COMP_TIMER_BODY;
        entity_comp->get_owner()->add_timer(timer);
        return timer;
    }

    template<class TEntityComp>
    void regist_event(TEntityComp* entity_comp, const GString& event_name, void(TEntityComp::*cb)()) {
        auto event = new Event0<TEntityComp>(entity_comp, cb);
        event->m_event_name = event_name;
        event->m_component_name = TEntityComp::get_name();
        entity_comp->get_owner()->add_event(event);
    }

    template<class TEntityComp, class T1>
    void regist_event(TEntityComp* entity_comp, const GString& event_name, void(TEntityComp::*cb)(T1)) {
        auto event = new Event1<TEntityComp, T1>(entity_comp, cb);
        event->m_event_name = event_name;
        event->m_component_name = TEntityComp::get_name();
        entity_comp->get_owner()->add_event(event);
    }

    template<class TEntityComp, class T1, class T2>
    void regist_event(TEntityComp* entity_comp, const GString& event_name, void(TEntityComp::*cb)(T1, T2)) {
        auto event = new Event2<TEntityComp, T1, T2>(entity_comp, cb);
        event->m_event_name = event_name;
        event->m_component_name = TEntityComp::get_name();
        entity_comp->get_owner()->add_event(event);
    }

    template<class TEntityComp, class T1, class T2, class T3>
    void regist_event(TEntityComp* entity_comp, const GString& event_name, void(TEntityComp::*cb)(T1, T2, T3)) {
        auto event = new Event3<TEntityComp, T1, T2, T3>(entity_comp, cb);
        event->m_event_name = event_name;
        event->m_component_name = TEntityComp::get_name();
        entity_comp->get_owner()->add_event(event);
    }

    template<class TEntityComp, class T1, class T2, class T3, class T4>
    void regist_event(TEntityComp* entity_comp, const GString& event_name, void(TEntityComp::*cb)(T1, T2, T3, T4)) {
        auto event = new Event4<TEntityComp, T1, T2, T3, T4>(entity_comp, cb);
        event->m_event_name = event_name;
        event->m_component_name = TEntityComp::get_name();
        entity_comp->get_owner()->add_event(event);
    }

    template<class TEntityComp, class T1, class T2, class T3, class T4, class T5>
    void regist_event(TEntityComp* entity_comp, const GString& event_name, void(TEntityComp::*cb)(T1, T2, T3, T4, T5)) {
        auto event = new Event5<TEntityComp, T1, T2, T3, T4, T5>(entity_comp, cb);
        event->m_event_name = event_name;
        event->m_component_name = TEntityComp::get_name();
        entity_comp->get_owner()->add_event(event);
    }

    unordered_map<GString, EntityComponentBase*> components;
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

    RpcManagerBase* get_rpc_mgr() { return rpc_mgr; }
    GString rpc_name_decode(Decoder& decoder) { return ""; }
    void rpc_name_encode(Encoder& encoder, const GString& rpc_name) {}

    RpcManagerBase* rpc_mgr = nullptr;

    EntityClassType* tclass;
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
        ERROR_LOG("entity comp rpc.%s can not call from client\n", rpc_name.c_str()); \
        return; \
    } \
\
    if (g_is_server && method->type == RpcType::CLIENT) { \
        ERROR_LOG("entity comp rpc.%s can only call to client\n", rpc_name.c_str()); \
        return; \
    }\
    rpc_method->exec((void*)this); \
}

#define COMP_RPC_METHOD(rpc_type, rpc_name) \
    TEntity::component_manager.entity_comp_rpc_regist(rpc_type, #rpc_name, &RMP(decltype(youjun))::rpc_name)
#define COMP_REGIST_TIMER(start, interval, repeat, cb, ...) \
    get_owner()->get_comp_mgr()->regist_timer(this, start, interval, repeat, #cb, &RMP(decltype(youjun))::cb)->set_args(__VA_ARGS__)
#define COMP_MIGRATE_TIMER_DEF(cb) \
    TEntity::timer_manager.store_timer_cb_for_migrate((GString(#cb) + TEntityComp::get_name()).c_str(), &RMP(decltype(youjun))::cb);

#define COMP_REGIST_EVENT(event) \
    get_owner()->get_comp_mgr()->regist_event(this, #event, &RMP(decltype(youjun))::event);
#define COMP_MIGRATE_EVENT_DEF(event) \
    TEntity::event_manager.store_event_cb_for_migrate((GString(#event) + TEntityComp::get_name()).c_str(), &RMP(decltype(youjun))::event)


#define REGIST_COMPONENT(TEntity, TEntityComp) \
    TEntityComp* component_##TEntityComp = new TEntityComp; \
    component_##TEntityComp->set_name(TEntityComp::get_name()); \
    component_##TEntityComp ->set_owner(nullptr); \
    component_manager.component_regist(component_##TEntityComp ); \
    TEntityComp::rpc_method_define<TEntity>(); \
    TEntityComp::property_define<TEntity>(); \
    TEntityComp::migrate_timer_define<TEntity, TEntityComp>(); \
    TEntityComp::migrate_event_define<TEntity, TEntityComp>();

#define GENERATE_COMPONENT_INNER(TEntityComp) \
public: \
    static GString get_name() { return #TEntityComp; } \
    EntityComponentBase* create_self(Entity* owner) { \
        TEntityComp* component = new TEntityComp; \
        component->set_name(TEntityComp::get_name()); \
        component->set_owner(owner); \
        return component; \
    } \
    COMP_RPC_CALL_DEFINE(TEntityComp) \
    TEntityComp* youjun;

#define COMP_RPC_DEFINE template<class TEntity> static void rpc_method_define
#define COMP_PROPERTY_DEFINE template<class TEntity> static void property_define
#define COMP_MIGRATE_TIMER_DEFINE template<class TEntity, class TEntityComp> static void migrate_timer_define
#define COMP_MIGRATE_EVENT_DEFINE template<class TEntity, class TEntityComp> static void migrate_event_define
