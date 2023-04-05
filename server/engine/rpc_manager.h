#pragma once

#include <queue>
#include <shared_mutex>
#include <unordered_map>
#include <vector>
#include <iostream>

#include "boost/thread.hpp"

#include "decode.h"
#include "encode.h"
#include "gvalue.h"
#include "log.h"

using namespace std;

class Session;
class Remote;
struct RpcMethodBase;

extern unordered_map<GString, uint16_t> g_rpc_names_l2s;
extern unordered_map<uint16_t, GString> g_rpc_names_s2l;

class RpcImp {
public:
    RpcImp() = delete;
    RpcImp(const GString& rpc_name, RpcMethodBase* rpc_method) : m_rpc_name(std::move(rpc_name)), m_rpc_method(rpc_method) {}
    ~RpcImp();

    GString& get_rpc_name() { return m_rpc_name; }

    void set_session(shared_ptr<Session> session) { m_session = session; }
    shared_ptr<Session> get_session() { return m_session; }

    void set_remote(shared_ptr<Remote> remote) { m_remote = remote; }
    shared_ptr<Remote> get_remote() { return m_remote; }

    RpcMethodBase* get_rpc_method() { return m_rpc_method; }

private:
    GString m_rpc_name;
    RpcMethodBase* m_rpc_method = nullptr;

    shared_ptr<Session> m_session = nullptr;
    shared_ptr<Remote> m_remote = nullptr;
};

typedef shared_ptr<RpcImp> InnerRpcPtr_Decode;

enum RpcType {
    SERVER_ONLY,
    EXPOSED,
    CLIENT
};

// rpc method imp
struct RpcMethodBase {
    RpcMethodBase() {}
    virtual ~RpcMethodBase() {}

    GString get_comp_name() { return component_name; }
    GString component_name = "";

    RpcType type = RpcType::SERVER_ONLY;

    virtual void decode(Decoder& decoder) {}
    virtual void exec() { ASSERT(false); }
    virtual void exec(void* _this) { ASSERT(false); }
    virtual RpcMethodBase* create_self() { return nullptr; }
    virtual RpcMethodBase* copy_self() { return nullptr; }
    virtual void set_rpc_name(const GString& rpc_name) {}
};

struct RpcMethod0 : public RpcMethodBase {
    typedef void(*CBType)();
    RpcMethod0(CBType _cb) : cb(_cb) {}
    CBType cb;

    void decode(Decoder& decoder) {}
    void exec() { cb(); }
    RpcMethodBase* create_self() { return new RpcMethod0(cb); }
    RpcMethodBase* copy_self() { return new RpcMethod0(cb); }
};

template<class T1>
struct RpcMethod1 : public RpcMethodBase {
    typedef void(*CBType)(T1);
    RpcMethod1(CBType _cb) : cb(_cb) {}
    RpcMethod1(CBType _cb, T1 _t1) : cb(_cb), t1(_t1) {}
    CBType cb;
    RMCVR(T1) t1;

    void decode(Decoder& decoder) {
        t1 = decoder_read<RMCVR(T1)>(decoder);
    }
    void exec() { cb(t1); }
    RpcMethodBase* create_self() { return new RpcMethod1(cb); }
    RpcMethodBase* copy_self() { return new RpcMethod1(cb, t1); }
};

template<class T1, class T2>
struct RpcMethod2 : public RpcMethodBase {
    typedef void(*CBType)(T1, T2);
    RpcMethod2(CBType _cb) : cb(_cb) {}
    CBType cb;
    RMCVR(T1) t1;
    RMCVR(T2) t2;

    void decode(Decoder& decoder) {
        t1 = decoder_read<RMCVR(T1)>(decoder);
        t2 = decoder_read<RMCVR(T2)>(decoder);
    }
    void exec() { cb(t1, t2); }
    RpcMethodBase* create_self() { return new RpcMethod2(cb); }
};

template<class T1, class T2, class T3>
struct RpcMethod3 : public RpcMethodBase {
    typedef void(*CBType)(T1, T2, T3);
    RpcMethod3(CBType _cb) : cb(_cb) {}
    CBType cb;
    RMCVR(T1) t1;
    RMCVR(T2) t2;
    RMCVR(T3) t3;

