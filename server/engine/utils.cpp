#include <stdio.h>
#include <string>
#include <iostream>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "utils.h"
#include "ini.h"

using namespace std;

void byte_print(char* buf, uint16_t length) {
    for (int i = 0; i < length; ++i) {
        printf("%02x ", buf[i]);
    }
    printf("\n");
}

boost::uuids::random_generator generator;

string gen_uuid() {
    boost::uuids::uuid random_u = generator();
    return boost::uuids::to_string(random_u);
}

string get_proc_name() {
    return g_ini.get_string("Common", "name");
}

string now_format() {

    pt::ptime current_date_microseconds = pt::microsec_clock::local_time();

    string t = boost::posix_time::to_iso_string(current_date_microseconds);
    t.replace(4, 0, string("-"));
    t.replace(7, 0, string("-"));
    size_t pos = t.find("T");
    t.replace(pos, 1, string(" "));
    t.replace(pos + 3, 0, string(":"));
    t.replace(pos + 6, 0, string(":"));

    return t;
}
