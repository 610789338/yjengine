#pragma once

#include <assert.h>
#include <stdint.h>
#include <string>
#include <functional>

// #include "gvalue.h"  // can not include this because circular include

using namespace std;


#define ASSERT(expr) assert((expr))
#define ASSERT_LOG(expr, ...) { if (!(expr)) { ERROR_LOG(__VA_ARGS__); assert(false);} }

#define IPPORT_STRING(ip, port) ((ip) + ":" + std::to_string(port))

class ConstructGuarantee {
public:
    ConstructGuarantee() = delete;
    ConstructGuarantee(function<void()> cb) { cb(); }
    ~ConstructGuarantee() {}
};

extern void byte_print(const char* buf, uint16_t length);
extern string gen_uuid();


extern string now_format(bool use_cache = true);
extern int64_t now_timestamp(bool use_cache = true);
extern int64_t nowms_timestamp(bool use_cache = true);
extern int64_t nowmicro_timestamp(bool use_cache = true);
extern uint32_t bkdr_hash(const char* str);
extern string get_listen_addr();

template<class ...Args>
char* str_format(const char* format, Args... args) {
    static char sfmt[1024] = {0};
    snprintf(sfmt, 1024, format, args...);
    return sfmt;
}