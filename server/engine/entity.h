#pragma once

#include <stdint.h>
#include <unordered_map>

#include "gvalue.h"
#include "log.h"
#include "rpc_manager.h"
#include "boost_asio.h"

#include "mailbox.h"
#include "entity_rpc_manager.h"

using namespace std;

class Entity;

#define RPC_CALL_DEFINE \
void rpc_call(const GString& rpc_name, const GArray& params) { \
    switch (params.size()) { \
    case 0: \
        get_rpc_mgr()->entity_rpc_call(this, rpc_name); \
        break; \
    case 1: \
        get_rpc_mgr()->entity_rpc_call(this, rpc_name, params[0]); \
        break; \
    case 2: \
        get_rpc_mgr()->entity_rpc_call(this, rpc_name, params[0], params[1]); \
        break; \
    case 3: \
        get_rpc_mgr()->entity_rpc_call(this, rpc_name, params[0], params[1], params[2]); \
        break; \
    case 4: \
        get_rpc_mgr()->entity_rpc_call(this, rpc_name, params[0], params[1], params[2], params[3]); \
        break; \
    case 5: \
        get_rpc_mgr()->entity_rpc_call(this, rpc_name, params[0], params[1], params[2], params[3], params[4]); \
        break; \
    case 6: \
        get_rpc_mgr()->entity_rpc_call(this, rpc_name, params[0], params[1], params[2], params[3], params[4], params[5]); \
        break; \
    case 7: \
        get_rpc_mgr()->entity_rpc_call(this, rpc_name, params[0], params[1], params[2], params[3], params[4], params[5], params[6]); \
        break; \
    case 8: \
        get_rpc_mgr()->entity_rpc_call(this, rpc_name, params[0], params[1], params[2], params[3], params[4], params[5], params[6], params[7]); \
        break; \
    case 9: \
        get_rpc_mgr()->entity_rpc_call(this, rpc_name, params[0], params[1], params[2], params[3], params[4], params[5], params[6], params[7], params[8]); \
        break; \
    case 10: \
        get_rpc_mgr()->entity_rpc_call(this, rpc_name, params[0], params[1], params[2], params[3], params[4], params[5], params[6], params[7], params[8], params[9]); \
        break; \
    case 11: \
        get_rpc_mgr()->entity_rpc_call(this, rpc_name, params[0], params[1], params[2], params[3], params[4], params[5], params[6], params[7], params[8], params[9], params[10]); \
        break; \
    case 12: \
        get_rpc_mgr()->entity_rpc_call(this, rpc_name, params[0], params[1], params[2], params[3], params[4], params[5], params[6], params[7], params[8], params[9], params[10], params[11]); \
        break; \
    case 13: \
        get_rpc_mgr()->entity_rpc_call(this, rpc_name, params[0], params[1], params[2], params[3], params[4], params[5], params[6], params[7], params[8], params[9], params[10], params[11], params[12]); \
        break; \
    case 14: \
        get_rpc_mgr()->entity_rpc_call(this, rpc_name, params[0], params[1], params[2], params[3], params[4], params[5], params[6], params[7], params[8], params[9], params[10], params[11], params[12], params[13]); \
        break; \
    case 15: \
        get_rpc_mgr()->entity_rpc_call(this, rpc_name, params[0], params[1], params[2], params[3], params[4], params[5], params[6], params[7], params[8], params[9], params[10], params[11], params[12], params[13], params[14]); \
        break; \
    default: \
        break; \
    } \
}

#define GENERATE_BASE_ENTITY_INNER() \
public: \
    static EntityRpcManager rpc_manager_base; \
    EntityRpcManager* get_rpc_mgr() { return &rpc_manager_base; } \
    RPC_CALL_DEFINE

#define GENERATE_CELL_ENTITY_INNER() \
public: \
    static EntityRpcManager rpc_manager_cell; \
    EntityRpcManager* get_rpc_mgr() { return &rpc_manager_cell; } \
    RPC_CALL_DEFINE

#define GENERATE_CLIENT_ENTITY_INNER() \
public: \
    static EntityRpcManager rpc_manager_client; \
    EntityRpcManager* get_rpc_mgr() { return &rpc_manager_client; } \
    RPC_CALL_DEFINE


