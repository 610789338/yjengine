#pragma once

#include <stdio.h>
#include <string>
#include <unordered_map>

#include "utils.h"

// #include "gvalue.h"  // can not include this because circular include

using namespace std;

extern const char* Error;
extern const char* Warn;
extern const char* Info;
extern const char* Debug;

static unordered_map<string, int8_t> log_levels{
    {Error, 1},
    {Warn,  2},
    {Info,  3},
    {Debug, 4}
};

struct LogElement {
    LogElement(const char* _level, char* _body) : level(_level), body(_body) {}
    const char* level;
    string body;
};

extern void push2log_queue(LogElement& log_ele);
extern void log_queue_tick();

// windows debug模式下打印一条日志的平均耗时是800 micro second = 0.8ms
// 两次ini_get_string耗时0.4ms，改成static
// 一次now_format耗时0.2ms，改成每帧开始时cache一次 - TODO
// 一次printf耗时0.2ms，终极解决方案是挪到子线程中 - TODO
// 最终大概可以优化到0.02ms/条
template<class... T>
void LOG(const char* level, T... args) {

    static string ini_log_level = ini_get_string("Common", "log_level", "unknown");
    if (ini_log_level == "unknown") {
        ini_log_level = ini_get_string("Common", "log_level", "unknown");
    }

    if (ini_log_level != "unknown" && log_levels.at(level) > log_levels.at(ini_log_level)) {
        return;
    }

    static char body[1024] = {0};
    snprintf(body, sizeof(body), args...);

    LogElement log_ele(level, body);
    push2log_queue(log_ele);
}

void LOG(const char* level, char* body);

#define ERROR_LOG(...) {LOG(Error, __VA_ARGS__);}
#define WARN_LOG(...)  {LOG(Warn,  __VA_ARGS__);}
#define INFO_LOG(...)  {LOG(Info,  __VA_ARGS__);}
#define DEBUG_LOG(...) {LOG(Debug, __VA_ARGS__);}
