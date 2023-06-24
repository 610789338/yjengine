#include "engine/utils.h"

#include "game/framework/distributed_notify.h"
#include "dungeon_manager.h"


GENERATE_ENTITY_OUT(DungeonMananger)


void DungeonMananger::enter_dungeon(const MailBox& avatar, int32_t dungeon_id) {

}

void DungeonMananger::req_create_dungeon(int32_t dungeon_id) {
    //GAME_UNICAST_RANDOM(create_dungeon, dungeon_id, get_self_mailbox());
}

void DungeonMananger::on_dungeon_create(const GString& dungeon_uuid, int32_t dungeon_id, const MailBox& dungeon_mailbox) {

}
