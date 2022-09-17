#pragma once

#include "engine/entity_property_manager.h"

struct AvatarEquip : public EntityPropertyComplex {
    MEM_PROP_BEGIN_Two_MEM(AvatarEquip, equip_id, equip_level);

    MEM_PROPERTY_SIMPLE(equip_id, int32_t, 0);
    MEM_PROPERTY_SIMPLE(equip_level, int16_t, 0);
};

struct AvatarHorse : public EntityPropertyComplex {
    MEM_PROP_BEGIN_Two_MEM(AvatarHorse, horse_level, horse_name);

    MEM_PROPERTY_SIMPLE(horse_level, int32_t, 0);
    MEM_PROPERTY_SIMPLE(horse_name, GString, "");
};

struct AvatarExtraData : public EntityPropertyComplex {
    MEM_PROP_BEGIN_Three_MEM(AvatarExtraData, last_login_time, last_logout_time, last_dungeon_id);

    MEM_PROPERTY_SIMPLE(last_login_time, int64_t, 0);
    MEM_PROPERTY_SIMPLE(last_logout_time, int64_t, 0);
    MEM_PROPERTY_SIMPLE(last_dungeon_id, int64_t, 0);
};

struct AvatarData : public EntityPropertyComplex {
    MEM_PROP_BEGIN_Eight_MEM(AvatarData,
        avatar_id,
        avatar_name,
        avatar_level,
        avatar_extra_data,
        avatar_title_ids,
        avatar_equips,
        avatar_horses,
        avatar_fashion_shows);

    MEM_PROPERTY_SIMPLE(avatar_id, int32_t, 0);
    MEM_PROPERTY_SIMPLE(avatar_name, GString, "");
    MEM_PROPERTY_SIMPLE(avatar_level, int32_t, 0);
    MEM_PROPERTY_COMPLEX(avatar_extra_data, AvatarExtraData);
    MEM_PROPERTY_ARRAY(avatar_title_ids, int32_t);
    MEM_PROPERTY_ARRAY(avatar_equips, AvatarEquip);
    MEM_PROPERTY_MAP(avatar_fashion_shows, int32_t);
    MEM_PROPERTY_MAP(avatar_horses, AvatarHorse);
};

template<class TEntity>
void account_property_test() {

    PROPERTY_SIMPLE(PropType::ALL, account_id, int32_t, 1001);
    PROPERTY_SIMPLE(PropType::BASE_AND_CLIENT, account_id_base, int32_t, 1002);
    PROPERTY_SIMPLE(PropType::CELL_AND_CLIENT, account_id_cell, int32_t, 1003);
    PROPERTY_SIMPLE(PropType::BASE_PRIVATE, account_id_base_private, int32_t, 1004);
    PROPERTY_SIMPLE(PropType::CELL_PRIVATE, account_id_cell_private, int32_t, 1005);
    PROPERTY_MAP(PropType::BASE_AND_CLIENT, avatar_datas, AvatarData);
}

template<class TEntity>
void account_property_sync_test() {
    PROPERTY_SIMPLE(PropType::BASE_AND_CLIENT, simple_test, int32_t, 1001);
    PROPERTY_COMPLEX(PropType::BASE_AND_CLIENT, complex_test, AvatarData);
    PROPERTY_ARRAY(PropType::BASE_AND_CLIENT, array_test, GString);
    PROPERTY_MAP(PropType::BASE_AND_CLIENT, map_test, double);
    PROPERTY_ARRAY(PropType::BASE_AND_CLIENT, complex_array_test, AvatarEquip);
    PROPERTY_MAP(PropType::BASE_AND_CLIENT, complex_map_test, AvatarEquip);
}

void avatar_data_create(AvatarData& avatar_data) {
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

void avatar_data_print(EntityPropertyBase* avatar_datas) {

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

    INFO_LOG("############################ END ############################\n\n");
}

bool use_property_sync_test = true;

template<class TEntity>
void account_property_define() {
    if (!use_property_sync_test) {
        account_property_test<TEntity>();
    }
    else {
        account_property_sync_test<TEntity>();
    }
}
