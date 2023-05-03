#pragma once

#include <set>
#include <unordered_map>
#include <stdint.h>
#include <functional>
#include <memory>  // shared_ptr need include memory, omg~~~

#include "gvalue.h"
#include "log.h"
#include "decode.h"
#include "encode.h"

using namespace std;

typedef int32_t TimerID;

class Encoder;
class Decoder;

struct TimerBase {
    TimerBase() {};
    virtual ~TimerBase() {}

    bool operator < (const TimerBase& other) const {
        return m_expiration < other.m_expiration;
    }

    virtual void callback() {}

    virtual void serialize(Encoder& encoder);
    virtual void unserialize(Decoder& decoder);

    virtual TimerBase* create_self() { return nullptr; }
    virtual void set_this(void* _this) {}

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
        // 当timer相等时，multiset.erase(value)不止删除一个
        // 所以在m_expiration相等情况下再比较id，确保timer不会相等
        return ls->m_expiration != rs->m_expiration ? ls->m_expiration < rs->m_expiration : ls->m_id < rs->m_id;
    }
};

template<class TEntity>
struct Timer0 : public TimerBase {
    typedef void(TEntity::*CBType)();

    Timer0() {}
    Timer0(TEntity* _this, CBType _cb) : __this(_this), m_cb(_cb) {}

    void callback() { (__this->*m_cb)(); }
    TimerBase* create_self() { 
        return new Timer0<TEntity>(__this, m_cb);
    }
    void set_this(void* _this) {
        __this = (TEntity*)_this;
    }

    TimerID set_args() {
        return m_id;
    }

    CBType m_cb;
    TEntity* __this;
};

template<class TEntity, class T1>
struct Timer1 : public TimerBase {
    typedef void(TEntity::*CBType)(T1);

    Timer1() {}
    Timer1(TEntity* _this, CBType _cb) : __this(_this), m_cb(_cb) {}

    void callback() { (__this->*m_cb)(t1); }

    void serialize(Encoder& encoder) {
        TimerBase::serialize(encoder);
        encoder.write(t1);
    }

    void unserialize(Decoder& decoder) {
        TimerBase::unserialize(decoder);
        t1 = decoder_read<RMCVR(T1)>(decoder);
    }

    TimerBase* create_self() { 
        auto timer = new Timer1<TEntity, T1>(__this, m_cb);
        timer->set_args(t1);
        return timer;
    }
    void set_this(void* _this) {
        __this = (TEntity*)_this;
    }

    TimerID set_args(T1 _t1) {
        t1 = _t1;
        return m_id;
    }

    CBType m_cb;
    TEntity* __this;
    RMCVR(T1) t1;
};

template<class TEntity, class T1, class T2>
struct Timer2 : public TimerBase {
    typedef void(TEntity::*CBType)(T1, T2);

    Timer2() {}
    Timer2(TEntity* _this, CBType _cb) : __this(_this), m_cb(_cb) {}

    void callback() { (__this->*m_cb)(t1, t2); }

    void serialize(Encoder& encoder) {
        TimerBase::serialize(encoder);
        encoder.write(t1);
        encoder.write(t2);
    }

    void unserialize(Decoder& decoder) {
        TimerBase::unserialize(decoder);
        t1 = decoder_read<RMCVR(T1)>(decoder);
        t2 = decoder_read<RMCVR(T2)>(decoder);
    }

    TimerBase* create_self() { 
        auto timer = new Timer2<TEntity, T1, T2>(__this, m_cb);
        timer->set_args(t1, t2);
        return timer;
    }
    void set_this(void* _this) {
        __this = (TEntity*)_this;
    }

    TimerID set_args(T1 _t1, T2 _t2) {
        t1 = _t1;
        t2 = _t2;
        return m_id;
    }

    CBType m_cb;
    TEntity* __this;
    RMCVR(T1) t1;
    RMCVR(T2) t2;
};

template<class TEntity, class T1, class T2, class T3>
struct Timer3 : public TimerBase {
    typedef void(TEntity::*CBType)(T1, T2, T3);

    Timer3() {}
    Timer3(TEntity* _this, CBType _cb) : __this(_this), m_cb(_cb) {}

    void callback() { (__this->*m_cb)(t1, t2, t3); }

    void serialize(Encoder& encoder) {
        TimerBase::serialize(encoder);
        encoder.write(t1);
        encoder.write(t2);
        encoder.write(t3);
    }

    void unserialize(Decoder& decoder) {
        TimerBase::unserialize(decoder);
        t1 = decoder_read<RMCVR(T1)>(decoder);
        t2 = decoder_read<RMCVR(T2)>(decoder);
        t3 = decoder_read<RMCVR(T3)>(decoder);
    }