    void decode(Decoder& decoder) {
        t1 = decoder_read<RMCVR(T1)>(decoder);
        t2 = decoder_read<RMCVR(T2)>(decoder);
        t3 = decoder_read<RMCVR(T3)>(decoder);
    }
    void exec() { cb(t1, t2, t3); }
    RpcMethodBase* create_self() { return new RpcMethod3(cb); }
};

// special for entity rpc
struct RpcMethod3_Special : public RpcMethodBase {
    typedef void(*CBType)(bool, const GString&, InnerRpcPtr_Decode);
    RpcMethod3_Special(CBType _cb) : cb(_cb) {}
    CBType cb;
    bool from_client;
    GString entity_uuid;
    InnerRpcPtr_Decode rpc_imp;

    void decode(Decoder& decoder);
    void exec() { cb(from_client, entity_uuid, rpc_imp); }
    RpcMethodBase* create_self() { return new RpcMethod3_Special(cb); }

    void set_rpc_name(const GString& _rpc_name) { rpc_name = _rpc_name; }
    GString rpc_name;
};

template<class T1, class T2, class T3, class T4>
struct RpcMethod4 : public RpcMethodBase {
    typedef void(*CBType)(T1, T2, T3, T4);
    RpcMethod4(CBType _cb) : cb(_cb) {}
    CBType cb;
    RMCVR(T1) t1;
    RMCVR(T2) t2;
    RMCVR(T3) t3;
    RMCVR(T4) t4;

    void decode(Decoder& decoder) {
        t1 = decoder_read<RMCVR(T1)>(decoder);
        t2 = decoder_read<RMCVR(T2)>(decoder);
        t3 = decoder_read<RMCVR(T3)>(decoder);
        t4 = decoder_read<RMCVR(T4)>(decoder);
    }
    void exec() { cb(t1, t2, t3, t4); }
    RpcMethodBase* create_self() { return new RpcMethod4(cb); }
};

template<class T1, class T2, class T3, class T4, class T5>
struct RpcMethod5 : public RpcMethodBase {
    typedef void(*CBType)(T1, T2, T3, T4, T5);
    RpcMethod5(CBType _cb) : cb(_cb) {}
    CBType cb;
    RMCVR(T1) t1;
    RMCVR(T2) t2;
    RMCVR(T3) t3;
    RMCVR(T4) t4;
    RMCVR(T5) t5;

    void decode(Decoder& decoder) {
        t1 = decoder_read<RMCVR(T1)>(decoder);
        t2 = decoder_read<RMCVR(T2)>(decoder);
        t3 = decoder_read<RMCVR(T3)>(decoder);
        t4 = decoder_read<RMCVR(T4)>(decoder);
        t5 = decoder_read<RMCVR(T5)>(decoder);
    }
    void exec() { cb(t1, t2, t3, t4, t5); }
    RpcMethodBase* create_self() { return new RpcMethod5(cb); }
};

template<class T1, class T2, class T3, class T4, class T5, class T6>
struct RpcMethod6 : public RpcMethodBase {
    typedef void(*CBType)(T1, T2, T3, T4, T5, T6);
    RpcMethod6(CBType _cb) : cb(_cb) {}
    CBType cb;
    RMCVR(T1) t1;
    RMCVR(T2) t2;
    RMCVR(T3) t3;
    RMCVR(T4) t4;
    RMCVR(T5) t5;
    RMCVR(T6) t6;

    void decode(Decoder& decoder) {
        t1 = decoder_read<RMCVR(T1)>(decoder);
        t2 = decoder_read<RMCVR(T2)>(decoder);
        t3 = decoder_read<RMCVR(T3)>(decoder);
        t4 = decoder_read<RMCVR(T4)>(decoder);
        t5 = decoder_read<RMCVR(T5)>(decoder);
        t6 = decoder_read<RMCVR(T6)>(decoder);
    }
    void exec() { cb(t1, t2, t3, t4, t5, t6); }
    RpcMethodBase* create_self() { return new RpcMethod6(cb); }
};

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7>
struct RpcMethod7 : public RpcMethodBase {
    typedef void(*CBType)(T1, T2, T3, T4, T5, T6, T7);
    RpcMethod7(CBType _cb) : cb(_cb) {}
    CBType cb;
    RMCVR(T1) t1;
    RMCVR(T2) t2;
    RMCVR(T3) t3;
    RMCVR(T4) t4;
    RMCVR(T5) t5;
    RMCVR(T6) t6;
    RMCVR(T7) t7;

