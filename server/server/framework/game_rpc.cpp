#include <iostream>
#include <map>

#include "engine/gvalue.h"
#include "engine/rpc_manager.h"
#include "engine/boost_asio.h"

using namespace std;

map<GString, Session*> g_gates;
map<GString, GString> g_gate_remotes;

extern shared_ptr<RpcImp> g_cur_imp;

void rpc_normal_param_test(GValue i8, GValue i16, GValue i32, GValue i64, GValue ui8, GValue ui16, GValue ui32, GValue ui64, GValue f, GValue d, GValue s, GValue array, GValue dict) {
    cout << "i8." << int(i8.as_int8())
        << " i16." << i16.as_int16()
        << " i32." << i32.as_int32()
        << " i64." << i64.as_int64()
        << " ui8." << unsigned int(ui8.as_uint8())
        << " ui16." << ui16.as_uint16()
        << " ui32." << ui32.as_uint32()
        << " ui64." << ui64.as_uint64()
        << " float." << f.as_float()
        << " double." << d.as_double()
        << " string." << s.as_string()
        << endl;
}

void register_from_gate(GValue gate_listen_addr) {
    auto session = g_cur_imp->get_session();
    g_gates.insert(make_pair(gate_listen_addr.as_string(), session));
    g_gate_remotes.insert(make_pair(session->get_remote_addr(), gate_listen_addr.as_string()));

    INFO_LOG("register from gate@%s\n", gate_listen_addr.as_string().c_str());

    REMOTE_RPC_CALL(session, "register_ask_from_game", session->get_local_addr(), true);
}

void on_gate_disconnected(GString gate_addr) {
    auto iter = g_gate_remotes.find(gate_addr);
    if (iter == g_gate_remotes.end()) {
        return;
    }

    auto gate_listen_addr = iter->second;
    g_gates.erase(gate_listen_addr);
    g_gate_remotes.erase(gate_addr);

    INFO_LOG("gate@%s disconnected\n", gate_listen_addr.c_str());
}

void client_disconnected(GString client_addr) {
    on_gate_disconnected(client_addr);
}

void game_rpc_handle_register() {
    RPC_REGISTER(rpc_normal_param_test, int8_t(), int16_t(), int32_t(), int64_t(), uint8_t(), uint16_t(), uint32_t(), uint64_t(), float(), double(), GString(), GArray(), GDict());
    RPC_REGISTER(register_from_gate, GString());
}
