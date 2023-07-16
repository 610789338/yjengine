#pragma once

#include "engine/engine.h"


class DungeonComponent : public EntityComponentBase {

    GENERATE_COMPONENT_INNER(DungeonComponent);

    COMP_RPC_DEFINE() {
        COMP_RPC_METHOD(RpcType::EXPOSED, enter_dungeon);
        COMP_RPC_METHOD(RpcType::SERVER_ONLY, enter_dungeon_ack);
    }

    COMP_PROPERTY_DEFINE() {
        _property_define<TEntity>();
    }

    template<class TEntity>
    static void _property_define() {
        PROPERTY_SIMPLE(PropType::CELL_PRIVATE, dungeon_id, int32_t, 0);
        PROPERTY_SIMPLE(PropType::CELL_PRIVATE, dungeon_mailbox, MailBox, MailBox());
        PROPERTY_SIMPLE(PropType::CELL_PRIVATE, next_dungeon_id, int32_t, 0);
        PROPERTY_SIMPLE(PropType::CELL_PRIVATE, next_dungeon_mailbox, MailBox, MailBox());
    }

    COMP_MIGRATE_TIMER_DEFINE() {
        COMP_MIGRATE_TIMER_DEF(timer_enter_dungeon);
    }

public:
    DungeonComponent() {}
    ~DungeonComponent() {}

    // TODO - delete after migrate support
    void before_migrate_in() {
        COMP_REGIST_EVENT("on_migrate_in", on_migrate_in);
    }

    void on_ready() {
        COMP_REGIST_TIMER(1, 10, true, timer_enter_dungeon);
        COMP_REGIST_EVENT("on_migrate_in", on_migrate_in);
    }

    void timer_enter_dungeon();

    void enter_dungeon(int32_t dungeon_id);
    void enter_dungeon_ack(int32_t dungeon_id, const MailBox& new_dungeon_mailbox);
    void leave_cur_dungeon();

    void on_migrate_in();
    void enter_next_dungeon();
};
