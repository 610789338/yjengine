#pragma once

#include "engine/entity_property_manager.h"
#include "alias.h"

struct DungeonProxy : public EntityPropertyComplex {
    MEM_PROP_BEGIN_2_MEM(DungeonProxy, 
        id, 
        mailbox);

    MEM_PROP_SIMPLE(id, int32_t, 0);
    MEM_PROP_SIMPLE(mailbox, MailBox, MailBox());
};

struct DungeonProxyContainer : public EntityPropertyComplex {
    MEM_PROP_BEGIN_1_MEM(DungeonProxyContainer, proxys);

    MEM_PROP_MAP(proxys, DungeonProxy);
};
