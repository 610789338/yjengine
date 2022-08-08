#pragma once

#include "engine/engine.h"


class CreateAvatarComponent : public EntityComponentBase {

    GENERATE_COMPONENT_INNER(CreateAvatarComponent)

public:
    CreateAvatarComponent(GString component_name, Entity* _owner) : EntityComponentBase(component_name, _owner) {}
    ~CreateAvatarComponent() {}

    void component_rpc_test(const GValue& msg) { 
        INFO_LOG("[base] component_rpc_test.%s\n", msg.as_string().c_str()); 
    }
};

template<class TEntity, class TEntityComp>
void CreateAvatarComponent::rpc_method_define() {
    COMP_RPC_METHOD(RpcType::EXPOSED, component_rpc_test, GString());
}

template<class TEntity, class TEntityComp>
void CreateAvatarComponent::property_define() {
    //PROPERTY(PropType::BASE_AND_CLIENT, "char_type", 1001);
}
