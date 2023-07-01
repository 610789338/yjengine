#include "engine/utils.h"

#include "game/framework/distributed_notify.h"
#include "dungeon_manager.h"


GENERATE_ENTITY_OUT(DungeonManager)


void DungeonManager::on_ready() {
    auto dungeon_ids_ini = ini_get_string("Dungeon", "dungeon_ids", "");
    if (dungeon_ids_ini != "") {
        vector<string> dungeon_ids;
        split(dungeon_ids_ini, "|", dungeon_ids);

        for (auto iter1 = dungeon_ids.begin(); iter1 != dungeon_ids.end(); ++iter1) {
            create_dungeon(std::stoi(*iter1));
        }
    }
}

void DungeonManager::create_dungeon(int32_t dungeon_id) {
    GDict init_data, base_init_data;
    base_init_data.insert(make_pair("dungeon_id", dungeon_id));
    init_data.insert(make_pair("base_data", base_init_data));
    GAME_UNICAST_RANDOM(create_entity, "Dungeon", "", "", "", init_data);
}

void DungeonManager::on_dungeon_create(int32_t dungeon_id, const MailBox& dungeon_mailbox) {
    auto& all_dungeon_proxys = GET_PROP(all_dungeon_proxys);
    if (!all_dungeon_proxys.has(std::to_string(dungeon_id))) {
        all_dungeon_proxys.insert(std::to_string(dungeon_id), DungeonProxyContainer());
    }

    auto& dungeon_proxys = all_dungeon_proxys.GET(std::to_string(dungeon_id)).MEM(proxys);
    DungeonProxy dungeon_proxy;
    dungeon_proxy.MEM(id).update(dungeon_id);
    dungeon_proxy.MEM(mailbox).update(dungeon_mailbox);
    dungeon_proxys.insert(dungeon_mailbox.get_entity_uuid(), dungeon_proxy);
}

void DungeonManager::enter_dungeon(const MailBox& avatar, int32_t dungeon_id) {

}