    TimerBase* create_self() {
        auto timer = new Timer3<TEntity, T1, T2, T3>(__this, m_cb);
        timer->set_args(t1, t2, t3);
        return timer;
    }
    void set_this(void* _this) {
        __this = (TEntity*)_this;
    }

    TimerID set_args(T1 _t1, T2 _t2, T3 _t3) {
        t1 = _t1;
        t2 = _t2;
        t3 = _t3;
        return m_id;
    }

    CBType m_cb;
    TEntity* __this;
    RMCVR(T1) t1;
    RMCVR(T2) t2;
    RMCVR(T3) t3;
};

template<class TEntity, class T1, class T2, class T3, class T4>
struct Timer4 : public TimerBase {
    typedef void(TEntity::*CBType)(T1, T2, T3, T4);

    Timer4() {}
    Timer4(TEntity* _this, CBType _cb) : __this(_this), m_cb(_cb) {}

    void callback() { (__this->*m_cb)(t1, t2, t3, t4); }

    void serialize(Encoder& encoder) {
        TimerBase::serialize(encoder);
        encoder.write(t1);
        encoder.write(t2);
        encoder.write(t3);
        encoder.write(t4);
    }

    void unserialize(Decoder& decoder) {
        TimerBase::unserialize(decoder);
        t1 = decoder_read<RMCVR(T1)>(decoder);
        t2 = decoder_read<RMCVR(T2)>(decoder);
        t3 = decoder_read<RMCVR(T3)>(decoder);
        t4 = decoder_read<RMCVR(T4)>(decoder);
    }

    TimerBase* create_self() {
        auto timer = new Timer4<TEntity, T1, T2, T3, T4>(__this, m_cb);
        timer->set_args(t1, t2, t3, t4);
        return timer;
    }
    void set_this(void* _this) {
        __this = (TEntity*)_this;
    }

    TimerID set_args(T1 _t1, T2 _t2, T3 _t3, T4 _t4) {
        t1 = _t1;
        t2 = _t2;
        t3 = _t3;
        t4 = _t4;
        return m_id;
    }

    CBType m_cb;
    TEntity* __this;
    RMCVR(T1) t1;
    RMCVR(T2) t2;
    RMCVR(T3) t3;
    RMCVR(T4) t4;
};

template<class TEntity, class T1, class T2, class T3, class T4, class T5>
struct Timer5 : public TimerBase {
    typedef void(TEntity::*CBType)(T1, T2, T3, T4, T5);

    Timer5() {}
    Timer5(TEntity* _this, CBType _cb) : __this(_this), m_cb(_cb) {}

    void callback() { (__this->*m_cb)(t1, t2, t3, t4, t5); }

    void serialize(Encoder& encoder) {
        TimerBase::serialize(encoder);
        encoder.write(t1);
        encoder.write(t2);
        encoder.write(t3);
        encoder.write(t4);
        encoder.write(t5);
    }

    void unserialize(Decoder& decoder) {
        TimerBase::unserialize(decoder);
        t1 = decoder_read<RMCVR(T1)>(decoder);
        t2 = decoder_read<RMCVR(T2)>(decoder);
        t3 = decoder_read<RMCVR(T3)>(decoder);
        t4 = decoder_read<RMCVR(T4)>(decoder);
        t5 = decoder_read<RMCVR(T5)>(decoder);
    }

    TimerBase* create_self() {
        auto timer = new Timer5<TEntity, T1, T2, T3, T4, T5>(__this, m_cb);
        timer->set_args(t1, t2, t3, t4, t5);
        return timer;
    }
    void set_this(void* _this) {
        __this = (TEntity*)_this;
    }

    TimerID set_args(T1 _t1, T2 _t2, T3 _t3, T4 _t4, T5 _t5) {
        t1 = _t1;
        t2 = _t2;
        t3 = _t3;
        t4 = _t4;
        t5 = _t5;
        return m_id;
    }

    CBType m_cb;
    TEntity* __this;
    RMCVR(T1) t1;
    RMCVR(T2) t2;
    RMCVR(T3) t3;
    RMCVR(T4) t4;
    RMCVR(T5) t5;
};

class TimerManagerBase {
public:
    TimerManagerBase() {}
    virtual ~TimerManagerBase() {
        for (auto iter = timer_cbs_store.begin(); iter != timer_cbs_store.end(); ++iter) {
            delete iter->second;
        }
    }

#define GENERATE_TIMER_BODY \
        int64_t start_time = nowms_timestamp() + int64_t(start * 1000); \
        timer->m_id = entity->next_timer_id++; \
        timer->m_interval = (float)interval; \
        timer->m_repeat = repeat; \
        timer->m_start_time = start_time; \
        timer->m_expiration = start_time; \
        timer->m_cb_name = cb_name;

