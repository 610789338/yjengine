#pragma once

#include <stdio.h>

#define ERROR_LOG(...) {printf(__VA_ARGS__);}
#define WARN_LOG(...) {printf(__VA_ARGS__);}
#define INFO_LOG(...) {printf(__VA_ARGS__);}
#define DEBUG_LOG(...) {printf(__VA_ARGS__);}
