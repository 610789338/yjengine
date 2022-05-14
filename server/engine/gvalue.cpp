#include "gvalue.h"


GValue::GValue(const GValue& rs) { 
    if (m_t == STRING_T) {
        delete m_v.s;
        m_v.s = nullptr;
    }

    switch (rs.m_t)
    {
    case STRING_T:
        m_v.s = new string(std::move(rs.as_string()));
        break;
    default:
        memmove(this, &rs, sizeof(GValue));
        break;
    }

    m_t = rs.m_t;
}

GValue& GValue::operator=(const GValue& rs) {
    if (m_t == STRING_T) {
        delete m_v.s;
        m_v.s = nullptr;
    }

    switch (rs.m_t)
    {
    case STRING_T:
        m_v.s = new string(std::move(rs.as_string()));
        break;
    default:
        memmove(this, &rs, sizeof(GValue));
        break;
    }

    m_t = rs.m_t;

    return *this;
}

void byte_print(char* buf, uint16_t length) {
    for (int i = 0; i < length; ++i) {
        printf("%02x ", buf[i]);
    }
    printf("\n");
}
