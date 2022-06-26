#include <vector>
#include "rpc_manager.h"
#include "boost_asio.h"
#include "log.h"

using namespace std;

shared_ptr<RpcImp> RpcManagerBase::rpc_decode(const char* buf, uint16_t pkg_len) {
    Decoder decoder(buf, pkg_len);
    GString rpc_name = decoder.read_string();

    auto iter = m_rpc_methods.find(rpc_name);
    ASSERT_LOG(iter != m_rpc_methods.end(), "rpc %s unregist\n", rpc_name.c_str());

    // 根据模板把参数反序列化出来
    vector<GValue> params;
    rpc_params_decode(decoder, params, iter->second->m_params_t);

    if (decoder.get_offset() < decoder.get_max_offset()) {
        auto remain_len = decoder.get_max_offset() - decoder.get_offset();
        WARN_LOG("rpc(%s) %d buf undecode\n", rpc_name.c_str(), remain_len);
    }

    return make_shared<RpcImp>(rpc_name, params);
}

void RpcManagerBase::rpc_params_decode(Decoder& decoder, vector<GValue>& params, const vector<GString>& params_t) {
    for (auto iter = params_t.begin(); iter != params_t.end(); ++iter) {
        if (*iter == typeid(bool).name()) {
            params.push_back(GValue(decoder.read_bool()));
        }
        else if (*iter == typeid(int8_t).name()) {
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
        else if (*iter == typeid(GString).name()) {
            params.push_back(GValue(std::move(decoder.read_string())));
        }
        else if (*iter == typeid(GArray).name()) {
            params.push_back(GValue(std::move(decoder.read_array())));
        }
        else if (*iter == typeid(GDict).name()) {
            params.push_back(GValue(std::move(decoder.read_dict())));
        }

        if (decoder.is_finish()) {
            break;
        }
    }
}

void RpcManagerBase::add_rpc_method(const GString& rpc_name, RpcMethodBase* method) {
    m_rpc_methods.emplace(rpc_name, method);
}

RpcMethodBase* RpcManagerBase::find_rpc_method(const GString& rpc_name) {
    auto iter = m_rpc_methods.find(rpc_name);
    if (iter == m_rpc_methods.end()) {
        return nullptr;
    }
    return iter->second;
}

RpcManager g_rpc_manager;

#define READ_PRE_CHECK(pre, max) {if( (pre) > (max) ) break;}
uint16_t RpcManager::rpc_imp_generate(const char *buf, uint16_t length, shared_ptr<Session> session, shared_ptr<Remote> remote) {

    uint16_t ret = 0;

    while (true) {
        
        // read pkg len
        READ_PRE_CHECK(ret + 2, length);
        Decoder decoder(buf + ret, 2);
        uint16_t pkg_len = decoder.read_uint16();
        ret += decoder.get_offset();  // pkg head len

        // 生成RPC实例，塞进队列
        READ_PRE_CHECK(ret + pkg_len, length);
        auto imp = rpc_decode(buf + ret, pkg_len);
        ret += pkg_len;  // pkg len

        imp->set_session(session);
        imp->set_remote(remote);
        imp_queue_push(imp);
    }

    return ret;
}

void RpcManager::imp_queue_push(shared_ptr<RpcImp> imp) {
    unique_lock<shared_mutex> lock(m_mutex);
    m_rpc_imp_queue.push(imp);
}

shared_ptr<RpcImp> RpcManager::imp_queue_pop() {
    unique_lock<shared_mutex> lock(m_mutex);
    
    if (m_rpc_imp_queue.empty())
        return nullptr;

    auto imp = m_rpc_imp_queue.front();
    m_rpc_imp_queue.pop();
    return imp;
}

bool RpcManager::imp_queue_empty() {
    shared_lock<shared_mutex> lock(m_mutex);
    return m_rpc_imp_queue.empty();
}

void args2array(vector<GValue>& rpc_params) {}

shared_ptr<RpcImp> g_cur_imp = nullptr;
void rpc_imp_input_tick() {
    if (g_rpc_manager.imp_queue_empty())
        return;

    auto imp = g_rpc_manager.imp_queue_pop();
    if (nullptr == imp)
        return;

    g_cur_imp = imp;

    auto params = imp->get_rpc_params();
    switch (params.size())
    {
    case 0:
        g_rpc_manager.rpc_call(imp->get_rpc_name());
        break;
    case 1:
        g_rpc_manager.rpc_call(imp->get_rpc_name(), params[0]);
        break;
    case 2:
        g_rpc_manager.rpc_call(imp->get_rpc_name(), params[0], params[1]);
        break;
    case 3:
        g_rpc_manager.rpc_call(imp->get_rpc_name(), params[0], params[1], params[2]);
        break;
    case 4:
        g_rpc_manager.rpc_call(imp->get_rpc_name(), params[0], params[1], params[2], params[3]);
        break;
    case 5:
        g_rpc_manager.rpc_call(imp->get_rpc_name(), params[0], params[1], params[2], params[3], params[4]);
        break;
    case 6:
        g_rpc_manager.rpc_call(imp->get_rpc_name(), params[0], params[1], params[2], params[3], params[4], params[5]);
        break;
    case 7:
        g_rpc_manager.rpc_call(imp->get_rpc_name(), params[0], params[1], params[2], params[3], params[4], params[5], params[6]);
        break;
    case 8:
        g_rpc_manager.rpc_call(imp->get_rpc_name(), params[0], params[1], params[2], params[3], params[4], params[5], params[6], params[7]);
        break;
    case 9:
        g_rpc_manager.rpc_call(imp->get_rpc_name(), params[0], params[1], params[2], params[3], params[4], params[5], params[6], params[7], params[8]);
        break;
    case 10:
        g_rpc_manager.rpc_call(imp->get_rpc_name(), params[0], params[1], params[2], params[3], params[4], params[5], params[6], params[7], params[8], params[9]);
        break;
    case 11:
        g_rpc_manager.rpc_call(imp->get_rpc_name(), params[0], params[1], params[2], params[3], params[4], params[5], params[6], params[7], params[8], params[9], params[10]);
        break;
    case 12:
        g_rpc_manager.rpc_call(imp->get_rpc_name(), params[0], params[1], params[2], params[3], params[4], params[5], params[6], params[7], params[8], params[9], params[10], params[11]);
        break;
    case 13:
        g_rpc_manager.rpc_call(imp->get_rpc_name(), params[0], params[1], params[2], params[3], params[4], params[5], params[6], params[7], params[8], params[9], params[10], params[11], params[12]);
        break;
    case 14:
        g_rpc_manager.rpc_call(imp->get_rpc_name(), params[0], params[1], params[2], params[3], params[4], params[5], params[6], params[7], params[8], params[9], params[10], params[11], params[12], params[13]);
        break;
    case 15:
        g_rpc_manager.rpc_call(imp->get_rpc_name(), params[0], params[1], params[2], params[3], params[4], params[5], params[6], params[7], params[8], params[9], params[10], params[11], params[12], params[13], params[14]);
        break;
    default:
        break;
    }

    auto session = g_cur_imp->get_session();
    if (imp->get_rpc_name() != "connect_from_client" && session && !session->get_verify()) {
        ERROR_LOG("client(%s) not verify\n", session->get_remote_addr().c_str());
        session->close();
    }
}

//RPC_REGISTER(rpc_normal_param_test, int8_t(), int16_t(), int32_t(), int64_t(), uint8_t(), uint16_t(), uint32_t(), uint64_t(), float(), double(), GString(), GArray(), GDict());
//void rpc_normal_param_test(GValue i8, GValue i16, GValue i32, GValue i64, GValue ui8, GValue ui16, GValue ui32, GValue ui64, GValue f, GValue d, GValue s, GValue array, GValue dict) {
//    cout << "i8." << int(i8.as_int8())
//        << " i16." << i16.as_int16()
//        << " i32." << i32.as_int32()
//        << " i64." << i64.as_int64()
//        << " ui8." << unsigned int(ui8.as_uint8())
//        << " ui16." << ui16.as_uint16()
//        << " ui32." << ui32.as_uint32()
//        << " ui64." << ui64.as_uint64()
//        << " float." << f.as_float()
//        << " double." << d.as_double()
//        << " string." << s.as_string()
//        << endl;
//}
