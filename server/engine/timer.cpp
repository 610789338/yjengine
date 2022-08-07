#include <vector>

#include "timer.h"
#include "utils.h"

TimerID g_time_id = 0;


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
    for (auto iter = m_timers.begin(); iter != m_timers.end();) {
        const auto& timer = *iter;
        if (timer->m_expiration < now) {
            timer_callback(timer);
            timer_fired.push_back(timer);
        }

        break;
    }

    for (auto iter = timer_fired.begin(); iter != timer_fired.end(); ++iter) {
        const auto& timer = *iter;
     
        _remove(timer);

        if (timer->m_repeat) {
            timer->m_expiration = now + int64_t(timer->m_interval * 1000);
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
