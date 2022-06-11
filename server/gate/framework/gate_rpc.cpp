#include <iostream>
#include <map>

#include "engine/gvalue.h"
#include "engine/rpc_manager.h"
#include "engine/boost_asio.h"
#include "engine/ini.h"
#include "engine/remote_manager.h"

using namespace std;

extern shared_ptr<RpcImp> g_cur_imp;

// ------------------------------  to && from game ------------------------------ 

void on_remote_connected() {
    auto remote = g_cur_imp->get_remote();
    g_remote_mgr.on_remote_connected(remote);
    INFO_LOG("on_game_connected %s\n", remote->get_remote_addr().c_str());
    
    REMOTE_RPC_CALL(remote, "register_from_gate");
}

void on_remote_disconnected(GValue remote_addr) {
    g_remote_mgr.on_remote_disconnected(remote_addr.as_string());
    INFO_LOG("on_game_disconnected %s\n", remote_addr.as_string().c_str());
}

void register_ack_from_game(GValue game_addr, GValue result) {
    if (result.as_bool()) {
        INFO_LOG("register ack from game@%s\n", game_addr.as_string().c_str());
    }
}


// ------------------------------  from && to client ------------------------------ 

void connect_from_client() {
    auto session = g_cur_imp->get_session();
    g_session_mgr.on_session_connected(session);
    INFO_LOG("on_connect_from_client %s\n", session->get_remote_addr().c_str());
}

void disconnect_from_client(GValue session_addr) {
    g_session_mgr.on_session_disconnected(session_addr.as_string());
    INFO_LOG("on_disconnect_from_client %s\n", session_addr.as_string().c_str());
}

void register_from_client(GValue identity) {

    auto session = g_cur_imp->get_session();
    auto local_identity = g_ini.get_string("Identity", "md5");
    if (identity.as_string() != local_identity) {
        ERROR_LOG("client identity(%s) != %s\n", identity.as_string().c_str(), local_identity.c_str());
        session->close();
        return;
    }

    session->set_verify(true);
    INFO_LOG("register from client@%s\n", session->get_remote_addr().c_str());

    REMOTE_RPC_CALL(session, "register_ack_from_gate", session->get_local_addr(), true);
}

void create_entity_from_client(GValue entity_type) {
    auto remote = g_remote_mgr.get_rand_remote();
    if ( nullptr == remote ) {
        return;
    }

    INFO_LOG("create_entity_from_client %s\n", entity_type.as_string().c_str());

    auto session = g_cur_imp->get_session();
    REMOTE_RPC_CALL(remote, "create_entity_from_client", entity_type.as_string(), session->get_remote_addr());
}

void rpc_handle_register() {
    RPC_REGISTER(on_remote_connected);
    RPC_REGISTER(on_remote_disconnected, GString());

    RPC_REGISTER(connect_from_client);
    RPC_REGISTER(disconnect_from_client, GString());

    RPC_REGISTER(register_ack_from_game, GString(), bool());
    RPC_REGISTER(register_from_client, GString(), GString());

    RPC_REGISTER(create_entity_from_client, GString());
}
