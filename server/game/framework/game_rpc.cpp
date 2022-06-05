#include <iostream>
#include <map>

#include "engine/rpc_manager.h"
#include "engine/boost_asio.h"

using namespace std;

extern shared_ptr<RpcImp> g_cur_imp;

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

void connect_from_client() {
    auto session = g_cur_imp->get_session();
    g_session_mgr.on_session_connected(session);
    INFO_LOG("on_connect_from_gate %s\n", session->get_remote_addr().c_str());
}

void disconnect_from_client(GValue session_addr) {
    g_session_mgr.on_session_disconnected(session_addr.as_string());
    INFO_LOG("on_disconnect_from_gate %s\n", session_addr.as_string().c_str());
}

void register_from_gate() {
    
    auto session = g_cur_imp->get_session();
    session->set_verify(true);
    INFO_LOG("register from gate@%s\n", session->get_remote_addr().c_str());
    
    REMOTE_RPC_CALL(session, "register_ack_from_game", session->get_local_addr(), true);
}

void game_rpc_handle_register() {
    //RPC_REGISTER(rpc_normal_param_test, int8_t(), int16_t(), int32_t(), int64_t(), uint8_t(), uint16_t(), uint32_t(), uint64_t(), float(), double(), GString(), GArray(), GDict());
    RPC_REGISTER(connect_from_client);
    RPC_REGISTER(disconnect_from_client, GString());
    RPC_REGISTER(register_from_gate);
}
