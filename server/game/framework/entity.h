#pragma once

#include <stdint.h>
#include <unordered_map>

#include "engine/engine.h"

#include "mailbox.h"

using namespace std;

class Entity;
extern unordered_map<GString, Entity*> g_entities;
extern unordered_map<GString, function<Entity*()>> g_entity_creator;
extern void regist_entity_creator(const GString& entity, const function<Entity*()>& creator);
extern Entity* create_entity(const GString& entity_type, const GString& entity_uuid, const GDict& create_data);

extern GString gen_uuid();

enum EntityType {
    EntityType_Base,
    EntityType_BaseWithCell,
    EntityType_BaseWithClient,
    EntityType_BaseWithCellAndClient,
    EntityType_Cell,
    EntityType_CellWithClient,

    EntityType_None
};

class Entity {
    friend struct _RpcDeCoratorHelper;
public:
    Entity() {}
    virtual ~Entity() {}

    virtual void on_create(const GDict& create_data) = 0;
    virtual void on_destroy() = 0;

    template<class... T, class... T2>
    void rpc_regist(const GString& rpc_name, void(Entity::*cb)(T... args), T2... args) { /*rpc_manager.rpc_regist(rpc_name, cb, args...);*/ }

    void rpc_call(const GString& rpc_name, const GArray& params);

    EntityType type = EntityType::EntityType_None;

    GString uuid = "";
    GString class_name = "";

    static RpcManager rpc_manager;
};

class BaseEntity : public Entity {

public:
    BaseEntity() {
        type = EntityType::EntityType_Base;
    }
    virtual ~BaseEntity() {}

    virtual void on_create(const GDict& create_data) {}
    virtual void on_destroy() {}
};

//struct _RpcDeCoratorHelper {
//    _RpcDeCoratorHelper() {
//    }
//
//    template<class... T, class... T2>
//    _RpcDeCoratorHelper(RpcManager& rpc_manager, const GString& rpc_name, void(*cb)(T... args), T2... args) { rpc_manager.rpc_regist(rpc_name, cb, args...); }
//};
//
//#define RPC_DECORATOR(entity_rpc_flag, rpc_name, ...) _RpcDeCoratorHelper(this->rpc_manager, #rpc_name, rpc_name, __VA_ARGS__);

class BaseEntityWithCell : virtual public BaseEntity {

public:
    BaseEntityWithCell() = delete;
    BaseEntityWithCell(const GString& cell_class) : cell_class_name(cell_class) {
        type = EntityType::EntityType_BaseWithCell;
    }
    virtual ~BaseEntityWithCell() {}

    virtual void on_create(const GDict& create_data);
    virtual void on_destroy();

    //RPC_DECORATOR(ServerOnly, on_cell_create, GString(), GString())
    //_RpcDeCoratorHelper(rpc_manager, "on_cell_create", on_cell_create, GString(), GString());

    //void __rdc() {
    //    rpc_regist("on_cell_create", &BaseEntityWithCell::on_cell_create, GString(), GString());
    //}
    virtual void on_cell_create(const GValue& cell_entity_uuid, const GValue& cell_addr);

    void create_cell(const GDict& create_data);

    GString cell_class_name;
    CellMailBox cell;
};

class BaseEntityWithClient : virtual public BaseEntity {

public:
    BaseEntityWithClient() {
        type = EntityType::EntityType_BaseWithClient;
    }
    virtual ~BaseEntityWithClient() {}

    virtual void on_create(const GDict& create_data);
    virtual void on_destroy();
    virtual void on_client_create();

    void create_client();

    ClientMailBox client;
};

class BaseEntityWithCellAndClient : public BaseEntityWithCell, public BaseEntityWithClient {

public:
    BaseEntityWithCellAndClient() = delete;
    BaseEntityWithCellAndClient(const GString& cell_class) : BaseEntityWithCell(cell_class) {
        type = EntityType::EntityType_BaseWithCellAndClient;
    }
    virtual ~BaseEntityWithCellAndClient() {}

    virtual void on_create(const GDict& create_data);
    virtual void on_destroy();
    virtual void on_cell_create(const GString& cell_entity_uuid, const GString& cell_addr);
    virtual void on_client_create();
};

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

    ClientMailBox client;
};
