#pragma once

#include <stdint.h>
#include <unordered_map>

#include "gvalue.h"
#include "boost_asio.h"
#include "mailbox.h"
#include "timer.h"
#include "entity_property_tree.h"

using namespace std;

class Entity;
struct RpcMethodBase;
class RpcManagerBase;
class ComponentManagerBase;
class EntityComponentBase;
struct EntityPropertyBase;
class TimerBase;
class TimerManagerBase;

enum EntityType {
    EntityType_Base,
    EntityType_BaseWithCell,
    EntityType_BaseWithClient,
    EntityType_BaseWithCellAndClient,
    EntityType_Cell,
    EntityType_CellWithClient,
    EntityType_Client,

    EntityType_None
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
    virtual void on_create(const GDict& create_data) = 0;
    virtual void on_destroy() = 0;
    virtual void rpc_call(bool from_client, const GString& rpc_name, const GArray& params) = 0;

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
    virtual void real_time_to_save() {}

    void give_propertys(unordered_map<GString, EntityPropertyBase*>& propertys);
    void ready();
    virtual void on_ready() {}
    virtual void propertys_unserialize(Decoder& decoder) { ASSERT(false); }

    // timer
    virtual TimerManagerBase* get_timer_manager() { ASSERT(false); return nullptr; }
    void timer_tick();
    void cancel_timer(TimerID time_id);
    void add_timer(TimerBase* timer);
    void remove_timer(TimerBase* timer);

    // migrate
    virtual void on_migrate_in(const GDict& create_data) { ASSERT(false); }
    virtual void on_migrate_out(GDict& create_data) { ASSERT(false); }

    GString uuid = "";
    GString class_name = "";

    MAP<GString, EntityPropertyBase*> propertys;
    unordered_map<EntityPropertyBase*, GString> propertys_turn;
    unordered_map<GString, EntityPropertyBase*> dirty_propertys;
    unordered_map<GString, EntityComponentBase*> components;

    multiset<TimerBase*, TimerCompare> timers;  // 不能用set，否则过期时间一样会被认为是重复key
    TimerID next_timer_id = 1;
    unordered_map<TimerID, TimerBase*> timer_ids;

    RpcManagerBase* rpc_mgr;
    bool is_ready = false;
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

class BaseEntityWithCell : virtual public BaseEntity {
public:
    enum {
        ENTITY_TYPE = EntityType::EntityType_BaseWithCell
    };

public:
    BaseEntityWithCell() = delete;
    BaseEntityWithCell(const GString& cell_class) : cell_class_name(cell_class) {
        cell.set_owner(this);
    }
    virtual ~BaseEntityWithCell() {}

    virtual void on_create(const GDict& create_data);
    virtual void create_cell(const GDict& create_data);
    virtual void on_cell_create(const GValue& cell_entity_uuid, const GValue& cell_addr);
    virtual void ready() { Entity::ready(); } // must exist
    virtual void on_destroy() {}

    // migrate
    virtual void migrate_req_from_cell();
    virtual void new_cell_migrate_in(const GValue& new_cell_addr);

    GString cell_class_name;
    CellMailBox cell;
};

class BaseEntityWithClient : virtual public BaseEntity {
public:
    enum {
        ENTITY_TYPE = EntityType::EntityType_BaseWithClient
    };

public:
    BaseEntityWithClient() = delete;
    BaseEntityWithClient(const GString& client_class) : client_class_name(client_class) {
        client.set_owner(this);
    }
    virtual ~BaseEntityWithClient() {}

    virtual void on_create(const GDict& create_data);
    virtual void create_client();
    virtual void on_client_create(const GValue& client_entity_uuid);
    virtual void ready() { Entity::ready(); } // must exist
    virtual void on_destroy() {}
    virtual void propertys_sync2client(bool force_all);

