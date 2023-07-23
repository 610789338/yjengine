#pragma once

#include <set>
#include <unordered_map>
#include <vector>
#include <stdint.h>
#include <functional>
#include <memory>  // shared_ptr need include memory, omg~~~

#include "gvalue.h"
#include "log.h"
#include "decode.h"
#include "encode.h"

using namespace std;

class Entity;
class Encoder;
class Decoder;

struct EventBase {
    EventBase() {};
    virtual ~EventBase() {}

    virtual EventBase* create_self() { return nullptr; }
    virtual void set_owner(void* _owner) {}
    virtual void exec(Decoder& decoder) {}

    // for migrate
    GString m_event_name = "";
    GString m_component_name = "";
};

template<class TEntity>
struct Event0 : public EventBase {
    typedef void(TEntity::*CBType)();

    Event0() {}
    Event0(TEntity* _owner, CBType _cb) : owner(_owner), m_cb(_cb) {}

    EventBase* create_self() { 
        return new Event0<TEntity>(owner, m_cb);
    }
    void set_owner(void* _owner) {
        owner = (TEntity*)_owner;
    }

    CBType m_cb;
    TEntity* owner;
    void exec(Decoder& decoder) { (owner->*m_cb)(); }
};

template<class TEntity, class T1>
struct Event1 : public EventBase {
    typedef void(TEntity::*CBType)(T1);

    Event1() {}
    Event1(TEntity* _owner, CBType _cb) : owner(_owner), m_cb(_cb) {}

    EventBase* create_self() { 
        auto event = new Event1<TEntity, T1>(owner, m_cb);
        return event;
    }
    void set_owner(void* _owner) {
        owner = (TEntity*)_owner;
    }

    CBType m_cb;
    TEntity* owner;

    RMCVR(T1) t1;

    void exec(Decoder& decoder) {
        t1 = decoder_read<RMCVR(T1)>(decoder);
        (owner->*m_cb)(t1);
    }
};

template<class TEntity, class T1, class T2>
struct Event2 : public EventBase {
    typedef void(TEntity::*CBType)(T1, T2);

    Event2() {}
    Event2(TEntity* _owner, CBType _cb) : owner(_owner), m_cb(_cb) {}

    EventBase* create_self() { 
        auto event = new Event2<TEntity, T1, T2>(owner, m_cb);
        return event;
    }
    void set_owner(void* _owner) {
        owner = (TEntity*)_owner;
    }

    CBType m_cb;
    TEntity* owner;
    RMCVR(T1) t1;
    RMCVR(T2) t2;

    void exec(Decoder& decoder) {
        t1 = decoder_read<RMCVR(T1)>(decoder);
        t2 = decoder_read<RMCVR(T2)>(decoder);
        (owner->*m_cb)(t1, t2);
    }
};

template<class TEntity, class T1, class T2, class T3>
struct Event3 : public EventBase {
    typedef void(TEntity::*CBType)(T1, T2, T3);

    Event3() {}
    Event3(TEntity* _owner, CBType _cb) : owner(_owner), m_cb(_cb) {}

    EventBase* create_self() {
        auto event = new Event3<TEntity, T1, T2, T3>(owner, m_cb);
        return event;
    }
    void set_owner(void* _owner) {
        owner = (TEntity*)_owner;
    }

    CBType m_cb;
    TEntity* owner;
    RMCVR(T1) t1;
    RMCVR(T2) t2;
    RMCVR(T3) t3;

    void exec(Decoder& decoder) {
        t1 = decoder_read<RMCVR(T1)>(decoder);
        t2 = decoder_read<RMCVR(T2)>(decoder);
        t3 = decoder_read<RMCVR(T3)>(decoder);
        (owner->*m_cb)(t1, t2, t3);
    }
};

template<class TEntity, class T1, class T2, class T3, class T4>
struct Event4 : public EventBase {
    typedef void(TEntity::*CBType)(T1, T2, T3, T4);

    Event4() {}
    Event4(TEntity* _owner, CBType _cb) : owner(_owner), m_cb(_cb) {}

    EventBase* create_self() {
        auto event = new Event4<TEntity, T1, T2, T3, T4>(owner, m_cb);
        return event;
    }
    void set_owner(void* _owner) {
        owner = (TEntity*)_owner;
    }

    CBType m_cb;
    TEntity* owner;
    RMCVR(T1) t1;
    RMCVR(T2) t2;
    RMCVR(T3) t3;
    RMCVR(T4) t4;

    void exec(Decoder& decoder) {
        t1 = decoder_read<RMCVR(T1)>(decoder);
        t2 = decoder_read<RMCVR(T2)>(decoder);
        t3 = decoder_read<RMCVR(T3)>(decoder);
        t4 = decoder_read<RMCVR(T4)>(decoder);
        (owner->*m_cb)(t1, t2, t3, t4);
    }
};

