#pragma once

#include "engine/engine.h"


class AoiComponent : public EntityComponentBase {

    GENERATE_COMPONENT_INNER(AoiComponent);

    COMP_RPC_DEFINE() {
    }

    COMP_PROPERTY_DEFINE() {
    }

    COMP_MIGRATE_TIMER_DEFINE() {
    }
    COMP_MIGRATE_EVENT_DEFINE() {
        COMP_MIGRATE_EVENT_DEF(on_other_enter_aoi);
        COMP_MIGRATE_EVENT_DEF(on_other_leave_aoi);
        COMP_MIGRATE_EVENT_DEF(clear_aoi);
    }

public:
    AoiComponent() {}
    ~AoiComponent() {}

    void on_ready() {
        COMP_REGIST_EVENT(on_other_enter_aoi);
        COMP_REGIST_EVENT(on_other_leave_aoi);
        COMP_REGIST_EVENT(clear_aoi);
    }

    void tick();

    void on_other_enter_aoi(const GString& entity_uuid, int64_t p);
    void on_other_leave_aoi(const GString& entity_uuid, int64_t p);
    void clear_aoi();

    unordered_map<GString, Entity*> others;
};
