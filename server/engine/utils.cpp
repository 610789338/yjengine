#include <stdio.h>
#include <string>
#include <iostream>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "utils.h"
#include "gvalue.h"

void byte_print(char* buf, uint16_t length) {
    for (int i = 0; i < length; ++i) {
        printf("%02x ", buf[i]);
    }
    printf("\n");
}

boost::uuids::random_generator generator;

GString gen_uuid() {
    boost::uuids::uuid random_u = generator();

    return boost::uuids::to_string(random_u);
}
