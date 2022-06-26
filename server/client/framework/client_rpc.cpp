#include <iostream>
#include <map>

#include "engine/engine.h"

using namespace std;

extern shared_ptr<RpcImp> g_cur_imp;

// ------------------------------  to && from gate ------------------------------ 

void on_remote_connected() {
    auto remote = g_cur_imp->get_remote();
    g_remote_mgr.on_remote_connected(remote);
    INFO_LOG("on_gate_connected %s\n", remote->get_remote_addr().c_str());

    REMOTE_RPC_CALL(remote, "regist_from_client", g_ini.get_string("Identity", "md5"));
}

void on_remote_disconnected(const GValue& remote_addr) {
    g_remote_mgr.on_remote_disconnected(remote_addr.as_string());
    INFO_LOG("on_gate_disconnected %s\n", remote_addr.as_string().c_str());
}

void regist_ack_from_gate(const GValue& game_addr, const GValue& result) {
    if (result.as_bool()) {
        INFO_LOG("regist ack from gate@%s\n", game_addr.as_string().c_str());
    }

    auto remote = g_remote_mgr.get_remote(game_addr.as_string());
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

void rpc_handle_regist() {
    RPC_REGISTER(on_remote_connected);
    RPC_REGISTER(on_remote_disconnected, GString());
    RPC_REGISTER(regist_ack_from_gate, GString(), bool());
    RPC_REGISTER(create_client_entity, GString(), GString(), GString(), GString(), GString());
}