#include "rpc_manager.h"
#include "log.h"

using namespace std;

RpcManager g_rpc_manager;

uint16_t read_uint16(const char *buf) {
    return (uint16_t(buf[0]) << 8) | uint16_t(buf[1]);
}

string read_string(const char *buf) {
    int end = 0;
    for (; end < 16*1024; ++end) {
        if (buf[end] == 0) break;
    }

    return string(buf, buf + end);
}

#define READ_PRE_CHECK(pre, max) {if( (pre) > (max) ) break;}
uint16_t RpcManager::rpc_imp_generate(const char *buf, uint16_t length) {

    uint16_t ret = 0;

    while (true) {
        
        // read pkg len
        READ_PRE_CHECK(ret + 2, length);
        uint16_t pkg_len = read_uint16(buf + ret);
        ret += 2;  // pkg head len

        // 生成RPC实例，塞进队列
        READ_PRE_CHECK(ret + pkg_len, length);
        auto imp = rpc_decode(buf + ret, pkg_len);
        ret += pkg_len;  // pkg len

        imp_queue_push(imp);
    }

    return ret;
}

shared_ptr<RpcImp> RpcManager::rpc_decode(const char* buf, uint16_t pkg_len) {
    string rpc_name = read_string(buf);
    
    uint16_t offset = 0;
    offset += uint16_t(rpc_name.length() + 1);

    auto iter = m_rpc_methods.find(rpc_name);
    if (iter == m_rpc_methods.end()) {
        ERROR_LOG("rpc %s unregister\n", rpc_name.c_str());
        return nullptr;
    }

    // 根据模板把参数反序列化出来，然后调用对应rpc
    auto &params_t = iter->second->m_params_t;
    for (auto iiter = params_t.begin(); iiter != params_t.end(); ++iiter) {
        if (*iiter == typeid(uint16_t).name()) {
            auto param = read_uint16(buf + offset);
            //rpc_call(rpc_name, param);

            offset += 2;
        }
        else {

        }

        if (offset >= pkg_len) {
            break;
        }

        // TODO - 复杂类型参数的反序列化 - 子线程
        // TODO - 反序列化参数之后的rpc调用 - 主线程
    }

    return make_shared<RpcImp>(std::move(rpc_name));
}

void RpcManager::imp_queue_push(shared_ptr<RpcImp> imp) {
    unique_lock<shared_mutex> lock(m_mutex);
    m_rpc_imp_queue.push(imp);
}

shared_ptr<RpcImp> RpcManager::imp_queue_pop() {
    unique_lock<shared_mutex> lock(m_mutex);
    auto imp = m_rpc_imp_queue.front();
    m_rpc_imp_queue.pop();
    return imp;
}

bool RpcManager::imp_queue_empty() {
    shared_lock<shared_mutex> lock(m_mutex);
    return m_rpc_imp_queue.empty();
}

void RpcManager::add_rpc_method(string rpc_name, RpcMethodBase* method) {
    m_rpc_methods.emplace(rpc_name, method);
}

auto RpcManager::find_rpc_method(string rpc_name) {
    return m_rpc_methods.find(rpc_name);
}

void rpc_imp_input_tick() {
    if (g_rpc_manager.imp_queue_empty())
        return;

    auto imp = g_rpc_manager.imp_queue_pop();
    INFO_LOG("rpc_tick %s\n", imp->get_rpc_name());
}
