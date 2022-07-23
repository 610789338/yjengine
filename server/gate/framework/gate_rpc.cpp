#include <iostream>

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
    
    REMOTE_RPC_CALL(remote, "regist_from_gate");
}

void on_remote_disconnected(const GValue& remote_addr) {
    g_remote_mgr.on_remote_disconnected(remote_addr.as_string());
    INFO_LOG("on_game_disconnected %s\n", remote_addr.as_string().c_str());
}

void regist_ack_from_game(const GValue& game_addr, const GValue& result) {
    if (result.as_bool()) {
        INFO_LOG("regist ack from game@%s\n", game_addr.as_string().c_str());
    }
}


// ------------------------------  from && to client ------------------------------ 

void connect_from_client() {
    auto session = g_cur_imp->get_session();
    g_session_mgr.on_session_connected(session);
    INFO_LOG("on_connect_from_client %s\n", session->get_remote_addr().c_str());
}

void disconnect_from_client(const GValue& session_addr) {
    g_session_mgr.on_session_disconnected(session_addr.as_string());
    INFO_LOG("on_disconnect_from_client %s\n", session_addr.as_string().c_str());
}

void regist_from_client(const GValue& identity) {

    auto session = g_cur_imp->get_session();
    auto local_identity = g_ini.get_string("Identity", "md5");
    if (identity.as_string() != local_identity) {
        ERROR_LOG("client identity(%s) != %s\n", identity.as_string().c_str(), local_identity.c_str());
        session->close();
        return;
    }

    session->set_verify(true);
    INFO_LOG("regist from client@%s\n", session->get_remote_addr().c_str());

    REMOTE_RPC_CALL(session, "regist_ack_from_gate", session->get_local_addr(), true);
}

void create_base_entity(const GValue& entity_class_name) {
    auto remote = g_remote_mgr.get_rand_remote();
    if ( nullptr == remote ) {
        return;
    }

    INFO_LOG("create_base_entity %s\n", entity_class_name.as_string().c_str());

    auto session = g_cur_imp->get_session();
    REMOTE_RPC_CALL(remote, "create_base_entity", entity_class_name.as_string(), /*client_addr*/session->get_remote_addr(), /*gate_addr*/remote->get_local_addr());
}

void create_cell_entity(const GValue& entity_class_name, const GValue& base_entity_uuid, const GValue& base_addr, const GValue& gate_addr, const GValue& client_addr) {
    auto remote = g_remote_mgr.get_rand_remote();
    if (nullptr == remote) {
        return;
    }

    INFO_LOG("create_cell_entity %s\n", entity_class_name.as_string().c_str());

    REMOTE_RPC_CALL(remote, "create_cell_entity", entity_class_name.as_string(), base_entity_uuid.as_string(), base_addr.as_string(), gate_addr.as_string(), client_addr.as_string());
}

void create_client_entity(const GValue& client_addr, const GValue& entity_class_name, const GValue& base_entity_uuid, const GValue& base_addr, const GValue& cell_entity_uuid, const GValue& cell_addr) {
    auto session = g_session_mgr.get_session(client_addr.as_string());
    if (nullptr == session) {
        return;
    }

    INFO_LOG("create_client_entity %s\n", entity_class_name.as_string().c_str());

    REMOTE_RPC_CALL(session, "create_client_entity", entity_class_name.as_string(), base_entity_uuid.as_string(), base_addr.as_string(), cell_entity_uuid.as_string(), cell_addr.as_string());
}

void call_base_entity(const GValue& base_addr, const GValue& entity_uuid, const GValue& inner_rpc) {
    auto remote = g_remote_mgr.get_remote(base_addr.as_string());
    if (nullptr == remote) {
        return;
    }

    //INFO_LOG("call_base_entity %s\n", entity_uuid.as_string().c_str());

    bool from_client = g_cur_imp->get_session() != nullptr;
    REMOTE_RPC_CALL(remote, "call_base_entity", from_client, entity_uuid.as_string(), inner_rpc.as_bin());
}

void call_cell_entity(const GValue& cell_addr, const GValue& entity_uuid, const GValue& inner_rpc) {
    auto remote = g_remote_mgr.get_remote(cell_addr.as_string());
    if (nullptr == remote) {
        return;
    }

    //INFO_LOG("call_cell_entity %s\n", entity_uuid.as_string().c_str());

    bool from_client = g_cur_imp->get_session() != nullptr;
    REMOTE_RPC_CALL(remote, "call_cell_entity", from_client, entity_uuid.as_string(), inner_rpc.as_bin());
}

void call_client_entity(const GValue& client_addr, const GValue& entity_uuid, const GValue& inner_rpc) {
    auto session = g_session_mgr.get_session(client_addr.as_string());
    if (nullptr == session) {
        return;
    }

    //INFO_LOG("call_client_entity %s\n", entity_uuid.as_string().c_str());

    REMOTE_RPC_CALL(session, "call_client_entity", entity_uuid.as_string(), inner_rpc.as_bin());
}

void rpc_handle_regist() {
    RPC_REGISTER(on_remote_connected);
    RPC_REGISTER(on_remote_disconnected, GString());

    RPC_REGISTER(connect_from_client);
    RPC_REGISTER(disconnect_from_client, GString());

    RPC_REGISTER(regist_ack_from_game, GString(), bool());
    RPC_REGISTER(regist_from_client, GString());

    RPC_REGISTER(create_base_entity, GString());
    RPC_REGISTER(create_cell_entity, GString(), GString(), GString(), GString(), GString());
    RPC_REGISTER(create_client_entity, GString(), GString(), GString(), GString(), GString(), GString());

    RPC_REGISTER(call_base_entity, GString(), GString(), GBin());
    RPC_REGISTER(call_cell_entity, GString(), GString(), GBin());
    RPC_REGISTER(call_client_entity, GString(), GString(), GBin());
}
