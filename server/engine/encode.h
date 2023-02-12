#pragma once

#include <stdint.h>

#include "gvalue.h"
#include "log.h"

using namespace std;

class Encoder {

#define OFFSET_CHECK(cur, add) ASSERT((cur) < (0xffff - add))
#define PKG_LEN_OFFSET 2

public:
    Encoder() = default;
    ~Encoder() = default;

    void write(bool v) { write_bool(v); }
    void write(int8_t v) { write_int8(v); }
    void write(int16_t v) { write_int16(v); }
    void write(int32_t v) { write_int32(v); }
    void write(int64_t v) { write_int64(v); }
    void write(uint8_t v) { write_uint8(v); }
    void write(uint16_t v) { write_uint16(v); }
    void write(uint32_t v) { write_uint32(v); }
    void write(uint64_t v) { write_uint64(v); }
    void write(float v) { write_float(v); }
    void write(double v) { write_double(v); }
    void write(const GString& v) { write_string(v); }
    void write(const char* v) { write_string(GString(v)); }
    void write(const GArray& v) { write_array(v); }
    void write(const GDict& v) { write_dict(v); }
    void write(const GBin& v) { write_bin(v); }
    void write(const GValue& v) { ASSERT_LOG(false, "can not use GValue type\n"); }

    void write_bool(bool v);
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
    void write_bin(const GBin& v);
    void write_end();

    bool anything() { return m_offset > PKG_LEN_OFFSET; }

    const char* get_buf() const { return m_buf; }
    uint16_t get_offset() const { return m_offset; }

    void write_gvalue(const GValue& v);

private:
    char m_buf[4 * 1024] = {0};
    uint16_t m_offset = PKG_LEN_OFFSET;  // pkg len offset
};
