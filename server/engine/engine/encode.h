#pragma once

#include <string>
#include <stdint.h>

using namespace std;

class Encoder {
public:
    Encoder();
    ~Encoder();

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

    void write_string(string v);

private:
    char m_buf[1024*1024];
    uint16_t m_offset;
};