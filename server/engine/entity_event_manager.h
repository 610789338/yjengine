#pragma once

#include <unordered_map>
#include <unordered_set>

#include "gvalue.h"
#include "encode.h"
#include "decode.h"

using namespace std;

struct EventCallBackBase {
    EventCallBackBase() {}
    virtual ~EventCallBackBase() {}

    virtual void exec(void* _this, Decoder& decoder) {}
};

template<class TEntity>
struct EventCallBack0 : public EventCallBackBase {
    typedef void(TEntity::*CBType)();
    EventCallBack0(CBType _cb) : cb(_cb) {}
    CBType cb;
    void exec(void* _this, Decoder& decoder) { (((TEntity*)_this)->*cb)(); }
};

template<class TEntity, class T1>
struct EventCallBack1 : public EventCallBackBase {
    typedef void(TEntity::*CBType)(T1);
    EventCallBack1(CBType _cb) : cb(_cb) {}
    CBType cb;
    RMCVR(T1) t1;

    void exec(void* _this, Decoder& decoder) {
        t1 = decoder_read<RMCVR(T1)>(decoder);
        (((TEntity*)_this)->*cb)(t1);
    }
};

template<class TEntity, class T1, class T2>
struct EventCallBack2 : public EventCallBackBase {
    typedef void(TEntity::*CBType)(T1, T2);
    EventCallBack2(CBType _cb) : cb(_cb) {}
    CBType cb;
    RMCVR(T1) t1;
    RMCVR(T2) t2;

    void exec(void* _this, Decoder& decoder) {
        t1 = decoder_read<RMCVR(T1)>(decoder);
        t2 = decoder_read<RMCVR(T2)>(decoder);
        (((TEntity*)_this)->*cb)(t1, t2);
    }
};

template<class TEntity, class T1, class T2, class T3>
struct EventCallBack3 : public EventCallBackBase {
    typedef void(TEntity::*CBType)(T1, T2, T3);
    EventCallBack3(CBType _cb) : cb(_cb) {}
    CBType cb;
    RMCVR(T1) t1;
    RMCVR(T2) t2;
    RMCVR(T3) t3;

    void exec(void* _this, Decoder& decoder) {
        t1 = decoder_read<RMCVR(T1)>(decoder);
        t2 = decoder_read<RMCVR(T2)>(decoder);
        t3 = decoder_read<RMCVR(T3)>(decoder);
        (((TEntity*)_this)->*cb)(t1, t2, t3);
    }
};

class EventManagerBase {
public:
    EventManagerBase() {}
    virtual ~EventManagerBase() {}

    template<class TEntity>
    void regist_event(const GString& event_name, void(TEntity::*cb)()) {
        EventCallBackBase* event_cb = new EventCallBack0<TEntity>(cb);
        auto cb_set = get_event_cb_set(event_name);
        cb_set->insert(event_cb);
    }

    template<class TEntity, class T1>
    void regist_event(const GString& event_name, void(TEntity::*cb)(T1)) {
        EventCallBackBase* event_cb = new EventCallBack1<TEntity, T1>(cb);
        auto cb_set = get_event_cb_set(event_name);
        cb_set->insert(event_cb);
    }

    template<class TEntity, class T1, class T2>
    void regist_event(const GString& event_name, void(TEntity::*cb)(T1, T2)) {
        EventCallBackBase* event_cb = new EventCallBack2<TEntity, T1, T2>(cb);
        auto cb_set = get_event_cb_set(event_name);
        cb_set->insert(event_cb);
    }

    template<class TEntity, class T1, class T2, class T3>
    void regist_event(const GString& event_name, void(TEntity::*cb)(T1, T2, T3)) {
        EventCallBackBase* event_cb = new EventCallBack3<TEntity, T1, T2, T3>(cb);
        auto cb_set = get_event_cb_set(event_name);
        cb_set->insert(event_cb);
    }

    void send_event(void* _this, const GString& event_name) {
        auto cb_set = get_event_cb_set(event_name);
        for (auto iter = cb_set->begin(); iter != cb_set->end(); ++iter) {
            Decoder decoder(nullptr, 0);
            decoder.skip_head_len();
            (*iter)->exec(_this, decoder);
        }
    }

    template<class T1>
    void send_event(void* _this, const GString& event_name, const T1& t1) {
        Encoder encoder;
        encoder.write(t1);
        encoder.write_end();

        auto cb_set = get_event_cb_set(event_name);
        for (auto iter = cb_set->begin(); iter != cb_set->end(); ++iter) {
            Decoder decoder(encoder.get_buf(), encoder.get_offset());
            decoder.skip_head_len();
            (*iter)->exec(_this, decoder);
        }
    }

    template<class T1, class T2>
    void send_event(void* _this, const GString& event_name, const T1& t1, const T2& t2) {
        Encoder encoder;
        encoder.write(t1);
        encoder.write(t2);
        encoder.write_end();

        auto cb_set = get_event_cb_set(event_name);
        for (auto iter = cb_set->begin(); iter != cb_set->end(); ++iter) {
            Decoder decoder(encoder.get_buf(), encoder.get_offset());
            decoder.skip_head_len();
            (*iter)->exec(_this, decoder);
        }
    }

    template<class T1, class T2, class T3>
    void send_event(void* _this, const GString& event_name, const T1& t1, const T2& t2, const T3& t3) {
        Encoder encoder;
        encoder.write(t1);
        encoder.write(t2);
        encoder.write(t3);
        encoder.write_end();

        auto cb_set = get_event_cb_set(event_name);
        for (auto iter = cb_set->begin(); iter != cb_set->end(); ++iter) {
            Decoder decoder(encoder.get_buf(), encoder.get_offset());
            decoder.skip_head_len();
            (*iter)->exec(_this, decoder);
        }
    }

    unordered_set<EventCallBackBase*>* get_event_cb_set(const GString& event_name) {
        auto iter = m_event_cbs.find(event_name);
        if (iter == m_event_cbs.end()) {
            m_event_cbs.insert(make_pair(event_name, unordered_set<EventCallBackBase*>()));
            iter = m_event_cbs.find(event_name);
        }

        return &(iter->second);
    }

private:
    unordered_map<GString, unordered_set<EventCallBackBase*>> m_event_cbs;
};

template<class EntityClassType>
class EventManager : public EventManagerBase {
public:
    EventManager() {}
    ~EventManager() {}

    EntityClassType* tclass;
};

#define REGIST_EVENT(event_name, rpc) \
event_manager.regist_event(event_name, &RMP(decltype(event_manager.tclass))::rpc); \
if (get_owner()) { \
    get_owner()->comp_regist_event(event_name, this); \
}


#define SEND_EVENT(event_name, ...) send_event(event_name, ##__VA_ARGS__)
#define COMP_SEND_EVENT(event_name, ...) get_owner()->send_event(event_name, ##__VA_ARGS__)
