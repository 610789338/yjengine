#include <stdio.h>
#include "utils.h"

void byte_print(char* buf, uint16_t length) {
    for (int i = 0; i < length; ++i) {
        printf("%02x ", buf[i]);
    }
    printf("\n");
}