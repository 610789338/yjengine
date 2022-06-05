#pragma once

#include <assert.h>
#include <stdint.h>

#define ASSERT(expr, ...) assert((expr))

#define IPPORT_STRING(ip, port) ((ip) + ":" + std::to_string(port))

extern void byte_print(char* buf, uint16_t length);
