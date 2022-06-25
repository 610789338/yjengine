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

template<class TEntity, class... T>
struct EntityRpcMethod : public RpcMethodBase {
    typedef void(TEntity::*CBType)(T... args);
    CBType cb;
};

// entity rpc manager
class EntityRpcManager : public RpcManagerBase {
public:
    EntityRpcManager() = delete;
    EntityRpcManager(const GString& belong);

    GString m_belong = "all";

    template<class TEntity, class... T, class... T2>
    void entity_rpc_regist(const GString& rpc_name, void(TEntity::*cb)(T...), T2... args) {
        ASSERT_LOG(sizeof...(T) == sizeof...(args), "rpc(%s) formal param size(%zu) != real param size(%zu)\n", rpc_name.c_str(), sizeof...(T), sizeof...(args));

        EntityRpcMethod<TEntity, T...>* method = new EntityRpcMethod<TEntity, T...>;
        method->cb = cb;
        method->rpc_real_params_parse(args...);

        RpcFormalParamsCheck<T...>();

        add_rpc_method(rpc_name, method);
    }

    template<class TEntity, class... T>
    void entity_rpc_regist(const GString& rpc_name, void(TEntity::*cb)(T...)) {
        EntityRpcMethod<TEntity, T...>* method = new EntityRpcMethod<TEntity, T...>;
        method->cb = cb;
        add_rpc_method(rpc_name, method);
    }

    template<class... T>
    void entity_rpc_call(const GString& rpc_name, T... args) {
        RpcMethodBase* method = find_rpc_method(rpc_name);

        if (sizeof...(args) != method->m_params_t.size()) {
            ERROR_LOG("rpc.%s args num.%zd error, must be %zd\n", rpc_name.c_str(), sizeof...(args), method->m_params_t.size());
            return;
        }

        //auto _cb = ((EntityRpcMethod<BaseEntityWithCellAndClient, T...>*)method)->cb;
        //auto _cb = (*method).cb;
        //(_this.*_cb)(args...);
    }
};

extern EntityRpcManager g_base_entity_rpc;
extern EntityRpcManager g_cell_entity_rpc;
extern void entity_rpc_mgr_init();

class Entity {
public:
    Entity() {}
    virtual ~Entity() {}

    virtual void on_create(const GDict& create_data) = 0;
    virtual void on_destroy() = 0;
    virtual EntityRpcManager* get_rpc_mgr() { return nullptr; }

    void rpc_call(const GString& rpc_name, const GArray& params);

    EntityType type = EntityType::EntityType_None;

    GString uuid = "";
    GString class_name = "";
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
    virtual void on_cell_create(const GValue& cell_entity_uuid, const GValue& cell_addr);
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
