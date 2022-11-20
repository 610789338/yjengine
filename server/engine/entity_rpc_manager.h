﻿#pragma once

#include "gvalue.h"
#include "log.h"
#include "rpc_manager.h"
#include "entity.h"


class Entity;

template<class TEntity>
struct EntityRpcMethod0 : public RpcMethodBase {
    typedef void(TEntity::*CBType)();
    EntityRpcMethod0(CBType _cb) : cb(_cb) {}
    CBType cb;

    void decode(Decoder& decoder) {}
    void exec(void* _this) { (((TEntity*)_this)->*cb)(); }
    RpcMethodBase* create_self() { return new EntityRpcMethod0(cb); }
};

template<class TEntity, class T1>
struct EntityRpcMethod1 : public RpcMethodBase {
    typedef void(TEntity::*CBType)(T1);
    EntityRpcMethod1(CBType _cb) : cb(_cb) {}
    CBType cb;
    RMCVR(T1) t1;

    void decode(Decoder& decoder) {
        t1 = decoder_read<RMCVR(T1)>(decoder);
    }
    void exec(void* _this) { (((TEntity*)_this)->*cb)(t1); }
    RpcMethodBase* create_self() { return new EntityRpcMethod1(cb); }
};

template<class TEntity, class T1, class T2>
struct EntityRpcMethod2 : public RpcMethodBase {
    typedef void(TEntity::*CBType)(T1, T2);
    EntityRpcMethod2(CBType _cb) : cb(_cb) {}
    CBType cb;
    RMCVR(T1) t1;
    RMCVR(T2) t2;

    void decode(Decoder& decoder) {
        t1 = decoder_read<RMCVR(T1)>(decoder);
        t2 = decoder_read<RMCVR(T2)>(decoder);
    }
    void exec(void* _this) { (((TEntity*)_this)->*cb)(t1, t2); }
    RpcMethodBase* create_self() { return new EntityRpcMethod2(cb); }
};

template<class TEntity, class T1, class T2, class T3>
struct EntityRpcMethod3 : public RpcMethodBase {
    typedef void(TEntity::*CBType)(T1, T2, T3);
    EntityRpcMethod3(CBType _cb) : cb(_cb) {}
    CBType cb;
    RMCVR(T1) t1;
    RMCVR(T2) t2;
    RMCVR(T3) t3;

    void decode(Decoder& decoder) {
        t1 = decoder_read<RMCVR(T1)>(decoder);
        t2 = decoder_read<RMCVR(T2)>(decoder);
        t3 = decoder_read<RMCVR(T3)>(decoder);
    }
    void exec(void* _this) { (((TEntity*)_this)->*cb)(t1, t2, t3); }
    RpcMethodBase* create_self() { return new EntityRpcMethod3(cb); }
};

template<class TEntity, class T1, class T2, class T3, class T4>
struct EntityRpcMethod4 : public RpcMethodBase {
    typedef void(TEntity::*CBType)(T1, T2, T3, T4);
    EntityRpcMethod4(CBType _cb) : cb(_cb) {}
    CBType cb;
    RMCVR(T1) t1;
    RMCVR(T2) t2;
    RMCVR(T3) t3;
    RMCVR(T4) t4;

    void decode(Decoder& decoder) {
        t1 = decoder_read<RMCVR(T1)>(decoder);
        t2 = decoder_read<RMCVR(T2)>(decoder);
        t3 = decoder_read<RMCVR(T3)>(decoder);
        t4 = decoder_read<RMCVR(T4)>(decoder);
    }
    void exec(void* _this) { (((TEntity*)_this)->*cb)(t1, t2, t3, t4); }
    RpcMethodBase* create_self() { return new EntityRpcMethod4(cb); }
};

template<class TEntity, class T1, class T2, class T3, class T4, class T5>
struct EntityRpcMethod5 : public RpcMethodBase {
    typedef void(TEntity::*CBType)(T1, T2, T3, T4, T5);
    EntityRpcMethod5(CBType _cb) : cb(_cb) {}
    CBType cb;
    RMCVR(T1) t1;
    RMCVR(T2) t2;
    RMCVR(T3) t3;
    RMCVR(T4) t4;
    RMCVR(T5) t5;