    void decode(Decoder& decoder) {
        t1 = decoder_read<RMCVR(T1)>(decoder);
        t2 = decoder_read<RMCVR(T2)>(decoder);
        t3 = decoder_read<RMCVR(T3)>(decoder);
        t4 = decoder_read<RMCVR(T4)>(decoder);
        t5 = decoder_read<RMCVR(T5)>(decoder);
        t6 = decoder_read<RMCVR(T6)>(decoder);
        t7 = decoder_read<RMCVR(T7)>(decoder);
    }
    void exec() { cb(t1, t2, t3, t4, t5, t6, t7); }
    RpcMethodBase* create_self() { return new RpcMethod7(cb); }
};

class RpcManagerBase {
    friend void entity_rpc_mgr_init();
public:
    RpcManagerBase() {}
    virtual ~RpcManagerBase() {}

    shared_ptr<RpcImp> rpc_decode(const char* buf, uint16_t pkg_len);
    GString rpc_name_decode(Decoder& decoder);

    template<class ...T>
    Encoder rpc_encode(const GString& rpc_name, const T&... args) {
        Encoder encoder;
        rpc_name_encode(encoder, rpc_name);
        rpc_params_encode(encoder, args...);
        encoder.write_end();
        return encoder;
    }

    void rpc_name_encode(Encoder& encoder, const GString& rpc_name_l);

    template<class T, class ...T2>
    void rpc_params_encode(Encoder& encoder, const T& arg, const T2&... args) {
        encoder.write(arg);
        rpc_params_encode(encoder, args...);
    }
    template<class T>
    void rpc_params_encode(Encoder& encoder, const T& arg) {
        encoder.write(arg);
    }
    void rpc_params_encode(Encoder& encoder) {
    }

    void add_rpc_method(const GString& rpc_name, RpcMethodBase* method);
    RpcMethodBase* find_rpc_method(const GString& rpc_name);

private:
    unordered_map<GString, RpcMethodBase*> m_rpc_methods;
};

// rpc manager
class RpcManager : public RpcManagerBase {
public:
    RpcManager();
    ~RpcManager() {}

    //GString rpc_name_decode(Decoder& decoder);
    //void rpc_name_encode(Encoder& encoder, const GString& rpc_name_l);

    void rpc_regist(const GString& rpc_name, void(*cb)()) {
        RpcMethodBase* method = new RpcMethod0(cb);
        add_rpc_method(rpc_name, method);
    }

    template<class T1>
    void rpc_regist(const GString& rpc_name, void(*cb)(T1)) {
        RpcMethodBase* method = new RpcMethod1<T1>(cb);
        add_rpc_method(rpc_name, method);
    }

    template<class T1, class T2>
    void rpc_regist(const GString& rpc_name, void(*cb)(T1, T2)) {
        RpcMethodBase* method = new RpcMethod2<T1, T2>(cb);
        add_rpc_method(rpc_name, method);
    }

    template<class T1, class T2, class T3>
    void rpc_regist(const GString& rpc_name, void(*cb)(T1, T2, T3)) {
        RpcMethodBase* method = new RpcMethod3<T1, T2, T3>(cb);
        add_rpc_method(rpc_name, method);
    }

    // special for entity rpc
    void rpc_regist(const GString& rpc_name, void(*cb)(bool, const GString&, InnerRpcPtr_Decode)) {
        RpcMethodBase* method = new RpcMethod3_Special(cb);
        add_rpc_method(rpc_name, method);
    }

    template<class T1, class T2, class T3, class T4>
    void rpc_regist(const GString& rpc_name, void(*cb)(T1, T2, T3, T4)) {
        RpcMethodBase* method = new RpcMethod4<T1, T2, T3, T4>(cb);
        add_rpc_method(rpc_name, method);
    }

    template<class T1, class T2, class T3, class T4, class T5>
    void rpc_regist(const GString& rpc_name, void(*cb)(T1, T2, T3, T4, T5)) {
        RpcMethodBase* method = new RpcMethod5<T1, T2, T3, T4, T5>(cb);
        add_rpc_method(rpc_name, method);
    }