template<class TEntity, class T1, class T2, class T3, class T4, class T5>
struct Event5 : public EventBase {
    typedef void(TEntity::*CBType)(T1, T2, T3, T4, T5);

    Event5() {}
    Event5(TEntity* _owner, CBType _cb) : owner(_owner), m_cb(_cb) {}

    EventBase* create_self() {
        auto event = new Event5<TEntity, T1, T2, T3, T4, T5>(owner, m_cb);
        return event;
    }
    void set_owner(void* _owner) {
        owner = (TEntity*)_owner;
    }

    CBType m_cb;
    TEntity* owner;
    RMCVR(T1) t1;
    RMCVR(T2) t2;
    RMCVR(T3) t3;
    RMCVR(T4) t4;
    RMCVR(T5) t5;

    void exec(Decoder& decoder) {
        t1 = decoder_read<RMCVR(T1)>(decoder);
        t2 = decoder_read<RMCVR(T2)>(decoder);
        t3 = decoder_read<RMCVR(T3)>(decoder);
        t4 = decoder_read<RMCVR(T4)>(decoder);
        t5 = decoder_read<RMCVR(T5)>(decoder);
        (owner->*m_cb)(t1, t2, t3, t4, t5);
    }
};

class EventManagerBase {
public:
    EventManagerBase() {}
    virtual ~EventManagerBase() {
        for (auto iter = event_cbs_store.begin(); iter != event_cbs_store.end(); ++iter) {
            delete iter->second;
        }
    }

    template<class TEntity>
    void regist_event(TEntity* entity, const GString& event_name, void(TEntity::*cb)()) {
        auto event = new Event0<TEntity>(entity, cb);
        event->m_event_name = event_name;
        entity->add_event(event);
    }

    template<class TEntity, class T1>
    void regist_event(TEntity* entity, const GString& event_name, void(TEntity::*cb)(T1)) {
        auto event = new Event1<TEntity, T1>(entity, cb);
        event->m_event_name = event_name;
        entity->add_event(event);
    }

    template<class TEntity, class T1, class T2>
    void regist_event(TEntity* entity, const GString& event_name, void(TEntity::*cb)(T1, T2)) {
        auto event = new Event2<TEntity, T1, T2>(entity, cb);
        event->m_event_name = event_name;
        entity->add_event(event);
    }

    template<class TEntity, class T1, class T2, class T3>
    void regist_event(TEntity* entity, const GString& event_name, void(TEntity::*cb)(T1, T2, T3)) {
        auto event = new Event3<TEntity, T1, T2, T3>(entity, cb);
        event->m_event_name = event_name;
        entity->add_event(event);
    }

    template<class TEntity, class T1, class T2, class T3, class T4>
    void regist_event(TEntity* entity, const GString& event_name, void(TEntity::*cb)(T1, T2, T3, T4)) {
        auto event = new Event4<TEntity, T1, T2, T3, T4>(entity, cb);
        event->m_event_name = event_name;
        entity->add_event(event);
    }

    template<class TEntity, class T1, class T2, class T3, class T4, class T5>
    void regist_event(TEntity* entity, const GString& event_name, void(TEntity::*cb)(T1, T2, T3, T4, T5)) {
        auto event = new Event5<TEntity, T1, T2, T3, T4, T5>(entity, cb);
        event->m_event_name = event_name;
        entity->add_event(event);
    }

    void send_event(Entity* owner, const GString& event_name) {
        auto cb_set = get_event_cb_set(owner, event_name);
        for (auto iter = cb_set->begin(); iter != cb_set->end(); ++iter) {
            Decoder decoder(nullptr, 0);
            decoder.skip_head_len();
            (*iter)->exec(decoder);
        }
    }

    template<class T1>
    void send_event(Entity* owner, const GString& event_name, const T1& t1) {
        Encoder encoder;
        encoder.write(t1);
        encoder.write_end();

        auto cb_set = get_event_cb_set(owner, event_name);
        for (auto iter = cb_set->begin(); iter != cb_set->end(); ++iter) {
            Decoder decoder(encoder.get_buf(), encoder.get_offset());
            decoder.skip_head_len();
            (*iter)->exec(decoder);
        }
    }

    template<class T1, class T2>
    void send_event(Entity* owner, const GString& event_name, const T1& t1, const T2& t2) {
        Encoder encoder;
        encoder.write(t1);
        encoder.write(t2);
        encoder.write_end();

        auto cb_set = get_event_cb_set(owner, event_name);
        for (auto iter = cb_set->begin(); iter != cb_set->end(); ++iter) {
            Decoder decoder(encoder.get_buf(), encoder.get_offset());
            decoder.skip_head_len();
            (*iter)->exec(decoder);
        }
    }

