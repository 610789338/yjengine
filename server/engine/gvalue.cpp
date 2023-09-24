#include "gvalue.h"
#include "mailbox.h"


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
    else if (GType::BIN_T == m_t && m_v.bin != nullptr) {
        delete m_v.bin;
        m_v.bin = nullptr;
    }
    else if (GType::MAILBOX_T == m_t && m_v.mailbox != nullptr) {
        delete m_v.mailbox;
        m_v.mailbox = nullptr;
    }
}

GValue::GValue(const GValue& rs) {
    release();

    switch (rs.m_t)
    {
    case STRING_T:
        m_v.s = new GString(rs.as_string());
        break;
    case ARRAY_T:
        m_v.array = new GArray(rs.as_array());
        break;
    case DICT_T:
        m_v.dict = new GDict(rs.as_dict());
        break;
    case BIN_T:
        m_v.bin = new GBin(rs.as_bin());
        break;
    case MAILBOX_T:
        m_v.mailbox = new MailBox(rs.as_mailbox());
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
        m_v.s = new GString(rs.as_string());
        break;
    case ARRAY_T:
        m_v.array = new GArray(rs.as_array());
        break;
    case DICT_T:
        m_v.dict = new GDict(rs.as_dict());
        break;
    case BIN_T:
        m_v.bin = new GBin(rs.as_bin());
        break;
    case MAILBOX_T:
        m_v.mailbox = new MailBox(rs.as_mailbox());
        break;
    default:
        memmove(this, &rs, sizeof(GValue));
        break;
    }

    m_t = rs.m_t;

    return *this;
}

GValue::GValue(GValue&& rs) {
    release();

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
    case BIN_T:
        m_v.bin = rs.m_v.bin;
        rs.m_v.bin = nullptr;
        break;
    case MAILBOX_T:
        m_v.mailbox = rs.m_v.mailbox;
        rs.m_v.mailbox = nullptr;
        break;
    default:
        memmove(this, &rs, sizeof(GValue));
        break;
    }

    m_t = rs.m_t;
    rs.m_t = NONE_T;
}

GValue& GValue::operator=(GValue&& rs) {
    release();

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
    case BIN_T:
        m_v.bin = rs.m_v.bin;
        rs.m_v.bin = nullptr;
        break;
    case MAILBOX_T:
        m_v.mailbox = rs.m_v.mailbox;
        rs.m_v.mailbox = nullptr;
        break;
    default:
        memmove(this, &rs, sizeof(GValue));
        break;
    }

    m_t = rs.m_t;
    rs.m_t = NONE_T;

    return *this;
}
