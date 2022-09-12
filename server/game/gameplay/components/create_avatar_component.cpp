#include "engine/engine.h"

#include "create_avatar_component.h"
#include "create_avatar_prop_def.h"

void CreateAvatarComponent::component_rpc_test(const GValue& msg) {

    INFO_LOG("[base] component_rpc_test.%s\n", msg.as_string().c_str());

    property_test();
}

extern void prop_read_for_test(EntityPropertyBase*);

void CreateAvatarComponent::property_test() {
    property_create();
    prop_read_for_test(get_prop("avatar_datas"));
    get_owner()->propertys_sync2client();

    property_update();
    prop_read_for_test(get_prop("avatar_datas"));
    get_owner()->propertys_sync2client();

    property_delete();
    prop_read_for_test(get_prop("avatar_datas"));
    get_owner()->propertys_sync2client();
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
