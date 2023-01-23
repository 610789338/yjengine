#pragma once

#include <stdint.h>
#include <unordered_map>
#include <unordered_set>

#include "gvalue.h"
#include "boost_asio.h"
#include "mailbox.h"
#include "timer.h"
#include "entity_rpc_manager.h"
#include "entity_property_tree.h"
#include "entity_property_manager.h"
#include "time.h"
#include "entity_component_manager.h"

using namespace std;

class Entity;
struct RpcMethodBase;
class RpcManagerBase;
class ComponentManagerBase;
class EntityComponentBase;
struct EntityPropertyBase;
struct TimerBase;
class TimerManagerBase;
class EventManagerBase;

enum EntityType {
    EntityType_Base,  // stub
    EntityType_BaseWithCell,  // monster
    EntityType_BaseWithCellAndClient,  // player
    EntityType_Cell,  // monster
    EntityType_CellWithClient,  // player
    EntityType_Client,  // player

    EntityType_None
};

enum MigrateState {
    Migrate_None,
    Migrate_Prepare,
    Migrate_RealMigrate,
};

class Entity {
public:
    enum {
        ENTITY_TYPE = EntityType::EntityType_None
    };

public:
    Entity() {}
    virtual ~Entity() { 
        release_component(); 
        release_property(); 
        release_timer(); 
    }

    void tick();

    virtual EntityType get_entity_type() { ASSERT(false); return EntityType::EntityType_None; }
    virtual void on_create(const GDict& create_data);
    virtual void ready_check_timer();
    virtual void on_reconnect_fromclient(const GString& client_addr, const GString& gate_addr) { ASSERT(false); } // for base and cell
    virtual void on_reconnect_success(const GDict& create_data) { ASSERT(false); } // for client
    virtual void on_destroy() = 0;
    virtual void rpc_call(bool from_client, const GString& rpc_name, RpcMethodBase* rpc_method) = 0;
    virtual void destroy_self();

    virtual RpcMethodBase* find_rpc_method(const GString& rpc_name) = 0;
    virtual RpcManagerBase* get_rpc_mgr() { return nullptr; }
    virtual ComponentManagerBase* get_comp_mgr() { return nullptr; }

    EntityPropertyBase* get_prop(const GString& prop_name) const { return propertys.at(prop_name); }
    EntityComponentBase* get_component(const GString& componet_name) const { return components.at(componet_name); }

    void release_component();
    void release_property();
    void release_timer();

    // proppertys
    virtual void propertys_sync2client(bool force_all = false);
    void serialize_all(Encoder& encoder);
    void serialize_client(Encoder& encoder, bool force_all = false);
    void serialize_db(Encoder& encoder);
    virtual int16_t prop_str2int(const GString& prop_name) { return 0; }
    virtual GString prop_int2str(int16_t idx) { return ""; }
    bool need_create_save_timer();
    virtual void create_dbsave_timer() { ASSERT(false); }
    double get_db_save_interval();
    virtual void real_time_to_save() { ASSERT(false); }

    void give_propertys(unordered_map<GString, EntityPropertyBase*>& propertys);
    void ready();
    virtual void on_ready() {}
    virtual void propertys_unserialize(Decoder& decoder);

    // timer
    virtual TimerManagerBase* get_timer_manager() { ASSERT(false); return nullptr; }
    void timer_tick();
    void cancel_timer(TimerID time_id);
    void add_timer(TimerBase* timer);
    void remove_timer(TimerBase* timer);

    // migrate
    virtual void on_migrate_out(GDict& migrate_data) { ASSERT(false); }
    virtual void on_migrate_in(const GDict& migrate_data) { ASSERT(false); }
    virtual void packet_migrate_data(const GDict& migrate_data) { ASSERT(false); }
    virtual void unpacket_migrate_data(const GDict& migrate_data) { ASSERT(false); }

    // event
    virtual EventManagerBase* get_event_manager() { return nullptr; }
    Entity* get_owner() { return nullptr; }
    void comp_regist_event(const GString& event_name, void* component) {
        auto iter = event_components.find(event_name);
        if (iter == event_components.end()) {
            event_components[event_name] = unordered_set<EntityComponentBase*>();
            iter = event_components.find(event_name);
        }

        if (iter->second.find((EntityComponentBase*)component) == iter->second.end()) {
            iter->second.insert((EntityComponentBase*)component);
        }
    }

    template<class... Args>
    void send_event(const GString& event_name, Args... args) {
        get_event_manager()->send_event(this, event_name, args...);
        auto iter = event_components.find(event_name);
        if (iter != event_components.end()) {
            for (auto iter_inner = iter->second.begin(); iter_inner != iter->second.end(); ++iter_inner) {
                (*iter_inner)->get_event_manager()->send_event(*iter_inner, event_name, args...);
            }
        }
    }

    // disaster backup
    virtual void on_game_disappear(const GString& game_addr) {}
    virtual void recover_by_disaster_backup(const GString& addr, const GString& client_addr, const GString& client_gate_addr) { ASSERT(false); }

    GString uuid = "";
    GString class_name = "";