    template<class T1, class T2, class T3>
    void send_event(Entity* owner, const GString& event_name, const T1& t1, const T2& t2, const T3& t3) {
        Encoder encoder;
        encoder.write(t1);
        encoder.write(t2);
        encoder.write(t3);
        encoder.write_end();

        auto cb_set = get_event_cb_set(owner, event_name);
        for (auto iter = cb_set->begin(); iter != cb_set->end(); ++iter) {
            Decoder decoder(encoder.get_buf(), encoder.get_offset());
            decoder.skip_head_len();
            (*iter)->exec(decoder);
        }
    }

    template<class T1, class T2, class T3, class T4>
    void send_event(Entity* owner, const GString& event_name, const T1& t1, const T2& t2, const T3& t3, const T4& t4) {
        Encoder encoder;
        encoder.write(t1);
        encoder.write(t2);
        encoder.write(t3);
        encoder.write(t4);
        encoder.write_end();

        auto cb_set = get_event_cb_set(owner, event_name);
        for (auto iter = cb_set->begin(); iter != cb_set->end(); ++iter) {
            Decoder decoder(encoder.get_buf(), encoder.get_offset());
            decoder.skip_head_len();
            (*iter)->exec(decoder);
        }
    }

    template<class T1, class T2, class T3, class T4, class T5>
    void send_event(Entity* owner, const GString& event_name, const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5) {
        Encoder encoder;
        encoder.write(t1);
        encoder.write(t2);
        encoder.write(t3);
        encoder.write(t4);
        encoder.write(t5);
        encoder.write_end();

        auto cb_set = get_event_cb_set(owner, event_name);
        for (auto iter = cb_set->begin(); iter != cb_set->end(); ++iter) {
            Decoder decoder(encoder.get_buf(), encoder.get_offset());
            decoder.skip_head_len();
            (*iter)->exec(decoder);
        }
    }

    vector<EventBase*>* get_event_cb_set(Entity* owner, const GString& event_name);

    template<class TEntity>
    void store_event_cb_for_migrate(const char* event_name, void(TEntity::*cb)()) {
        auto event_cb_store = new Event0<TEntity>();
        event_cb_store->m_cb = cb;
        event_cbs_store.insert(make_pair(event_name, event_cb_store));
    }

    template<class TEntity, class T1>
    void store_event_cb_for_migrate(const char* event_name, void(TEntity::*cb)(T1)) {
        auto event_cb_store = new Event1<TEntity, T1>();
        event_cb_store->m_cb = cb;
        event_cbs_store.insert(make_pair(event_name, event_cb_store));
    }

    template<class TEntity, class T1, class T2>
    void store_event_cb_for_migrate(const char* event_name, void(TEntity::*cb)(T1, T2)) {
        auto event_cb_store = new Event2<TEntity, T1, T2>();
        event_cb_store->m_cb = cb;
        event_cbs_store.insert(make_pair(event_name, event_cb_store));
    }

    template<class TEntity, class T1, class T2, class T3>
    void store_event_cb_for_migrate(const char* event_name, void(TEntity::*cb)(T1, T2, T3)) {
        auto event_cb_store = new Event3<TEntity, T1, T2, T3>();
        event_cb_store->m_cb = cb;
        event_cbs_store.insert(make_pair(event_name, event_cb_store));
    }

    template<class TEntity, class T1, class T2, class T3, class T4>
    void store_event_cb_for_migrate(const char* event_name, void(TEntity::*cb)(T1, T2, T3, T4)) {
        auto event_cb_store = new Event4<TEntity, T1, T2, T3, T4>();
        event_cb_store->m_cb = cb;
        event_cbs_store.insert(make_pair(event_name, event_cb_store));
    }

    template<class TEntity, class T1, class T2, class T3, class T4, class T5>
    void store_event_cb_for_migrate(const char* event_name, void(TEntity::*cb)(T1, T2, T3, T4, T5)) {
        auto event_cb_store = new Event5<TEntity, T1, T2, T3, T4, T5>();
        event_cb_store->m_cb = cb;
        event_cbs_store.insert(make_pair(event_name, event_cb_store));
    }

    void restore_event(Entity* entity, const GBin& event_bin);

    unordered_map<GString, EventBase*> event_cbs_store;
};

template<class EntityClassType>
class EventManager : public EventManagerBase {
public:
    EventManager() {
        EntityClassType::migrate_event_define(); 
    }

    EntityClassType* tclass;
};

#define REGIST_EVENT(event) get_event_manager()->regist_event(this, #event, &RMP(decltype(event_manager.tclass))::event);
//#define SEND_EVENT(event_name, ...) get_event_manager()->send_event(this, event_name, ##__VA_ARGS__)
#define MIGRATE_EVENT_DEF(event) event_manager.store_event_cb_for_migrate(#event, &RMP(decltype(event_manager.tclass))::event)

#define RESTORE_EVENT(event_bin) get_event_manager()->restore_event(this, event_bin)