    template<class T1, class T2, class T3, class T4, class T5, class T6>
    void rpc_regist(const GString& rpc_name, void(*cb)(T1, T2, T3, T4, T5, T6)) {
        RpcMethodBase* method = new RpcMethod6<T1, T2, T3, T4, T5, T6>(cb);
        add_rpc_method(rpc_name, method);
    }

    template<class T1, class T2, class T3, class T4, class T5, class T6, class T7>
    void rpc_regist(const GString& rpc_name, void(*cb)(T1, T2, T3, T4, T5, T6, T7)) {
        RpcMethodBase* method = new RpcMethod7<T1, T2, T3, T4, T5, T6, T7>(cb);
        add_rpc_method(rpc_name, method);
    }

    uint16_t rpc_imp_generate(const char* buf, uint16_t length, shared_ptr<Session> session, shared_ptr<Remote> remote);

    void imp_queue_push(shared_ptr<RpcImp> imp);
    shared_ptr<RpcImp> imp_queue_pop();
	bool imp_queue_empty();

private:
    queue<shared_ptr<RpcImp>> m_rpc_imp_queue;
    boost::shared_mutex m_mutex;
};

extern RpcManager g_rpc_manager;

#define RPC_REGISTER(rpc_name) g_rpc_manager.rpc_regist(#rpc_name, rpc_name)
#define REMOTE_RPC_CALL(r, rpc_name, ...) (r)->remote_rpc_call(rpc_name, ##__VA_ARGS__)
#define LOCAL_RPC_CALL(r, rpc_name, ...) (r)->local_rpc_call(rpc_name, ##__VA_ARGS__)


// ----------------------------------------------------------------------------- //
// ----------------------------------------------------------------------------- //

struct RemoteRpcQueueEleBase {
    RemoteRpcQueueEleBase(const GString& _rpc_name) : rpc_name(_rpc_name) {}
    virtual ~RemoteRpcQueueEleBase() {}

    GString rpc_name;

    virtual Encoder encode() = 0;
};

#define GEN_INNER_RPC(rpc_name, ...) g_remote_rpc_queue_ele_mgr.gen_remote_rpc_queue_ele(rpc_name, ##__VA_ARGS__);
typedef shared_ptr<RemoteRpcQueueEleBase> InnerRpcPtr_Encode;


struct RemoteRpcQueueEle0 : public RemoteRpcQueueEleBase {
    RemoteRpcQueueEle0(const GString& _rpc_name)
        : RemoteRpcQueueEleBase(_rpc_name) {}
    virtual ~RemoteRpcQueueEle0() {}

    virtual Encoder encode() {
        return Encoder(std::move(g_rpc_manager.rpc_encode(rpc_name)));
    }
};

template<class T1>
struct RemoteRpcQueueEle1 : public RemoteRpcQueueEleBase {
    RemoteRpcQueueEle1(const GString& _rpc_name, T1 _t1)
        : t1(_t1), RemoteRpcQueueEleBase(_rpc_name) {}
    virtual ~RemoteRpcQueueEle1() {}

    virtual Encoder encode() {
        return Encoder(std::move(g_rpc_manager.rpc_encode(rpc_name, t1)));
    }

    RMCVR(T1) t1;
};

template<class T1, class T2>
struct RemoteRpcQueueEle2 : public RemoteRpcQueueEleBase {
    RemoteRpcQueueEle2(const GString& _rpc_name, T1 _t1, T2 _t2)
        : t1(_t1), t2(_t2), RemoteRpcQueueEleBase(_rpc_name) {}
    virtual ~RemoteRpcQueueEle2() {}

    virtual Encoder encode() {
        return Encoder(std::move(g_rpc_manager.rpc_encode(rpc_name, t1, t2)));
    }

    RMCVR(T1) t1;
    RMCVR(T2) t2;
};

template<class T1, class T2, class T3>
struct RemoteRpcQueueEle3 : public RemoteRpcQueueEleBase {
    RemoteRpcQueueEle3(const GString& _rpc_name, T1 _t1, T2 _t2, T3 _t3)
        : t1(_t1), t2(_t2), t3(_t3), RemoteRpcQueueEleBase(_rpc_name) {}
    virtual ~RemoteRpcQueueEle3() {}

