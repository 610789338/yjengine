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
