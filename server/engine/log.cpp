#include <queue>
#include <mutex>

#include "boost/thread.hpp"

#include "log.h"

using namespace std;

const char* Error = "ERROR";
const char* Warn  = "WARN";
const char* Info  = "INFO";
const char* Debug = "DEBUG";

static queue<LogElement> g_log_queue;
static boost::shared_mutex g_log_mutex;

void LOG(const char* level, char* body) {
    static string ini_log_level = ini_get_string("Common", "log_level", "unknown");
    if (ini_log_level == "unknown") {
        ini_log_level = ini_get_string("Common", "log_level", "unknown");
    }

    if (ini_log_level != "unknown" && log_levels.at(level) > log_levels.at(ini_log_level)) {
        return;
    }

    LogElement log_ele(level, body);
    push2log_queue(log_ele);
}

void push2log_queue(LogElement& log_ele) {
    unique_lock<boost::shared_mutex> lock(g_log_mutex);
    g_log_queue.push(log_ele);
}

void log_queue_tick() {
    unique_lock<boost::shared_mutex> lock(g_log_mutex);

    int8_t max_loop = 100;
    while (!g_log_queue.empty() && --max_loop > 0) {
        auto log_ele = g_log_queue.front();

        static string proc_name = ini_get_string("Common", "name", "unknown");
        if (proc_name == "unknown") {
            proc_name = ini_get_string("Common", "name", "unknown");
        }

        if (log_ele.level[0] == 'W' || log_ele.level[0] == 'I') {
            // Warn && Info
            printf("%s - [%s ] - [%s] - %s", now_format().c_str(), log_ele.level, proc_name.c_str(), log_ele.body.c_str());
        }
        else {
            // Error && Debug
            printf("%s - [%s] - [%s] - %s", now_format().c_str(), log_ele.level, proc_name.c_str(), log_ele.body.c_str());
        }

        g_log_queue.pop();
    }
}
