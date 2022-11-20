#pragma once

#include <stdint.h>
#include "gvalue.h"

using namespace std;

class Decoder {
public:
    Decoder() = delete;
    Decoder(const char* buf, uint16_t max_offset);
    ~Decoder();

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

template<class T> T decoder_read(Decoder& decoder) { return T(); }
