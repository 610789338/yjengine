#pragma once

#include <string>
#include <queue>
#include <shared_mutex>
#include <unordered_map>
#include <vector>
#include "decode.h"
#include "gvalue.h"

using namespace std;

class RpcImp {
public:
    RpcImp() = delete;
    RpcImp(string& rpc_name, vector<GValue>& rpc_params) 
        : m_rpc_name(std::move(rpc_name)), m_rpc_params(std::move(rpc_params)) {}
    ~RpcImp() {}

    string& get_rpc_name() { return m_rpc_name; }
    vector<GValue>& get_rpc_params() { return m_rpc_params; }

private:
    string m_rpc_name;
    vector<GValue> m_rpc_params;
};

// rpc method imp
struct RpcMethodBase { vector<string> m_params_t; };

template<class... T>
struct RpcMethod : public RpcMethodBase {
    RpcMethod() {}

    void(*cb)(T... args);
};

// rpc manager
class RpcManager {
public:
    uint16_t rpc_imp_generate(const char* buf, uint16_t length);
    shared_ptr<RpcImp> rpc_decode(const char* buf, uint16_t pkg_len);
    void rpc_params_decode(Decoder& decoder, vector<GValue>& params, vector<string>& m_params_t);

    void imp_queue_push(shared_ptr<RpcImp> imp);
    shared_ptr<RpcImp> imp_queue_pop();
	bool imp_queue_empty();

    void add_rpc_method(string rpc_name, RpcMethodBase* method);
    auto find_rpc_method(string rpc_name);

private:
    queue<shared_ptr<RpcImp>> m_rpc_imp_queue;
    shared_mutex m_mutex;
    unordered_map<string, RpcMethodBase*> m_rpc_methods;
};

// 变参函数模板 - 可展开实参参数包
// template<class T, class ...Args>
// void rpc_params_parse(RpcMethodBase* method, const T& t, const Args&... rest) {
//     method->m_params_t.push_back(string(typeid(T).name()));

//     rpc_params_parse(method, rest...);
// }

// template<class T>
// void rpc_params_parse(RpcMethodBase* method, const T& t) {
//     method->m_params_t.push_back(string(typeid(T).name()));
// }

// 变参类模板 - 可展开类型参数包
template<class T, class... T2>
struct RpcParamsParse {
    RpcParamsParse() : t{ string(typeid(T).name()) } {
        auto next = RpcParamsParse<T2...>();
        t.insert(t.end(), next.t.begin(), next.t.end());
    }
    vector<string> t;
};

template<class T>
struct RpcParamsParse<T> {
    RpcParamsParse() : t{ string(typeid(T).name()) } {}
    vector<string> t;
};

template<class... T>
void rpc_register(string rpc_name, void(*cb)(T... args))
{
    RpcMethod<T...>* method = new RpcMethod<T...>;
    method->cb = cb;
    method->m_params_t = std::move(RpcParamsParse<T...>().t);

    g_rpc_manager.add_rpc_method(rpc_name, method);
}

template<class... T>
void rpc_call(string rpc_name, T... args)
{
    auto iter = g_rpc_manager.find_rpc_method(rpc_name);
    ((RpcMethod<T...>*)(iter->second))->cb(args...);
}

#define RPC_REGISTER(name) rpc_register(#name, name)
#define RPC_CALL(name, ...) rpc_call(#name, __VA_ARGS__)

extern RpcManager g_rpc_manager;
extern void rpc_imp_input_tick();
