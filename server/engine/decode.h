#pragma once

#include <stdint.h>
#include "gvalue.h"

using namespace std;

class Decoder {
public:
    Decoder() = delete;
    Decoder(const char* buf, uint16_t max_offset);
    ~Decoder();

    template<class T>
    T read() { return T(); }

    template<> bool read<bool>() { return read_bool(); }
    template<> int8_t read<int8_t>() { return read_int8(); }
    template<> int16_t read<int16_t>() { return read_int16(); }
    template<> int32_t read<int32_t>() { return read_int32(); }
    template<> int64_t read<int64_t>() { return read_int64(); }
    template<> uint8_t read<uint8_t>() { return read_uint8(); }
    template<> uint16_t read<uint16_t>() { return read_uint16(); }
    template<> uint32_t read<uint32_t>() { return read_uint32(); }
    template<> uint64_t read<uint64_t>() { return read_uint64(); }

    template<> float read<float>() { return read_float(); }
    template<> double read<double>() { return read_double(); }

    template<> GString read<GString>() { return read_string(); }
    template<> GArray read<GArray>() { return read_array(); }
    template<> GDict read<GDict>() { return read_dict(); }
    template<> GBin read<GBin>() { return read_bin(); }

    bool    read_bool();
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
    GBin read_bin();
    GValue read_gvalue();

    uint16_t get_max_offset() { return m_max_offset; }
    uint16_t get_offset() { return m_offset; }
    const char* get_buf() const { return m_buf; }
    bool is_finish();

private:
    const char* m_buf;
    uint16_t m_max_offset;
    uint16_t m_offset;
    bool m_need_release_buf;
};

