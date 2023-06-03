#pragma once

#include "engine/engine.h"

#include "common/prop_def/components/itembag_prop_def.h"


class ItemBagComponent : public EntityComponentBase {

    GENERATE_COMPONENT_INNER(ItemBagComponent);

    COMP_RPC_DEFINE() {}

    COMP_MIGRATE_TIMER_DEFINE() {}

    COMP_PROPERTY_DEFINE() {
        ITEMBAG_PROPERTY_DEFINE();
    }

public:
    ItemBagComponent() {}
    ~ItemBagComponent() {}

    virtual void on_ready() {
    }
};
