#include "engine/engine.h"

#include "create_avatar_component.h"
#include "create_avatar_prop_def.h"

void CreateAvatarComponent::component_rpc_test(const GValue& msg) {

    INFO_LOG("[base] component_rpc_test.%s\n", msg.as_string().c_str());

    property_test();
}

void CreateAvatarComponent::property_test() {
    property_create();
    property_read();
    property_update();
    property_delete();
    property_read();
}

void CreateAvatarComponent::property_create() {

    AvatarData avatar_data;
    avatar_data.avatar_id = 101;
    avatar_data.avatar_name = "youjun";
    avatar_data.avatar_level = 666;

    avatar_data.avatar_extra_data.last_login_time = 0;
    avatar_data.avatar_extra_data.last_logout_time = 0;
    avatar_data.avatar_extra_data.last_dungeon_id = 0;

    avatar_data.avatar_title_ids.push_back(1);
    avatar_data.avatar_title_ids.push_back(2);
    avatar_data.avatar_title_ids.push_back(3);

    AvatarEquip avatar_equip;
    avatar_equip.equip_id = 1001;
    avatar_equip.equip_level = 1;
    avatar_data.avatar_equips.push_back(avatar_equip);

    avatar_equip.equip_id = 1002;
    avatar_equip.equip_level = 1;
    avatar_data.avatar_equips.push_back(avatar_equip);

    avatar_equip.equip_id = 1003;
    avatar_equip.equip_level = 1;
    avatar_data.avatar_equips.push_back(avatar_equip);

    avatar_data.avatar_fashion_shows.insert("fashion_shows_1", 60);
    avatar_data.avatar_fashion_shows.insert("fashion_shows_2", 70);
    avatar_data.avatar_fashion_shows.insert("fashion_shows_3", 80);

    AvatarHorse avatar_horse;
    avatar_horse.horse_level = 100;
    avatar_horse.horse_name = "gold_dragon";
    avatar_data.avatar_horses.insert("horse_1", avatar_horse);

    avatar_horse.horse_level = 90;
    avatar_horse.horse_name = "phoenix";
    avatar_data.avatar_horses.insert("horse_2", avatar_horse);

    avatar_horse.horse_level = 80;
    avatar_horse.horse_name = "panda";
    avatar_data.avatar_horses.insert("horse_3", avatar_horse);

    get_prop("avatar_datas")->insert(to_string(avatar_data.avatar_id.as_int32()), avatar_data);

    avatar_data.avatar_id = 102;
    avatar_data.avatar_name = "qianqian";
    get_prop("avatar_datas")->insert(to_string(avatar_data.avatar_id.as_int32()), avatar_data);

    avatar_data.avatar_id = 103;
    avatar_data.avatar_name = "duoduo";
    get_prop("avatar_datas")->insert(to_string(avatar_data.avatar_id.as_int32()), avatar_data);
}

