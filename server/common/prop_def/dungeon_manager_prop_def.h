#pragma once

#include "engine/entity_property_manager.h"
#include "alias.h"

struct DungeonProxy : public EntityPropertyComplex {
    MEM_PROP_BEGIN_3_MEM(DungeonProxy, 
        uuid, 
        id, 
        addr);

    MEM_PROP_SIMPLE(uuid, GString, "");
    MEM_PROP_SIMPLE(id, int32_t, 0);
    MEM_PROP_SIMPLE(addr, GString, "");
};
