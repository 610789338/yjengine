#pragma once

#include <stdint.h>

#include "gvalue.h"

using namespace std;

class Encoder {

#define OFFSET_CHECK(cur, add) ASSERT((cur) < (0xffff - add))

public:
    Encoder() = default;
    ~Encoder() = default;

    template<class T>
    void write(T v) {}

    template<class T> void write(bool v) { write_bool(v); }
    template<class T> void write(int8_t v) { write_int8(v); }
    template<class T> void write(int16_t v) { write_int16(v); }
    template<class T> void write(int32_t v) { write_int32(v); }
    template<class T> void write(int64_t v) { write_int64(v); }
    template<class T> void write(uint8_t v) { write_uint8(v); }
    template<class T> void write(uint16_t v) { write_uint16(v); }
    template<class T> void write(uint32_t v) { write_uint32(v); }
    template<class T> void write(uint64_t v) { write_uint64(v); }
    template<class T> void write(float v) { write_float(v); }
    template<class T> void write(double v) { write_double(v); }
    template<class T> void write(const GString& v) { write_string(v); }
    template<class T> void write(const char* v) { write_string(GString(v)); }
    template<class T> void write(const GArray& v) { write_array(v); }
    template<class T> void write(const GDict& v) { write_dict(v); }
    template<class T> void write(const GValue& v) { ASSERT_LOG(false, "can not use GValuetype\n"); }

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
    void write_end();

    char* get_buf() { return m_buf; }
    uint16_t get_offset() { return m_offset; }

private:
    void write_gvalue(const GValue& v);

    char m_buf[4 * 1024] = {0};
    uint16_t m_offset = 2;  // pkg len offset
};