    virtual Encoder encode() {
        return Encoder(std::move(g_rpc_manager.rpc_encode(rpc_name, t1, t2, t3)));
    }

    RMCVR(T1) t1;
    RMCVR(T2) t2;
    RMCVR(T3) t3;
};

// special for entity rpc
struct RemoteRpcQueueEle3_Special : public RemoteRpcQueueEleBase {
    RemoteRpcQueueEle3_Special(const GString& _rpc_name, GString _addr, GString _entity_uuid, InnerRpcPtr_Encode _inner_rpc)
        : addr(_addr), entity_uuid(_entity_uuid), inner_rpc(_inner_rpc), RemoteRpcQueueEleBase(_rpc_name) {}
    virtual ~RemoteRpcQueueEle3_Special() {}

    virtual Encoder encode() {
        Encoder encoder = std::move(inner_rpc->encode());
        GBin inner_rpc_bin;
        encoder.move_to_bin(inner_rpc_bin);
        //GBin inner_rpc_bin(encoder.get_buf(), encoder.get_offset());
        return Encoder(std::move(g_rpc_manager.rpc_encode(rpc_name, addr, entity_uuid, inner_rpc_bin)));
    }

    GString addr;
    GString entity_uuid;
    shared_ptr<RemoteRpcQueueEleBase> inner_rpc;
};

template<class T1, class T2, class T3, class T4>
struct RemoteRpcQueueEle4 : public RemoteRpcQueueEleBase {
    RemoteRpcQueueEle4(const GString& _rpc_name, T1 _t1, T2 _t2, T3 _t3, T4 _t4)
        : t1(_t1), t2(_t2), t3(_t3), t4(_t4), RemoteRpcQueueEleBase(_rpc_name) {}
    virtual ~RemoteRpcQueueEle4() {}

    virtual Encoder encode() {
        return Encoder(std::move(g_rpc_manager.rpc_encode(rpc_name, t1, t2, t3, t4)));
    }

    RMCVR(T1) t1;
    RMCVR(T2) t2;
    RMCVR(T3) t3;
    RMCVR(T4) t4;
};

template<class T1, class T2, class T3, class T4, class T5>
struct RemoteRpcQueueEle5 : public RemoteRpcQueueEleBase {
    RemoteRpcQueueEle5(const GString& _rpc_name, T1 _t1, T2 _t2, T3 _t3, T4 _t4, T5 _t5)
        : t1(_t1), t2(_t2), t3(_t3), t4(_t4), t5(_t5), RemoteRpcQueueEleBase(_rpc_name) {}
    virtual ~RemoteRpcQueueEle5() {}

    virtual Encoder encode() {
        return Encoder(std::move(g_rpc_manager.rpc_encode(rpc_name, t1, t2, t3, t4, t5)));
    }

    RMCVR(T1) t1;
    RMCVR(T2) t2;
    RMCVR(T3) t3;
    RMCVR(T4) t4;
    RMCVR(T5) t5;
};

template<class T1, class T2, class T3, class T4, class T5, class T6>
struct RemoteRpcQueueEle6 : public RemoteRpcQueueEleBase {
    RemoteRpcQueueEle6(const GString& _rpc_name, T1 _t1, T2 _t2, T3 _t3, T4 _t4, T5 _t5, T6 _t6)
        : t1(_t1), t2(_t2), t3(_t3), t4(_t4), t5(_t5), t6(_t6), RemoteRpcQueueEleBase(_rpc_name) {}
    virtual ~RemoteRpcQueueEle6() {}

    virtual Encoder encode() {
        return Encoder(std::move(g_rpc_manager.rpc_encode(rpc_name, t1, t2, t3, t4, t5, t6)));
    }

    RMCVR(T1) t1;
    RMCVR(T2) t2;
    RMCVR(T3) t3;
    RMCVR(T4) t4;
    RMCVR(T5) t5;
    RMCVR(T6) t6;
};

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7>
struct RemoteRpcQueueEle7 : public RemoteRpcQueueEleBase {
    RemoteRpcQueueEle7(const GString& _rpc_name, T1 _t1, T2 _t2, T3 _t3, T4 _t4, T5 _t5, T6 _t6, T7 _t7)
        : t1(_t1), t2(_t2), t3(_t3), t4(_t4), t5(_t5), t6(_t6), t7(_t7), RemoteRpcQueueEleBase(_rpc_name) {}
    virtual ~RemoteRpcQueueEle7() {}

