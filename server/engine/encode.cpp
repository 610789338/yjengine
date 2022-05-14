#include "encode.h"

void Encoder::write_int8(int8_t v) {
    m_buf[m_offset + 0] = v >> 0 & 0xFF;
    m_offset += 1;
}

void Encoder::write_int16(int16_t v) {
    m_buf[m_offset + 0] = v >> 8 & 0xFF;
    m_buf[m_offset + 1] = v >> 0 & 0xFF;
    m_offset += 2;
}

void Encoder::write_int32(int32_t v) {
    m_buf[m_offset + 0] = v >> 24 & 0xFF;
    m_buf[m_offset + 1] = v >> 16 & 0xFF;
    m_buf[m_offset + 2] = v >> 8 & 0xFF;
    m_buf[m_offset + 3] = v >> 0 & 0xFF;
    m_offset += 4;
}

void Encoder::write_int64(int64_t v) {
    m_buf[m_offset + 0] = v >> 56 & 0xFF;
    m_buf[m_offset + 1] = v >> 48 & 0xFF;
    m_buf[m_offset + 2] = v >> 40 & 0xFF;
    m_buf[m_offset + 3] = v >> 32 & 0xFF;
    m_buf[m_offset + 4] = v >> 24 & 0xFF;
    m_buf[m_offset + 5] = v >> 16 & 0xFF;
    m_buf[m_offset + 6] = v >> 8 & 0xFF;
    m_buf[m_offset + 7] = v >> 0 & 0xFF;
    m_offset += 8;
}

void Encoder::write_uint8(uint8_t v) {
    m_buf[m_offset + 0] = v >> 0 & 0xFF;
    m_offset += 1;
}

void Encoder::write_uint16(uint16_t v) {
    m_buf[m_offset + 0] = v >> 8 & 0xFF;
    m_buf[m_offset + 1] = v >> 0 & 0xFF;
    m_offset += 2;
}

void Encoder::write_uint32(uint32_t v) {
    m_buf[m_offset + 0] = v >> 24 & 0xFF;
    m_buf[m_offset + 1] = v >> 16 & 0xFF;
    m_buf[m_offset + 2] = v >> 8 & 0xFF;
    m_buf[m_offset + 3] = v >> 0 & 0xFF;
    m_offset += 4;
}

void Encoder::write_uint64(uint64_t v) {
    m_buf[m_offset + 0] = v >> 56 & 0xFF;
    m_buf[m_offset + 1] = v >> 48 & 0xFF;
    m_buf[m_offset + 2] = v >> 40 & 0xFF;
    m_buf[m_offset + 3] = v >> 32 & 0xFF;
    m_buf[m_offset + 4] = v >> 24 & 0xFF;
    m_buf[m_offset + 5] = v >> 16 & 0xFF;
    m_buf[m_offset + 6] = v >> 8 & 0xFF;
    m_buf[m_offset + 7] = v >> 0 & 0xFF;
    m_offset += 8;
}

void Encoder::write_float(float v) {
    char* p = (char*)&v;
    m_buf[m_offset + 0] = *(p + 0);
    m_buf[m_offset + 1] = *(p + 1);
    m_buf[m_offset + 2] = *(p + 2);
    m_buf[m_offset + 3] = *(p + 3);
    m_offset += 4;
}

void Encoder::write_double(double v) {
    char* p = (char*)&v;
    m_buf[m_offset + 0] = *(p + 0);
    m_buf[m_offset + 1] = *(p + 1);
    m_buf[m_offset + 2] = *(p + 2);
    m_buf[m_offset + 3] = *(p + 3);
    m_buf[m_offset + 4] = *(p + 4);
    m_buf[m_offset + 5] = *(p + 5);
    m_buf[m_offset + 6] = *(p + 6);
    m_buf[m_offset + 7] = *(p + 7);
    m_offset += 8;
}

void Encoder::write_string(string v) {
    memmove(m_buf + m_offset, v.c_str(), v.length());
    memset(m_buf + m_offset + v.length(), 0, 1);  // string end
    m_offset += uint16_t(v.length()) + 1;
}

void Encoder::write_end() {
    m_buf[0] = m_offset >> 8 & 0xFF;
    m_buf[1] = m_offset >> 0 & 0xFF;
}
