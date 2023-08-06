#pragma once

#include <stdint.h>
#include <unordered_map>
#include <unordered_set>

#include "gvalue.h"
#include "boost_asio.h"
#include "mailbox.h"
#include "timer.h"
#include "event.h"
#include "entity_rpc_manager.h"
#include "entity_property_tree.h"
#include "entity_property_manager.h"
#include "entity_component_manager.h"
#include "event.h"

using namespace std;

enum EntityType : int8_t {
    EntityType_Base,  // stub
    EntityType_BaseWithCell,  // monster
    EntityType_BaseWithClient,  // account
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
    virtual void apply_init_data(const GDict& create_data) {}
    virtual void on_create(const GDict& create_data);
    virtual void ready_check_timer();
    virtual void create_heart_beat_timer() {}
    virtual void on_reconnect_fromclient(const GString& client_addr, const GString& gate_addr) { ASSERT(false); } // for base and cell
    virtual void on_reconnect_success(const GDict& create_data) { ASSERT(false); } // for client
    virtual void on_destroy() = 0;
    virtual void rpc_call(bool from_client, const GString& rpc_name, RpcMethodBase* rpc_method) = 0;
    virtual void destroy_self();
    virtual BaseMailBox& get_base_mailbox() { ASSERT(false); static BaseMailBox* p = nullptr; return *p; }
    virtual CellMailBox& get_cell_mailbox() { ASSERT(false); static CellMailBox* p = nullptr; return *p; }
    virtual ClientMailBox& get_client_mailbox() { ASSERT(false); static ClientMailBox* p = nullptr; return *p; }

    virtual RpcMethodBase* find_rpc_method(const GString& rpc_name) = 0;
    virtual RpcManagerBase* get_rpc_mgr() { return nullptr; }
    virtual ComponentManagerBase* get_comp_mgr() { return nullptr; }

    EntityPropertyBase* get_prop(const GString& prop_name) const { return propertys.at(prop_name); }
    EntityPropertyBase& get_prop_ref(const GString& prop_name) const { return *propertys.at(prop_name); }
    EntityComponentBase* get_component(const GString& componet_name) const { return components.at(componet_name); }

    void release_component();
    void release_property();
    void release_timer();
    void release_event();

    // proppertys
    virtual void propertys_sync2client(bool force_all = false);
    void serialize_all(Encoder& encoder, bool need_clean_dirty = false);
    void serialize_db(Encoder& encoder, bool need_clean_dirty = false);
    void serialize_client(Encoder& encoder, bool need_clean_dirty = false);
    void serialize_client_all(Encoder& encoder, bool need_clean_dirty = false);
    void serialize_otherclient(Encoder& encoder);
    void serialize_otherclient_all(Encoder& encoder);
    virtual int16_t prop_str2int(const GString& prop_name) { return 0; }
    virtual GString prop_int2str(int16_t idx) { return ""; }
    bool is_base_entity();
    bool is_cell_entity();
    bool need_create_save_timer();
    bool need_create_disater_backup_timer();
    virtual void create_dbsave_timer() { ASSERT(false); }
    virtual void create_disater_backup_timer() { ASSERT(false); }
    double get_db_save_interval();
    virtual void real_time_to_save() { ASSERT(false); }
    virtual void real_time_to_disaster_backup() { ASSERT(false); }

    void give_propertys(unordered_map<GString, EntityPropertyBase*>& propertys);
    void ready();
    virtual void on_ready() {}
    virtual void propertys_unserialize(Decoder& decoder);

    // timer
    virtual TimerManagerBase* get_timer_manager() { ASSERT(false); return nullptr; }
    void timer_tick();
    void cancel_timer(TimerID time_id);
    void add_timer(TimerBase* timer);
    void add_event(EventBase* event);
    void remove_timer(TimerBase* timer);

    // migrate
    virtual void begin_migrate(const GString& new_addr) { ASSERT(false); }
    virtual void on_migrate_out(GDict& migrate_data) { ASSERT(false); }
    virtual void on_migrate_in(const GDict& migrate_data) { ASSERT(false); }
    virtual void packet_migrate_data(const GDict& migrate_data) { ASSERT(false); }
    virtual void unpacket_migrate_data(const GDict& migrate_data) { ASSERT(false); }

    // event
    virtual EventManagerBase* get_event_manager() { ASSERT(false); return nullptr; }
    Entity* get_owner() { return nullptr; }

    template<class... Args>
    void send_event(const GString& event_name, Args... args) {
        get_event_manager()->send_event(this, event_name, args...);
    }

