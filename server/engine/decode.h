#pragma once

#include <stdint.h>
#include "gvalue.h"

using namespace std;

class Decoder {
public:
    Decoder() = delete;
    Decoder(const char* buf, uint16_t max_offset);
    ~Decoder();

    int8_t  read_int8();
    int16_t read_int16();
    int32_t read_int32();
    int64_t read_int64();

    uint8_t read_uint8();
    uint16_t read_uint16();
    uint32_t read_uint32();
    uint64_t read_uint64();

    float read_float();
    double read_double();

    GString read_string();
    GArray read_array();
    GDict read_dict();

    uint16_t get_offset() { return m_offset; }
    bool is_finish();

private:
    GValue read_gvalue();

    const char* m_buf;
    uint16_t m_max_offset;
    uint16_t m_offset;
    bool m_need_release_buf;
};