#define GENERATE_BASE_ENTITY_OUT(TCLASS) \
class TCLASS; \
EntityRpcManager TCLASS::rpc_manager_base("base", #TCLASS, []()->TCLASS* { return new TCLASS();});

#define GENERATE_CELL_ENTITY_OUT(TCLASS) \
class TCLASS; \
EntityRpcManager TCLASS::rpc_manager_cell("cell", #TCLASS, []()->TCLASS* { return new TCLASS();});

#define GENERATE_CLIENT_ENTITY_OUT(TCLASS) \
class TCLASS; \
EntityRpcManager TCLASS::rpc_manager_client("client", #TCLASS, []()->TCLASS* { return new TCLASS();});


extern unordered_map<GString, Entity*> g_entities;
extern void regist_entity_creator(const GString& entity, const function<Entity*()>& creator);
extern Entity* create_entity(const GString& entity_type, const GString& entity_uuid, const GDict& create_data);

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
    Entity() {}
    virtual ~Entity() {}

    virtual void on_create(const GDict& create_data) = 0;
    virtual void on_destroy() = 0;
    virtual void rpc_call(const GString& rpc_name, const GArray& params) = 0;

    virtual EntityRpcManager* get_rpc_mgr() { return nullptr; }

    EntityType type = EntityType::EntityType_None;

    GString uuid = "";
    GString class_name = "";
};


// ------------------------------- base ------------------------------- //
class BaseEntity : public Entity {

public:
    BaseEntity() {
        type = EntityType::EntityType_Base;
    }
    virtual ~BaseEntity() {}

    virtual void on_create(const GDict& create_data) {}
    virtual void on_destroy() {}
};

class BaseEntityWithCell : virtual public BaseEntity {

public:
    BaseEntityWithCell() = delete;
    BaseEntityWithCell(const GString& cell_class) : cell_class_name(cell_class) {
        type = EntityType::EntityType_BaseWithCell;
    }
    virtual ~BaseEntityWithCell() {}

    virtual void on_create(const GDict& create_data);
    virtual void on_destroy();

    virtual void on_cell_create(const GValue& cell_entity_uuid, const GValue& cell_addr);

    void create_cell(const GDict& create_data);

    GString cell_class_name;
    CellMailBox cell;
};

class BaseEntityWithClient : virtual public BaseEntity {

public:
    BaseEntityWithClient() = delete;
    BaseEntityWithClient(const GString& client_class) : client_class_name(client_class) {
        type = EntityType::EntityType_BaseWithClient;
    }
    virtual ~BaseEntityWithClient() {}

    virtual void on_create(const GDict& create_data);
    virtual void on_destroy();
    virtual void on_client_create(const GValue& client_entity_uuid);

    void create_client();

    GString client_class_name;
    ClientMailBox client;
};

class BaseEntityWithCellAndClient : public BaseEntityWithCell, public BaseEntityWithClient {

public:
    BaseEntityWithCellAndClient() = delete;
    BaseEntityWithCellAndClient(const GString& cell_class, const GString& client_class) : BaseEntityWithCell(cell_class), BaseEntityWithClient(client_class) {
        type = EntityType::EntityType_BaseWithCellAndClient;
    }
    virtual ~BaseEntityWithCellAndClient() {}

    virtual void on_create(const GDict& create_data);
    virtual void on_destroy();
    virtual void on_cell_create(const GValue& cell_entity_uuid, const GValue& cell_addr);
    virtual void on_client_create(const GValue& client_entity_uuid);
};


// ------------------------------- cell ------------------------------- //
class CellEntity : public Entity {

public:
    CellEntity() {
        type = EntityType::EntityType_Cell;
    }
    virtual ~CellEntity() {}

    virtual void on_create(const GDict& create_data) {}
    virtual void on_destroy() {}

    BaseMailBox base;
};

class CellEntityWithClient : public CellEntity {

public:
    CellEntityWithClient() {
        type = EntityType::EntityType_CellWithClient;
    }
    virtual ~CellEntityWithClient() {}

    virtual void on_create(const GDict& create_data);
    virtual void on_destroy();
    virtual void on_client_create(const GValue& client_entity_uuid);

    ClientMailBox client;
};

// ------------------------------- client ------------------------------- //


class ClientEntity : public Entity {

public:
    ClientEntity() {
        type = EntityType::EntityType_Client;
        base.set_side("client");
        cell.set_side("client");
    }
    virtual ~ClientEntity() {}

    virtual void on_create(const GDict& create_data);
    virtual void on_destroy() {}

    BaseMailBox base;
    CellMailBox cell;
};