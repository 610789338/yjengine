#include <iostream>

#include "engine/gvalue.h"
#include "engine/rpc_manager.h"
#include "engine/boost_asio.h"
#include "engine/ini.h"
#include "engine/remote_manager.h"

#include "gate_instance.h"

using namespace std;

extern shared_ptr<RpcImp> g_cur_imp;

// ------------------------------  to && from game ------------------------------ 

void on_remote_connected() {
    auto remote = g_cur_imp->get_remote();
    g_remote_mgr.on_remote_connected(remote);
    INFO_LOG("on game connected %s\n", remote->get_remote_addr().c_str());

    REMOTE_RPC_CALL(remote, "regist_from_gate", get_listen_addr());
}

void on_remote_disconnected(GString remote_addr) {
    g_remote_mgr.on_remote_disconnected(remote_addr);
    g_gate_instance->on_game_disappear(remote_addr);
    INFO_LOG("on game disconnected %s\n", remote_addr.c_str());
}

GArray g_game_entity_rpc_names;
GArray g_client_entity_rpc_names;

void regist_ack_from_game(GString game_addr, bool result, GArray game_entity_rpc_names) {
    if (result) {
        INFO_LOG("regist ack from game@%s\n", game_addr.c_str());
    }

    g_game_entity_rpc_names = game_entity_rpc_names;
}


// ------------------------------  from && to client ------------------------------ 

void connect_from_client() {
    auto session = g_cur_imp->get_session();
    g_session_mgr.on_session_connected(session);
    INFO_LOG("connect from client %s\n", session->get_remote_addr().c_str());
}

void disconnect_from_client(GString session_addr) {
    g_session_mgr.on_session_disconnected(session_addr);
    INFO_LOG("disconnect from client %s\n", session_addr.c_str());
}

void regist_from_client(GString identity, GArray client_entity_rpc_names) {

    auto session = g_cur_imp->get_session();
    auto local_identity = ini_get_string("Identity", "md5");
    if (identity != local_identity) {
        ERROR_LOG("client identity(%s) != %s\n", identity.c_str(), local_identity.c_str());
        session->close();
        return;
    }

    session->set_verify(true);
    INFO_LOG("regist from client@%s\n", session->get_remote_addr().c_str());

    REMOTE_RPC_CALL(session, "regist_ack_from_gate", session->get_local_addr(), true);

    g_client_entity_rpc_names = client_entity_rpc_names;
}

void get_client_entity_rpc_names_from_game(GString game_addr) {
    auto remote = g_remote_mgr.get_remote(game_addr);
    if (nullptr == remote) {
        return;
    }

    REMOTE_RPC_CALL(remote, "get_client_entity_rpc_names_ack", g_client_entity_rpc_names);
}

void get_game_entity_rpc_names_from_client(GString client_addr) {
    auto session = g_session_mgr.get_session(client_addr);
    if (nullptr == session) {
        return;
    }

    REMOTE_RPC_CALL(session, "get_game_entity_rpc_names_ack", g_game_entity_rpc_names);
}

void create_base_entity(GString entity_class_name, GString entity_uuid) {
    auto remote = g_remote_mgr.get_rand_remote();
    if ( nullptr == remote ) {
        return;
    }

    INFO_LOG("create_base_entity %s uuid %s\n", entity_class_name.c_str(), entity_uuid.c_str());

    auto session = g_cur_imp->get_session();
    REMOTE_RPC_CALL(remote, "create_base_entity", entity_class_name, /*client_addr*/session->get_remote_addr(), /*gate_addr*/get_listen_addr(), /*entity_uuid*/ entity_uuid);
}

void create_cell_entity(GString entity_class_name, GString entity_uuid, GString base_addr, GString gate_addr, GString client_addr, GBin cell_bin) {
    auto remote = g_remote_mgr.get_rand_remote();
    if (nullptr == remote) {
        return;
    }

    INFO_LOG("create_cell_entity %s\n", entity_class_name.c_str());

    REMOTE_RPC_CALL(remote, "create_cell_entity", entity_class_name, entity_uuid, base_addr, gate_addr, client_addr, cell_bin);
}

void create_client_entity(GString client_addr, GString entity_class_name, GString entity_uuid, GString base_addr, GString cell_addr) {
    auto session = g_session_mgr.get_session(client_addr);
    if (nullptr == session) {
        return;
    }

    INFO_LOG("create_client_entity %s\n", entity_class_name.c_str());

    REMOTE_RPC_CALL(session, "create_client_entity", entity_class_name, entity_uuid, base_addr, cell_addr);
}