    virtual Encoder encode() {
        return Encoder(std::move(g_rpc_manager.rpc_encode(rpc_name, t1, t2, t3, t4, t5, t6, t7)));
    }

    RMCVR(T1) t1;
    RMCVR(T2) t2;
    RMCVR(T3) t3;
    RMCVR(T4) t4;
    RMCVR(T5) t5;
    RMCVR(T6) t6;
    RMCVR(T7) t7;
};


class RemoteRpcQueueEleManager {
public:
    RemoteRpcQueueEleManager() = default;
    ~RemoteRpcQueueEleManager() = default;

    shared_ptr<RemoteRpcQueueEleBase> gen_remote_rpc_queue_ele(const GString& rpc_name) {
        return shared_ptr<RemoteRpcQueueEleBase>(new RemoteRpcQueueEle0(rpc_name));
    }

    template<class T1>
    shared_ptr<RemoteRpcQueueEleBase> gen_remote_rpc_queue_ele(const GString& rpc_name, T1 t1) {
        return shared_ptr<RemoteRpcQueueEleBase>(new RemoteRpcQueueEle1<T1>(rpc_name, t1));
    }

    template<class T1, class T2>
    shared_ptr<RemoteRpcQueueEleBase> gen_remote_rpc_queue_ele(const GString& rpc_name, T1 t1, T2 t2) {
        return shared_ptr<RemoteRpcQueueEleBase>(new RemoteRpcQueueEle2<T1, T2>(rpc_name, t1, t2));
    }

    template<class T1, class T2, class T3>
    shared_ptr<RemoteRpcQueueEleBase> gen_remote_rpc_queue_ele(const GString& rpc_name, T1 t1, T2 t2, T3 t3) {
        return shared_ptr<RemoteRpcQueueEleBase>(new RemoteRpcQueueEle3<T1, T2, T3>(rpc_name, t1, t2, t3));
    }

    // special for entity rpc
    shared_ptr<RemoteRpcQueueEleBase> gen_remote_rpc_queue_ele(const GString& rpc_name, GString addr, GString entity_uuid, InnerRpcPtr_Encode inner_rpc) {
        return shared_ptr<RemoteRpcQueueEleBase>(new RemoteRpcQueueEle3_Special(rpc_name, addr, entity_uuid, inner_rpc));
    }

    template<class T1, class T2, class T3, class T4>
    shared_ptr<RemoteRpcQueueEleBase> gen_remote_rpc_queue_ele(const GString& rpc_name, T1 t1, T2 t2, T3 t3, T4 t4) {
        return shared_ptr<RemoteRpcQueueEleBase>(new RemoteRpcQueueEle4<T1, T2, T3, T4>(rpc_name, t1, t2, t3, t4));
    }

    template<class T1, class T2, class T3, class T4, class T5>
    shared_ptr<RemoteRpcQueueEleBase> gen_remote_rpc_queue_ele(const GString& rpc_name, T1 t1, T2 t2, T3 t3, T4 t4, T5 t5) {
        return shared_ptr<RemoteRpcQueueEleBase>(new RemoteRpcQueueEle5<T1, T2, T3, T4, T5>(rpc_name, t1, t2, t3, t4, t5));
    }

    template<class T1, class T2, class T3, class T4, class T5, class T6>
    shared_ptr<RemoteRpcQueueEleBase> gen_remote_rpc_queue_ele(const GString& rpc_name, T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6) {
        return shared_ptr<RemoteRpcQueueEleBase>(new RemoteRpcQueueEle6<T1, T2, T3, T4, T5, T6>(rpc_name, t1, t2, t3, t4, t5, t6));
    }

    template<class T1, class T2, class T3, class T4, class T5, class T6, class T7>
    shared_ptr<RemoteRpcQueueEleBase> gen_remote_rpc_queue_ele(const GString& rpc_name, T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6, T7 t7) {
        return shared_ptr<RemoteRpcQueueEleBase>(new RemoteRpcQueueEle7<T1, T2, T3, T4, T5, T6, T7>(rpc_name, t1, t2, t3, t4, t5, t6, t7));
    }
};

extern RemoteRpcQueueEleManager g_remote_rpc_queue_ele_mgr;