    template<class TEntity>
    Timer0<TEntity>* regist_timer(TEntity* entity, double start, double interval, bool repeat, const GString& cb_name, void(TEntity::*cb)()) {
        ASSERT_LOG(!repeat || interval > 0.0999, "can't create repeat timer with interval < 0.1");
        auto timer = new Timer0<TEntity>(entity, cb);
        GENERATE_TIMER_BODY;
        entity->add_timer(timer);
        //INFO_LOG("entity.%s regist_timer.%d cb_name.%s timer addr.%lld\n", entity->uuid.c_str(), timer->m_id, cb_name.c_str(), (long long)timer);
        return timer;
    }

    template<class TEntity, class T1>
    Timer1<TEntity, T1>* regist_timer(TEntity* entity, double start, double interval, bool repeat, const GString& cb_name, void(TEntity::*cb)(T1)) {
        ASSERT_LOG(!repeat || interval > 0.0999, "can't create repeat timer with interval < 0.1");
        auto timer = new Timer1<TEntity, T1>(entity, cb);
        GENERATE_TIMER_BODY;
        entity->add_timer(timer);
        //INFO_LOG("entity.%s regist_timer.%d cb_name.%s timer addr.%lld\n", entity->uuid.c_str(), timer->m_id, cb_name.c_str(), (long long)timer);
        return timer;
    }

    template<class TEntity, class T1, class T2>
    Timer2<TEntity, T1, T2>* regist_timer(TEntity* entity, double start, double interval, bool repeat, const GString& cb_name, void(TEntity::*cb)(T1, T2)) {
        ASSERT_LOG(!repeat || interval > 0.0999, "can't create repeat timer with interval < 0.1");
        auto timer = new Timer2<TEntity, T1, T2>(entity, cb);
        GENERATE_TIMER_BODY;
        entity->add_timer(timer);
        //INFO_LOG("entity.%s regist_timer.%d cb_name.%s timer addr.%lld\n", entity->uuid.c_str(), timer->m_id, cb_name.c_str(), (long long)timer);
        return timer;
    }

    template<class TEntity, class T1, class T2, class T3>
    Timer3<TEntity, T1, T2, T3>* regist_timer(TEntity* entity, double start, double interval, bool repeat, const GString& cb_name, void(TEntity::*cb)(T1, T2, T3)) {
        ASSERT_LOG(!repeat || interval > 0.0999, "can't create repeat timer with interval < 0.1");
        auto timer = new Timer3<TEntity, T1, T2, T3>(entity, cb);
        GENERATE_TIMER_BODY;
        entity->add_timer(timer);
        //INFO_LOG("entity.%s regist_timer.%d cb_name.%s timer addr.%lld\n", entity->uuid.c_str(), timer->m_id, cb_name.c_str(), (long long)timer);
        return timer;
    }

    template<class TEntity, class T1, class T2, class T3, class T4>
    Timer4<TEntity, T1, T2, T3, T4>* regist_timer(TEntity* entity, double start, double interval, bool repeat, const GString& cb_name, void(TEntity::*cb)(T1, T2, T3, T4)) {
        ASSERT_LOG(!repeat || interval > 0.0999, "can't create repeat timer with interval < 0.1");
        auto timer = new Timer4<TEntity, T1, T2, T3, T4>(entity, cb);
        GENERATE_TIMER_BODY;
        entity->add_timer(timer);
        //INFO_LOG("entity.%s regist_timer.%d cb_name.%s timer addr.%lld\n", entity->uuid.c_str(), timer->m_id, cb_name.c_str(), (long long)timer);
        return timer;
    }

    template<class TEntity, class T1, class T2, class T3, class T4, class T5>
    Timer5<TEntity, T1, T2, T3, T4, T5>* regist_timer(TEntity* entity, double start, double interval, bool repeat, const GString& cb_name, void(TEntity::*cb)(T1, T2, T3, T4, T5)) {
        ASSERT_LOG(!repeat || interval > 0.0999, "can't create repeat timer with interval < 0.1");
        auto timer = new Timer5<TEntity, T1, T2, T3, T4, T5>(entity, cb);
        GENERATE_TIMER_BODY;
        entity->add_timer(timer);
        //INFO_LOG("entity.%s regist_timer.%d cb_name.%s timer addr.%lld\n", entity->uuid.c_str(), timer->m_id, cb_name.c_str(), (long long)timer);
        return timer;
    }

