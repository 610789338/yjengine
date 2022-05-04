#pragma once

#include <stdint.h>
#include <string>
#include <typeinfo>

using namespace std;

enum GType {INT8, INT16, INT32, INT64, UINT8, UINT16, UINT32, UINT64, FLOAT, DOUBLE, STRING};

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
    GValue(int8_t v) : m_t(GType::INT8) { m_v.i8 = v;}
    GValue(int16_t v) : m_t(GType::INT16) { m_v.i16 = v; }
    GValue(int32_t v) : m_t(GType::INT32) { m_v.i32 = v; }
    GValue(int64_t v) : m_t(GType::INT64) { m_v.i64 = v; }
    GValue(uint8_t v) : m_t(GType::UINT8) { m_v.ui8 = v; }
    GValue(uint16_t v) : m_t(GType::UINT16) { m_v.ui16 = v; }
    GValue(uint32_t v) : m_t(GType::UINT32) { m_v.ui32 = v; }
    GValue(uint64_t v) : m_t(GType::UINT64) { m_v.ui64 = v; }
    GValue(float v) : m_t(GType::FLOAT) { m_v.f = v; }
    GValue(double v) : m_t(GType::DOUBLE) { m_v.d = v; }
    GValue(string v) : m_t(GType::STRING) { m_v.s = v; }
    GValue(const GValue& rs) { memmove(this, &rs, sizeof(GValue)); }
    GValue& operator=(const GValue& rs) { memmove(this, &rs, sizeof(GValue)); return *this; }
    ~GValue() {}

    auto real();

private:
    GType m_t;
    GV m_v;
};

