#pragma once

#include "engine/entity_property_manager.h"

struct Item : public EntityPropertyComplex {
    MEM_PROP_BEGIN_4_MEM(Item,
        uuid,
        item_id,
        pile_cnt,
        extra_data);

    MEM_PROP_SIMPLE(uuid, GString, "");
    MEM_PROP_SIMPLE(item_id, int32_t, 0);
    MEM_PROP_SIMPLE(pile_cnt, int16_t, 0);
    MEM_PROP_SIMPLE(extra_data, GValue, GValue(GDict()));
};

struct ItemBag : public EntityPropertyComplex {
    MEM_PROP_BEGIN_3_MEM(ItemBag, 
        itembag_id, 
        item_list,
        item_limit);

    MEM_PROP_SIMPLE(itembag_id, int8_t, 0);
    MEM_PROP_MAP(item_list, Item);
    MEM_PROP_SIMPLE(item_limit, int16_t, 1024);
};

template<class TEntity>
void itembag_property_define() {
    PROPERTY_MAP(PropType::BASE_AND_CLIENT, itembags, ItemBag);
}

#define ITEMBAG_PROPERTY_DEFINE \
    itembag_property_define<TEntity>
