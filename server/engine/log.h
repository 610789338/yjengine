#pragma once

#include <stdio.h>

#include "boost/date_time/posix_time/posix_time.hpp"

// #include "gvalue.h"  // can not include this because circular include

using namespace std;
namespace pt = boost::posix_time;

extern string get_proc_name();
extern string now_format();

template<class... T>
void LOG(const char* level, T... args) {

    static string proc_name = get_proc_name();

    static char body[1024];
    snprintf(body, sizeof(body), args...);

    printf("%s - [%s] - [%s] - %s", now_format().c_str(), level, proc_name.c_str(), body);
}

template<class T>
void LOG(const char* level, T body) {
    static string proc_name = get_proc_name();
    printf("%s - [%s] - [%s] - %s", now_format().c_str(), level, proc_name.c_str(), body);
}

#define ERROR_LOG(...) {LOG("ERROR", __VA_ARGS__);}
#define WARN_LOG(...)  {LOG("WARN ", __VA_ARGS__);}
#define INFO_LOG(...)  {LOG("INFO ", __VA_ARGS__);}
#define DEBUG_LOG(...) {LOG("DEBUG", __VA_ARGS__);}
