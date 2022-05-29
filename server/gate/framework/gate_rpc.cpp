#include <iostream>
#include <map>

#include "engine/gvalue.h"
#include "engine/rpc_manager.h"
#include "engine/boost_asio.h"
#include "engine/ini.h"
#include "remote_manager.h"

using namespace std;

map<GString, Session*> g_clients;
map<GString, GString>  g_client_remotes;

extern shared_ptr<RpcImp> g_cur_imp;

void register_ask_from_game(GValue game_addr, GValue result) {
    if (result.as_bool()) {
        INFO_LOG("register ask from game@%s success\n", game_addr.as_string().c_str());
    }

    // 循环无敌连环发
    auto remote = g_remote_mgr.get_remote(game_addr.as_string());
    REMOTE_RPC_CALL(remote, "register_from_gate", server->get_listen_addr());
}

void register_from_client(GValue gate_addr, GValue identity) {
    auto session = g_cur_imp->get_session();
    if (identity.as_string() != g_ini.get_string("Identity", "md5")) {
        session->close();
    }
    g_clients.insert(make_pair(gate_addr.as_string(), session));
    g_client_remotes.insert(make_pair(session->get_remote_addr(), gate_addr.as_string()));
}

void gate_rpc_handle_register() {
    RPC_REGISTER(register_ask_from_game, GString(), bool());
    RPC_REGISTER(register_from_client, GString(), GString());
}
