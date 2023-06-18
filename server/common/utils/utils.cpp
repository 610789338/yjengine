//#include "utils.h"
//
//void avatar_data_print(EntityPropertyBase* avatar_data) {
//    INFO_LOG("avatar_id(%d) avatar_name(%s) avatar_level(%d)\n",
//        avatar_data->get("avatar_id")->as_int32(),
//        avatar_data->get("avatar_name")->as_string().c_str(),
//        avatar_data->get("avatar_level")->as_int32()
//    );
//
//    INFO_LOG("last_login_time(%lld) last_logout_time(%lld) last_dungeon_id(%lld)\n",
//        avatar_data->get("avatar_extra_data")->get("last_login_time")->as_int64(),
//        avatar_data->get("avatar_extra_data")->get("last_logout_time")->as_int64(),
//        avatar_data->get("avatar_extra_data")->get("last_dungeon_id")->as_int64()
//    );
//
//    auto avatar_title_ids = avatar_data->get("avatar_title_ids");
//    for (int32_t j = 0; j < avatar_title_ids->size(); ++j) {
//        INFO_LOG("    title_id(%d)\n", avatar_title_ids->get(j)->as_int32());
//    }
//
//    auto avatar_equips = avatar_data->get("avatar_equips");
//    for (int32_t j = 0; j < avatar_equips->size(); ++j) {
//        auto equip_id = avatar_equips->get(j)->get("equip_id")->as_int32();
//        auto equip_level = avatar_equips->get(j)->get("equip_level")->as_int16();
//        INFO_LOG("    equip_id(%d) equip_level(%d)\n", equip_id, equip_level);
//    }
//
//    auto avatar_fashion_shows = avatar_data->get("avatar_fashion_shows");
//    auto keys1 = avatar_fashion_shows->keys();
//    for (auto iter = keys1.begin(); iter != keys1.end(); ++iter) {
//        INFO_LOG("    fashion_shows(%s - %d)\n", (*iter).c_str(), avatar_fashion_shows->get(*iter)->as_int32());
//    }
//
//    auto avatar_horses = avatar_data->get("avatar_horses");
//    auto keys2 = avatar_horses->keys();
//    for (auto iter = keys2.begin(); iter != keys2.end(); ++iter) {
//        INFO_LOG("    horses(%s) level(%d) name(%s)\n",
//            (*iter).c_str(),
//            avatar_horses->get(*iter)->get("horse_level")->as_int32(),
//            avatar_horses->get(*iter)->get("horse_name")->as_string().c_str()
//        );
//    }
//}
//
//void avatar_datas_print(EntityPropertyBase* avatar_datas) {
//    return;
//
//    auto keys = avatar_datas->keys();
//    for (auto iter = keys.begin(); iter != keys.end(); ++iter) {
//        auto avatar_data = avatar_datas->get(*iter);
//        avatar_data_print(avatar_data);
//    }
//
//    INFO_LOG("############################ END ############################\n\n");
//}
