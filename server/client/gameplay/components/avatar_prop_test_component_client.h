#pragma once

#include "engine/engine.h"

#include "common/prop_def/components/avatar_prop_def.h"


class AvatarPropTestComponentClient : public EntityComponentBase {

    GENERATE_COMPONENT_INNER(AvatarPropTestComponentClient);

    COMP_RPC_DEFINE() {
        COMP_RPC_METHOD(RpcType::CLIENT, prop_sync_compare);
    }

    COMP_MIGRATE_TIMER_DEFINE() {}
    COMP_MIGRATE_EVENT_DEFINE() {}

    COMP_PROPERTY_DEFINE() {
        AVATAR_PROPERTY_TEST_DEFINE();
    }

public:
    AvatarPropTestComponentClient() {}
    ~AvatarPropTestComponentClient() {}

    virtual void on_ready() {}

    void prop_sync_compare(const GBin& v);
};
