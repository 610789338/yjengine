#pragma once

#include "engine/engine.h"


class AoiComponentClient : public EntityComponentBase {

    GENERATE_COMPONENT_INNER(AoiComponentClient);

    COMP_RPC_DEFINE() {
        COMP_RPC_METHOD(RpcType::CLIENT, on_other_enter_aoi);
        COMP_RPC_METHOD(RpcType::CLIENT, on_other_leave_aoi);
        COMP_RPC_METHOD(RpcType::CLIENT, other_prop_sync_from_cell);
    }

    COMP_MIGRATE_TIMER_DEFINE() {}
    COMP_MIGRATE_EVENT_DEFINE() {}

    COMP_PROPERTY_DEFINE() {
    }

public:
    AoiComponentClient() {}
    ~AoiComponentClient() {}

    virtual void on_ready() {
    }

    void on_other_enter_aoi(const GString& entity_uuid, const GString& entity_name, const GBin& v);
    void on_other_leave_aoi(const GString& entity_uuid);
    void other_prop_sync_from_cell(const GString& entity_uuid, const GBin& v);

    unordered_map<GString, Entity*> others;
};
