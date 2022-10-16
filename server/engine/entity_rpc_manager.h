#pragma once

#include "gvalue.h"
#include "log.h"
#include "rpc_manager.h"
#include "entity.h"

//entity rpc难点在于：
// 1，entity基类记录所有rpc的话有两个痛点：
//   * 不同类型entity的rpc全部记录在一起，冗余臃肿
//   * 记录的rpc是带类型信息的，但是在获取rpc时外部无法传入类型信息，也就无法通过类型转换还原回调函数
//     所以rpc mgr中记录的rpc的类型信息必须是单一的，这样就无需类型转换，所以改为每个entity子类单独记录自己的rpc
//
// 2，每个entity子类除了单独记录自己的 rpc，还要自动继承基础rpc

class Entity;

template<class TEntity, class... T>
struct EntityRpcMethod : public RpcMethodBase {
    typedef void(TEntity::*CBType)(T... args);
    CBType cb;
};

extern GArray* get_local_entity_rpc_names();
extern unordered_map<GString, uint16_t> g_entity_rpc_name_l2s;
extern unordered_map<uint16_t, GString> g_entity_rpc_name_s2l;

// entity rpc manager
template<class EntityClassType>
class EntityRpcManager : public RpcManagerBase {

public:
    EntityRpcManager() = delete;
    EntityRpcManager(enum EntityType entity_type, const GString& entity_class_name, const function<Entity*()>& creator) {

        // entity base rpc
        if (entity_type == EntityType::EntityType_Base) {
        }
        else if (entity_type == EntityType::EntityType_BaseWithCell) {
            entity_rpc_regist(RpcType::SERVER_ONLY, "on_cell_create",     &BaseEntityWithCell::on_cell_create, GString(), GString());
        }
        else if (entity_type == EntityType::EntityType_BaseWithClient) {
            entity_rpc_regist(RpcType::EXPOSED,     "on_client_create",   &BaseEntityWithClient::on_client_create, GString());
        }
        else if (entity_type == EntityType::EntityType_BaseWithCellAndClient) {
            entity_rpc_regist(RpcType::SERVER_ONLY, "on_cell_create",     &BaseEntityWithCellAndClient::on_cell_create, GString(), GString());
            entity_rpc_regist(RpcType::EXPOSED,     "on_client_create",   &BaseEntityWithCellAndClient::on_client_create, GString());
            entity_rpc_regist(RpcType::SERVER_ONLY, "ready",              &BaseEntityWithCellAndClient::ready);
        }
        else if (entity_type == EntityType::EntityType_CellWithClient) {
            entity_rpc_regist(RpcType::EXPOSED,    "on_client_create",    &CellEntityWithClient::on_client_create, GString());
        }
        else if (entity_type == EntityType::EntityType_Client) {
            entity_rpc_regist(RpcType::CLIENT, "ready",                   &ClientEntity::ready);
            entity_rpc_regist(RpcType::CLIENT, "prop_sync_from_base",     &ClientEntity::prop_sync_from_base, GBin());
            entity_rpc_regist(RpcType::CLIENT, "prop_sync_from_cell",     &ClientEntity::prop_sync_from_cell, GBin());
        }

        regist_migrate_rpc(entity_type);
        regist_entity_creator(entity_class_name, creator);

        EntityClassType::rpc_method_define();
    }

    void regist_migrate_rpc(enum EntityType entity_type) {
        if (entity_type == EntityType::EntityType_BaseWithCell) {
            entity_rpc_regist(RpcType::SERVER_ONLY, "migrate_req_from_cell", &BaseEntityWithCell::migrate_req_from_cell);
            entity_rpc_regist(RpcType::SERVER_ONLY, "new_cell_migrate_in", &BaseEntityWithCell::new_cell_migrate_in, GString());
        }
        else if (entity_type == EntityType::EntityType_BaseWithCellAndClient) {
            entity_rpc_regist(RpcType::SERVER_ONLY, "migrate_req_from_cell", &BaseEntityWithCellAndClient::migrate_req_from_cell);
            entity_rpc_regist(RpcType::SERVER_ONLY, "new_cell_migrate_in", &BaseEntityWithCellAndClient::new_cell_migrate_in, GString());
        }
        else if (entity_type == EntityType::EntityType_Cell) {
            entity_rpc_regist(RpcType::EXPOSED, "begin_migrate", &CellEntity::begin_migrate, GString());
            entity_rpc_regist(RpcType::EXPOSED, "migrate_reqack_from_base", &CellEntity::migrate_reqack_from_base, bool());
            entity_rpc_regist(RpcType::EXPOSED, "on_new_cell_migrate_finish", &CellEntity::on_new_cell_migrate_finish);
        }
        else if (entity_type == EntityType::EntityType_CellWithClient) {
            entity_rpc_regist(RpcType::EXPOSED, "begin_migrate", &CellEntityWithClient::begin_migrate, GString());
            entity_rpc_regist(RpcType::EXPOSED, "migrate_reqack_from_base", &CellEntityWithClient::migrate_reqack_from_base, bool());
            entity_rpc_regist(RpcType::EXPOSED, "migrate_reqack_from_client", &CellEntityWithClient::migrate_reqack_from_client, bool());
            entity_rpc_regist(RpcType::EXPOSED, "on_new_cell_migrate_finish", &CellEntityWithClient::on_new_cell_migrate_finish);
        }
        else if (entity_type == EntityType::EntityType_Client) {
            entity_rpc_regist(RpcType::SERVER_ONLY, "migrate_req_from_cell", &ClientEntity::migrate_req_from_cell);
            entity_rpc_regist(RpcType::SERVER_ONLY, "new_cell_migrate_in", &ClientEntity::new_cell_migrate_in, GString());
        }
    }

