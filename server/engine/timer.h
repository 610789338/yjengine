#pragma once

#include <set>
#include <unordered_map>
#include <stdint.h>
#include <functional>
#include <memory>  // shared_ptr need include memory, omg~~~

#include "gvalue.h"
#include "log.h"

using namespace std;

typedef int TimerID;

class TimerBase {
public:
    TimerBase() = delete;
    TimerBase(TimerID id, float interval, bool repeat, int64_t expiration)
        : m_id(id)
        , m_interval(interval)
        , m_repeat(repeat)
        , m_expiration(expiration) {}
    virtual ~TimerBase() {}

    bool operator < (const TimerBase& other) const {
        return m_expiration < other.m_expiration;
    }

    template<class T, class ...T2>
    void params_parse(const T& t, const T2&... rest) {
        m_params_t.push_back(t);
        params_parse(rest...);
    }
    template<class T>
    void params_parse(const T& t) {
        m_params_t.push_back(t);
    }
    void params_parse() {
    }

    vector<GValue> m_params_t;

    TimerID m_id;
    float m_interval;
    bool m_repeat;

    int64_t m_expiration; // ms timestamp
};

class TimerCompare {
public:
    bool operator()(const TimerBase* ls, const TimerBase* rs) const {
        return ls->m_expiration < rs->m_expiration;
    }
};

template<class TEntity, class... T>
class Timer : public TimerBase {

    typedef void(TEntity::*CBType)(T... args);

public:
    Timer() = delete;
    Timer(TEntity* _this, TimerID id, float interval, bool repeat, int64_t expiration) 
        : TimerBase(id, interval, repeat, expiration)
        , __this(_this) {}
    ~Timer() {}

    template<class... T>
    void _callback(T... args) {
        (__this->*m_cb)(args...);
    }

    CBType m_cb;
    TEntity* __this;
};

class TimerManagerBase {
public:
    void cancel_timer(TimerID time_id);
    void tick();

    virtual void timer_callback(TimerBase* timer) = 0;

protected:
    void _insert(TimerBase* timer);
    void _remove(TimerBase* timer);

    set<TimerBase*, TimerCompare> m_timers;
    unordered_map<TimerID, TimerBase*> m_timer_ids;
};

extern TimerID g_time_id;
extern set<TimerManagerBase*>* timer_mgr_set;
extern void timer_tick();

template<class TEntity>
class TimerManager : public TimerManagerBase {

public:
    TimerManager() {
        if (timer_mgr_set == nullptr) {
            timer_mgr_set = new set<TimerManagerBase*>;
        }
        timer_mgr_set->insert(this);
    }
    ~TimerManager() {
        timer_mgr_set->erase(this);
    }

    template<class... T, class... T2>
    TimerID regist_timer(TEntity* entity, float start, float interval, bool repeat, void(TEntity::*cb)(T...), T2... args) {

        int64_t expiration = nowms_timestamp() + int64_t(start * 1000);
        DEBUG_LOG("regist_timer expiration.%lld\n", expiration);

        TimerID time_id = ++g_time_id;
        auto timer = new Timer<TEntity, T...>(entity, time_id, interval, repeat, expiration);
        timer->m_cb = cb;
        timer->params_parse(args...);

        _insert(timer);

        return time_id;
    }

    void timer_callback(TimerBase* timer) {
        const auto& prs = timer->m_params_t;
        const auto& tid = timer->m_id;
        switch (prs.size()) {
        case 0: {
            Timer<TEntity>* _timer = (Timer<TEntity>*)timer;
            _timer->_callback();
            break;
            }
        case 1: {
            Timer<TEntity, GValue>* _timer = (Timer<TEntity, GValue>*)timer;
            _timer->_callback(prs[0]);
            break;
            }
        case 2: {
            Timer<TEntity, GValue, GValue>* _timer = (Timer<TEntity, GValue, GValue>*)timer;
            _timer->_callback(prs[0], prs[1]);
            break;
            }
        }
    }
};

#define RETIST_TIMER(start, interval, repeat, cb, ...) timer_manager.regist_timer(this, start, interval, repeat, cb, __VA_ARGS__);
#define CANCELL_TIMER(timer_id) timer_manager.cancel_timer(timer_id);
