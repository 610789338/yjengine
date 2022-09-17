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
    virtual ~Entity() { release_component(); }

    void tick();
    virtual void on_tick() {}

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

    // proppertys
    virtual void propertys_sync2client(bool force_all = false);
    void serialize2client(Encoder& encoder, bool force_all = false);
    virtual int16_t prop_str2int(const GString& prop_name) { return 0; }
    virtual GString prop_int2str(int16_t idx) { return ""; }

    void give_propertys(unordered_map<GString, EntityPropertyBase*>& propertys);
    void ready();
    virtual void on_ready() {}

    GString uuid = "";
    GString class_name = "";

    unordered_map<GString, EntityPropertyBase*> propertys;
    unordered_map<EntityPropertyBase*, GString> propertys_turn;
    unordered_map<GString, EntityPropertyBase*> dirty_propertys;
    unordered_map<GString, EntityComponentBase*> components;

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
};


// ------------------------------- cell ------------------------------- //
class CellEntity : public Entity {
public:
    enum {
        ENTITY_TYPE = EntityType::EntityType_Cell
    };

public:
    CellEntity() {
        base.set_owner(this);
    }
    virtual ~CellEntity() {}

    virtual void on_create(const GDict& create_data) {}
    virtual void ready() { Entity::ready(); } // must exist
    virtual void on_destroy() {}

    BaseMailBox base;
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

    ClientMailBox client;
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

    BaseMailBox base;
    CellMailBox cell;
};


// ------------------------------- definition ------------------------------- //


#define GENERATE_ENTITY_INNER(TCLASS) \
public: \
    static EntityPropertyManager<TCLASS> property_manager; \
    static EntityRpcManager<TCLASS> rpc_manager; \
    RpcManagerBase* get_rpc_mgr() { return &rpc_manager; } \
    RpcMethodBase* find_rpc_method(const GString& rpc_name) { return rpc_manager.find_rpc_method(rpc_name); } \
    RPC_CALL_DEFINE(TCLASS) \
    ComponentManagerBase* get_comp_mgr() { return &component_manager; } \
    static EntityComponentManager<TCLASS> component_manager; \
    static TimerManager<TCLASS> timer_manager; \
    static PropertyTree property_tree; \
    static void generate_property_tree(); \
    int16_t prop_str2int(const GString& prop_name) { return property_manager.s2i_map.at(prop_name); } \
    GString prop_int2str(int16_t idx) { return property_manager.i2s_map.at(idx); }

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
extern Entity* create_entity(const GString& entity_type, const GString& entity_uuid, const GDict& create_data);