    template<class TEntity, class... T, class... T2>
    void entity_rpc_regist(enum RpcType entity_rpc_type, const GString& rpc_name, void(TEntity::*cb)(T...), T2... args) {
        ASSERT_LOG(sizeof...(T) == sizeof...(args), "rpc(%s) formal param size(%zu) != real param size(%zu)\n", rpc_name.c_str(), sizeof...(T), sizeof...(args));

        EntityRpcMethod<TEntity, T...>* method = new EntityRpcMethod<TEntity, T...>;
        method->cb = cb;
        method->type = entity_rpc_type;
        method->rpc_real_params_parse(args...);

        RpcFormalParamsCheck<T...>();
        add_rpc_method(rpc_name, method);

        get_local_entity_rpc_names()->push_back(rpc_name);
    }

    template<class TEntity, class... T>
    void entity_rpc_regist(enum RpcType entity_rpc_type, const GString& rpc_name, void(TEntity::*cb)(T...)) {
        ASSERT_LOG(sizeof...(T) == 0, "rpc(%s) formal param size(%zu) != 0\n", rpc_name.c_str(), sizeof...(T));

        EntityRpcMethod<TEntity, T...>* method = new EntityRpcMethod<TEntity, T...>;
        method->cb = cb;
        method->type = entity_rpc_type;
        add_rpc_method(rpc_name, method);

        get_local_entity_rpc_names()->push_back(rpc_name);
    }

    GString rpc_name_decode(Decoder& decoder) {
        uint8_t rpc_name_s = decoder.read_uint16();
        auto iter = g_entity_rpc_name_s2l.find(rpc_name_s);
        ASSERT_LOG(iter != g_entity_rpc_name_s2l.end(), "rpc.%d can not decompress\n", rpc_name_s);
        return iter->second;
    }

    void rpc_name_encode(Encoder& encoder, const GString& rpc_name) {
        auto iter = g_entity_rpc_name_l2s.find(rpc_name);
        ASSERT_LOG(iter != g_entity_rpc_name_l2s.end(), "rpc.%s can not compress\n", rpc_name.c_str());
        encoder.write_uint16(iter->second);
    }

    template<class TEntity, class... T>
    void entity_rpc_call(TEntity* _this, const GString& rpc_name, T... args) {

        RpcMethodBase* method = find_rpc_method(rpc_name);
        if (sizeof...(args) != method->m_params_t.size()) {
            ERROR_LOG("entity rpc.%s args num.%zd error, must be %zd\n", rpc_name.c_str(), sizeof...(args), method->m_params_t.size());
            return;
        }

        EntityRpcMethod<TEntity, T...>* entity_method = (EntityRpcMethod<TEntity, T...>*)method;
        auto _cb = entity_method->cb;
        (_this->*_cb)(args...);
    }

    EntityClassType tclass;
};

#define RPC_CALL_DEFINE(TCLASS) \
void rpc_call(bool from_client, const GString& rpc_name, const GArray& params) { \
\
    auto method = get_rpc_mgr()->find_rpc_method(rpc_name); \
    if (!method) { \
        ERROR_LOG("entity rpc.%s not exist\n", rpc_name.c_str()); \
        return; \
    } \
\
    if (!method->component_name.empty()) { \
        get_comp_mgr()->rpc_call(this, from_client, rpc_name, params); \
        return; \
    } \
\
    if (from_client && method->type != RpcType::EXPOSED) { \
        ERROR_LOG("entity rpc.%s can not call from client\n", rpc_name.c_str()); \
        return; \
    } \
 \
    auto rpc_mgr = (EntityRpcManager<TCLASS>*)get_rpc_mgr(); \
    switch (params.size()) { \
    case 0: \
        rpc_mgr->entity_rpc_call(this, rpc_name); \
        break; \
    case 1: \
        rpc_mgr->entity_rpc_call(this, rpc_name, params[0]); \
        break; \
    case 2: \
        rpc_mgr->entity_rpc_call(this, rpc_name, params[0], params[1]); \
        break; \
    case 3: \
        rpc_mgr->entity_rpc_call(this, rpc_name, params[0], params[1], params[2]); \
        break; \
    case 4: \
        rpc_mgr->entity_rpc_call(this, rpc_name, params[0], params[1], params[2], params[3]); \
        break; \
    case 5: \
        rpc_mgr->entity_rpc_call(this, rpc_name, params[0], params[1], params[2], params[3], params[4]); \
        break; \
    case 6: \
        rpc_mgr->entity_rpc_call(this, rpc_name, params[0], params[1], params[2], params[3], params[4], params[5]); \
        break; \
    case 7: \
        rpc_mgr->entity_rpc_call(this, rpc_name, params[0], params[1], params[2], params[3], params[4], params[5], params[6]); \
        break; \
    case 8: \
        rpc_mgr->entity_rpc_call(this, rpc_name, params[0], params[1], params[2], params[3], params[4], params[5], params[6], params[7]); \
        break; \
    case 9: \
        rpc_mgr->entity_rpc_call(this, rpc_name, params[0], params[1], params[2], params[3], params[4], params[5], params[6], params[7], params[8]); \
        break; \
    case 10: \
        rpc_mgr->entity_rpc_call(this, rpc_name, params[0], params[1], params[2], params[3], params[4], params[5], params[6], params[7], params[8], params[9]); \
        break; \
    default: \
        break; \
    } \
}

#define RPC_METHOD(rpc_type, rpc_name, ...) rpc_manager.entity_rpc_regist(rpc_type, #rpc_name, &decltype(rpc_manager.tclass)::rpc_name, ##__VA_ARGS__)
