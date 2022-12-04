#include "engine/utils.h"

#include "account.h"
#include "common/prop_def/account_prop_def.h"


GENERATE_ENTITY_OUT(BaseAccount)


void BaseAccount::property_define() {
    account_property_define<BaseAccount>();
}

void BaseAccount::on_ready() {

    INFO_LOG("BaseAccount on_ready\n");

    cell.call("msg_from_base", "hello, i am base");
    client.call("msg_from_base", "hello, i am base");

    //test_timer = REGIST_TIMER(5, 60, true, account_timer_test, "1 minutes");
    prop_timer = REGIST_TIMER(0, 1, true, account_timer_prop_sync);
    migrate_timer = REGIST_TIMER(0, 5, true, account_migrate_timer);

    REGIST_EVENT("event_test", entity_event_test);
    REGIST_TIMER(5, 0, false, account_event_timer);

#ifndef __PROP_SYNC_TEST__
    property_test();
#endif
}

void BaseAccount::entity_event_test(const GString& msg) {
    INFO_LOG("[base] entity_event_test.%s \n", msg.c_str());
}

void BaseAccount::account_event_timer() {
    SEND_EVENT("event_test", "hahahaha");
}

void BaseAccount::msg_from_cell(const GString& msg) {
    INFO_LOG("[base] msg.%s from cell\n", msg.c_str()); 
}

void BaseAccount::msg_from_client(const GString& msg) {
    INFO_LOG("[base] msg.%s from client\n", msg.c_str()); 
}

void BaseAccount::account_timer_test(const GString& arg1) {
    INFO_LOG("[Base] this is timer test arg1.%s\n", arg1.c_str());
}

void BaseAccount::account_migrate_timer() {
    GArray addrs;
    addrs.push_back(get_listen_addr());
    addrs.push_back(IPPORT_STRING(ini_get_string("MigrateAddr", "ip"), ini_get_int("MigrateAddr", "port")));

    if (cell.get_addr() == addrs[0].as_string()) {
        cell.call("begin_migrate", addrs[1].as_string());
    }
    else {
        cell.call("begin_migrate", addrs[0].as_string());
    }
}

void BaseAccount::add_migrate_int_from_client() {
    cell.call("add_migrate_int_from_base");
}

void BaseAccount::property_test() {
    property_create();
    avatar_datas_print(get_prop("avatar_datas"));
    propertys_sync2client(false);

    property_update();
    avatar_datas_print(get_prop("avatar_datas"));
    propertys_sync2client(false);

    //property_delete();
    //avatar_datas_print(get_prop("avatar_datas"));
    //propertys_sync2client(false);
}

void BaseAccount::property_create() {
    AvatarData avatar_data;
    avatar_data_create(avatar_data);
    get_prop("avatar_datas")->insert(to_string(avatar_data.avatar_id.as_int32()), avatar_data);

    avatar_data.avatar_id = 102;
    avatar_data.avatar_name = "qianqian";
    get_prop("avatar_datas")->insert(to_string(avatar_data.avatar_id.as_int32()), avatar_data);

    avatar_data.avatar_id = 103;
    avatar_data.avatar_name = "duoduo";
    get_prop("avatar_datas")->insert(to_string(avatar_data.avatar_id.as_int32()), avatar_data);
}

void BaseAccount::property_delete() {

    get_prop("avatar_datas")->get("101")->get("avatar_title_ids")->pop_back();
    get_prop("avatar_datas")->get("101")->get("avatar_title_ids")->pop_back();
    get_prop("avatar_datas")->get("101")->get("avatar_title_ids")->pop_back();

    get_prop("avatar_datas")->get("102")->get("avatar_fashion_shows")->erase("fashion_shows_1");
    get_prop("avatar_datas")->get("102")->get("avatar_fashion_shows")->erase("fashion_shows_2");
    get_prop("avatar_datas")->get("102")->get("avatar_fashion_shows")->erase("fashion_shows_3");

    get_prop("avatar_datas")->get("101")->get("avatar_horses")->erase("horse_3");

    get_prop("avatar_datas")->erase("103");
}

void BaseAccount::property_update() {

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

void BaseAccount::account_timer_prop_sync() {
#ifdef __PROP_SYNC_TEST__
    property_sync_test();
#endif
}

void BaseAccount::property_sync_test() {

    Encoder encoder;
    serialize_client(encoder, true);
    encoder.write_end();

    if (encoder.anything()) {
        client.call("prop_sync_compare", GBin(encoder.get_buf(), encoder.get_offset()));
    }
    
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

void BaseAccount::property_sync_test_create() {

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

void BaseAccount::property_sync_test_clear() {

    get_prop("complex_test")->get("avatar_title_ids")->clear();
    get_prop("complex_test")->get("avatar_equips")->clear();
    get_prop("complex_test")->get("avatar_fashion_shows")->clear();
    get_prop("complex_test")->get("avatar_horses")->clear();
    get_prop("array_test")->clear();
    get_prop("map_test")->clear();
    get_prop("complex_array_test")->clear();
    get_prop("complex_map_test")->clear();
}

void BaseAccount::property_sync_test_add() {

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

void BaseAccount::property_sync_test_del() {

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

void BaseAccount::property_sync_test_update() {

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

GENERATE_ENTITY_OUT(CellAccount)

void CellAccount::regist_components() {
}

void CellAccount::rpc_method_define() {
    RPC_METHOD(RpcType::SERVER_ONLY, msg_from_base);
    RPC_METHOD(RpcType::EXPOSED, msg_from_client);

    RPC_METHOD(RpcType::SERVER_ONLY, add_migrate_int_from_base);
    RPC_METHOD(RpcType::EXPOSED, add_migrate_int_from_client);
}

void CellAccount::property_define() {
    account_property_define<CellAccount>();
}

void CellAccount::timer_cb_store() {
    STORE_TIMER_CB_FOR_MIGRATE(account_timer_test);
}

void CellAccount::msg_from_base(const GString& msg) {
    INFO_LOG("[cell] msg.%s from base\n", msg.c_str());
}

void CellAccount::msg_from_client(const GString& msg) {
    INFO_LOG("[cell] msg.%s from client\n", msg.c_str());
}

void CellAccount::add_migrate_int_from_base() {
    auto old_migrate_int = get_prop("migrate_int")->as_int32();
    get_prop("migrate_int")->update(old_migrate_int + 1);

    INFO_LOG("add_migrate_int_from_base %d\n", get_prop("migrate_int")->as_int32());
}

void CellAccount::add_migrate_int_from_client() {
    auto old_migrate_int = get_prop("migrate_int")->as_int32();
    get_prop("migrate_int")->update(old_migrate_int + 1);

    INFO_LOG("add_migrate_int_from_client %d\n", get_prop("migrate_int")->as_int32());
}

void CellAccount::on_ready() {

    INFO_LOG("CellAccount on_ready\n");

    base.call("msg_from_cell", "hello, i am cell");
    client.call("msg_from_cell", "hello, i am cell");

    get_prop("test_timer")->update(REGIST_TIMER(0, 0, false, account_timer_test, "arg1"));
}

void CellAccount::account_timer_test(const GString& arg1) {
    INFO_LOG("[Cell] @@@@@@ this is timer test arg1.%s next_timer_id.%d\n", arg1.c_str(), get_prop("test_timer")->as_int32());

    get_prop("test_timer")->update(REGIST_TIMER(3, 0, false, account_timer_test, "arg1"));
}
