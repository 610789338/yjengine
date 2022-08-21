#pragma once

#include "engine/engine.h"
#include "../entities/account_prop_def.h"


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
        COMP_PROPERTY_MAP(PropType::BASE_AND_CLIENT, avatar_datas, AvatarData);
    }

    void component_rpc_test(const GValue& msg);

    void property_test();
    void property_create();
    void property_read();
    void property_delete();
    void property_update();
};
