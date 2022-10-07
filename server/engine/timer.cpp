#include <vector>

#include "timer.h"
#include "utils.h"
#include "encode.h"
#include "decode.h"

TimerID g_timer_id = 0;


void TimerBase::serialize(Encoder& encoder) {
    encoder.write_array(m_params_t);
    encoder.write_float(m_interval);
    encoder.write_bool(m_repeat);
    encoder.write_int64(m_start_time);
    encoder.write_int32(m_fire_num);
    encoder.write_int64(m_expiration);
    encoder.write_string(m_cb_name);
}

void TimerBase::unserialize(Decoder& decoder) {
    m_params_t = decoder.read_array();
    m_interval = decoder.read_float();
    m_repeat = decoder.read_bool();
    m_start_time = decoder.read_int64();
    m_fire_num = decoder.read_int32();
    m_expiration = decoder.read_int64();
    m_cb_name = decoder.read_string();
}

void TimerManagerBase::cancel_timer(TimerID timer_id) {
    auto iter = m_timer_ids.find(timer_id);
    if (iter == m_timer_ids.end())
        return;
    
    TimerBase* timer = iter->second;
    _remove(timer);
    delete timer;
}

void TimerManagerBase::tick() {

    auto now = nowms_timestamp();

    vector<TimerBase*> timer_fired;
    for (auto iter = m_timers.begin(); iter != m_timers.end(); ++iter) {
        const auto& timer = *iter;
        if (timer->m_expiration <= now) {
            ++timer->m_fire_num;
            timer_callback(timer);
            timer_fired.push_back(timer);
        }
        else {
            break;
        }
    }

    for (auto iter = timer_fired.begin(); iter != timer_fired.end(); ++iter) {
        const auto& timer = *iter;
     
        _remove(timer);

        if (timer->m_repeat) {
            timer->m_expiration = timer->m_start_time + int64_t(timer->m_fire_num * timer->m_interval * 1000);
            _insert(timer);
        }
    }
}

void TimerManagerBase::_insert(TimerBase* timer) {
    m_timers.insert(timer);
    m_timer_ids.insert(make_pair(timer->m_id, timer));
}

void TimerManagerBase::_remove(TimerBase* timer) {
    m_timers.erase(timer);
    m_timer_ids.erase(timer->m_id);
}

set<TimerManagerBase*>* timer_mgr_set = nullptr;

void timer_tick() {
    if (timer_mgr_set == nullptr)
        return;

    for (auto iter = timer_mgr_set->begin(); iter != timer_mgr_set->end(); ++iter) {
        (*iter)->tick();
    }
}
