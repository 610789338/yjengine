#pragma once

#include <assert.h>
#include <stdint.h>

#include "log.h"

#define ASSERT(expr) assert((expr))
#define ASSERT_LOG(expr, ...) { if (!(expr)) { ERROR_LOG(__VA_ARGS__); assert(false);} }

#define IPPORT_STRING(ip, port) ((ip) + ":" + std::to_string(port))

extern void byte_print(char* buf, uint16_t length);
