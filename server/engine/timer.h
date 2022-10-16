#pragma once

#include <set>
#include <unordered_map>
#include <stdint.h>
#include <functional>
#include <memory>  // shared_ptr need include memory, omg~~~

#include "gvalue.h"
#include "log.h"
#include "decode.h"

using namespace std;

typedef int32_t TimerID;

class Encoder;
class Decoder;

class TimerBase {
public:
    TimerBase() {};
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

    void serialize(Encoder& encoder);
    void unserialize(Decoder& decoder);

    GArray m_params_t;

    TimerID m_id = 0;
    float m_interval = 0.0f;
    bool m_repeat = false;

    int64_t m_start_time = 0;
    int32_t m_fire_num = 0;
    int64_t m_expiration = 0; // ms timestamp

    GString m_cb_name = ""; // for migrate
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
    Timer(TEntity* _this) : __this(_this) {}
    ~Timer() {}

    template<class... T1>
    void _callback(T1... args) {
        (__this->*m_cb)(args...);
    }

    CBType m_cb;
    TEntity* __this;
};

class TimerCBStoreBase {
public:
    TimerCBStoreBase() {}
    virtual ~TimerCBStoreBase() {}

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

    GArray m_params_t;
};

template<class TEntity, class... T>
class TimerCBStore : public TimerCBStoreBase {

    typedef void(TEntity::*CBType)(T... args);

public:
    TimerCBStore() {}
    ~TimerCBStore() {}

    CBType m_cb;
};

class TimerManagerBase {
public:
    virtual void timer_callback(TimerBase* timer) = 0;
    virtual void restore_timer(void* _entity, const GString& cb_name, const GBin& timer_bin) {}
};

template<class TEntity>
class TimerManager : public TimerManagerBase {

public:
    TimerManager() {
        TEntity::timer_cb_store();
    }
    ~TimerManager() {
        for (auto iter = timer_cbs_store.begin(); iter != timer_cbs_store.end(); ++iter) {
            delete iter->second;
        }
    }

    void restore_timer(void* _entity, const GString& cb_name, const GBin& timer_bin) {
        auto iter = timer_cbs_store.find(cb_name);
        if (iter == timer_cbs_store.end()) {
            return;
        }

        TEntity* entity = (TEntity*)_entity;
        TimerBase* timer = nullptr;
        switch (iter->second->m_params_t.size()) {
        case 0: {
            timer = new Timer<TEntity>(entity);
            ((Timer<TEntity>*)timer)->m_cb = ((TimerCBStore<TEntity>*)iter->second)->m_cb;
            break;
            }
        case 1: {
            timer = new Timer<TEntity, GValue>(entity);
            ((Timer<TEntity, GValue>*)timer)->m_cb = ((TimerCBStore<TEntity, GValue>*)iter->second)->m_cb;
            break;
            }
        case 2: {
            timer = new Timer<TEntity, GValue, GValue>(entity);
            ((Timer<TEntity, GValue, GValue>*)timer)->m_cb = ((TimerCBStore<TEntity, GValue, GValue>*)iter->second)->m_cb;
            break;
            }
        }

        Decoder decoder(timer_bin.buf, timer_bin.size);
        decoder.read_int16(); // skip pkg len offset
        timer->unserialize(decoder);

        entity->add_timer(timer);
    }

    template<class... T, class... T2>
    TimerID regist_timer(TEntity* entity, double start, double interval, bool repeat, const GString& cb_name, void(TEntity::*cb)(T...), T2... args) {
        ASSERT_LOG(sizeof...(T) == sizeof...(args), "timer formal param size(%zu) != real param size(%zu)\n", sizeof...(T), sizeof...(args));
        ASSERT_LOG(!repeat || interval > 0.0999, "can't create repeat timer with interval < 0.1");

        int64_t start_time = nowms_timestamp() + int64_t(start * 1000);

        auto timer = new Timer<TEntity, T...>(entity);
        timer->m_id = entity->next_timer_id++;
        timer->m_interval = (float)interval;
        timer->m_repeat = repeat;
        timer->m_start_time = start_time;
        timer->m_expiration = start_time;
        timer->m_cb = cb;
        timer->params_parse(args...);
        timer->m_cb_name = cb_name;

        entity->add_timer(timer);

        //INFO_LOG("entity.%s regist_timer.%d cb_name.%s timer addr.%lld\n", entity->uuid.c_str(), timer->m_id, cb_name.c_str(), (long long)timer);

        return timer->m_id;
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

    template<class... T, class... T2>
    void store_timer_cb_for_migrate(const char* cb_name, void(TEntity::*cb)(T...), T2... args) {
        ASSERT_LOG(sizeof...(T) == sizeof...(args), "timer formal param size(%zu) != real param size(%zu)\n", sizeof...(T), sizeof...(args));

        auto timer_cb_store = new TimerCBStore<TEntity, T...>();
        timer_cb_store->m_cb = cb;
        timer_cb_store->params_parse(args...);
        timer_cbs_store.insert(make_pair(cb_name, timer_cb_store));
    }

    unordered_map<GString, TimerCBStoreBase*> timer_cbs_store;
};

#define REGIST_TIMER(start, interval, repeat, cb, ...) timer_manager.regist_timer(this, start, interval, repeat, #cb, &cb, ##__VA_ARGS__)
#define CANCELL_TIMER(timer_id) cancel_timer(timer_id)
#define STORE_TIMER_CB_FOR_MIGRATE(cb, ...) timer_manager.store_timer_cb_for_migrate(#cb, &cb, ##__VA_ARGS__)
#define RESTORE_TIMER(cb_name, timer_bin) get_timer_manager()->restore_timer(this, cb_name, timer_bin)
