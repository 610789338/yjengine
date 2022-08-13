#include "engine/entity_property_manager.h"


struct AvatarData : public EntityPropertyComplex {
    MEM_PROPERTY_SIMPLE(avatar_id, int32_t, 0);
    MEM_PROPERTY_SIMPLE(avatar_name, GString, "");
};

struct AvatarDatas : public EntityPropertyComplex {
    MEM_PROPERTY_ARRAY(avatar_data, AvatarData);
};