    MAP<GString, EntityPropertyBase*> propertys;
    unordered_map<EntityPropertyBase*, GString> propertys_turn;
    unordered_map<GString, EntityPropertyBase*> dirty_propertys;
    unordered_map<GString, EntityComponentBase*> components;
    unordered_map<GString, unordered_set<EntityComponentBase*>> event_components;

    multiset<TimerBase*, TimerCompare> timers;  // 不能用set，否则过期时间一样会被认为是重复key
    TimerID next_timer_id = 1;
    unordered_map<TimerID, TimerBase*> timer_ids;
    TimerID ready_check_timerid = 0;

    RpcManagerBase* rpc_mgr;
    bool is_ready = false;
    bool need_destroy = false;
};

// ------------------------------- base ------------------------------- //
class BaseEntity : public Entity {
public:
    enum{
        ENTITY_TYPE = EntityType::EntityType_Base
    };

public:
    BaseEntity() {}
    virtual ~BaseEntity() {}

    virtual void on_create(const GDict& create_data);
    virtual void ready() { Entity::ready(); }  // must exist
    virtual void on_destroy() {}
};

class BaseEntityWithCell : public BaseEntity {
public:
    enum {
        ENTITY_TYPE = EntityType::EntityType_BaseWithCell
    };

public:
    BaseEntityWithCell() { cell.set_owner(this); }
    virtual ~BaseEntityWithCell() {}

    virtual void on_create(const GDict& create_data);
    virtual void create_cell(const GDict& create_data);
    virtual void on_cell_create(const GString& cell_addr);
    virtual void ready() { Entity::ready(); } // must exist
    virtual void on_destroy() {}

    // migrate
    virtual void migrate_req_from_cell();
    virtual void new_cell_migrate_in(const GString& new_cell_addr);

    CellMailBox cell;
    GBin disaster_backup_of_self;
    GBin disaster_backup_of_cell;
    GDict disaster_backup_of_cell_migrate_data;
};

class BaseEntityWithCellAndClient : public BaseEntityWithCell {
public:
    enum {
        ENTITY_TYPE = EntityType::EntityType_BaseWithCellAndClient
    };

public:
    BaseEntityWithCellAndClient() { client.set_owner(this); }
    virtual ~BaseEntityWithCellAndClient() {}

    virtual void on_create(const GDict& create_data);
    virtual void ready_check_timer();
    virtual void on_cell_create(const GString& cell_addr);
    virtual void create_client();
    virtual void ready(); // must exist
    virtual void destroy_self();
    virtual void on_reconnect_fromclient(const GString& client_addr, const GString& gate_addr);
    virtual void on_client_reconnect_success();
    virtual void on_destroy() {}
    virtual void propertys_sync2client(bool force_all);
    virtual void kick_client();

    virtual void real_time_to_save();

    void new_cell_migrate_in(const GString& new_cell_addr);

    // migrate
    void migrate_req_from_cell() { BaseEntityWithCell::migrate_req_from_cell(); }

    // disaster backup
    virtual void base_disaster_backup(const GBin& cell_all_db, const GDict& migrate_data);
    virtual void on_game_disappear(const GString& game_addr);
    virtual void recover_by_disaster_backup(const GString& cell_addr, const GString& client_addr, const GString& client_gate_addr);
    virtual void cell_recover_by_disaster_backup_success(const GString& cell_addr);

    ClientMailBox client;
};


// ------------------------------- cell ------------------------------- //
class CellEntity : public Entity {
public:
    enum {
        ENTITY_TYPE = EntityType::EntityType_Cell
    };

public:
    CellEntity() { base.set_owner(this); }
    virtual ~CellEntity() {}

    virtual void on_create(const GDict& create_data) {}
    virtual void ready() { Entity::ready(); } // must exist
    virtual void on_destroy() {}

    // migrate
    virtual void begin_migrate(const GString& new_addr);
    virtual void migrate_reqack_from_base(bool is_ok);
    virtual void real_begin_migrate();
    virtual void migrate_entity();
    virtual void on_new_cell_migrate_finish();

    BaseMailBox base;

    MigrateState migrate_state = Migrate_None;
    GString new_cell_addr = "";
    bool is_reqack_from_base = false;
    GDict migrate_timers;

    GBin  disaster_backup_of_base;
    GBin  disaster_backup_of_self;
    GDict disaster_backup_of_self_migrate_data;
};

class CellEntityWithClient : public CellEntity {
public:
    enum {
        ENTITY_TYPE = EntityType::EntityType_CellWithClient
    };

public:
    CellEntityWithClient() {
        client.set_owner(this);
    }
    virtual ~CellEntityWithClient() {}

    virtual void on_create(const GDict& create_data);
    virtual void ready_check_timer();
    virtual void ready(); // must exist
    virtual void on_reconnect_fromclient(const GString& client_addr, const GString& gate_addr);
    virtual void on_client_reconnect_success();
    virtual void destroy_self();
    virtual void on_destroy() {}
    virtual void propertys_sync2client(bool force_all);

