#pragma once

#include "engine/entity_property_manager.h"


template<class TEntity>
void itembag_property_define() {
    PROPERTY_SIMPLE(PropType::BASE_AND_CLIENT, itembag_id, int32_t, 1001);
}

#define ITEMBAG_PROPERTY_DEFINE \
    itembag_property_define<TEntity>
