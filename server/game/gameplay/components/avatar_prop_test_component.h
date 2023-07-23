#pragma once

#include "engine/engine.h"

#include "common/prop_def/components/avatar_prop_def.h"


class AvatarPropTestComponent : public EntityComponentBase {

    GENERATE_COMPONENT_INNER(AvatarPropTestComponent);

    COMP_RPC_DEFINE() {}

    COMP_PROPERTY_DEFINE() {
        AVATAR_PROPERTY_TEST_DEFINE();
    }

    COMP_MIGRATE_TIMER_DEFINE() {
        COMP_MIGRATE_TIMER_DEF(avatar_timer_prop_sync);
    }
    COMP_MIGRATE_EVENT_DEFINE() {}

public:
    AvatarPropTestComponent() {}
    ~AvatarPropTestComponent() {}

    void on_ready();

    // property sync
    void avatar_timer_prop_sync();
    void property_sync_test();
    void property_sync_test_create();
    void property_sync_test_clear();
    void property_sync_test_add();
    void property_sync_test_del();
    void property_sync_test_update();
    void avatar_data_create(AvatarData& avatar_data);

private:
    int32_t sync_count = 0;
    int32_t incr = 0;
};