    // migrate
    virtual void begin_migrate(const GString& new_addr);
    virtual void migrate_reqack_from_base(bool is_ok);
    virtual void migrate_reqack_from_client(bool is_ok);
    virtual void real_begin_migrate();
    virtual void on_migrate_out(GDict& migrate_data);
    virtual void packet_migrate_data(GDict& migrate_data);
    virtual void on_migrate_in(const GDict& migrate_data);
    virtual void unpacket_migrate_data(const GDict& migrate_data);
    virtual void on_new_cell_migrate_finish();

    void migrate_check_timer();

    // db save
    void cell_real_time_to_save(const GString& base_uuid, const GBin& base_bin);

    // disaster backup
    virtual void cell_disaster_backup(const GBin& base_all_db);
    virtual void on_game_disappear(const GString& game_addr);
    virtual void recover_by_disaster_backup(const GString& base_addr, const GString& client_addr, const GString& client_gate_addr);
    virtual void base_recover_by_disaster_backup_success(const GString& base_addr);

    ClientMailBox client;
    bool is_reqack_from_client = false;
};

// ------------------------------- client ------------------------------- //


class ClientEntity : public Entity {
public:
    enum {
        ENTITY_TYPE = EntityType::EntityType_Client
    };

public:
    ClientEntity() {
        base.set_side("client");
        base.set_owner(this);

        cell.set_side("client");
        cell.set_owner(this);
    }
    virtual ~ClientEntity() {}

    virtual void on_create(const GDict& create_data);
    virtual void on_reconnect_success(const GDict& create_data);
    virtual void ready(); // must exist
    virtual void destroy_by_create_check_timer() {}
    virtual void destroy_self();
    virtual void on_destroy() {}
    void prop_sync_from_base(const GBin& bin);
    void prop_sync_from_cell(const GBin& bin);
    virtual void on_prop_sync_from_server() {}
    virtual void on_kick();

    virtual void base_recover_by_disaster_backup_success(const GString& base_addr);
    virtual void cell_recover_by_disaster_backup_success(const GString& cell_addr);

    // migrate
    void migrate_req_from_cell();
    void new_cell_migrate_in(const GString& new_cell_addr);

    BaseMailBox base;
    CellMailBox cell;
};


// ------------------------------- definition ------------------------------- //


#define GENERATE_ENTITY_INNER(TCLASS) \
public: \
    EntityType get_entity_type() { return (EntityType)TCLASS::ENTITY_TYPE; } \
    static EntityPropertyManager<TCLASS> property_manager; \
    static EntityRpcManager<TCLASS> rpc_manager; \
    RpcManagerBase* get_rpc_mgr() { return &rpc_manager; } \
    RpcMethodBase* find_rpc_method(const GString& rpc_name) { return rpc_manager.find_rpc_method(rpc_name); } \
    RPC_CALL_DEFINE(TCLASS) \
    static EntityComponentManager<TCLASS> component_manager; \
    ComponentManagerBase* get_comp_mgr() { return &component_manager; } \
    static TimerManager<TCLASS> timer_manager; \
    TimerManagerBase* get_timer_manager() { return &timer_manager; } \
    static PropertyTree property_tree; \
    static void generate_property_tree(); \
    int16_t prop_str2int(const GString& prop_name) { return property_manager.s2i_map.at(prop_name); } \
    GString prop_int2str(int16_t idx) { return property_manager.i2s_map.at(idx); } \
    void create_dbsave_timer() { REGIST_TIMER(get_db_save_interval(), get_db_save_interval(), true, time_to_save); } \
    void time_to_save() { real_time_to_save(); } \
    EventManager<TCLASS> event_manager; \
    EventManagerBase* get_event_manager() { return &event_manager; }

#define GENERATE_ENTITY_OUT(TCLASS) \
EntityPropertyManager<TCLASS> TCLASS::property_manager; \
EntityRpcManager<TCLASS> TCLASS::rpc_manager((EntityType)TCLASS::ENTITY_TYPE, #TCLASS, []()->TCLASS* { \
    auto entity = new TCLASS(); \
    entity->give_propertys(property_manager.propertys); \
    component_manager.generate_entity_components(entity); \
    entity->rpc_mgr = &TCLASS::rpc_manager; \
    return entity; }); \
TimerManager<TCLASS> TCLASS::timer_manager; \
EntityComponentManager<TCLASS> TCLASS::component_manager; \
PropertyTree TCLASS::property_tree(property_manager.propertys);


extern unordered_map<GString, Entity*> g_base_entities;
extern unordered_map<GString, Entity*> g_cell_entities;
extern unordered_map<GString, Entity*> g_client_entities;
extern void regist_entity_creator(const GString& entity, const function<Entity*()>& creator);
extern Entity* create_local_base_entity(const GString& entity_type, const GString& entity_uuid);
extern Entity* create_local_cell_entity(const GString& entity_type, const GString& entity_uuid);
extern Entity* create_local_client_entity(const GString& entity_type, const GString& entity_uuid);
extern void destroy_local_base_entity(const GString& entity_uuid);
extern void destroy_local_cell_entity(const GString& entity_uuid);
extern void destroy_local_client_entity(const GString& entity_uuid);