void create_client_entity_onreconnect(GString client_addr, GString entity_class_name, GString entity_uuid, GString base_addr, GString cell_addr) {
    auto session = g_session_mgr.get_session(client_addr);
    if (nullptr == session) {
        return;
    }

    INFO_LOG("create_client_entity_onreconnect %s\n", entity_class_name.c_str());

    REMOTE_RPC_CALL(session, "create_client_entity_onreconnect", entity_class_name, entity_uuid, base_addr, cell_addr);
}

void call_base_entity(GString base_addr, GString entity_uuid, GBin inner_rpc) {
    auto remote = g_remote_mgr.get_remote(base_addr);
    if (nullptr == remote) {
        return;
    }

    //INFO_LOG("call_base_entity %s\n", entity_uuid.c_str());

    bool from_client = g_cur_imp->get_session() != nullptr;
    REMOTE_RPC_CALL(remote, "call_base_entity", from_client, entity_uuid, inner_rpc);
}

void call_cell_entity(GString cell_addr, GString entity_uuid, GBin inner_rpc) {
    auto remote = g_remote_mgr.get_remote(cell_addr);
    if (nullptr == remote) {
        return;
    }

    //INFO_LOG("call_cell_entity %s\n", entity_uuid.c_str());

    bool from_client = g_cur_imp->get_session() != nullptr;
    REMOTE_RPC_CALL(remote, "call_cell_entity", from_client, entity_uuid, inner_rpc);
}

void call_client_entity(GString client_addr, GString entity_uuid, GBin inner_rpc) {
    auto session = g_session_mgr.get_session(client_addr);
    if (nullptr == session) {
        return;
    }

    //INFO_LOG("call_client_entity %s\n", entity_uuid.c_str());

    REMOTE_RPC_CALL(session, "call_client_entity", entity_uuid, inner_rpc);
}

void base_recover_by_disaster_backup(const GString& cell_addr, const GString& client_addr, const GString& client_gate_addr, const GString& entity_class_name, 
                                        const GString& entity_uuid, const GBin& disaster_backup_of_base, const GDict& disaster_backup_of_base_migrate_data) {
    auto remote = g_remote_mgr.get_rand_remote();
    if (nullptr == remote) {
        return;
    }

    REMOTE_RPC_CALL(remote, "base_recover_by_disaster_backup", cell_addr, client_addr, client_gate_addr, entity_class_name, entity_uuid, disaster_backup_of_base, disaster_backup_of_base_migrate_data);
}

void cell_recover_by_disaster_backup(const GString& base_addr, const GString& client_addr, const GString& client_gate_addr, const GString& entity_class_name, 
                                        const GString& entity_uuid, const GBin& disaster_backup_of_cell, const GDict& disaster_backup_of_cell_migrate_data) {
    auto remote = g_remote_mgr.get_rand_remote();
    if (nullptr == remote) {
        return;
    }

    REMOTE_RPC_CALL(remote, "cell_recover_by_disaster_backup", base_addr, client_addr, client_gate_addr, entity_class_name, entity_uuid, disaster_backup_of_cell, disaster_backup_of_cell_migrate_data);
}

extern void migrate_rpc_handle_regist();
extern void heartbeat_from_game();

void rpc_handle_regist() {
    RPC_REGISTER(on_remote_connected);
    RPC_REGISTER(on_remote_disconnected);

    RPC_REGISTER(connect_from_client);
    RPC_REGISTER(disconnect_from_client);

    RPC_REGISTER(regist_ack_from_game);
    RPC_REGISTER(regist_from_client);

    RPC_REGISTER(get_client_entity_rpc_names_from_game);
    RPC_REGISTER(get_game_entity_rpc_names_from_client);

    RPC_REGISTER(create_base_entity);
    RPC_REGISTER(create_cell_entity);
    RPC_REGISTER(create_client_entity);
    RPC_REGISTER(create_client_entity_onreconnect);

    RPC_REGISTER(call_base_entity);
    RPC_REGISTER(call_cell_entity);
    RPC_REGISTER(call_client_entity);

    RPC_REGISTER(heartbeat_from_game);

    RPC_REGISTER(base_recover_by_disaster_backup);
    RPC_REGISTER(cell_recover_by_disaster_backup);

    migrate_rpc_handle_regist();
}
