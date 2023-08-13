#include "game/framework/stub_base.h"

#include "dungeon_component.h"

void DungeonComponent::timer_enter_dungeon() {
    //INFO_LOG("[DungeonManager] timer_enter_dungeon %d entity.%s\n", 
    //    get_owner()->get_self_mailbox().get_flag(), get_owner()->get_self_mailbox().get_entity_uuid().c_str());
    call_stub("DungeonManager", "enter_random_dungeon", get_owner()->get_self_mailbox());
}

void DungeonComponent::enter_dungeon(int32_t dungeon_id) {
    if (GET_PROP(dungeon_id).as_int32() == dungeon_id) {
        WARN_LOG("already in dungeon.%d\n", dungeon_id);
        return;
    }

    call_stub("DungeonManager", "enter_dungeon", get_owner()->get_self_mailbox(), dungeon_id);
}

void DungeonComponent::enter_dungeon_ack(int32_t new_dungeon_id, const MailBox& new_dungeon_mailbox) {
    if (GET_PROP(dungeon_id).as_int32() == new_dungeon_id) {
        return;
    }

    if (GET_PROP(next_dungeon_mailbox).as_mailbox().is_valid()) {
        WARN_LOG("already entering %d\n", GET_PROP(next_dungeon_id).as_int32());
        return;
    }

    leave_cur_dungeon();
    GET_PROP(next_dungeon_id).update(new_dungeon_id);
    GET_PROP(next_dungeon_mailbox).update(new_dungeon_mailbox);
    if (new_dungeon_mailbox.get_addr() != get_listen_addr()) {
        COMP_SEND_EVENT("begin_migrate", new_dungeon_mailbox.get_addr());
    }
    else {
        enter_next_dungeon();
    }
}

void DungeonComponent::leave_cur_dungeon() {
    if (GET_PROP(dungeon_id).as_int32() == 0) {
        return;
    }

    auto& old_dungeon_mailbox = GET_PROP(dungeon_mailbox).as_mailbox();
    auto dungeon_entity = old_dungeon_mailbox.get_owner();
    if (dungeon_entity) {
        dungeon_entity->send_event("avatar_leave", get_owner()->uuid, (int64_t)get_owner());
    }

    GET_PROP(dungeon_id).update(0);
    GET_PROP(dungeon_mailbox).update(MailBox());
}

void DungeonComponent::on_migrate_in() {
    enter_next_dungeon();
}

void DungeonComponent::enter_next_dungeon() {
    auto& new_dungeon_mailbox = GET_PROP(next_dungeon_mailbox).as_mailbox();
    if (!new_dungeon_mailbox.is_valid()) {
        return;
    }

    auto dungeon_entity = new_dungeon_mailbox.get_owner();
    if (dungeon_entity) {
        dungeon_entity->send_event("avatar_enter", get_owner()->uuid, (int64_t)get_owner());
    }

    GET_PROP(dungeon_id).update(GET_PROP(next_dungeon_id).as_int32());
    GET_PROP(dungeon_mailbox).update(GET_PROP(next_dungeon_mailbox).as_mailbox());
    GET_PROP(next_dungeon_id).update(0);
    GET_PROP(next_dungeon_mailbox).update(MailBox());
}