void CreateAvatarComponent::property_read() {
    
    auto avatar_datas = get_prop("avatar_datas");
    auto keys = avatar_datas->keys();
    for (auto iter = keys.begin(); iter != keys.end(); ++iter) {

        auto avatar_data = avatar_datas->get(*iter);
        INFO_LOG("avatar_id(%d) avatar_name(%s) avatar_level(%d)\n", 
            avatar_data->get("avatar_id")->as_int32(),
            avatar_data->get("avatar_name")->as_string().c_str(),
            avatar_data->get("avatar_level")->as_int32()
        );

        INFO_LOG("last_login_time(%lld) last_logout_time(%lld) last_dungeon_id(%lld)\n",
            avatar_data->get("avatar_extra_data")->get("last_login_time")->as_int64(),
            avatar_data->get("avatar_extra_data")->get("last_logout_time")->as_int64(),
            avatar_data->get("avatar_extra_data")->get("last_dungeon_id")->as_int64()
        );

        auto avatar_title_ids = avatar_data->get("avatar_title_ids");
        for (int32_t j = 0; j < avatar_title_ids->size(); ++j) {
            INFO_LOG("    title_id(%d)\n", avatar_title_ids->get(j)->as_int32());
        }

        auto avatar_equips = avatar_data->get("avatar_equips");
        for (int32_t j = 0; j < avatar_equips->size(); ++j) {
            auto equip_id = avatar_equips->get(j)->get("equip_id")->as_int32();
            auto equip_level = avatar_equips->get(j)->get("equip_level")->as_int16();
            INFO_LOG("    equip_id(%d) equip_level(%d)\n", equip_id, equip_level);
        }

        auto avatar_fashion_shows = avatar_data->get("avatar_fashion_shows");
        auto keys1 = avatar_fashion_shows->keys();
        for (auto iter = keys1.begin(); iter != keys1.end(); ++iter) {
            INFO_LOG("    fashion_shows(%s - %d)\n", (*iter).c_str(), avatar_fashion_shows->get(*iter)->as_int32());
        }

        auto avatar_horses = avatar_data->get("avatar_horses");
        auto keys2 = avatar_horses->keys();
        for (auto iter = keys2.begin(); iter != keys2.end(); ++iter) {
            INFO_LOG("    horses(%s) level(%d) name(%s)\n", 
                (*iter).c_str(), 
                avatar_horses->get(*iter)->get("horse_level")->as_int32(),
                avatar_horses->get(*iter)->get("horse_name")->as_string().c_str()
            );
        }
    }
}

void CreateAvatarComponent::property_delete() {

    get_prop("avatar_datas")->get("101")->get("avatar_title_ids")->pop_back();
    get_prop("avatar_datas")->get("101")->get("avatar_title_ids")->pop_back();
    get_prop("avatar_datas")->get("101")->get("avatar_title_ids")->pop_back();

    get_prop("avatar_datas")->get("102")->get("avatar_fashion_shows")->erase("fashion_shows_1");
    get_prop("avatar_datas")->get("102")->get("avatar_fashion_shows")->erase("fashion_shows_2");
    get_prop("avatar_datas")->get("102")->get("avatar_fashion_shows")->erase("fashion_shows_3");

    get_prop("avatar_datas")->get("101")->get("avatar_horses")->erase("horse_3");

    get_prop("avatar_datas")->erase("103");
}

void CreateAvatarComponent::property_update() {

    get_prop("avatar_datas")->get("101")->get("avatar_level")->update(996);
    get_prop("avatar_datas")->get("101")->get("avatar_name")->update("johnyou");
    get_prop("avatar_datas")->get("101")->get("avatar_title_ids")->update(0, 10);

    get_prop("avatar_datas")->get("101")->get("avatar_extra_data")->get("last_login_time")->update(1);
    get_prop("avatar_datas")->get("101")->get("avatar_extra_data")->get("last_logout_time")->update(1);
    get_prop("avatar_datas")->get("101")->get("avatar_extra_data")->get("last_dungeon_id")->update(1);

    AvatarEquip avatar_equip;
    avatar_equip.equip_id = 1004;
    avatar_equip.equip_level = 10;
    get_prop("avatar_datas")->get("101")->get("avatar_equips")->update(2, avatar_equip);
    get_prop("avatar_datas")->get("101")->get("avatar_equips")->get(2)->get("equip_level")->update(20);

    get_prop("avatar_datas")->get("102")->get("avatar_fashion_shows")->get("fashion_shows_1")->update(100);
    get_prop("avatar_datas")->get("102")->get("avatar_fashion_shows")->get("fashion_shows_2")->update(100);
    get_prop("avatar_datas")->get("102")->get("avatar_fashion_shows")->get("fashion_shows_3")->update(100);

    get_prop("avatar_datas")->get("101")->get("avatar_horses")->get("horse_1")->get("horse_level")->update(110);
    get_prop("avatar_datas")->get("101")->get("avatar_horses")->get("horse_2")->get("horse_name")->update("hell_dragon");

    AvatarHorse avatar_horse;
    avatar_horse.horse_level = 120;
    avatar_horse.horse_name = "gold_angel";
    get_prop("avatar_datas")->get("102")->get("avatar_horses")->update("horse_3", avatar_horse);
}
