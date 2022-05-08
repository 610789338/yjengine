#include "decode.h"
#include "log.h"

Decoder::Decoder(const char* buf, uint16_t max_offset) {
    m_buf = buf;
    m_max_offset = max_offset;
    m_offset = 0;
    m_need_release_buf = false;

    if (nullptr == m_buf) {
        m_buf = new char[max_offset];
        m_need_release_buf = true;
    }
}

Decoder::~Decoder() {
    if (true == m_need_release_buf && m_buf != nullptr) {
        delete[] m_buf;
    }

    m_buf = nullptr;
}

int8_t Decoder::read_int8() {
    int8_t v = int8_t(m_buf[m_offset]);
    m_offset += 1;
    return v;
}

int16_t Decoder::read_int16() {
    int16_t v = (int16_t(m_buf[m_offset]) << 8)
        | int16_t(m_buf[m_offset + 1] << 0);
    m_offset += 2;
    return v;
}

int32_t Decoder::read_int32() {
    int32_t v = (int32_t(m_buf[m_offset]) << 24)
        | int32_t(m_buf[m_offset + 1]) << 16
        | int32_t(m_buf[m_offset + 2]) << 8
        | int32_t(m_buf[m_offset + 3]) << 0;
    m_offset += 4;
    return v;
}

int64_t Decoder::read_int64() {
    int64_t v = (int64_t(m_buf[m_offset]) << 56)
        | int64_t(m_buf[m_offset + 1]) << 48
        | int64_t(m_buf[m_offset + 2]) << 40
        | int64_t(m_buf[m_offset + 3]) << 32
        | int64_t(m_buf[m_offset + 4]) << 24
        | int64_t(m_buf[m_offset + 5]) << 16
        | int64_t(m_buf[m_offset + 6]) << 8
        | int64_t(m_buf[m_offset + 7]) << 0;
    m_offset += 8;
    return v;
}

uint8_t Decoder::read_uint8() {
    uint8_t v = uint8_t(m_buf[m_offset]);
    m_offset += 1;
    return v;
}

uint16_t Decoder::read_uint16() {
    uint16_t v = (uint16_t(m_buf[m_offset]) << 8) 
        | uint16_t(m_buf[m_offset + 1] << 0);
    m_offset += 2;
    return v;
}

uint32_t Decoder::read_uint32() {
    uint32_t v = (uint32_t(m_buf[m_offset]) << 24)
        | uint32_t(m_buf[m_offset + 1]) << 16
        | uint32_t(m_buf[m_offset + 2]) << 8
        | uint32_t(m_buf[m_offset + 3]) << 0;
    m_offset += 4;
    return v;
}

uint64_t Decoder::read_uint64() {
    uint64_t v = (uint64_t(m_buf[m_offset]) << 56)
        | uint64_t(m_buf[m_offset + 1]) << 48
        | uint64_t(m_buf[m_offset + 2]) << 40
        | uint64_t(m_buf[m_offset + 3]) << 32
        | uint64_t(m_buf[m_offset + 4]) << 24
        | uint64_t(m_buf[m_offset + 5]) << 16
        | uint64_t(m_buf[m_offset + 6]) << 8
        | uint64_t(m_buf[m_offset + 7]) << 0;
    m_offset += 8;
    return v;
}

float Decoder::read_float() {
    float v;
    char* p = (char*)&v;
    *(p + 0) = m_buf[m_offset + 0];
    *(p + 1) = m_buf[m_offset + 1];
    *(p + 2) = m_buf[m_offset + 2];
    *(p + 3) = m_buf[m_offset + 3];

    m_offset += 4;
    return v;
}

double Decoder::read_double() {
    double v;
    char* p = (char*)&v;
    *(p + 0) = m_buf[m_offset + 0];
    *(p + 1) = m_buf[m_offset + 1];
    *(p + 2) = m_buf[m_offset + 2];
    *(p + 3) = m_buf[m_offset + 3];
    *(p + 4) = m_buf[m_offset + 4];
    *(p + 5) = m_buf[m_offset + 5];
    *(p + 6) = m_buf[m_offset + 6];
    *(p + 7) = m_buf[m_offset + 7];

    m_offset += 8;
    return v;
}

string Decoder::read_string() {
    int end = 0;
    for (; end < 16 * 1024; ++end) {
        if (m_buf[end] == 0) break;
    }

    string v(m_buf + m_offset, m_buf + m_offset  + end);
    m_offset += (end + 1);
    return v;
}

bool Decoder::is_finish() {
    if (m_offset == m_max_offset) {
        return true;
    }
    else if (m_offset > m_max_offset) {
        ERROR_LOG("decode error m_offset.%d > max_offset.%d", m_offset, m_max_offset);
        return true;
    }
    
    return false;
}