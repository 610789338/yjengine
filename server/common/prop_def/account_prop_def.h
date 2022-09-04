#pragma once

#include "engine/entity_property_manager.h"
#include "game/gameplay/components/create_avatar_prop_def.h"


template<class TEntity>
void account_property_define() {

    PROPERTY_SIMPLE(PropType::ALL, account_id, int32_t, 1001);
    PROPERTY_SIMPLE(PropType::BASE_AND_CLIENT, account_id_base, int32_t, 1002);
    PROPERTY_SIMPLE(PropType::CELL_AND_CLIENT, account_id_cell, int32_t, 1003);
    PROPERTY_SIMPLE(PropType::BASE_PRIVATE, account_id_base_private, int32_t, 1004);
    PROPERTY_SIMPLE(PropType::CELL_PRIVATE, account_id_cell_private, int32_t, 1005);

    create_avatar_property_define<TEntity>();
}
