// �����л�ģ��

#include "decode.h"
#include "log.h"
#include "byte_swap.h"

Decoder::Decoder(const char* buf, uint16_t max_offset) {
    m_buf = buf;
    m_max_offset = max_offset;
    m_offset = 0;
}

Decoder::~Decoder() {
    m_buf = nullptr;
}

bool Decoder::read_bool() {
    if (is_finish()) return false;

    bool v = *(bool*)(m_buf + m_offset);
    m_offset += 1;
    return v;
}

int8_t Decoder::read_int8() {
    if (is_finish()) return 0;

    int8_t v = *(int8_t*)(m_buf + m_offset);
    m_offset += 1;
    return v;
}

int16_t Decoder::read_int16() {
    if (is_finish()) return 0;

    int16_t v = *(int16_t*)(m_buf + m_offset);
    m_offset += 2;
    return byte_swap(v);
}

int32_t Decoder::read_int32() {
    if (is_finish()) return 0;

    int32_t v = *(int32_t*)(m_buf + m_offset);
    m_offset += 4;
    return byte_swap(v);
}

int64_t Decoder::read_int64() {
    if (is_finish()) return 0;

    int64_t v = *(int64_t*)(m_buf + m_offset);
    m_offset += 8;
    return byte_swap(v);
}

uint8_t Decoder::read_uint8() {
    if (is_finish()) return 0;

    uint8_t v = *(uint8_t*)(m_buf + m_offset);
    m_offset += 1;
    return v;
}

uint16_t Decoder::read_uint16() {
    if (is_finish()) return 0;

    uint16_t v = *(uint16_t*)(m_buf + m_offset);
    m_offset += 2;
    return byte_swap(v);
}

uint32_t Decoder::read_uint32() {
    if (is_finish()) return 0;

    uint32_t v = *(uint32_t*)(m_buf + m_offset);
    m_offset += 4;
    return byte_swap(v);
}

uint64_t Decoder::read_uint64() {
    if (is_finish()) return 0;

    uint64_t v = *(uint64_t*)(m_buf + m_offset);
    m_offset += 8;
    return byte_swap(v);
}

float Decoder::read_float() {
    if (is_finish()) return 0.0f;

    float v = *(float*)(m_buf + m_offset);
    m_offset += 4;
    return byte_swap(v);
}

double Decoder::read_double() {
    if (is_finish()) return 0.0;

    double v = *(double*)(m_buf + m_offset);
    m_offset += 8;
    return byte_swap(v);
}

GString Decoder::read_string() {
    if (is_finish()) return GString();

    int end = 0;
    for (; end < 16 * 1024; ++end) {
        if (m_buf[m_offset + end] == 0) break;
    }

    GString v(m_buf + m_offset, m_buf + m_offset  + end);
    m_offset += (end + 1);
    return v;
}

GArray Decoder::read_array() {
    if (is_finish()) return GArray();

    GArray array;
    uint16_t len = read_uint16();
    for (auto i = 0; i < len; ++i) {
        array.push_back(read_gvalue());
    }

    return array;
}

GDict Decoder::read_dict() {
    if (is_finish()) return GDict();

    GDict dict;

    uint16_t len = read_uint16();
    for (auto i = 0; i < len; ++i) {
        auto key = read_string();
        auto value = read_gvalue();
        dict.insert(make_pair(key, std::move(value)));
    }

    return dict;
}

GBin Decoder::read_bin() {
    if (is_finish()) return GBin(nullptr, 0);

    uint16_t size = read_uint16();

    GBin bin(m_buf + m_offset, size);
    m_offset += bin.size;
    return bin;
}

MailBox Decoder::read_mailbox() {
    if (is_finish()) return MailBox();

    auto entity_uuid = read_string();
    auto addr = read_string();
    auto flag = read_int8();

    MailBox mailbox;
    mailbox.set_entity_and_addr(entity_uuid, addr);
    mailbox.set_flag(flag);

    return mailbox;
}

GValue Decoder::read_gvalue() {
    uint8_t t = read_uint8();

    switch (t)
    {
    case GType::INT8_T:
        return GValue(read_int8());
    case GType::INT16_T:
        return GValue(read_int16());
    case GType::INT32_T:
        return GValue(read_int32());
    case GType::INT64_T:
        return GValue(read_int64());
    case GType::UINT8_T:
        return GValue(read_uint8());
    case GType::UINT16_T:
        return GValue(read_uint16());
    case GType::UINT32_T:
        return GValue(read_uint32());
    case GType::UINT64_T:
        return GValue(read_uint64());
    case GType::FLOAT_T:
        return GValue(read_float());
    case GType::DOUBLE_T:
        return GValue(read_double());
    case GType::STRING_T:
        return GValue(read_string());
    case GType::ARRAY_T:
        return GValue(read_array());
    case GType::DICT_T:
        return GValue(read_dict());
    case GType::BIN_T:
        return GValue(read_bin());
    case GType::MAILBOX_T:
        return GValue(read_mailbox());
    default:
        ERROR_LOG("error type.%d\n", t);
        ASSERT(false);
        break;
    }

    return GValue(-1);
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
template<> bool decoder_read<bool>(Decoder& decoder) { return decoder.read_bool(); }
template<> int8_t decoder_read<int8_t>(Decoder& decoder) { return decoder.read_int8(); }
template<> int16_t decoder_read<int16_t>(Decoder& decoder) { return decoder.read_int16(); }
template<> int32_t decoder_read<int32_t>(Decoder& decoder) { return decoder.read_int32(); }
template<> int64_t decoder_read<int64_t>(Decoder& decoder) { return decoder.read_int64(); }
template<> uint8_t decoder_read<uint8_t>(Decoder& decoder) { return decoder.read_uint8(); }
template<> uint16_t decoder_read<uint16_t>(Decoder& decoder) { return decoder.read_uint16(); }
template<> uint32_t decoder_read<uint32_t>(Decoder& decoder) { return decoder.read_uint32(); }
template<> uint64_t decoder_read<uint64_t>(Decoder& decoder) { return decoder.read_uint64(); }

template<> float decoder_read<float>(Decoder& decoder) { return decoder.read_float(); }
template<> double decoder_read<double>(Decoder& decoder) { return decoder.read_double(); }

template<> GString decoder_read<GString>(Decoder& decoder) { return decoder.read_string(); }
template<> GArray decoder_read<GArray>(Decoder& decoder) { return decoder.read_array(); }
template<> GDict decoder_read<GDict>(Decoder& decoder) { return decoder.read_dict(); }
template<> GBin decoder_read<GBin>(Decoder& decoder) { return decoder.read_bin(); }
template<> MailBox decoder_read<MailBox>(Decoder& decoder) { return decoder.read_mailbox(); }
