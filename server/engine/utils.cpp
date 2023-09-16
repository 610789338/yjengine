#include <stdio.h>
#include <string>
#include <iostream>
#include <signal.h>

#ifdef GPERFTOOLS
#include <gperftools/heap-profiler.h>
#endif

#include "boost/uuid/uuid.hpp"
#include "boost/uuid/uuid_generators.hpp"
#include "boost/uuid/uuid_io.hpp"
#include "boost/date_time/posix_time/posix_time.hpp"
#include "boost/thread.hpp"

#include "utils.h"
#include "ini.h"
#include "boost_asio.h"

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

uint32_t bkdr_hash(const char* str) {
    register unsigned int hash = 0;
    while (unsigned int ch = (unsigned int)*str++) {
        hash = hash * 131 + ch;
    }

    return hash;
}

void split(string str, const string& pattern, vector<string>& result) {
    result.clear();

    string::size_type pos;
    str += pattern; // TODO
    int32_t size = (int32_t)str.size();

    for (int32_t i = 0; i < size; i++) {
        pos = str.find(pattern, i);
        if (pos < size) {
            string s = str.substr(i, pos - i);
            result.push_back(s);
            i = int32_t(pos + pattern.size() - 1);
        }
    }
}

static boost::thread* p_thread = nullptr;
static bool g_assist_thread_exit = false;
void assist_thread() {
    while (true) {
        // TODO - open heart beat check
        // heart_beat_tick();
        log_queue_tick();

        if (g_assist_thread_exit && log_queue_empty()) {
            break;
        }
        boost::this_thread::sleep(boost::posix_time::milliseconds(1));
    }
}

void assist_thread_start() {
    p_thread = new boost::thread(assist_thread);
    // boost::thread t(assist_thread);
}

void assist_thread_exit() {
    if (p_thread != nullptr) {
        g_assist_thread_exit = true;
        p_thread->join();
        delete p_thread;
        p_thread = nullptr;
    }
}

static bool g_stop_main_tick = false;
void sig_term(int signo) {
#ifdef GPERFTOOLS
    INFO_LOG("sig_kill\n");
    HeapProfilerDump("youjun");
    HeapProfilerStop();
#endif

    g_stop_main_tick = true;
}

void sig_int(int signo) {
#ifdef GPERFTOOLS
    INFO_LOG("sig_int\n");
    HeapProfilerDump("youjun");
    HeapProfilerStop();
#endif

    g_stop_main_tick = true;
}

extern void engine_tick();
extern void engine_exit();
void main_tick(const int64_t ms_pertick = 100) {

    signal(SIGTERM, sig_term);
    signal(SIGINT, sig_int);

    auto _id = boost::this_thread::get_id();
    GString thread_id = boost::lexical_cast<GString>(_id);

    union {
        int i;
        char c;
    }un;
    un.i = 1;

    INFO_LOG("main tick start - main thread.0x%s, %s endian\n", thread_id.c_str(), un.c == 1 ? "little" : "big");

#ifdef GPERFTOOLS
    static string proc_name = ini_get_string("Common", "name", "unknown");
    HeapProfilerStart(proc_name.c_str());
#endif

    auto const tick_origin = nowms_timestamp(false);
    int32_t loop_num = 0;

    // main tick
    while (!g_stop_main_tick) {
        auto const tick_begin = nowms_timestamp(false);
        //if (loop_num % 100 == 0) {
        //    DEBUG_LOG("loop.%d - cost %dms - should be %dms\n", loop_num, tick_begin - tick_origin, loop_num * 100);
        //}
        engine_tick();
        auto const tick_end = nowms_timestamp(false);

        if (tick_end - tick_begin < ms_pertick) {
            // sleep大概有1/1000的误差，用error_ms来矫正
            int64_t error_ms = (tick_begin - tick_origin) - (loop_num * ms_pertick);
            boost::this_thread::sleep(boost::posix_time::milliseconds(ms_pertick - (tick_end - tick_begin) - error_ms));
        }
        else {
            WARN_LOG("long tick %dms\n", tick_end - tick_begin);
        }

        ++loop_num;
    }
    
    INFO_LOG("main tick end\n");

    engine_exit();
}

string get_listen_addr() {
    return server->get_listen_addr();
}