    GString client_class_name;
    ClientMailBox client;
};

class BaseEntityWithCellAndClient : public BaseEntityWithCell, public BaseEntityWithClient {
public:
    enum {
        ENTITY_TYPE = EntityType::EntityType_BaseWithCellAndClient
    };

public:
    BaseEntityWithCellAndClient() = delete;
    BaseEntityWithCellAndClient(const GString& cell_class, const GString& client_class) : BaseEntityWithCell(cell_class), BaseEntityWithClient(client_class) {}
    virtual ~BaseEntityWithCellAndClient() {}

    virtual void on_create(const GDict& create_data);
    virtual void on_cell_create(const GValue& cell_entity_uuid, const GValue& cell_addr);
    virtual void create_client();
    virtual void on_client_create(const GValue& client_entity_uuid);
    virtual void ready() { Entity::ready(); } // must exist
    virtual void on_destroy() {}
    virtual void propertys_sync2client(bool force_all) { BaseEntityWithClient::propertys_sync2client(force_all); }

    virtual void real_time_to_save();

    void new_cell_migrate_in(const GValue& new_cell_addr);

    // migrate
    void migrate_req_from_cell() { BaseEntityWithCell::migrate_req_from_cell(); }
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

    void propertys_unserialize(Decoder& decoder);

    // migrate
    virtual void begin_migrate(const GValue& new_addr);
    virtual void migrate_reqack_from_base(const GValue& is_ok);
    virtual void real_begin_migrate();
    virtual void migrate_entity();
    virtual void on_migrate_out(GDict& create_data);
    virtual void on_migrate_in(const GDict& create_data);
    virtual void on_new_cell_migrate_finish();
    void destroy_self();

    BaseMailBox base;

    bool is_migrating = false;
    GString new_cell_addr = "";
    bool is_reqack_from_base = false;
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
    virtual void on_client_create(const GValue& client_entity_uuid);
    virtual void ready() { Entity::ready(); } // must exist
    virtual void on_destroy() {}
    virtual void propertys_sync2client(bool force_all);

    // migrate
    virtual void begin_migrate(const GValue& new_addr);
    virtual void migrate_reqack_from_base(const GValue& is_ok);
    virtual void migrate_reqack_from_client(const GValue& is_ok);
    virtual void real_begin_migrate();
    virtual void on_migrate_out(GDict& create_data);
    virtual void on_migrate_in(const GDict& create_data);
    virtual void on_new_cell_migrate_finish();

    // db save
    void cell_real_time_to_save(const GValue& base_bin);

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
    virtual void ready() { Entity::ready(); } // must exist
    virtual void on_destroy() {}
    void prop_sync_from_base(const GValue& bin);
    void prop_sync_from_cell(const GValue& bin);
    void propertys_unserialize(Decoder& decoder);
    virtual void on_prop_sync_from_server() {}

    // migrate
    void migrate_req_from_cell();
    void new_cell_migrate_in(const GValue& new_cell_addr);

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
    void create_dbsave_timer() { REGIST_TIMER(get_db_save_interval(), get_db_save_interval(), true, TCLASS::time_to_save); } \
    void time_to_save() { real_time_to_save(); }

#define GENERATE_ENTITY_OUT(TCLASS) \
EntityPropertyManager<TCLASS> TCLASS::property_manager; \
EntityRpcManager<TCLASS> TCLASS::rpc_manager((EntityType)TCLASS::ENTITY_TYPE, #TCLASS, []()->TCLASS* { \
    auto entity = new TCLASS(); \
    entity->give_propertys(property_manager.propertys); \
    component_manager.generate_entity_components(entity); \
    entity->rpc_mgr = &TCLASS::rpc_manager; \
    return entity; }); \
EntityComponentManager<TCLASS> TCLASS::component_manager; \
TimerManager<TCLASS> TCLASS::timer_manager; \
PropertyTree TCLASS::property_tree(property_manager.propertys);


extern unordered_map<GString, Entity*> g_entities;
extern void regist_entity_creator(const GString& entity, const function<Entity*()>& creator);
extern Entity* create_entity(const GString& entity_type, const GString& entity_uuid);
extern void destroy_entity(const GString& entity_uuid);
