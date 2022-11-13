#pragma once

#include "engine/engine.h"

struct AvatarData;

class CreateAvatarComponent : public EntityComponentBase {

    GENERATE_COMPONENT_INNER(CreateAvatarComponent)

public:
    CreateAvatarComponent() {}
    ~CreateAvatarComponent() {}

    template<class TEntity, class TEntityComp>
    static void rpc_method_define() {
        COMP_RPC_METHOD(RpcType::EXPOSED, component_rpc_test);
    }

    template<class TEntity>
    static void timer_cb_store() {
        COMP_STORE_TIMER_CB_FOR_MIGRATE(CreateAvatarComponent::component_timer_test);
    }

    void on_ready() {
        COMP_REGIST_TIMER(0, 5, true, CreateAvatarComponent::component_timer_test, "args1");
    }

    void component_rpc_test(const GString& msg);
    void component_timer_test(const GString& msg);
};
