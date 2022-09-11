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

extern string get_proc_name();

extern void update_time_now_cache();
extern string now_format(bool use_cache = false);
extern int64_t now_timestamp(bool use_cache = false);
extern int64_t nowms_timestamp(bool use_cache = false);
extern int64_t nowmicro_timestamp(bool use_cache = false);
