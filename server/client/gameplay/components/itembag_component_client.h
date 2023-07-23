#pragma once

#include "engine/engine.h"

#include "common/prop_def/components/itembag_prop_def.h"


class ItemBagComponentClient : public EntityComponentBase {

    GENERATE_COMPONENT_INNER(ItemBagComponentClient);

    COMP_RPC_DEFINE() {}

    COMP_MIGRATE_TIMER_DEFINE() {}
    COMP_MIGRATE_EVENT_DEFINE() {}

    COMP_PROPERTY_DEFINE() {
        ITEMBAG_PROPERTY_DEFINE();
    }

public:
    ItemBagComponentClient() {}
    ~ItemBagComponentClient() {}

    virtual void on_ready() {
    }
};
