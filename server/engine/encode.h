#pragma once

#include <stdint.h>

#include "gvalue.h"

using namespace std;

class Encoder {

#define OFFSET_CHECK(cur, add) ASSERT((cur) < (0xffff - add))

public:
    Encoder() = default;
    ~Encoder() = default;

    void write_int8(int8_t v);
    void write_int16(int16_t v);
    void write_int32(int32_t v);
    void write_int64(int64_t v);

    void write_uint8(uint8_t v);
    void write_uint16(uint16_t v);
    void write_uint32(uint32_t v);
    void write_uint64(uint64_t v);

    void write_float(float v);
    void write_double(double v);

    void write_string(const GString& v);
    void write_array(const GArray& v);
    void write_dict(const GDict& v);
    void write_end();

    char* get_buf() { return m_buf; }
    uint16_t get_offset() { return m_offset; }

private:
    void write_gvalue(const GValue& v);

    char m_buf[4 * 1024] = {0};
    uint16_t m_offset = 2;  // pkg len offset
};
