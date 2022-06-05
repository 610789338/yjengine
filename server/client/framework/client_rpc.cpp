#include <iostream>
#include <map>

#include "engine/gvalue.h"
#include "engine/rpc_manager.h"
#include "engine/boost_asio.h"
#include "engine/ini.h"
#include "engine/remote_manager.h"

using namespace std;

extern shared_ptr<RpcImp> g_cur_imp;

// ------------------------------  to && from gate ------------------------------ 

void on_remote_connected() {
    auto remote = g_cur_imp->get_remote();
    g_remote_mgr.on_remote_connected(remote);
    INFO_LOG("on_gate_connected %s\n", remote->get_remote_addr().c_str());

    REMOTE_RPC_CALL(remote, "register_from_client", g_ini.get_string("Identity", "md5"));
}

void on_remote_disconnected(GValue remote_addr) {
    g_remote_mgr.on_remote_disconnected(remote_addr.as_string());
    INFO_LOG("on_gate_disconnected %s\n", remote_addr.as_string().c_str());
}

void register_ack_from_gate(GValue game_addr, GValue result) {
    if (result.as_bool()) {
        INFO_LOG("register ack from gate@%s\n", game_addr.as_string().c_str());
    }

    //// 循环无敌连环发
    //auto remote = g_remote_mgr.get_remote(game_addr.as_string());
    //REMOTE_RPC_CALL(remote, "register_from_client", g_ini.get_string("Identity", "md5"));
}

void client_rpc_handle_register() {
    RPC_REGISTER(on_remote_connected);
    RPC_REGISTER(on_remote_disconnected, GString());

    RPC_REGISTER(register_ack_from_gate, GString(), bool());
}
