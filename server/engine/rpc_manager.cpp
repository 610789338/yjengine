#include <vector>
#include "rpc_manager.h"
#include "boost_asio.h"
#include "log.h"
#include "entity.h"

using namespace std;

unordered_map<GString, uint16_t> g_rpc_names_l2s;
unordered_map<uint16_t, GString> g_rpc_names_s2l;

static void gen_global_rpc_name_turn() {
    uint8_t idx = 0;
    // game
    g_rpc_names_l2s.insert(make_pair("connect_from_client", idx++));
    g_rpc_names_l2s.insert(make_pair("disconnect_from_client", idx++));
    g_rpc_names_l2s.insert(make_pair("regist_from_gate", idx++));
    g_rpc_names_l2s.insert(make_pair("get_client_entity_rpc_names_ack", idx++));
    g_rpc_names_l2s.insert(make_pair("create_entity", idx++));
    //g_rpc_names_l2s.insert(make_pair("create_base_entity", idx++));
    g_rpc_names_l2s.insert(make_pair("create_cell_entity", idx++));
    g_rpc_names_l2s.insert(make_pair("call_base_entity", idx++));
    g_rpc_names_l2s.insert(make_pair("call_base_entity_b2c", idx++));
    g_rpc_names_l2s.insert(make_pair("call_cell_entity", idx++));
    g_rpc_names_l2s.insert(make_pair("entity_property_migrate_from_oldcell", idx++));
    g_rpc_names_l2s.insert(make_pair("heartbeat_from_game", idx++));
    g_rpc_names_l2s.insert(make_pair("on_game_disappear", idx++));
    g_rpc_names_l2s.insert(make_pair("base_recover_by_disaster_backup", idx++));
    g_rpc_names_l2s.insert(make_pair("cell_recover_by_disaster_backup", idx++));
    g_rpc_names_l2s.insert(make_pair("call_game", idx++));
    g_rpc_names_l2s.insert(make_pair("create_stub", idx++));
    g_rpc_names_l2s.insert(make_pair("on_stub_create", idx++));

    // gate
    g_rpc_names_l2s.insert(make_pair("on_remote_connected", idx++));
    g_rpc_names_l2s.insert(make_pair("on_remote_disconnected", idx++));
    g_rpc_names_l2s.insert(make_pair("regist_ack_from_game", idx++));
    g_rpc_names_l2s.insert(make_pair("regist_from_client", idx++));
    g_rpc_names_l2s.insert(make_pair("get_client_entity_rpc_names_from_game", idx++));
    g_rpc_names_l2s.insert(make_pair("get_game_entity_rpc_names_from_client", idx++));
    g_rpc_names_l2s.insert(make_pair("create_client_entity", idx++));
    g_rpc_names_l2s.insert(make_pair("create_client_entity_onreconnect", idx++));
    g_rpc_names_l2s.insert(make_pair("call_client_entity", idx++));
    g_rpc_names_l2s.insert(make_pair("heartbeat_from_gate", idx++));
    g_rpc_names_l2s.insert(make_pair("game_broadcast", idx++));
    g_rpc_names_l2s.insert(make_pair("game_unicast", idx++));
    g_rpc_names_l2s.insert(make_pair("game_unicast_random", idx++));

    // client
    g_rpc_names_l2s.insert(make_pair("regist_ack_from_gate", idx++));
    g_rpc_names_l2s.insert(make_pair("get_game_entity_rpc_names_ack", idx++));
    for (auto iter = g_rpc_names_l2s.begin(); iter != g_rpc_names_l2s.end(); ++iter) {
        g_rpc_names_s2l.insert(make_pair(iter->second, iter->first));
    }
}

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
    ret->get_rpc_method()->set_rpc_name(rpc_name);
    ret->get_rpc_method()->decode(decoder);

    if (decoder.get_offset() < decoder.get_max_offset()) {
        auto remain_len = decoder.get_max_offset() - decoder.get_offset();
        WARN_LOG("rpc(%s) %d buf undecode\n", rpc_name.c_str(), remain_len);
    }

    return ret;
}

extern unordered_map<GString, uint16_t> all_rpc_names_l2s;
extern unordered_map<uint16_t, GString> all_rpc_names_s2l;

GString RpcManagerBase::rpc_name_decode(Decoder& decoder) {
    uint16_t rpc_name_s = decoder.read_uint16();

    {
        shared_lock<boost::shared_mutex> lock(g_rpc_name_turn_mutex);
        auto iter = all_rpc_names_s2l.find(rpc_name_s);
        if (iter != all_rpc_names_s2l.end()) {
            return iter->second;
        }
    }

    auto iter2 = g_rpc_names_s2l.find(rpc_name_s);
    ASSERT_LOG(iter2 != g_rpc_names_s2l.end(), "rpc.%d can not decompress\n", rpc_name_s);
    return iter2->second;
}

