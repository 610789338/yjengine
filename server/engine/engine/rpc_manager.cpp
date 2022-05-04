#include <vector>
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
    Decoder decoder(buf, pkg_len);
    string rpc_name = decoder.read_string();
    
    auto iter = m_rpc_methods.find(rpc_name);
    if (iter == m_rpc_methods.end()) {
        ERROR_LOG("rpc %s unregister\n", rpc_name.c_str());
        return nullptr;
    }
    
    // 根据模板把参数反序列化出来
    vector<GValue> params;
    rpc_params_decode(decoder, params, iter->second->m_params_t);
    return make_shared<RpcImp>(rpc_name, params);
}

void RpcManager::rpc_params_decode(Decoder& decoder, vector<GValue>& params, vector<string>& params_t) {
    for (auto iter = params_t.begin(); iter != params_t.end(); ++iter) {
        if (*iter == typeid(int8_t).name()) {
            params.push_back(GValue(decoder.read_int8()));
        }
        else if (*iter == typeid(int16_t).name()) {
            params.push_back(GValue(decoder.read_int16()));
        }
        else if (*iter == typeid(int32_t).name()) {
            params.push_back(GValue(decoder.read_int32()));
        }
        else if (*iter == typeid(int64_t).name()) {
            params.push_back(GValue(decoder.read_int64()));
        }
        else if (*iter == typeid(uint8_t).name()) {
            params.push_back(GValue(decoder.read_uint8()));
        }
        else if (*iter == typeid(uint16_t).name()) {
            params.push_back(GValue(decoder.read_uint16()));
        }
        else if (*iter == typeid(uint32_t).name()) {
            params.push_back(GValue(decoder.read_uint32()));
        }
        else if (*iter == typeid(uint64_t).name()) {
            params.push_back(GValue(decoder.read_uint64()));
        }
        else if (*iter == typeid(float).name()) {
            params.push_back(GValue(decoder.read_float()));
        }
        else if (*iter == typeid(double).name()) {
            params.push_back(GValue(decoder.read_double()));
        }
        else if (*iter == typeid(string).name()) {
            params.push_back(GValue(decoder.read_string()));
        }

        if (decoder.is_finish()) {
            break;
        }
    }
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

    // TODO
    auto params = imp->get_rpc_params();
    switch (params.size())
    {
    case 0:
        rpc_call(imp->get_rpc_name());
        break;
    case 1:
        rpc_call(imp->get_rpc_name(), params[0].real());
        break;
    case 2:
        rpc_call(imp->get_rpc_name(), params[0].real(), params[1].real());
        break;
    case 3:
        rpc_call(imp->get_rpc_name(), params[0].real(), params[1].real(), params[2].real());
        break;
    default:
        break;
    }

    INFO_LOG("rpc_tick %s\n", imp->get_rpc_name().c_str());
}
