#include <vector>
#include "rpc_manager.h"
#include "boost_asio.h"
#include "log.h"

using namespace std;

RpcImp::~RpcImp() {
    if (m_rpc_method) {
        delete m_rpc_method;
        m_rpc_method = nullptr;
    }
}

shared_ptr<RpcImp> RpcManagerBase::rpc_decode(const char* buf, uint16_t pkg_len) {
    Decoder decoder(buf, pkg_len);
    const GString& rpc_name = rpc_name_decode(decoder);

    auto iter = find_rpc_method(rpc_name);
    ASSERT_LOG(iter != nullptr, "rpc %s unregist\n", rpc_name.c_str());

    auto ret = make_shared<RpcImp>(rpc_name, iter->create_self());
    ret->get_rpc_method()->decode(decoder);

    if (decoder.get_offset() < decoder.get_max_offset()) {
        auto remain_len = decoder.get_max_offset() - decoder.get_offset();
        WARN_LOG("rpc(%s) %d buf undecode\n", rpc_name.c_str(), remain_len);
    }

    return ret;
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

RpcManager::RpcManager() {
    uint8_t idx = 0;

    // game
    m_l2s.insert(make_pair("connect_from_client", idx++));
    m_l2s.insert(make_pair("disconnect_from_client", idx++));
    m_l2s.insert(make_pair("regist_from_gate", idx++));
    m_l2s.insert(make_pair("get_client_entity_rpc_names_ack", idx++));
    m_l2s.insert(make_pair("create_base_entity", idx++));
    m_l2s.insert(make_pair("create_cell_entity", idx++));
    m_l2s.insert(make_pair("call_base_entity", idx++));
    m_l2s.insert(make_pair("call_cell_entity", idx++));
    m_l2s.insert(make_pair("entity_property_migrate_from_oldcell", idx++));
    m_l2s.insert(make_pair("heartbeat_from_game", idx++));

    // gate
    m_l2s.insert(make_pair("on_remote_connected", idx++));
    m_l2s.insert(make_pair("on_remote_disconnected", idx++));
    m_l2s.insert(make_pair("regist_ack_from_game", idx++));
    m_l2s.insert(make_pair("regist_from_client", idx++));
    m_l2s.insert(make_pair("get_client_entity_rpc_names_from_game", idx++));
    m_l2s.insert(make_pair("get_game_entity_rpc_names_from_client", idx++));
    m_l2s.insert(make_pair("create_client_entity", idx++));
    m_l2s.insert(make_pair("create_client_entity_onreconnect", idx++));
    m_l2s.insert(make_pair("call_client_entity", idx++));
    m_l2s.insert(make_pair("heartbeat_from_gate", idx++));

    // client
    m_l2s.insert(make_pair("regist_ack_from_gate", idx++));
    m_l2s.insert(make_pair("get_game_entity_rpc_names_ack", idx++));

    for (auto iter = m_l2s.begin(); iter != m_l2s.end(); ++iter) {
        m_s2l.insert(make_pair(iter->second, iter->first));
    }
}

GString RpcManager::rpc_name_decode(Decoder& decoder) {
    uint8_t rpc_name_s = decoder.read_uint8();
    auto iter = m_s2l.find(rpc_name_s);
    ASSERT_LOG(iter != m_s2l.end(), "rpc.%d can not decompress\n", rpc_name_s);
    return iter->second;
}

void RpcManager::rpc_name_encode(Encoder& encoder, const GString& rpc_name_l) {
    auto iter = m_l2s.find(rpc_name_l);
    ASSERT_LOG(iter != m_l2s.end(), "rpc.%s can not compress\n", rpc_name_l.c_str());
    encoder.write_uint8(iter->second);
}

#define READ_PRE_CHECK(pre, max) {if( (pre) > (max) ) break;}
uint16_t RpcManager::rpc_imp_generate(const char *buf, uint16_t length, shared_ptr<Session> session, shared_ptr<Remote> remote) {

    uint16_t ret = 0;

    while (true) {
        
        // read pkg len
        READ_PRE_CHECK(ret + 2, length);
        Decoder decoder(buf + ret, 2);
        uint16_t pkg_len = decoder.read_uint16();

        // 生成RPC实例，塞进队列
        READ_PRE_CHECK(ret + 2 + pkg_len, length);
        ret += 2;  // pkg head len
        auto imp = rpc_decode(buf + ret, pkg_len);
        //DEBUG_LOG("receive rpc.%s\n", imp->get_rpc_name().c_str());
        ret += pkg_len;  // pkg len

        imp->set_session(session);
        imp->set_remote(remote);
        imp_queue_push(imp);
    }

    return ret;
}

void RpcManager::imp_queue_push(shared_ptr<RpcImp> imp) {
    unique_lock<boost::shared_mutex> lock(m_mutex);
    m_rpc_imp_queue.push(imp);
}

shared_ptr<RpcImp> RpcManager::imp_queue_pop() {
    unique_lock<boost::shared_mutex> lock(m_mutex);
    
    if (m_rpc_imp_queue.empty())
        return nullptr;

    auto imp = m_rpc_imp_queue.front();
    m_rpc_imp_queue.pop();
    return imp;
}

bool RpcManager::imp_queue_empty() {
    shared_lock<boost::shared_mutex> lock(m_mutex);
    return m_rpc_imp_queue.empty();
}

shared_ptr<RpcImp> g_cur_imp = nullptr;
void _rpc_imp_input_tick();
void rpc_imp_input_tick() {
    if (g_rpc_manager.imp_queue_empty())
        return;

    int8_t max_loop = 50;
    while (--max_loop > 0) {

        _rpc_imp_input_tick();

        if (g_rpc_manager.imp_queue_empty())
            return;
    }
}

void _rpc_imp_input_tick() {
    auto imp = g_rpc_manager.imp_queue_pop();
    if (nullptr == imp) return;

    g_cur_imp = imp;

    imp->get_rpc_method()->exec();

    auto session = imp->get_session();
    if (imp->get_rpc_name() != "connect_from_client" && 
        imp->get_rpc_name() != "heartbeat_from_gate" && 
        imp->get_rpc_name() != "heartbeat_from_client" && 
        session && !session->get_verify()) {
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
