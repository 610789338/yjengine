#include "gvalue.h"


void GValue::release() {
    if (GType::STRING_T == m_t && m_v.s != nullptr) {
        delete m_v.s;
        m_v.s = nullptr;
    }
    else if (GType::ARRAY_T == m_t && m_v.array != nullptr) {
        delete m_v.array;
        m_v.array = nullptr;
    }
    else if (GType::DICT_T == m_t && m_v.dict != nullptr) {
        delete m_v.dict;
        m_v.dict = nullptr;
    }
}

GValue::GValue(const GValue& rs) {
    release();

    switch (rs.m_t)
    {
    case STRING_T:
        m_v.s = new GString(std::move(rs.as_string()));
        break;
    case ARRAY_T:
        m_v.array = new GArray(std::move(rs.as_array()));
        break;
    case DICT_T:
        m_v.dict = new GDict(std::move(rs.as_dict()));
        break;
    default:
        memmove(this, &rs, sizeof(GValue));
        break;
    }

    m_t = rs.m_t;
}

GValue& GValue::operator=(const GValue& rs) {
    release();

    switch (rs.m_t)
    {
    case STRING_T:
        m_v.s = new GString(std::move(rs.as_string()));
        break;
    case ARRAY_T:
        m_v.array = new GArray(std::move(rs.as_array()));
        break;
    case DICT_T:
        m_v.dict = new GDict(std::move(rs.as_dict()));
        break;
    default:
        memmove(this, &rs, sizeof(GValue));
        break;
    }

    m_t = rs.m_t;

    return *this;
}

GValue::GValue(GValue&& rs) {

    switch (rs.m_t)
    {
    case STRING_T:
        m_v.s = rs.m_v.s;
        rs.m_v.s = nullptr;
        break;
    case ARRAY_T:
        m_v.array = rs.m_v.array;
        rs.m_v.array = nullptr;
        break;
    case DICT_T:
        m_v.dict = rs.m_v.dict;
        rs.m_v.dict = nullptr;
        break;
    default:
        memmove(this, &rs, sizeof(GValue));
        break;
    }

    m_t = rs.m_t;
}

GValue& GValue::operator=(GValue&& rs) {

    switch (rs.m_t)
    {
    case STRING_T:
        m_v.s = rs.m_v.s;
        rs.m_v.s = nullptr;
        break;
    case ARRAY_T:
        m_v.array = rs.m_v.array;
        rs.m_v.array = nullptr;
        break;
    case DICT_T:
        m_v.dict = rs.m_v.dict;
        rs.m_v.dict = nullptr;
        break;
    default:
        memmove(this, &rs, sizeof(GValue));
        break;
    }

    m_t = rs.m_t;

    return *this;
}