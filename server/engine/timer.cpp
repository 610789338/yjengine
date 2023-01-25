#include <vector>

#include "timer.h"
#include "utils.h"
#include "encode.h"
#include "decode.h"
#include "entity.h"


void TimerBase::serialize(Encoder& encoder) {
    encoder.write_int32(m_id);
    encoder.write_float(m_interval);
    encoder.write_bool(m_repeat);
    encoder.write_int64(m_start_time);
    encoder.write_int32(m_fire_num);
    encoder.write_int64(m_expiration);
    encoder.write_string(m_cb_name);
}

void TimerBase::unserialize(Decoder& decoder) {
    m_id = decoder.read_int32();
    m_interval = decoder.read_float();
    m_repeat = decoder.read_bool();
    m_start_time = decoder.read_int64();
    m_fire_num = decoder.read_int32();
    m_expiration = decoder.read_int64();
    m_cb_name = decoder.read_string();
}

void TimerManagerBase::restore_timer(void* entity, const GString& cb_name, const GBin& timer_bin) {
    auto iter = timer_cbs_store.find(cb_name);
    if (iter == timer_cbs_store.end()) {
        return;
    }

    TimerBase* timer = iter->second->create_self();
    timer->set_this(entity);

    Decoder decoder(timer_bin.buf, timer_bin.size);
    decoder.skip_head_len();
    timer->unserialize(decoder);

    ((Entity*)entity)->add_timer(timer);
}