    // migrate
    GArray migrate_timers;
    GArray migrate_events;

    // disaster backup
    virtual void on_game_disappear(const GString& game_addr) {}
    virtual void recover_by_disaster_backup(const GString& addr, const GString& client_addr, const GString& client_gate_addr) { ASSERT(false); }

    virtual MailBox get_self_mailbox() { ASSERT(false); MailBox* p = nullptr; return *p; }

    GString uuid = "";
    GString entity_name = "";

    MAP<GString, EntityPropertyBase*> propertys;
    unordered_map<EntityPropertyBase*, GString> propertys_turn;
    unordered_map<GString, EntityPropertyBase*> dirty_propertys;
    unordered_map<GString, EntityComponentBase*> components;
    //unordered_map<GString, unordered_set<EntityComponentBase*>> event_components;

    multiset<TimerBase*, TimerCompare> timers;  // 不能用set，否则过期时间一样会被认为是重复key
    TimerID next_timer_id = 1;
    unordered_map<TimerID, TimerBase*> timer_ids;
    TimerID ready_check_timerid = 0;
    TimerID heart_beat_timerid = 0;

    unordered_map<GString, vector<EventBase*>> events;

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

    virtual MailBox get_self_mailbox();
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
    virtual void ready_check_timer();
    virtual void create_cell(const GDict& create_data);
    virtual void on_cell_create(const GString& cell_addr);
    virtual void ready(); // must exist
    virtual void destroy_self();
    virtual void create_heart_beat_timer();
    virtual void heart_beat_timer();
    virtual void respect_from_cell();
    virtual void on_destroy() {}

    virtual void real_time_to_save();
    virtual void real_time_to_disaster_backup();

    // migrate
    virtual void migrate_req_from_cell();
    virtual void new_cell_migrate_in(const GString& new_cell_addr);
    virtual void packet_migrate_data(GDict& migrate_data);
    virtual void unpacket_migrate_data(const GDict& migrate_data);

    // disaster backup
    virtual void base_disaster_backup(const GBin& cell_all_db, const GDict& cell_migrate_data);
    virtual void on_game_disappear(const GString& game_addr);
    virtual void recover_by_disaster_backup(const GString& cell_addr, const GString& client_addr, const GString& client_gate_addr);
    virtual void cell_recover_by_disaster_backup_success(const GString& cell_addr);

    virtual CellMailBox& get_cell_mailbox() { return cell; }

    CellMailBox cell;
    int64_t last_respect_from_cell = 0;

    GBin disaster_backup_of_self;
    GDict disaster_backup_of_self_migrate_data;
    GBin disaster_backup_of_cell;
    GDict disaster_backup_of_cell_migrate_data;
};

class BaseEntityWithClient : public BaseEntity {
public:
    enum {
        ENTITY_TYPE = EntityType::EntityType_BaseWithClient
    };

public:
    BaseEntityWithClient() { client.set_owner(this); }
    virtual ~BaseEntityWithClient() {}

    virtual void on_create(const GDict& create_data);
    virtual void ready_check_timer();
    virtual void create_client();
    virtual void ready(); // must exist
    virtual void destroy_self();
    virtual void on_reconnect_fromclient(const GString& client_addr, const GString& gate_addr);
    virtual void on_client_reconnect_success();
    virtual void on_destroy() {}
    virtual void propertys_sync2client(bool force_all);
    virtual void kick_client();

    virtual void real_time_to_save();

    virtual ClientMailBox& get_client_mailbox() { return client; }

    ClientMailBox client;
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
    virtual void create_cell(const GDict& create_data);
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
    virtual void real_time_to_disaster_backup();

    // migrate
    void migrate_req_from_cell() { BaseEntityWithCell::migrate_req_from_cell(); }
    void new_cell_migrate_in(const GString& new_cell_addr);
    void packet_migrate_data(GDict& migrate_data);
    void unpacket_migrate_data(const GDict& migrate_data);

    // disaster backup
    virtual void base_disaster_backup(const GBin& cell_all_db, const GDict& migrate_data);
    virtual void on_game_disappear(const GString& game_addr);
    virtual void recover_by_disaster_backup(const GString& cell_addr, const GString& client_addr, const GString& client_gate_addr);
    virtual void cell_recover_by_disaster_backup_success(const GString& cell_addr);

    virtual CellMailBox& get_cell_mailbox() { return cell; }
    virtual ClientMailBox& get_client_mailbox() { return client; }

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

