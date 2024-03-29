#include "engine/utils.h"

#include "game/framework/distributed_notify.h"
#include "dungeon_manager.h"


GENERATE_ENTITY_OUT(DungeonManager)


void DungeonManager::on_ready() {
    REGIST_TIMER(5, 0, false, create_dungeons);
}

void DungeonManager::create_dungeons() {
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

void DungeonManager::enter_random_dungeon(const MailBox& avatar) {
    auto& all_dungeon_proxys = GET_PROP(all_dungeon_proxys);

    auto keys = all_dungeon_proxys.keys();
    if (keys.empty()) {
        return;
    }
    auto nowms = nowms_timestamp();
    auto idx = nowms % keys.size();
    auto dungeon_id = std::stoi(keys[idx]);
    enter_dungeon(avatar, dungeon_id);
}

void DungeonManager::enter_dungeon(const MailBox& avatar, int32_t dungeon_id) {
    auto& all_dungeon_proxys = GET_PROP(all_dungeon_proxys);
    if (!all_dungeon_proxys.has(std::to_string(dungeon_id))) {
        ERROR_LOG("%s enter dungeon.%d failed", avatar.get_entity_uuid().c_str(), dungeon_id);
        return;
    }

    INFO_LOG("avatar.%s enter dungeon.%d\n", avatar.get_entity_uuid().c_str(), dungeon_id);

    auto& dungeon_proxys = all_dungeon_proxys.GET(std::to_string(dungeon_id)).MEM(proxys);
    auto keys = dungeon_proxys.keys();
    auto nowms = nowms_timestamp();
    auto idx = nowms % keys.size();
    auto& dungeon_proxy = dungeon_proxys.GET(keys[idx]);
    avatar.to_proxy().call("enter_dungeon_ack", dungeon_id, dungeon_proxy.MEM(mailbox).as_mailbox());
}
