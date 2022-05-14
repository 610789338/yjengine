#pragma once

#include <assert.h>
#include <stdint.h>

#define ASSERT(expr, ...) assert((expr))

extern void byte_print(char* buf, uint16_t length);