    template<class TEntity>
    void store_timer_cb_for_migrate(const char* cb_name, void(TEntity::*cb)()) {
        auto timer_cb_store = new Timer0<TEntity>();
        timer_cb_store->m_cb = cb;
        timer_cbs_store.insert(make_pair(cb_name, timer_cb_store));
    }

    template<class TEntity, class T1>
    void store_timer_cb_for_migrate(const char* cb_name, void(TEntity::*cb)(T1)) {
        auto timer_cb_store = new Timer1<TEntity, T1>();
        timer_cb_store->m_cb = cb;
        timer_cbs_store.insert(make_pair(cb_name, timer_cb_store));
    }

    template<class TEntity, class T1, class T2>
    void store_timer_cb_for_migrate(const char* cb_name, void(TEntity::*cb)(T1, T2)) {
        auto timer_cb_store = new Timer2<TEntity, T1, T2>();
        timer_cb_store->m_cb = cb;
        timer_cbs_store.insert(make_pair(cb_name, timer_cb_store));
    }

    template<class TEntity, class T1, class T2, class T3>
    void store_timer_cb_for_migrate(const char* cb_name, void(TEntity::*cb)(T1, T2, T3)) {
        auto timer_cb_store = new Timer3<TEntity, T1, T2, T3>();
        timer_cb_store->m_cb = cb;
        timer_cbs_store.insert(make_pair(cb_name, timer_cb_store));
    }

    template<class TEntity, class T1, class T2, class T3, class T4>
    void store_timer_cb_for_migrate(const char* cb_name, void(TEntity::*cb)(T1, T2, T3, T4)) {
        auto timer_cb_store = new Timer4<TEntity, T1, T2, T3, T4>();
        timer_cb_store->m_cb = cb;
        timer_cbs_store.insert(make_pair(cb_name, timer_cb_store));
    }

    template<class TEntity, class T1, class T2, class T3, class T4, class T5>
    void store_timer_cb_for_migrate(const char* cb_name, void(TEntity::*cb)(T1, T2, T3, T4, T5)) {
        auto timer_cb_store = new Timer5<TEntity, T1, T2, T3, T4, T5>();
        timer_cb_store->m_cb = cb;
        timer_cbs_store.insert(make_pair(cb_name, timer_cb_store));
    }

    void restore_timer(void* entity, const GString& cb_name, const GBin& timer_bin);

    unordered_map<GString, TimerBase*> timer_cbs_store;
};

template<class EntityClassType>
class TimerManager : public TimerManagerBase {
public:
    TimerManager() {
        EntityClassType::migrate_timer_def_inside();
        EntityClassType::migrate_timer_define(); 
    }

    EntityClassType* tclass;
};

#define REGIST_TIMER_INNER(start, interval, repeat, cb_name, cb, ...) get_timer_manager()->regist_timer(this, start, interval, repeat, cb_name, cb)->set_args(__VA_ARGS__)
#define REGIST_TIMER(start, interval, repeat, cb, ...) timer_manager.regist_timer(this, start, interval, repeat, #cb, &RMP(decltype(timer_manager.tclass))::cb)->set_args(__VA_ARGS__)
#define CANCEL_TIMER(timer_id) cancel_timer(timer_id)
#define MIGRATE_TIMER_DEF(cb) timer_manager.store_timer_cb_for_migrate(#cb, &RMP(decltype(timer_manager.tclass))::cb)
#define RESTORE_TIMER(cb_name, timer_bin) get_timer_manager()->restore_timer(this, cb_name, timer_bin)

#define MIGRATE_TIMER_DEF_INSIDE(TCLASS) \
    static void migrate_timer_def_inside() { \
        timer_manager.store_timer_cb_for_migrate("time_to_save", &TCLASS::time_to_save); \
        timer_manager.store_timer_cb_for_migrate("time_to_disaster_backup", &TCLASS::time_to_disaster_backup); \
        EntityType entity_type = (EntityType)TCLASS::ENTITY_TYPE; \
        if (entity_type == EntityType::EntityType_BaseWithCell) { \
            timer_manager.store_timer_cb_for_migrate("heart_beat_timer", &BaseEntityWithCell::heart_beat_timer); \
        } \
        else if (entity_type == EntityType::EntityType_BaseWithCellAndClient) { \
            timer_manager.store_timer_cb_for_migrate("heart_beat_timer", &BaseEntityWithCell::heart_beat_timer); \
        } \
        else if (entity_type == EntityType::EntityType_Cell) { \
            timer_manager.store_timer_cb_for_migrate("heart_beat_timer", &CellEntity::heart_beat_timer); \
        } \
        else if (entity_type == EntityType::EntityType_CellWithClient) { \
            timer_manager.store_timer_cb_for_migrate("heart_beat_timer", &CellEntity::heart_beat_timer); \
        } \
    }
