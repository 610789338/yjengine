#pragma once

#include "engine/engine.h"


class CreateAvatarComponent : public EntityComponentBase {

    GENERATE_COMPONENT_INNER(CreateAvatarComponent)

public:
    CreateAvatarComponent() {}
    ~CreateAvatarComponent() {}

    template<class TEntity, class TEntityComp>
    static void rpc_method_define() {
        COMP_RPC_METHOD(RpcType::EXPOSED, component_rpc_test, GString());
    }

    void component_rpc_test(const GValue& msg);

    void property_test();
    void property_create();
    void property_delete();
    void property_update();
};
