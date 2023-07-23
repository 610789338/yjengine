#include "engine/utils.h"
#include "game/framework/stub_base.h"

#include "dungeon.h"


GENERATE_ENTITY_OUT(Dungeon)


void Dungeon::apply_init_data(const GDict& init_data) {
    GET_PROP(dungeon_id).update(init_data.at("dungeon_id").as_int32());
}

void Dungeon::on_ready() {
    INFO_LOG("Dungeon(%d) ready\n", GET_PROP(dungeon_id).as_int32());
    call_stub("DungeonManager", "on_dungeon_create", GET_PROP(dungeon_id).as_int32(), get_self_mailbox());

    REGIST_EVENT(avatar_enter);
    REGIST_EVENT(avatar_leave);
}

void Dungeon::avatar_enter(const GString& avatar_uuid, const MailBox& avatar_mailbox) {
    INFO_LOG("[Dungeon.%d] avatar enter.%s\n", GET_PROP(dungeon_id).as_int32(), avatar_uuid.c_str());
    GET_PROP(avatars).insert(avatar_uuid, avatar_mailbox);
}

void Dungeon::avatar_leave(const GString& avatar_uuid, const MailBox& avatar_mailbox) {
    INFO_LOG("[Dungeon.%d] avatar leave.%s\n", GET_PROP(dungeon_id).as_int32(), avatar_uuid.c_str());
    auto& avatars = GET_PROP(avatars);
    if (avatars.has(avatar_uuid)) {
        avatars.erase(avatar_uuid);
    }
}