    virtual void on_create(const GDict& create_data);
    virtual void ready_check_timer();
    virtual void ready() { Entity::ready(); } // must exist
    virtual void create_heart_beat_timer();
    virtual void heart_beat_timer();
    virtual void respect_from_base();
    virtual void on_destroy() {}
    virtual void destroy_self();

    // migrate
    virtual void begin_migrate(const GString& new_addr);
    virtual void migrate_reqack_from_base(bool is_ok);
    virtual void real_begin_migrate();
    virtual void migrate_entity();
    virtual void on_migrate_out(GDict& migrate_data);
    virtual void packet_migrate_data(GDict& migrate_data);
    virtual void on_migrate_in(const GDict& migrate_data);
    virtual void unpacket_migrate_data(const GDict& migrate_data);
    virtual void on_new_cell_migrate_finish();

    void migrate_check_timer();
    void migrate_failed_rollback(const GString& reason);

    // db save
    void cell_real_time_to_save(const GString& base_uuid, const GBin& base_bin);

    // disaster backup
    virtual void cell_disaster_backup(const GBin& base_all_db, const GDict& base_migrate_data);
    virtual void on_game_disappear(const GString& game_addr);
    virtual void recover_by_disaster_backup(const GString& base_addr, const GString& client_addr, const GString& client_gate_addr);
    virtual void base_recover_by_disaster_backup_success(const GString& base_addr);

    virtual BaseMailBox& get_base_mailbox() { return base; }
    MailBox get_self_mailbox();

    BaseMailBox base;

    MigrateState migrate_state = Migrate_None;
    TimerID migrate_check_timer_id = 0;
    GString new_cell_addr = "";
    bool is_reqack_from_base = false;

    GBin  disaster_backup_of_base;
    GDict disaster_backup_of_base_migrate_data;
    GBin  disaster_backup_of_self;
    GDict disaster_backup_of_self_migrate_data;

    int64_t last_respect_from_base = 0;
};

class CellEntityWithClient : public CellEntity {
public:
    enum {
        ENTITY_TYPE = EntityType::EntityType_CellWithClient
    };

public:
    CellEntityWithClient() { client.set_owner(this); }
    virtual ~CellEntityWithClient() {}

    virtual void on_create(const GDict& create_data);
    virtual void ready_check_timer();
    virtual void ready(); // must exist
    virtual void create_heart_beat_timer();
    virtual void heart_beat_timer();
    virtual void respect_from_base();
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
    void migrate_failed_rollback(const GString& reason);

    // db save
    void cell_real_time_to_save(const GString& base_uuid, const GBin& base_bin);

    // disaster backup
    virtual void cell_disaster_backup(const GBin& base_all_db, const GDict& base_migrate_data);
    virtual void on_game_disappear(const GString& game_addr);
    virtual void recover_by_disaster_backup(const GString& base_addr, const GString& client_addr, const GString& client_gate_addr);
    virtual void base_recover_by_disaster_backup_success(const GString& base_addr);

    virtual BaseMailBox& get_base_mailbox() { return base; }
    virtual ClientMailBox& get_client_mailbox() { return client; }

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
        base.set_owner(this);
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

    virtual BaseMailBox& get_base_mailbox() { return base; }
    virtual CellMailBox& get_cell_mailbox() { return cell; }

    BaseMailBox base;
    CellMailBox cell;
};


extern unordered_map<GString, Entity*> g_base_entities;
extern unordered_map<GString, Entity*> g_cell_entities;
extern unordered_map<GString, Entity*> g_client_entities;
extern void regist_entity_creator(const GString& entity, const function<Entity*()>& creator);
extern Entity* create_local_base_entity(const GString& entity_type, const GString& entity_uuid);
extern Entity* create_local_cell_entity(const GString& entity_type, const GString& entity_uuid);
extern Entity* create_local_client_entity(const GString& entity_type, const GString& entity_uuid);
extern Entity* create_other_entity(const GString& entity_type, const GString& entity_uuid);
extern void destroy_local_base_entity(const GString& entity_uuid);
extern void destroy_local_cell_entity(const GString& entity_uuid);
extern void destroy_local_client_entity(const GString& entity_uuid);

extern Entity* thread_safe_get_base_entity(GString entity_uuid);
extern Entity* thread_safe_get_cell_entity(GString entity_uuid);
extern Entity* thread_safe_get_client_entity(GString entity_uuid);

extern RpcManagerBase* thread_safe_get_base_entity_rpcmgr(GString entity_uuid);
extern RpcManagerBase* thread_safe_get_cell_entity_rpcmgr(GString entity_uuid);
extern RpcManagerBase* thread_safe_get_client_entity_rpcmgr(GString entity_uuid);
