#include <vector>

#include "timer.h"
#include "utils.h"
#include "encode.h"
#include "decode.h"


void TimerBase::serialize(Encoder& encoder) {
    encoder.write_int32(m_id);
    encoder.write_array(m_params_t);
    encoder.write_float(m_interval);
    encoder.write_bool(m_repeat);
    encoder.write_int64(m_start_time);
    encoder.write_int32(m_fire_num);
    encoder.write_int64(m_expiration);
    encoder.write_string(m_cb_name);
}

void TimerBase::unserialize(Decoder& decoder) {
    m_id = decoder.read_int32();
    m_params_t = decoder.read_array();
    m_interval = decoder.read_float();
    m_repeat = decoder.read_bool();
    m_start_time = decoder.read_int64();
    m_fire_num = decoder.read_int32();
    m_expiration = decoder.read_int64();
    m_cb_name = decoder.read_string();
}
