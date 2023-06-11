#pragma once

#include "engine/engine.h"

#include "common/prop_def/components/itembag_prop_def.h"


class ItemBagComponent : public EntityComponentBase {

    GENERATE_COMPONENT_INNER(ItemBagComponent);

    COMP_RPC_DEFINE() {}

    COMP_PROPERTY_DEFINE() {
        ITEMBAG_PROPERTY_DEFINE();
    }

    COMP_MIGRATE_TIMER_DEFINE() {}

public:
    ItemBagComponent() {}
    ~ItemBagComponent() {}

    virtual void on_ready();
    void init_itembag();
    void add_item(int8_t itembag_id, int32_t item_id, int16_t num);
    void del_item(int8_t itembag_id, int32_t item_id, int16_t num, const GString& uuid);
    void add_uuid_item(int8_t itembag_id, int32_t item_id, int16_t num);
    void del_uuid_item(int8_t itembag_id, const GString& uuid);

    bool get_holdn_pile_item(int8_t itembag_id, int32_t item_id, Item** item);
    void add_new_pile_item(int8_t itembag_id, int32_t item_id, int16_t num);
};
