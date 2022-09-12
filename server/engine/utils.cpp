#include <stdio.h>
#include <string>
#include <iostream>

#include "boost/uuid/uuid.hpp"
#include "boost/uuid/uuid_generators.hpp"
#include "boost/uuid/uuid_io.hpp"
#include "boost/date_time/posix_time/posix_time.hpp"

#include "utils.h"
#include "ini.h"
#include "entity_property_manager.h"

using namespace std;

void byte_print(const char* buf, uint16_t length) {
    for (int i = 0; i < length; ++i) {
        printf("%02x ", buf[i]);
    }
    printf("\n");
}

boost::uuids::random_generator generator;

string gen_uuid() {
    boost::uuids::uuid random_u = generator();
    return boost::uuids::to_string(random_u);
}

string get_proc_name() {
    return ini_get_string("Common", "name");
}

static const boost::posix_time::ptime timestamp0 = boost::posix_time::time_from_string("1970-01-01 08:00:00");
static boost::posix_time::ptime now_cache = boost::posix_time::microsec_clock::local_time();

void update_time_now_cache() {
    now_cache = boost::posix_time::microsec_clock::local_time();
}

boost::posix_time::ptime& get_time_now_cache() {
    static ConstructGuarantee cg( [] { now_cache = boost::posix_time::microsec_clock::local_time(); });
    return now_cache;
}

// window下debug模式调用一次平均耗时200 micro second = 0.2 ms
string now_format(bool use_cache) {

    string t = boost::posix_time::to_iso_string(get_time_now_cache());
    if (!use_cache) {
        t = boost::posix_time::to_iso_string(boost::posix_time::microsec_clock::local_time());
    }

    t.replace(4, 0, string("-"));
    t.replace(7, 0, string("-"));
    size_t pos = t.find("T");
    t.replace(pos, 1, string(" "));
    t.replace(pos + 3, 0, string(":"));
    t.replace(pos + 6, 0, string(":"));

    return t;
}

/*
 * linux下debug模式调用一次平均耗时0.8 micro second
 * windows下debug模式调用一次平均耗时5.8 micro second
 */
int64_t now_timestamp(bool use_cache) {
    
    boost::posix_time::ptime now = get_time_now_cache();
    if (!use_cache) {
        now = boost::posix_time::microsec_clock::local_time();
    }

    const boost::posix_time::time_duration time_duration{ now - timestamp0 };
    return (int64_t)time_duration.total_seconds();
}

int64_t nowms_timestamp(bool use_cache) {

    boost::posix_time::ptime now = get_time_now_cache();
    if (!use_cache) {
        now = boost::posix_time::microsec_clock::local_time();
    }

    const boost::posix_time::time_duration time_duration{ now - timestamp0 };
    return (int64_t)time_duration.total_microseconds()/1000;
}

int64_t nowmicro_timestamp(bool use_cache) {

    boost::posix_time::ptime now = get_time_now_cache();
    if (!use_cache) {
        now = boost::posix_time::microsec_clock::local_time();
    }

    const boost::posix_time::time_duration time_duration{ now - timestamp0 };
    return (int64_t)time_duration.total_microseconds();
}

void prop_read_for_test(EntityPropertyBase* avatar_datas) {

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