extern void try_gen_all_rpc_names();
void RpcManagerBase::rpc_name_encode(Encoder& encoder, const GString& rpc_name_l) {
    {
        if (all_rpc_names_l2s.empty()) {
            try_gen_all_rpc_names();
        }
        
        shared_lock<boost::shared_mutex> lock(g_rpc_name_turn_mutex);
        auto iter = all_rpc_names_l2s.find(rpc_name_l);
        if (iter != all_rpc_names_l2s.end()) {
            encoder.write_uint16(iter->second);
            return;
        }
    }

    auto iter2 = g_rpc_names_l2s.find(rpc_name_l);
    ASSERT_LOG(iter2 != g_rpc_names_l2s.end(), "rpc.%s can not compress\n", rpc_name_l.c_str());
    encoder.write_uint16(iter2->second);
}

void RpcManagerBase::add_rpc_method(const GString& rpc_name, RpcMethodBase* method) {
    ASSERT_LOG(m_rpc_methods.find(rpc_name) == m_rpc_methods.end(), "rpc method(%s) already exist\n", rpc_name.c_str());
    m_rpc_methods.emplace(rpc_name, method);
}

RpcMethodBase* RpcManagerBase::find_rpc_method(const GString& rpc_name) {
    auto iter = m_rpc_methods.find(rpc_name);
    if (iter == m_rpc_methods.end()) {
        return nullptr;
    }
    return iter->second;
}

void RpcManagerBase::release_rpc_method() {
    for (auto iter = m_rpc_methods.begin(); iter != m_rpc_methods.end(); ++iter) {
        delete iter->second;
    }
    m_rpc_methods.clear();
}

unordered_map<GString, RpcMethodBase*>& RpcManagerBase::get_rpc_methods() {
    return m_rpc_methods;
}

RpcManager::RpcManager() {
    gen_global_rpc_name_turn();
}

//GString RpcManager::rpc_name_decode(Decoder& decoder) {
//    uint8_t rpc_name_s = decoder.read_uint8();
//    auto iter = m_s2l.find(rpc_name_s);
//    ASSERT_LOG(iter != m_s2l.end(), "rpc.%d can not decompress\n", rpc_name_s);
//    return iter->second;
//}

//void RpcManager::rpc_name_encode(Encoder& encoder, const GString& rpc_name_l) {
//    auto iter = m_l2s.find(rpc_name_l);
//    ASSERT_LOG(iter != m_l2s.end(), "rpc.%s can not compress\n", rpc_name_l.c_str());
//    encoder.write_uint8(iter->second);
//}

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

RpcManager g_rpc_manager;

RemoteRpcQueueEleManager g_remote_rpc_queue_ele_mgr;

void RpcMethod1_Special::decode(Decoder& decoder) {
    GBin inner_rpc_bin = decoder_read<GBin>(decoder);
    Decoder inner_decoder(inner_rpc_bin.buf, inner_rpc_bin.size);
    auto const pkg_len = inner_decoder.read_uint16();
    rpc_imp = g_rpc_manager.rpc_decode(inner_rpc_bin.buf + inner_decoder.get_offset(), pkg_len);
}

void RpcMethod3_Special::decode(Decoder& decoder) {
    from_client = decoder_read<bool>(decoder);
    entity_uuid = decoder_read<GString>(decoder);
    GBin inner_rpc_bin = decoder_read<GBin>(decoder);

    Decoder inner_decoder(inner_rpc_bin.buf, inner_rpc_bin.size);
    auto const pkg_len = inner_decoder.read_uint16();
    RpcManagerBase* rpcmgr = nullptr;
    if (rpc_name == "call_base_entity") {
        rpcmgr = thread_safe_get_base_entity_rpcmgr(entity_uuid);
        if (!rpcmgr) {
            ERROR_LOG("call_base_entity entity.%s not exist\n", entity_uuid.c_str());
            return;
        }
    }
    else if (rpc_name == "call_cell_entity") {
        rpcmgr = thread_safe_get_cell_entity_rpcmgr(entity_uuid);
        if (!rpcmgr) {
            ERROR_LOG("call_cell_entity entity.%s not exist\n", entity_uuid.c_str());
            return;
        }
    }
    else if (rpc_name == "call_client_entity") {
        rpcmgr = thread_safe_get_client_entity_rpcmgr(entity_uuid);
        if (!rpcmgr) {
            ERROR_LOG("call_client_entity entity.%s not exist\n", entity_uuid.c_str());
            return;
        }
    }
    else {
        ASSERT_LOG("error special rpc.%s\n", rpc_name.c_str());
    }

    rpc_imp = rpcmgr->rpc_decode(inner_rpc_bin.buf + inner_decoder.get_offset(), pkg_len);
}

void _rpc_imp_input_tick();
void rpc_imp_input_tick() {
    if (g_rpc_manager.imp_queue_empty())
        return;

    int8_t max_loop = 100;
    while (--max_loop > 0) {

        _rpc_imp_input_tick();

        if (g_rpc_manager.imp_queue_empty())
            return;
    }
}

shared_ptr<RpcImp> g_cur_imp = nullptr;
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
