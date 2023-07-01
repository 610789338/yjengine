#include "engine/utils.h"
#include "game/framework/stub_base.h"

#include "dungeon.h"


GENERATE_ENTITY_OUT(Dungeon)


void Dungeon::apply_init_data(const GDict& init_data) {
    GET_PROP(dungeon_id).update(init_data.at("dungeon_id").as_int32());
}

void Dungeon::on_ready() {
    INFO_LOG("Dungeonon(%d) ready\n", GET_PROP(dungeon_id).as_int32());
    call_stub("DungeonManager", "on_dungeon_create", GET_PROP(dungeon_id).as_int32(), get_self_mailbox());
}
