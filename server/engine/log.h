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

// windows debugģʽ�´�ӡһ����־��ƽ����ʱ��800 micro second = 0.8ms
// ����ini_get_string��ʱ0.4ms���ĳ�static
// һ��now_format��ʱ0.2ms���ĳ�ÿ֡��ʼʱcacheһ�� - TODO
// һ��printf��ʱ0.2ms���ռ����������Ų�����߳��� - TODO
// ���մ�ſ����Ż���0.02ms/��
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
