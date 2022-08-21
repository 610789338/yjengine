#include "engine/entity_property_manager.h"

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

struct AvatarData : public EntityPropertyComplex {
    MEM_PROP_BEGIN_Seven_MEM(AvatarData, 
        avatar_id, 
        avatar_name, 
        avatar_level, 
        avatar_title_ids, 
        avatar_equips, 
        avatar_horses, 
        avatar_fashion_shows);

    MEM_PROPERTY_SIMPLE(avatar_id, int32_t, 0);
    MEM_PROPERTY_SIMPLE(avatar_name, GString, "");
    MEM_PROPERTY_SIMPLE(avatar_level, int32_t, 0);
    MEM_PROPERTY_ARRAY(avatar_title_ids, int32_t);
    MEM_PROPERTY_ARRAY(avatar_equips, AvatarEquip);
    MEM_PROPERTY_MAP(avatar_fashion_shows, int32_t);
    MEM_PROPERTY_MAP(avatar_horses, AvatarHorse);
};
