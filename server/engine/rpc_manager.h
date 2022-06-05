#pragma once

#include <queue>
#include <shared_mutex>
#include <unordered_map>
#include <vector>

#include "decode.h"
#include "gvalue.h"
#include "log.h"
#include "encode.h"

using namespace std;

class Session;
class Remote;

class RpcImp {
public:
    RpcImp() = delete;
    RpcImp(GString& rpc_name) : m_rpc_name(std::move(rpc_name)) {}
    RpcImp(GString& rpc_name, vector<GValue>& rpc_params) 
        : m_rpc_name(std::move(rpc_name)), m_rpc_params(std::move(rpc_params)) {}
    
    ~RpcImp() {}

    GString& get_rpc_name() { return m_rpc_name; }
    vector<GValue>& get_rpc_params() { return m_rpc_params; }

    void set_session(shared_ptr<Session> session) { m_session = session; }
    shared_ptr<Session> get_session() { return m_session; }

    void set_remote(shared_ptr<Remote> remote) { m_remote = remote; }
    shared_ptr<Remote> get_remote() { return m_remote; }
private:
    GString m_rpc_name;
    vector<GValue> m_rpc_params;

    shared_ptr<Session> m_session = nullptr;
    shared_ptr<Remote> m_remote = nullptr;
};

// rpc method imp
struct RpcMethodBase { 
    vector<GString> m_params_t; 
};

template<class... T>
struct RpcMethod : public RpcMethodBase {
    RpcMethod() {}

    void(*cb)(T... args);
    void args_num_check(T... args) {}

    // 变参函数模板 - 可展开实参参数包
    template<class T1, class ...T2>
    void rpc_params_parse(const T1& t, const T2&... rest) {
        m_params_t.push_back(GString(typeid(T1).name()));
        rpc_params_parse(rest...);
    }
    template<class T3>
    void rpc_params_parse(const T3& t) {
        m_params_t.push_back(GString(typeid(T3).name()));
    }
    void rpc_params_parse() {
    }
};

// rpc manager
class RpcManager {
public:
    uint16_t rpc_imp_generate(const char* buf, uint16_t length, shared_ptr<Session> session, shared_ptr<Remote> remote);

    shared_ptr<RpcImp> rpc_decode(const char* buf, uint16_t pkg_len);
    void rpc_params_decode(Decoder& decoder, vector<GValue>& params, vector<GString>& m_params_t);
    
    template<class ...T>
    Encoder rpc_encode(const GString& rpc_name, const T&... args) {
        Encoder encoder;
        encoder.write_string(rpc_name);
        rpc_params_encode(encoder, args...);
        encoder.write_end();

        return encoder;
    }
    template<class T, class ...T2>
    void rpc_params_encode(Encoder& encoder, const T& arg, const T2&... args) {
        encoder.write<T>(arg);
        rpc_params_encode(encoder, args...);
    }
    template<class T>
    void rpc_params_encode(Encoder& encoder, const T& arg) {
        encoder.write<T>(arg);
    }
    void rpc_params_encode(Encoder& encoder) {
    }

    void imp_queue_push(shared_ptr<RpcImp> imp);
    shared_ptr<RpcImp> imp_queue_pop();
	bool imp_queue_empty();

    void add_rpc_method(GString rpc_name, RpcMethodBase* method);
    auto find_rpc_method(GString rpc_name);

private:
    queue<shared_ptr<RpcImp>> m_rpc_imp_queue;
    shared_mutex m_mutex;
    unordered_map<GString, RpcMethodBase*> m_rpc_methods;
};

extern RpcManager g_rpc_manager;


// 变参结构体模板 - 可展开类型参数包
//template<class T, class... T2>
//struct RpcParamsParse {
//    RpcParamsParse() : t{ GString(typeid(T).name()) } {
//        auto next = RpcParamsParse<T2...>();
//        t.insert(t.end(), next.t.begin(), next.t.end());
//    }
//    vector<GString> t;
//};
//
//template<class T>
//struct RpcParamsParse<T> {
//    RpcParamsParse() : t{ GString(typeid(T).name()) } {}
//    vector<GString> t;
//};

template<class... T, class... T2>
void rpc_register(GString rpc_name, void(*cb)(T... args), T2... args) {
    RpcMethod<T...>* method = new RpcMethod<T...>;
    method->cb = cb;
    method->args_num_check(args...);
    method->rpc_params_parse(args...);
    //method->m_params_t = std::move(RpcParamsParse<T2...>().t);

    g_rpc_manager.add_rpc_method(rpc_name, method);
}

template<class... T>
void rpc_call(GString rpc_name, T... args) {
    auto iter = g_rpc_manager.find_rpc_method(rpc_name);

    if (sizeof...(args) != iter->second->m_params_t.size()) {
        ERROR_LOG("rpc.%s args num.%zd error, must be %zd\n", rpc_name.c_str(), sizeof...(args), iter->second->m_params_t.size());
        return;
    }

    ((RpcMethod<T...>*)(iter->second))->cb(args...);
}

template<class T, class ...T2>
void args2vector(vector<GValue>& rpc_params, T arg, T2 ...args) {
    rpc_params.push_back(arg);
    args2vector(rpc_params, args...);
}
template<class T>
void args2vector(vector<GValue>& rpc_params, T arg) {
    rpc_params.push_back(arg);
}
extern void args2vector(vector<GValue>& rpc_params);

template<class... T>
void local_rpc_call(shared_ptr<Session>& session, GString rpc_name, T ...args) {
    vector<GValue> rpc_params;
    args2vector(rpc_params, args...);
    auto imp = make_shared<RpcImp>(rpc_name, rpc_params);
    imp->set_session(session);
    g_rpc_manager.imp_queue_push(imp);

}
template<class... T>
void local_rpc_call(shared_ptr<Remote>& remote, GString rpc_name, T ...args) {
    vector<GValue> rpc_params;
    args2vector(rpc_params, args...);
    auto imp = make_shared<RpcImp>(rpc_name, rpc_params);
    imp->set_remote(remote);
    g_rpc_manager.imp_queue_push(imp);

}

#define RPC_REGISTER(rpc_name, ...) rpc_register(#rpc_name, rpc_name, __VA_ARGS__)
#define REMOTE_RPC_CALL(r, rpc_name, ...) (r)->remote_rpc_call(rpc_name, __VA_ARGS__)
#define LOCAL_RPC_CALL(r, rpc_name, ...) local_rpc_call((r), (rpc_name), __VA_ARGS__)

extern void rpc_imp_input_tick();
