#pragma once

#include <stdint.h>

#include "gvalue.h"
#include "log.h"

using namespace std;

class Encoder {
public:
    Encoder() {
        m_buf = new char[m_capacity];
    }
    Encoder(const Encoder& other)
        : m_buf(new char[other.m_capacity])
        , m_offset(other.m_offset)
        , m_capacity(other.m_capacity)
    {
        memmove(m_buf, other.m_buf, other.m_offset);
    }
    Encoder(Encoder&& other)
        : m_buf(other.m_buf)
        , m_offset(other.m_offset)
        , m_capacity(other.m_capacity)
    {
        other.Reset();
    }

    Encoder& operator=(const Encoder& other) {
        delete[]m_buf;

        m_buf = new char[other.m_capacity];
        memmove(m_buf, other.m_buf, other.m_offset);

        m_offset = other.m_offset;
        m_capacity = other.m_capacity;

        return *this;
    }
    Encoder& operator =(Encoder&& other) {
        delete[]m_buf;

        m_buf = other.m_buf;
        m_offset = other.m_offset;
        m_capacity = other.m_capacity;

        other.Reset();

        return *this;
    }

    ~Encoder() {
        if (m_buf) {
            delete[]m_buf;
            Reset();
        }
    }

#define INIT_CAPACITY 64
#define PKG_LEN_OFFSET 2
#define MAX_OFFSET 4096

    void Reset() {
        m_buf = nullptr;
        m_offset = PKG_LEN_OFFSET;
        m_capacity = INIT_CAPACITY;
    }

private:
    char* m_buf = nullptr;
    uint16_t m_offset = PKG_LEN_OFFSET;  // pkg len offset
    uint16_t m_capacity = INIT_CAPACITY;

    inline void OFFSET_CHECK(uint32_t add) {
        ASSERT((m_offset + add) <= MAX_OFFSET);
        if (m_offset + add > m_capacity) {
            auto new_buf = new char[(m_offset + add) * 2];
            memmove(new_buf, m_buf, m_capacity);
            delete[] m_buf;
            m_buf = new_buf;
            m_capacity = (m_offset + add) * 2;
        };
    }

public:
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

    void move_to_bin(GBin& bin);

    void write_gvalue(const GValue& v);
};
