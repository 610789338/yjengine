#pragma once

#include <stdint.h>
#include <string>

using namespace std;

enum GType {INT8_T, INT16_T, INT32_T, INT64_T, UINT8_T, UINT16_T, UINT32_T, UINT64_T, FLOAT_T, DOUBLE_T, STRING_T};

class GValue {

    union GV {
        GV() {}
        ~GV() {}

        int8_t  i8;
        int16_t i16;
        int32_t i32;
        int64_t i64;
        uint8_t  ui8;
        uint16_t ui16;
        uint32_t ui32;
        uint64_t ui64;
        float f;
        double d;
        string s;
    };

public:
    GValue() = delete;
    GValue(int8_t v) : m_t(GType::INT8_T) { m_v.i8 = v;}
    GValue(int16_t v) : m_t(GType::INT16_T) { m_v.i16 = v; }
    GValue(int32_t v) : m_t(GType::INT32_T) { m_v.i32 = v; }
    GValue(int64_t v) : m_t(GType::INT64_T) { m_v.i64 = v; }
    GValue(uint8_t v) : m_t(GType::UINT8_T) { m_v.ui8 = v; }
    GValue(uint16_t v) : m_t(GType::UINT16_T) { m_v.ui16 = v; }
    GValue(uint32_t v) : m_t(GType::UINT32_T) { m_v.ui32 = v; }
    GValue(uint64_t v) : m_t(GType::UINT64_T) { m_v.ui64 = v; }
    GValue(float v) : m_t(GType::FLOAT_T) { m_v.f = v; }
    GValue(double v) : m_t(GType::DOUBLE_T) { m_v.d = v; }
    GValue(string v) : m_t(GType::STRING_T) { m_v.s = v; }
    GValue(const GValue& rs) { memmove(this, &rs, sizeof(GValue)); }
    GValue& operator=(const GValue& rs) { memmove(this, &rs, sizeof(GValue)); return *this; }
    ~GValue() {}

    int16_t as_int16() { return m_v.i16; }

private:
    GType m_t;
    GV m_v;
};

