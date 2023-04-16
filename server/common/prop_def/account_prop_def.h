#pragma once

#include "engine/entity_property_manager.h"


template<class TEntity>
void account_property_sync_test() {
    PROPERTY_SIMPLE(PropType::BASE_AND_CLIENT, simple_test, int32_t, 1001);
    PROPERTY_ARRAY(PropType::BASE_AND_CLIENT, array_test, GString);
    PROPERTY_MAP(PropType::BASE_AND_CLIENT, map_test, double);
}

template<class TEntity>
void account_property_define() {
    account_property_sync_test<TEntity>();
}
