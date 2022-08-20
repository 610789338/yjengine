#include "engine/engine.h"

#include "create_avatar_component.h"

void CreateAvatarComponent::component_rpc_test(const GValue& msg) {

    INFO_LOG("[base] component_rpc_test.%s\n", msg.as_string().c_str());

    //property_test();
}

//void CreateAvatarComponent::property_test() {
//    property_create();
//    property_read();
//    property_update();
//    property_delete();
//    property_read();
//}
//
//void CreateAvatarComponent::property_create() {
//
//    AvatarData avatar_data;
//    avatar_data.avatar_id = 101;
//    avatar_data.avatar_name = "youjun";
//    avatar_data.avatar_level = 666;
//    avatar_data.avatar_title_ids.push_back(int32_t(1));
//    avatar_data.avatar_title_ids.push_back(int32_t(2));
//    avatar_data.avatar_title_ids.push_back(int32_t(3));
//
//    AvatarEquip avatar_equip;
//    avatar_equip.equip_id = 1001;
//    avatar_equip.equip_level = 1;
//    avatar_data.avatar_equips.push_back(avatar_equip);
//
//    avatar_equip.equip_id = 1002;
//    avatar_equip.equip_level = 1;
//    avatar_data.avatar_equips.push_back(avatar_equip);
//
//    avatar_equip.equip_id = 1003;
//    avatar_equip.equip_level = 1;
//    avatar_data.avatar_equips.push_back(avatar_equip);
//
//    avatar_data.avatar_fashion_shows.insert("fashion_shows_1", int32_t(60));
//    avatar_data.avatar_fashion_shows.insert("fashion_shows_2", int32_t(70));
//    avatar_data.avatar_fashion_shows.insert("fashion_shows_3", int32_t(80));
//
//    AvatarHorse avatar_horse;
//    avatar_horse.horse_level = 100;
//    avatar_horse.horse_name = "gold_dragon";
//    avatar_data.avatar_horses.insert("horse_1", avatar_horse);
//
//    avatar_horse.horse_level = 90;
//    avatar_horse.horse_name = "phoenix";
//    avatar_data.avatar_horses.insert("horse_2", avatar_horse);
//
//    avatar_horse.horse_level = 80;
//    avatar_horse.horse_name = "panda";
//    avatar_data.avatar_horses.insert("horse_3", avatar_horse);
//
//    get_prop("avatar_datas")->insert(itoa(avatar_data.avatar_id), avatar_data);
//
//    avatar_data.avatar_id = 102;
//    avatar_data.avatar_name = "qianqian";
//    get_prop("avatar_datas")->insert(itoa(avatar_data.avatar_id), avatar_data);
//
//    avatar_data.avatar_id = 103;
//    avatar_data.avatar_name = "duoduo";
//    get_prop("avatar_datas")->insert(itoa(avatar_data.avatar_id), avatar_data);
//}
//
//void CreateAvatarComponent::property_read() {
//    
//    for (int32_t i = 0; i < get_prop("avatar_datas")->size(); ++i) {
//        auto avatar_data = get_prop("avatar_datas")->get(i);
//        INFO_LOG("avatar_id(%s) avatar_name(%s) avatar_level(%d)\n", 
//            avatar_data->avatar_id.get().as_string().c_str(),
//            avatar_data->avatar_name.get().as_string().c_str(),
//            avatar_data->avatar_level.get().as_int32()
//        );
//
//        for (int32_t j = 0; j < avatar_data->avatar_title_ids.size(); ++j) {
//            INFO_LOG("    title_id(%d)\n", avatar_data->avatar_title_ids.get(j).get().as_int32());
//        }
//
//        for (int32_t j = 0; j < avatar_data->avatar_equips.size(); ++j) {
//            auto avatar_equip = avatar_data->avatar_equips.get(j);
//            INFO_LOG("    equip_id(%d) equip_level(%d)\n", avatar_equip.equip_id, avatar_equip.equip_level);
//        }
//
//        auto keys = avatar_data->avatar_fashion_shows.keys();
//        for (auto iter = keys.begin(); iter != keys.end(); ++iter) {
//            INFO_LOG("    fashion_shows(%s - %d)\n", (*iter).c_str(), avatar_data->avatar_fashion_shows.get(*iter).get().as_int32());
//        }
//
//        auto keys = avatar_data->avatar_horses.keys();
//        for (auto iter = keys.begin(); iter != keys.end(); ++iter) {
//            INFO_LOG("    horses(%s) level(%d) name(%s)\n", 
//                (*iter).c_str(), 
//                avatar_data->avatar_horses.get(*iter).horse_level.get().as_int32(),
//                avatar_data->avatar_horses.get(*iter).horse_name.get().as_string().c_str()
//            );
//        }
//    }
//}
//
//void CreateAvatarComponent::property_delete() {
//
//    get_prop("avatar_datas")->get(0)->get("avatar_title_ids")->pop_back();
//    get_prop("avatar_datas")->get(0)->get("avatar_title_ids")->pop_back();
//    get_prop("avatar_datas")->get(0)->get("avatar_title_ids")->pop_back();
//
//    get_prop("avatar_datas")->get(1)->get("avatar_fashion_shows")->erase("fashion_shows_1");
//    get_prop("avatar_datas")->get(1)->get("avatar_fashion_shows")->erase("fashion_shows_2");
//    get_prop("avatar_datas")->get(1)->get("avatar_fashion_shows")->erase("fashion_shows_3");
//
//    get_prop("avatar_datas")->get(0)->get("avatar_horses")->erase("horse_1");
//    get_prop("avatar_datas")->get(0)->get("avatar_horses")->erase("horse_2");
//    get_prop("avatar_datas")->get(0)->get("avatar_horses")->erase("horse_3");
//
//    get_prop("avatar_datas")->pop_back();
//}
//
//void CreateAvatarComponent::property_update() {
//    get_prop("avatar_datas")->get(0)->get("avatar_level")->update(int32_t(996));
//    get_prop("avatar_datas")->get(0)->get("avatar_name")->update("johnyou");
//    get_prop("avatar_datas")->get(0)->get("avatar_title_ids")->update(0, int32_t(10));
//
//    AvatarEquip avatar_equip;
//    avatar_equip.equip_id = 1004;
//    avatar_equip.equip_level = 10;
//    get_prop("avatar_datas")->get(0)->get("avatar_equips")->update(2, avatar_equip);
//    get_prop("avatar_datas")->get(0)->get("avatar_equips")->get(2)->get("equip_level")->update(int16_t(20));
//
//    get_prop("avatar_datas")->get(1)->get("avatar_fashion_shows")->get("fashion_shows_1")->update(int32_t(100));
//    get_prop("avatar_datas")->get(1)->get("avatar_fashion_shows")->get("fashion_shows_2")->update(int32_t(100));
//    get_prop("avatar_datas")->get(1)->get("avatar_fashion_shows")->get("fashion_shows_3")->update(int32_t(100));
//
//    get_prop("avatar_datas")->get(2)->get("avatar_horses")->get("horse_1")->get("horse_level")->update(int32_t(110));
//    get_prop("avatar_datas")->get(2)->get("avatar_horses")->get("horse_2")->get("horse_name")->update("hell_dragon");
//
//    AvatarHorse avatar_horse;
//    avatar_horse.horse_level = 120;
//    avatar_horse.horse_name = "gold_angel";
//    get_prop("avatar_datas")->get(2)->get("avatar_horses")->modify("horse_3", avatar_horse);
//}
