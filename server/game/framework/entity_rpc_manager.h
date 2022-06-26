#pragma once

#include "engine/engine.h"

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

// entity rpc manager
class EntityRpcManager : public RpcManagerBase {
public:
    EntityRpcManager() = delete;
    EntityRpcManager(const GString& belong, const GString& entity_class_name, const function<Entity*()>& creator);

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

    template<class TEntity, class... T>
    void entity_rpc_call(TEntity* _this, const GString& rpc_name, T... args) {
        RpcMethodBase* method = find_rpc_method(rpc_name);

        if (sizeof...(args) != method->m_params_t.size()) {
            ERROR_LOG("rpc.%s args num.%zd error, must be %zd\n", rpc_name.c_str(), sizeof...(args), method->m_params_t.size());
            return;
        }

        auto _cb = ((EntityRpcMethod<TEntity, T...>*)method)->cb;
        //auto _cb = (*method).cb;
        (_this->*_cb)(args...);
    }
};
