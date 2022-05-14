#pragma once

#include <stdint.h>
#include <string>
#include <vector>
#include <map>

#include "utils.h"

using namespace std;

class GValue;
typedef string GString;
typedef vector<GValue> GArray;
typedef map<GString, GValue> GDict;

enum GType : uint8_t {INT8_T, INT16_T, INT32_T, INT64_T, UINT8_T, UINT16_T, UINT32_T, UINT64_T, FLOAT_T, DOUBLE_T, STRING_T, ARRAY_T, DICT_T};

class GValue {

    union GV {
        GV() {}
        ~GV() {}

        int8_t  i8 = 0;
        int16_t i16;
        int32_t i32;
        int64_t i64;
        uint8_t  ui8;
        uint16_t ui16;
        uint32_t ui32;
        uint64_t ui64;
        float f;
        double d;
        GString* s;
        //GString s = "";  // 必须显示初始化，否则是invalid string
        GArray* array;
        GDict* dict;
    };

public:
    GValue() = delete;
    ~GValue() { release(); };
    void release();
    
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
    GValue(const GString& v) : m_t(GType::STRING_T) { m_v.s = new GString(v); }
    GValue(const GArray& v) : m_t(GType::ARRAY_T) { m_v.array = new GArray(v); }
    GValue(const GDict& v) : m_t(GType::DICT_T) { m_v.dict = new GDict(v); }
    
    GValue(const GValue& rs);
    GValue& operator=(const GValue& rs);

    GValue(GValue&& rs);
    GValue& operator=(GValue&& rs);

    int8_t as_int8() const { ASSERT(m_t == GType::INT8_T); return m_v.i8; }
    int16_t as_int16() const { ASSERT(m_t == GType::INT16_T); return m_v.i16; }
    int32_t as_int32() const { ASSERT(m_t == GType::INT32_T); return m_v.i32; }
    int64_t as_int64() const { ASSERT(m_t == GType::INT64_T); return m_v.i64; }
    uint8_t as_uint8() const { ASSERT(m_t == GType::UINT8_T); return m_v.ui8; }
    uint16_t as_uint16() const { ASSERT(m_t == GType::UINT16_T); return m_v.ui16; }
    uint32_t as_uint32() const { ASSERT(m_t == GType::UINT32_T); return m_v.ui32; }
    uint64_t as_uint64() const { ASSERT(m_t == GType::UINT64_T); return m_v.ui64; }
    float as_float() const { ASSERT(m_t == GType::FLOAT_T); return m_v.f; }
    double as_double() const { ASSERT(m_t == GType::DOUBLE_T); return m_v.d; }
    GString& as_string() const { ASSERT(m_t == GType::STRING_T); return *(m_v.s); }
    GArray& as_array() const { ASSERT(m_t == GType::ARRAY_T); return *(m_v.array); }
    GDict& as_dict() const { ASSERT(m_t == GType::DICT_T); return *(m_v.dict); }

    GType type() const { return m_t; }
private:
    GType m_t;
    GV m_v;
};
