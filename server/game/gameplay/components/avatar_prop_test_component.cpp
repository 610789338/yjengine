#include "avatar_prop_test_component.h"

void AvatarPropTestComponent::on_ready() {
    COMP_REGIST_TIMER(0, 1, true, avatar_timer_prop_sync);
}

void AvatarPropTestComponent::avatar_timer_prop_sync() {
    property_sync_test();
}

void AvatarPropTestComponent::property_sync_test() {
    Encoder encoder;
    get_owner()->serialize_all_client(encoder);
    encoder.write_end();

    if (encoder.anything()) {
        Client().call("prop_sync_compare", GBin(encoder.get_buf(), encoder.get_offset()));
    }

    //INFO_LOG("AvatarPropTestComponent::property_sync_test %d\n", sync_count);
    if (sync_count == 0) {
        property_sync_test_clear();
    }
    else if (sync_count == 1) {
        property_sync_test_create();
    }
    else if (sync_count % 10 == 5) {
        property_sync_test_del();
    }
    else if (sync_count % 10 == 6) {
        property_sync_test_add();
    }
    else {
        auto select = nowms_timestamp() % 2;
        switch (select)
        {
        case 0:
            property_sync_test_add();
            break;
        default:
            property_sync_test_update();
            break;
        }
    }

    ++sync_count;
    sync_count %= 100;
}

void AvatarPropTestComponent::property_sync_test_create() {

    AvatarData* avatar_data = (AvatarData*)get_prop("complex_test");
    avatar_data_create(*avatar_data);

    get_prop("array_test")->push_back("array1");
    get_prop("array_test")->push_back("array2");
    get_prop("array_test")->push_back("array3");

    get_prop("map_test")->insert("map1", 1.1);
    get_prop("map_test")->insert("map2", 2.2);
    get_prop("map_test")->insert("map3", 3.3);

    AvatarEquip avatar_equip;
    avatar_equip.equip_id = 1001;
    avatar_equip.equip_level = 1;
    get_prop("complex_array_test")->push_back(avatar_equip);
    get_prop("complex_map_test")->insert(to_string(avatar_equip.equip_id.as_int32()), avatar_equip);

    avatar_equip.equip_id = 1002;
    avatar_equip.equip_level = 1;
    get_prop("complex_array_test")->push_back(avatar_equip);
    get_prop("complex_map_test")->insert(to_string(avatar_equip.equip_id.as_int32()), avatar_equip);

    avatar_equip.equip_id = 1003;
    avatar_equip.equip_level = 1;
    get_prop("complex_array_test")->push_back(avatar_equip);
    get_prop("complex_map_test")->insert(to_string(avatar_equip.equip_id.as_int32()), avatar_equip);
}

void AvatarPropTestComponent::property_sync_test_clear() {

    get_prop("complex_test")->get("avatar_title_ids")->clear();
    get_prop("complex_test")->get("avatar_equips")->clear();
    get_prop("complex_test")->get("avatar_fashion_shows")->clear();
    get_prop("complex_test")->get("avatar_horses")->clear();
    get_prop("array_test")->clear();
    get_prop("map_test")->clear();
    get_prop("complex_array_test")->clear();
    get_prop("complex_map_test")->clear();
}

void AvatarPropTestComponent::property_sync_test_add() {

    get_prop("complex_test")->get("avatar_title_ids")->push_back(incr);

    AvatarEquip avatar_equip;
    avatar_equip.equip_id = incr * 10;
    avatar_equip.equip_level = (int16_t)(incr & 0xFFFF);
    get_prop("complex_test")->get("avatar_equips")->push_back(avatar_equip);

    get_prop("complex_test")->get("avatar_fashion_shows")->insert(to_string(incr), incr);

    AvatarHorse avatar_horse;
    avatar_horse.horse_level = (int16_t)(incr & 0xFFFF);
    avatar_horse.horse_name = to_string(incr);
    get_prop("complex_test")->get("avatar_horses")->insert(to_string(incr), avatar_horse);

    get_prop("array_test")->push_back(to_string(incr * 100000 + incr));
    get_prop("complex_array_test")->push_back(avatar_equip);
    get_prop("complex_map_test")->insert(to_string(avatar_equip.equip_id.as_int32()), avatar_equip);

    ++incr;
}

void AvatarPropTestComponent::property_sync_test_del() {

    get_prop("complex_test")->get("avatar_title_ids")->pop_back();
    get_prop("complex_test")->get("avatar_equips")->pop_back();

    {
        auto fashion_shows = get_prop("complex_test")->get("avatar_fashion_shows");
        auto keys = fashion_shows->keys();
        for (auto iter = keys.begin(); iter != keys.end(); ++iter) {
            fashion_shows->erase(*iter);
        }
    }

    {
        auto horses = get_prop("complex_test")->get("avatar_horses");
        auto keys = horses->keys();
        for (auto iter = keys.begin(); iter != keys.end(); ++iter) {
            horses->erase(*iter);
        }
    }

    get_prop("array_test")->pop_back();
    get_prop("complex_array_test")->pop_back();

    {
        auto map_test = get_prop("map_test");
        auto keys = map_test->keys();
        for (auto iter = keys.begin(); iter != keys.end(); ++iter) {
            map_test->erase(*iter);
        }
    }

    {
        auto complex_map_test = get_prop("complex_map_test");
        auto keys = complex_map_test->keys();
        for (auto iter = keys.begin(); iter != keys.end(); ++iter) {
            complex_map_test->erase(*iter);
        }
    }
}

void AvatarPropTestComponent::property_sync_test_update() {

    get_prop("complex_test")->get("avatar_level")->update(incr);
    get_prop("complex_test")->get("avatar_name")->update(to_string(incr));
    get_prop("complex_test")->get("avatar_extra_data")->get("last_logout_time")->update(incr);

    AvatarEquip avatar_equip;
    avatar_equip.equip_id = incr;
    avatar_equip.equip_level = incr >> 16;
    get_prop("complex_test")->get("avatar_equips")->update(1, avatar_equip);
    get_prop("complex_test")->get("avatar_equips")->get(1)->get("equip_level")->update(incr);

    {
        auto fashion_shows = get_prop("complex_test")->get("avatar_fashion_shows");
        auto keys = fashion_shows->keys();
        fashion_shows->get(keys[0])->update(incr);
    }

    {
        auto horses = get_prop("complex_test")->get("avatar_horses");
        auto keys = horses->keys();
        horses->get(keys[0])->get("horse_level")->update(incr);
        horses->get(keys[0])->get("horse_name")->update(to_string(incr));
    }

    ++incr;
}

void AvatarPropTestComponent::avatar_data_create(AvatarData& avatar_data) {
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
}
