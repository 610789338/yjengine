#include <iostream>

#include "engine/engine.h"

using namespace std;

extern shared_ptr<RpcImp> g_cur_imp;

// ------------------------------  to && from gate ------------------------------ 

void on_remote_connected() {
    auto remote = g_cur_imp->get_remote();
    g_remote_mgr.on_remote_connected(remote);
    INFO_LOG("on_gate_connected %s\n", remote->get_remote_addr().c_str());

    REMOTE_RPC_CALL(remote, "regist_from_client", g_ini.get_string("Identity", "md5"), *g_local_entity_rpc_names);
}

void on_remote_disconnected(const GValue& remote_addr) {
    g_remote_mgr.on_remote_disconnected(remote_addr.as_string());
    INFO_LOG("on_gate_disconnected %s\n", remote_addr.as_string().c_str());
}

void regist_ack_from_gate(const GValue& gate_addr, const GValue& result) {
    if (result.as_bool()) {
        INFO_LOG("regist ack from gate@%s\n", gate_addr.as_string().c_str());
    }

    auto remote = g_remote_mgr.get_remote(gate_addr.as_string());
    REMOTE_RPC_CALL(remote, "get_game_entity_rpc_names_from_client", remote->get_local_addr());
}

void get_game_entity_rpc_names_ack(const GValue& game_entity_rpc_names) {

    auto remote = g_remote_mgr.get_rand_remote();
    if (game_entity_rpc_names.as_array().empty()) {
        REMOTE_RPC_CALL(remote, "get_game_entity_rpc_names_from_client", remote->get_local_addr());
        return;
    }

    g_entity_rpc_name_l2s.clear();
    g_entity_rpc_name_s2l.clear();

    auto& game_entity_rpc_name_array = game_entity_rpc_names.as_array();
    for (size_t i = 0; i < game_entity_rpc_name_array.size(); ++i) {
        g_entity_rpc_name_l2s.insert(make_pair(game_entity_rpc_name_array[i].as_string(), (uint16_t)g_entity_rpc_name_l2s.size()));
    }

    for (size_t i = 0; i < g_local_entity_rpc_names->size(); ++i) {
        g_entity_rpc_name_l2s.insert(make_pair((*g_local_entity_rpc_names)[i].as_string(), (uint16_t)g_entity_rpc_name_l2s.size()));
    }

    for (auto iter = g_entity_rpc_name_l2s.begin(); iter != g_entity_rpc_name_l2s.end(); ++iter) {
        g_entity_rpc_name_s2l.insert(make_pair(iter->second, iter->first));
    }

    REMOTE_RPC_CALL(remote, "create_base_entity", "BaseAccount");
}

void create_client_entity(const GValue& entity_class_name, const GValue& base_entity_uuid, const GValue& base_addr, const GValue& cell_entity_uuid, const GValue& cell_addr) {

    GDict create_data;
    create_data.insert(make_pair("base_entity_uuid", base_entity_uuid));
    create_data.insert(make_pair("base_addr", base_addr));
    create_data.insert(make_pair("cell_entity_uuid", cell_entity_uuid));
    create_data.insert(make_pair("cell_addr", cell_addr));
    create_entity(entity_class_name.as_string(), gen_uuid(), create_data);
}

void call_client_entity(const GValue& entity_uuid, const GValue& inner_rpc) {

    auto iter = g_entities.find(entity_uuid.as_string());
    if (iter == g_entities.end()) {
        ERROR_LOG("call_client_entity entity.%s not exist\n", entity_uuid.as_string().c_str());
        return;
    }

    Decoder decoder(inner_rpc.as_bin().buf, inner_rpc.as_bin().size);
    auto const pkg_len = decoder.read_uint16();
    auto const rpc_imp = iter->second->rpc_mgr->rpc_decode(inner_rpc.as_bin().buf + decoder.get_offset(), pkg_len);

    INFO_LOG("call_client_entity %s - %s\n", entity_uuid.as_string().c_str(), rpc_imp->get_rpc_name().c_str());
    iter->second->rpc_call(false, rpc_imp->get_rpc_name(), rpc_imp->get_rpc_params());
}


void rpc_handle_regist() {
    RPC_REGISTER(on_remote_connected);
    RPC_REGISTER(on_remote_disconnected, GString());
    RPC_REGISTER(regist_ack_from_gate, GString(), bool());
    RPC_REGISTER(create_client_entity, GString(), GString(), GString(), GString(), GString());

    RPC_REGISTER(get_game_entity_rpc_names_ack, GArray());

    RPC_REGISTER(call_client_entity, GString(), GBin());
}
