#pragma once

#include "engine/entity_property_manager.h"


struct AvatarEquip : public EntityPropertyComplex {
    MEM_PROP_BEGIN_2_MEM(AvatarEquip, equip_id, equip_level);

    MEM_PROP_SIMPLE(equip_id, int32_t, 0);
    MEM_PROP_SIMPLE(equip_level, int16_t, 0);
};

struct AvatarHorse : public EntityPropertyComplex {
    MEM_PROP_BEGIN_2_MEM(AvatarHorse, horse_level, horse_name);

    MEM_PROP_SIMPLE(horse_level, int32_t, 0);
    MEM_PROP_SIMPLE(horse_name, GString, "");
};

struct AvatarExtraData : public EntityPropertyComplex {
    MEM_PROP_BEGIN_3_MEM(AvatarExtraData, last_login_time, last_logout_time, last_dungeon_id);

    MEM_PROP_SIMPLE(last_login_time, int64_t, 0);
    MEM_PROP_SIMPLE(last_logout_time, int64_t, 0);
    MEM_PROP_SIMPLE(last_dungeon_id, int64_t, 0);
};

struct AvatarData : public EntityPropertyComplex {
    MEM_PROP_BEGIN_8_MEM(AvatarData,
        avatar_id,
        avatar_name,
        avatar_level,
        avatar_extra_data,
        avatar_title_ids,
        avatar_equips,
        avatar_horses,
        avatar_fashion_shows);

    MEM_PROP_SIMPLE(avatar_id, int32_t, 0);
    MEM_PROP_SIMPLE(avatar_name, GString, "");
    MEM_PROP_SIMPLE(avatar_level, int32_t, 0);
    MEM_PROP_COMPLEX(avatar_extra_data, AvatarExtraData);
    MEM_PROP_ARRAY(avatar_title_ids, int32_t);
    MEM_PROP_ARRAY(avatar_equips, AvatarEquip);
    MEM_PROP_MAP(avatar_fashion_shows, int32_t);
    MEM_PROP_MAP(avatar_horses, AvatarHorse);
};

template<class TEntity>
void avatar_property_sync_test() {
    //PROPERTY_SIMPLE(PropType::BASE_AND_CLIENT, simple_test, int32_t, 1001);
    //PROPERTY_COMPLEX(PropType::BASE_AND_CLIENT, complex_test, AvatarData);
    //PROPERTY_ARRAY(PropType::BASE_AND_CLIENT, array_test, GString);
    //PROPERTY_MAP(PropType::BASE_AND_CLIENT, map_test, double);
    //PROPERTY_ARRAY(PropType::BASE_AND_CLIENT, complex_array_test, AvatarEquip);
    //PROPERTY_MAP(PropType::BASE_AND_CLIENT, complex_map_test, AvatarEquip);

    PROPERTY_SIMPLE(PropType::CELL_AND_CLIENT, simple_test, int32_t, 1001);
    PROPERTY_COMPLEX(PropType::CELL_AND_CLIENT, complex_test, AvatarData);
    PROPERTY_ARRAY(PropType::CELL_AND_CLIENT, array_test, GString);
    PROPERTY_MAP(PropType::CELL_AND_CLIENT, map_test, double);
    PROPERTY_ARRAY(PropType::CELL_AND_CLIENT, complex_array_test, AvatarEquip);
    PROPERTY_MAP(PropType::CELL_AND_CLIENT, complex_map_test, AvatarEquip);
}

template<class TEntity>
void avatar_property_define() {
    avatar_property_sync_test<TEntity>();
}

#define AVATAR_PROPERTY_TEST_DEFINE \
    avatar_property_sync_test<TEntity>
    