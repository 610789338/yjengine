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

    template<class T>
    void write(T v) { ASSERT(false); }

    template<> void write<bool>(bool v) { write_bool(v); }
    template<> void write<int8_t>(int8_t v) { write_int8(v); }
    template<> void write<int16_t>(int16_t v) { write_int16(v); }
    template<> void write<int32_t>(int32_t v) { write_int32(v); }
    template<> void write<int64_t>(int64_t v) { write_int64(v); }
    template<> void write<uint8_t>(uint8_t v) { write_uint8(v); }
    template<> void write<uint16_t>(uint16_t v) { write_uint16(v); }
    template<> void write<uint32_t>(uint32_t v) { write_uint32(v); }
    template<> void write<uint64_t>(uint64_t v) { write_uint64(v); }
    template<> void write<float>(float v) { write_float(v); }
    template<> void write<double>(double v) { write_double(v); }
    template<> void write<GString>(GString v) { write_string(v); }

    template<> void write<const bool&>(const bool& v) { write_bool(v); }
    template<> void write<const int8_t&>(const int8_t& v) { write_int8(v); }
    template<> void write<const int16_t&>(const int16_t& v) { write_int16(v); }
    template<> void write<const int32_t&>(const int32_t& v) { write_int32(v); }
    template<> void write<const int64_t&>(const int64_t& v) { write_int64(v); }
    template<> void write<const uint8_t&>(const uint8_t& v) { write_uint8(v); }
    template<> void write<const uint16_t&>(const uint16_t& v) { write_uint16(v); }
    template<> void write<const uint32_t&>(const uint32_t& v) { write_uint32(v); }
    template<> void write<const uint64_t&>(const uint64_t& v) { write_uint64(v); }
    template<> void write<const float&>(const float& v) { write_float(v); }
    template<> void write<const double&>(const double& v) { write_double(v); }

    template<> void write<const GString&>(const GString& v) { write_string(v); }
    template<> void write<const char*>(const char* v) { write_string(GString(v)); }
    template<> void write<const GArray&>(const GArray& v) { write_array(v); }
    template<> void write<const GDict&>(const GDict& v) { write_dict(v); }
    template<> void write<const GBin&>(const GBin& v) { write_bin(v); }
    template<> void write<const GValue&>(const GValue& v) { ASSERT_LOG(false, "can not use GValue type\n"); }

    template<class T> void write(const char* v) { write_string(GString(v)); }

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
