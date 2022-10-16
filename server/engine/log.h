#pragma once

#include <stdio.h>
#include <string>
#include <unordered_map>

 #include "utils.h"

// #include "gvalue.h"  // can not include this because circular include

using namespace std;

static unordered_map<string, int8_t> log_levels{
    {"ERROR", 1},
    {"WARN",  2},
    {"INFO",  3},
    {"DEBUG", 4}
};

extern int32_t ini_get_int(const char* node_name, const char* child_name, int32_t _defaul);
extern string ini_get_string(const char* node_name, const char* child_name, string _default);

// window debugģʽ�´�ӡһ����־��ƽ����ʱ��800 micro second = 0.8ms
// ����ini_get_string��ʱ0.4ms���ĳ�static
// һ��now_format��ʱ0.2ms���ĳ�ÿ֡��ʼʱcacheһ�� - TODO
// һ��printf��ʱ0.2ms���ռ����������Ų�����߳��� - TODO
// ���մ�ſ����Ż���0.02ms/��
template<class... T>
void LOG_s(const char* level, T... args) {

    static string ini_log_level = ini_get_string("Common", "log_level", "unknown");
    if (ini_log_level == "unknown") {
        ini_log_level = ini_get_string("Common", "log_level", "unknown");
    }

    if (ini_log_level != "unknown" && log_levels.at(level) > log_levels.at(ini_log_level)) {
        return;
    }

    static string proc_name = ini_get_string("Common", "name", "unknown");
    if (proc_name == "unknown") {
        proc_name = ini_get_string("Common", "name", "unknown");
    }

    static char body[1024] = {0};
    snprintf(body, sizeof(body), args...);

    // TODO - child thread print
    printf("%s - [%s ] - [%s] - %s", now_format().c_str(), level, proc_name.c_str(), body);
}

template<class T>
void LOG_s(const char* level, T body) {

    static string ini_log_level;
    ini_log_level = ini_get_string("Common", "log_level", "DEBUG");
    if (log_levels.at(level) > log_levels.at(ini_log_level)) {
        return;
    }

    static string proc_name;
    proc_name = ini_get_string("Common", "name", "unknown");

    printf("%s - [%s ] - [%s] - %s", now_format().c_str(), level, proc_name.c_str(), body);
}

template<class... T>
void LOG_l(const char* level, T... args) {

    static string ini_log_level = ini_get_string("Common", "log_level", "unknown");
    if (ini_log_level == "unknown") {
        ini_log_level = ini_get_string("Common", "log_level", "unknown");
    }

    if (ini_log_level != "unknown" && log_levels.at(level) > log_levels.at(ini_log_level)) {
        return;
    }

    static string proc_name = ini_get_string("Common", "name", "unknown");
    if (proc_name == "unknown") {
        proc_name = ini_get_string("Common", "name", "unknown");
    }

    static char body[1024] = {0};
    snprintf(body, sizeof(body), args...);

    // TODO - child thread print
    printf("%s - [%s] - [%s] - %s", now_format().c_str(), level, proc_name.c_str(), body);
}

template<class T>
void LOG_l(const char* level, T body) {

    static string ini_log_level;
    ini_log_level = ini_get_string("Common", "log_level", "DEBUG");
    if (log_levels.at(level) > log_levels.at(ini_log_level)) {
        return;
    }

    static string proc_name;
    proc_name = ini_get_string("Common", "name", "unknown");

    printf("%s - [%s] - [%s] - %s", now_format().c_str(), level, proc_name.c_str(), body);
}

#define ERROR_LOG(...) {LOG_l("ERROR", __VA_ARGS__);}
#define WARN_LOG(...)  {LOG_s("WARN",  __VA_ARGS__);}
#define INFO_LOG(...)  {LOG_s("INFO",  __VA_ARGS__);}
#define DEBUG_LOG(...) {LOG_l("DEBUG", __VA_ARGS__);}
