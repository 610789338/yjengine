#pragma once

#include <stdint.h>
#include "gvalue.h"

using namespace std;

class Decoder {
public:
    Decoder() = delete;
    Decoder(const char* buf, uint16_t max_offset);
    ~Decoder();

    bool    read_bool();
    int8_t  read_int8();
    int16_t read_int16();
    int32_t read_int32();
    int64_t read_int64();

    uint8_t read_uint8();
    uint16_t read_uint16();
    uint32_t read_uint32();
    uint64_t read_uint64();

    float read_float();
    double read_double();

    GString read_string();
    GArray read_array();
    GDict read_dict();
    GBin read_bin();
    MailBox read_mailbox();
    GValue read_gvalue();

    uint16_t get_max_offset() { return m_max_offset; }
    uint16_t get_offset() { return m_offset; }
    const char* get_buf() const { return m_buf; }
    bool is_finish();
    void skip_head_len() { read_int16(); }

private:
    const char* m_buf;
    uint16_t m_max_offset;
    uint16_t m_offset;
};

template<class T> T decoder_read(Decoder& decoder) { return T(); }

template<> bool decoder_read<bool>(Decoder& decoder);
template<> int8_t decoder_read<int8_t>(Decoder& decoder);
template<> int16_t decoder_read<int16_t>(Decoder& decoder);
template<> int32_t decoder_read<int32_t>(Decoder& decoder);
template<> int64_t decoder_read<int64_t>(Decoder& decoder);
template<> uint8_t decoder_read<uint8_t>(Decoder& decoder);
template<> uint16_t decoder_read<uint16_t>(Decoder& decoder);
template<> uint32_t decoder_read<uint32_t>(Decoder& decoder);
template<> uint64_t decoder_read<uint64_t>(Decoder& decoder);

template<> float decoder_read<float>(Decoder& decoder);
template<> double decoder_read<double>(Decoder& decoder);

template<> GString decoder_read<GString>(Decoder& decoder);
template<> GArray decoder_read<GArray>(Decoder& decoder);
template<> GDict decoder_read<GDict>(Decoder& decoder);
template<> GBin decoder_read<GBin>(Decoder& decoder);
template<> MailBox decoder_read<MailBox>(Decoder& decoder);
