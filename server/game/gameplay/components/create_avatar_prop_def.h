#pragma once

#include "engine/engine.h"


struct AvatarEquip : public EntityPropertyComplex {
    MEM_PROP_BEGIN_Two_MEM(AvatarEquip, equip_id, equip_level);

    MEM_PROPERTY_SIMPLE(equip_id, int32_t, 0);
    MEM_PROPERTY_SIMPLE(equip_level, int16_t, 0);
};

struct AvatarHorse : public EntityPropertyComplex {
    MEM_PROP_BEGIN_Two_MEM(AvatarHorse, horse_level, horse_name);

    MEM_PROPERTY_SIMPLE(horse_level, int32_t, 0);
    MEM_PROPERTY_SIMPLE(horse_name, GString, "");
};

struct AvatarExtraData : public EntityPropertyComplex {
    MEM_PROP_BEGIN_Three_MEM(AvatarExtraData, last_login_time, last_logout_time, last_dungeon_id);

    MEM_PROPERTY_SIMPLE(last_login_time, int64_t, 0);
    MEM_PROPERTY_SIMPLE(last_logout_time, int64_t, 0);
    MEM_PROPERTY_SIMPLE(last_dungeon_id, int64_t, 0);
};

struct AvatarData : public EntityPropertyComplex {
    MEM_PROP_BEGIN_Eight_MEM(AvatarData,
        avatar_id,
        avatar_name,
        avatar_level,
        avatar_extra_data,
        avatar_title_ids,
        avatar_equips,
        avatar_horses,
        avatar_fashion_shows);

    MEM_PROPERTY_SIMPLE(avatar_id, int32_t, 0);
    MEM_PROPERTY_SIMPLE(avatar_name, GString, "");
    MEM_PROPERTY_SIMPLE(avatar_level, int32_t, 0);
    MEM_PROPERTY_COMPLEX(avatar_extra_data, AvatarExtraData);
    MEM_PROPERTY_ARRAY(avatar_title_ids, int32_t);
    MEM_PROPERTY_ARRAY(avatar_equips, AvatarEquip);
    MEM_PROPERTY_MAP(avatar_fashion_shows, int32_t);
    MEM_PROPERTY_MAP(avatar_horses, AvatarHorse);
};

template<class TEntity>
void create_avatar_property_define() {
    PROPERTY_MAP(PropType::BASE_AND_CLIENT, avatar_datas, AvatarData);
}
