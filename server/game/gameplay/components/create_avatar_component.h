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
    
    template<class TEntity>
    static void property_define() {
        COMP_PROPERTY(PropType::BASE_AND_CLIENT, "char_type", 1001);
    }

    void component_rpc_test(const GValue& msg);
};
