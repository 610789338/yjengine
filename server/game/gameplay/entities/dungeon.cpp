#include "engine/utils.h"
#include "game/framework/stub_base.h"

#include "dungeon.h"


GENERATE_ENTITY_OUT(Dungeon)


void Dungeon::apply_init_data(const GDict& init_data) {
    dungeon_id = init_data.at("dungeon_id").as_int32();
}

void Dungeon::on_ready() {
    INFO_LOG("Dungeon(%d) ready\n", dungeon_id);
    call_stub("DungeonManager", "on_dungeon_create", dungeon_id, get_self_mailbox());

    REGIST_EVENT(avatar_enter);
    REGIST_EVENT(avatar_leave);
}

void Dungeon::avatar_enter(const GString& avatar_uuid, int64_t p) {
    INFO_LOG("[Dungeon.%d] avatar.%s enter\n", dungeon_id, avatar_uuid.c_str());
    Entity* avatar = (Entity*)p;
    for (auto iter = avatars.begin(); iter != avatars.end(); ++iter) {
        auto other = iter->second;
        other->send_event("on_other_enter_aoi", avatar_uuid, (int64_t)avatar);
        avatar->send_event("on_other_enter_aoi", iter->first, (int64_t)other);
    }

    avatars.insert(make_pair(avatar_uuid, avatar));
}

void Dungeon::avatar_leave(const GString& avatar_uuid, int64_t p) {
    INFO_LOG("[Dungeon.%d] avatar.%s leave\n", dungeon_id, avatar_uuid.c_str());
    Entity* avatar = (Entity*)p;
    avatars.erase(avatar_uuid);
    for (auto iter = avatars.begin(); iter != avatars.end(); ++iter) {
        auto other = iter->second;
        other->send_event("on_other_leave_aoi", avatar_uuid, (int64_t)avatar);
    }

    avatar->send_event("clear_aoi");
}
