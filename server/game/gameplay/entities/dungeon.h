#pragma once

#include "engine/engine.h"

#include "common/utils/utils.h"


class Dungeon : public BaseEntity {

    GENERATE_ENTITY_INNER(Dungeon);

    static void regist_components() {
    }
    static void rpc_method_define() {
    }
    static void property_define() {
        _property_define<Dungeon>();
    }
    template<class TEntity>
    static void _property_define() {
        PROPERTY_SIMPLE(PropType::BASE_PRIVATE, dungeon_id, int32_t, 0);
        PROPERTY_MAP(PropType::BASE_PRIVATE, avatars, MailBox);
    }

    static void migrate_timer_define() {}
    static void migrate_event_define() {}

public:
    Dungeon() {}
    ~Dungeon() {}

    void apply_init_data(const GDict& init_data);
    void on_ready();

    void avatar_enter(const GString& avatar_uuid, const MailBox& avatar_mailbox);
    void avatar_leave(const GString& avatar_uuid, const MailBox& avatar_mailbox);
};
