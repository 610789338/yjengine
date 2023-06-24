#pragma once

#include "log.h"
#include "rpc_manager.h"
#include "remote_manager.h"
#include "boost_asio.h"
#include "gvalue.h"
#include "ini.h"
#include "mailbox.h"
#include "entity.h"
#include "entity_rpc_manager.h"
#include "entity_property_manager.h"
#include "entity_component_manager.h"

// ------------------------------- definition ------------------------------- //


#define GENERATE_ENTITY_INNER(TCLASS) \
public: \
    EntityType get_entity_type() { return (EntityType)TCLASS::ENTITY_TYPE; } \
    static EntityPropertyManager<TCLASS> property_manager; \
    static EntityRpcManager<TCLASS> rpc_manager; \
    RpcManagerBase* get_rpc_mgr() { return &rpc_manager; } \
    RpcMethodBase* find_rpc_method(const GString& rpc_name) { return rpc_manager.find_rpc_method(rpc_name); } \
    RPC_METHOD_DEFINE_INSIDE(TCLASS); \
    RPC_CALL_DEFINE(TCLASS); \
    static EntityComponentManager<TCLASS> component_manager; \
    ComponentManagerBase* get_comp_mgr() { return &component_manager; } \
    static TimerManager<TCLASS> timer_manager; \
    TimerManagerBase* get_timer_manager() { return &timer_manager; } \
    MIGRATE_TIMER_DEF_INSIDE(TCLASS); \
    static PropertyTree property_tree; \
    static void generate_property_tree(); \
    int16_t prop_str2int(const GString& prop_name) { return property_manager.s2i_map.at(prop_name); } \
    GString prop_int2str(int16_t idx) { return property_manager.i2s_map.at(idx); } \
    void create_dbsave_timer() { REGIST_TIMER(get_db_save_interval(), get_db_save_interval(), true, time_to_save); } \
    void time_to_save() { real_time_to_save(); } \
    void create_disater_backup_timer() { \
        REGIST_TIMER(1,                      get_db_save_interval(), false, time_to_disaster_backup); \
        REGIST_TIMER(get_db_save_interval(), get_db_save_interval(), true,  time_to_disaster_backup); \
    } \
    void time_to_disaster_backup() { real_time_to_disaster_backup(); } \
    EventManager<TCLASS> event_manager; \
    EventManagerBase* get_event_manager() { return &event_manager; } \
    GString get_entity_name() { return #TCLASS; }

#define GENERATE_ENTITY_OUT(TCLASS) \
EntityPropertyManager<TCLASS> TCLASS::property_manager; \
EntityRpcManager<TCLASS> TCLASS::rpc_manager(#TCLASS, (EntityType)TCLASS::ENTITY_TYPE, []()->TCLASS* { \
    auto entity = new TCLASS(); \
    entity->give_propertys(property_manager.propertys); \
    component_manager.generate_entity_components(entity); \
    entity->rpc_mgr = &TCLASS::rpc_manager; \
    return entity; }); \
TimerManager<TCLASS> TCLASS::timer_manager; \
EntityComponentManager<TCLASS> TCLASS::component_manager; \
PropertyTree TCLASS::property_tree(property_manager.propertys);

extern void set_engine_listen_ipport(GString ip, uint16_t port);
extern void engine_init();
extern void engine_tick();