    void decode(Decoder& decoder) {
        t1 = decoder_read<RMCVR(T1)>(decoder);
        t2 = decoder_read<RMCVR(T2)>(decoder);
        t3 = decoder_read<RMCVR(T3)>(decoder);
        t4 = decoder_read<RMCVR(T4)>(decoder);
        t5 = decoder_read<RMCVR(T5)>(decoder);
    }
    void exec(void* _this) { (((TEntity*)_this)->*cb)(t1, t2, t3, t4, t5); }
    RpcMethodBase* create_self() { return new EntityRpcMethod5(cb); }
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
        if (entity_type == EntityType::EntityType_BaseWithCell) {
            entity_rpc_regist(RpcType::SERVER_ONLY, "on_cell_create", &BaseEntityWithCell::on_cell_create);
        }
        else if (entity_type == EntityType::EntityType_BaseWithCellAndClient) {
            entity_rpc_regist(RpcType::SERVER_ONLY, "on_cell_create", &BaseEntityWithCellAndClient::on_cell_create);
            entity_rpc_regist(RpcType::EXPOSED, "on_client_reconnect_success", &BaseEntityWithCellAndClient::on_client_reconnect_success);
            entity_rpc_regist(RpcType::EXPOSED, "ready", &BaseEntityWithCellAndClient::ready);
        }
        else if (entity_type == EntityType::EntityType_CellWithClient) {
            entity_rpc_regist(RpcType::SERVER_ONLY, "on_reconnect_fromclient", &CellEntityWithClient::on_reconnect_fromclient);
            entity_rpc_regist(RpcType::SERVER_ONLY, "on_client_reconnect_success", &CellEntityWithClient::on_client_reconnect_success);
            entity_rpc_regist(RpcType::SERVER_ONLY, "cell_real_time_to_save", &CellEntityWithClient::cell_real_time_to_save);
            entity_rpc_regist(RpcType::SERVER_ONLY, "ready", &CellEntityWithClient::ready);
        }
        else if (entity_type == EntityType::EntityType_Client) {
            entity_rpc_regist(RpcType::CLIENT, "prop_sync_from_base", &ClientEntity::prop_sync_from_base);
            entity_rpc_regist(RpcType::CLIENT, "prop_sync_from_cell", &ClientEntity::prop_sync_from_cell);
            entity_rpc_regist(RpcType::CLIENT, "ready", &ClientEntity::ready);
            entity_rpc_regist(RpcType::CLIENT, "on_kick", &ClientEntity::on_kick);
        }

        regist_migrate_rpc(entity_type);
        regist_entity_creator(entity_class_name, creator);

