#pragma once

#include <assert.h>
#include <stdint.h>
#include <string>

#include "log.h"

// #include "gvalue.h"  // can not include this because circular include

using namespace std;


#define ASSERT(expr) assert((expr))
#define ASSERT_LOG(expr, ...) { if (!(expr)) { ERROR_LOG(__VA_ARGS__); assert(false);} }

#define IPPORT_STRING(ip, port) ((ip) + ":" + std::to_string(port))

extern void byte_print(char* buf, uint16_t length);
extern string gen_uuid();

extern string get_proc_name();
extern string now_format();

extern int64_t now_timestamp();
extern int64_t nowms_timestamp();
