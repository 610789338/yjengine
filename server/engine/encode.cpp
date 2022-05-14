// 序列化模块
// 暂未处理大小端

#include "encode.h"
#include "log.h"

void Encoder::write_int8(int8_t v) {
    OFFSET_CHECK(m_offset, 1);
    *(int8_t*)(m_buf + m_offset) = v;
    m_offset += 1;
}

void Encoder::write_int16(int16_t v) {
    OFFSET_CHECK(m_offset, 2);
    *(int16_t*)(m_buf + m_offset) = v;
    m_offset += 2;
}

void Encoder::write_int32(int32_t v) {
    OFFSET_CHECK(m_offset, 4);
    *(int32_t*)(m_buf + m_offset) = v;
    m_offset += 4;
}

void Encoder::write_int64(int64_t v) {
    OFFSET_CHECK(m_offset, 8);
    *(int64_t*)(m_buf + m_offset) = v;
    m_offset += 8;
}

void Encoder::write_uint8(uint8_t v) {
    OFFSET_CHECK(m_offset, 1);
    *(uint8_t*)(m_buf + m_offset) = v;
    m_offset += 1;
}

void Encoder::write_uint16(uint16_t v) {
    OFFSET_CHECK(m_offset, 2);
    *(uint16_t*)(m_buf + m_offset) = v;
    m_offset += 2;
}

void Encoder::write_uint32(uint32_t v) {
    OFFSET_CHECK(m_offset, 4);
    *(uint32_t*)(m_buf + m_offset) = v;
    m_offset += 4;
}

void Encoder::write_uint64(uint64_t v) {
    OFFSET_CHECK(m_offset, 8);
    *(uint64_t*)(m_buf + m_offset) = v;
    m_offset += 8;
}

void Encoder::write_float(float v) {
    OFFSET_CHECK(m_offset, 4);
    *(float*)(m_buf + m_offset) = v;
    m_offset += 4;
}

void Encoder::write_double(double v) {
    OFFSET_CHECK(m_offset, 8);
    *(double*)(m_buf + m_offset) = v;
    m_offset += 8;
}

void Encoder::write_string(const GString& v) {
    OFFSET_CHECK(m_offset, v.length() + 1);

    memmove(m_buf + m_offset, v.c_str(), v.length());
    memset(m_buf + m_offset + v.length(), 0, 1);  // string end
    m_offset += uint16_t(v.length()) + 1;
}

void Encoder::write_array(const GArray& v) {
    ASSERT(v.size() < 0xffff);

    write_uint16(uint16_t(v.size()));

    for (auto iter = v.begin(); iter != v.end(); ++iter) {
        write_gvalue(*iter);
    }
}

void Encoder::write_dict(const GDict& v) {
    ASSERT(v.size() < 0xffff);

    write_uint16(uint16_t(v.size()));

    for (auto iter = v.begin(); iter != v.end(); ++iter) {
        write_string(iter->first);
        write_gvalue(iter->second);
    }
}

void Encoder::write_gvalue(const GValue& v) {
    write_uint8(v.type());

    switch (v.type())
    {
    case GType::INT8_T:
        write_int8(v.as_int8());
        break;
    case GType::INT16_T:
        write_int16(v.as_int16());
        break;
    case GType::INT32_T:
        write_int32(v.as_int32());
        break;
    case GType::INT64_T:
        write_int64(v.as_int64());
        break;
    case GType::UINT8_T:
        write_uint8(v.as_uint8());
        break;
    case GType::UINT16_T:
        write_uint16(v.as_uint16());
        break;
    case GType::UINT32_T:
        write_uint32(v.as_uint32());
        break;
    case GType::UINT64_T:
        write_uint64(v.as_uint64());
        break;
    case GType::FLOAT_T:
        write_float(v.as_float());
        break;
    case GType::DOUBLE_T:
        write_double(v.as_double());
        break;
    case GType::STRING_T:
        write_string(v.as_string());
        break;
    case GType::ARRAY_T:
        write_array(v.as_array());
        break;
    case GType::DICT_T:
        write_dict(v.as_dict());
        break;
    default:
        ERROR_LOG("error type.%d\n", v.type());
        ASSERT(false);
        break;
    }
}

void Encoder::write_end() {
    *(uint16_t*)m_buf = m_offset - 2; // pkg len = total offset - 2
}