        EntityClassType::rpc_method_define();
    }

    void regist_migrate_rpc(enum EntityType entity_type) {
        if (entity_type == EntityType::EntityType_BaseWithCell) {
            entity_rpc_regist(RpcType::SERVER_ONLY, "migrate_req_from_cell", &BaseEntityWithCell::migrate_req_from_cell);
            entity_rpc_regist(RpcType::SERVER_ONLY, "new_cell_migrate_in", &BaseEntityWithCell::new_cell_migrate_in);
        }
        else if (entity_type == EntityType::EntityType_BaseWithCellAndClient) {
            entity_rpc_regist(RpcType::SERVER_ONLY, "migrate_req_from_cell", &BaseEntityWithCellAndClient::migrate_req_from_cell);
            entity_rpc_regist(RpcType::SERVER_ONLY, "new_cell_migrate_in", &BaseEntityWithCellAndClient::new_cell_migrate_in);
        }
        else if (entity_type == EntityType::EntityType_Cell) {
            entity_rpc_regist(RpcType::EXPOSED, "begin_migrate", &CellEntity::begin_migrate);
            entity_rpc_regist(RpcType::EXPOSED, "migrate_reqack_from_base", &CellEntity::migrate_reqack_from_base);
            entity_rpc_regist(RpcType::EXPOSED, "on_new_cell_migrate_finish", &CellEntity::on_new_cell_migrate_finish);
        }
        else if (entity_type == EntityType::EntityType_CellWithClient) {
            entity_rpc_regist(RpcType::EXPOSED, "begin_migrate", &CellEntityWithClient::begin_migrate);
            entity_rpc_regist(RpcType::EXPOSED, "migrate_reqack_from_base", &CellEntityWithClient::migrate_reqack_from_base);
            entity_rpc_regist(RpcType::EXPOSED, "migrate_reqack_from_client", &CellEntityWithClient::migrate_reqack_from_client);
            entity_rpc_regist(RpcType::EXPOSED, "on_new_cell_migrate_finish", &CellEntityWithClient::on_new_cell_migrate_finish);
        }
        else if (entity_type == EntityType::EntityType_Client) {
            entity_rpc_regist(RpcType::SERVER_ONLY, "migrate_req_from_cell", &ClientEntity::migrate_req_from_cell);
            entity_rpc_regist(RpcType::SERVER_ONLY, "new_cell_migrate_in", &ClientEntity::new_cell_migrate_in);
        }
    }

    template<class TEntity>
    void entity_rpc_regist(enum RpcType entity_rpc_type, const GString& rpc_name, void(TEntity::*cb)()) {
        RpcMethodBase* method = new EntityRpcMethod0<TEntity>(cb);
        method->type = entity_rpc_type;
        add_rpc_method(rpc_name, method);
        get_local_entity_rpc_names()->push_back(rpc_name);
    }

    template<class TEntity, class T1>
    void entity_rpc_regist(enum RpcType entity_rpc_type, const GString& rpc_name, void(TEntity::*cb)(T1)) {
        RpcMethodBase* method = new EntityRpcMethod1<TEntity, T1>(cb);
        method->type = entity_rpc_type;
        add_rpc_method(rpc_name, method);
        get_local_entity_rpc_names()->push_back(rpc_name);
    }

    template<class TEntity, class T1, class T2>
    void entity_rpc_regist(enum RpcType entity_rpc_type, const GString& rpc_name, void(TEntity::*cb)(T1, T2)) {
        RpcMethodBase* method = new EntityRpcMethod2<TEntity, T1, T2>(cb);
        method->type = entity_rpc_type;
        add_rpc_method(rpc_name, method);
        get_local_entity_rpc_names()->push_back(rpc_name);
    }

    template<class TEntity, class T1, class T2, class T3>
    void entity_rpc_regist(enum RpcType entity_rpc_type, const GString& rpc_name, void(TEntity::*cb)(T1, T2, T3)) {
        RpcMethodBase* method = new EntityRpcMethod3<TEntity, T1, T2, T3>(cb);
        method->type = entity_rpc_type;
        add_rpc_method(rpc_name, method);
        get_local_entity_rpc_names()->push_back(rpc_name);
    }

    template<class TEntity, class T1, class T2, class T3, class T4>
    void entity_rpc_regist(enum RpcType entity_rpc_type, const GString& rpc_name, void(TEntity::*cb)(T1, T2, T3, T4)) {
        RpcMethodBase* method = new EntityRpcMethod4<TEntity, T1, T2, T3, T4>(cb);
        method->type = entity_rpc_type;
        add_rpc_method(rpc_name, method);
        get_local_entity_rpc_names()->push_back(rpc_name);
    }

    template<class TEntity, class T1, class T2, class T3, class T4, class T5>
    void entity_rpc_regist(enum RpcType entity_rpc_type, const GString& rpc_name, void(TEntity::*cb)(T1, T2, T3, T4, T5)) {
        RpcMethodBase* method = new EntityRpcMethod5<TEntity, T1, T2, T3, T4, T5>(cb);
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

    EntityClassType tclass;
};

#define RPC_CALL_DEFINE(TCLASS) \
void rpc_call(bool from_client, const GString& rpc_name, RpcMethodBase* rpc_method) { \
\
    auto method = get_rpc_mgr()->find_rpc_method(rpc_name); \
    if (!method->component_name.empty()) { \
        get_comp_mgr()->rpc_call(this, from_client, rpc_name, rpc_method); \
        return; \
    } \
\
    if (from_client && method->type != RpcType::EXPOSED) { \
        ERROR_LOG("entity rpc.%s can not call from client\n", rpc_name.c_str()); \
        return; \
    } \
\
    rpc_method->exec((void*)this); \
}

#define RPC_METHOD(rpc_type, rpc_name) rpc_manager.entity_rpc_regist(rpc_type, #rpc_name, &decltype(rpc_manager.tclass)::rpc_name)
