#include <iostream>
#include <map>

#include "engine/engine.h"


using namespace std;

extern shared_ptr<RpcImp> g_cur_imp;

// ------------------------------  from gate ------------------------------ 

void connect_from_client() {
    auto session = g_cur_imp->get_session();
    g_session_mgr.on_session_connected(session);
    INFO_LOG("on_connect_from_gate %s\n", session->get_remote_addr().c_str());
}

void disconnect_from_client(GValue session_addr) {
    g_session_mgr.on_session_disconnected(session_addr.as_string());
    INFO_LOG("on_disconnect_from_gate %s\n", session_addr.as_string().c_str());
}

void regist_from_gate() {
    
    auto session = g_cur_imp->get_session();
    session->set_verify(true);
    INFO_LOG("regist from gate@%s\n", session->get_remote_addr().c_str());
    
    REMOTE_RPC_CALL(session, "regist_ack_from_game", session->get_local_addr(), true);
}

void create_base_entity(const GValue& entity_class_name, const GValue& client_addr, const GValue& gate_addr) {

    GDict create_data;
    create_data.insert(make_pair("client_addr", client_addr));
    create_data.insert(make_pair("gate_addr", gate_addr));
    create_entity(entity_class_name.as_string(), gen_uuid(), create_data);
}

void create_cell_entity(const GValue& entity_class_name, const GValue&  base_entity_uuid, const GValue&  base_addr, const GValue& gate_addr, const GValue& client_addr) {

    GDict create_data;
    create_data.insert(make_pair("base_entity_uuid", base_entity_uuid));
    create_data.insert(make_pair("base_addr", base_addr));
    create_data.insert(make_pair("gate_addr", gate_addr));
    create_data.insert(make_pair("client_addr", client_addr));
    create_entity(entity_class_name.as_string(), gen_uuid(), create_data);
}

void call_base_entity(const GValue& from_client, const GValue& entity_uuid, const GValue& rpc_name, const GValue& rpc_params) {

    INFO_LOG("call_base_entity %s - %s\n", entity_uuid.as_string().c_str(), rpc_name.as_string().c_str());

    auto iter = g_entities.find(entity_uuid.as_string());
    if (iter == g_entities.end()) {
        ERROR_LOG("call_base_entity entity.%s not exist\n", entity_uuid.as_string().c_str());
        return;
    }

    iter->second->rpc_call(from_client.as_bool(), rpc_name.as_string(), rpc_params.as_array());
}

void call_cell_entity(const GValue& from_client, const GValue& entity_uuid, const GValue& rpc_name, const GValue& rpc_params) {

    INFO_LOG("call_cell_entity %s - %s\n", entity_uuid.as_string().c_str(), rpc_name.as_string().c_str());

    auto iter = g_entities.find(entity_uuid.as_string());
    if (iter == g_entities.end()) {
        ERROR_LOG("call_cell_entity entity.%s not exist\n", entity_uuid.as_string().c_str());
        return;
    }

    iter->second->rpc_call(from_client.as_bool(), rpc_name.as_string(), rpc_params.as_array());
}

void rpc_handle_regist() {

    RPC_REGISTER(connect_from_client);
    RPC_REGISTER(disconnect_from_client, GString());
    RPC_REGISTER(regist_from_gate);

    RPC_REGISTER(create_base_entity, GString(), GString(), GString());
    RPC_REGISTER(create_cell_entity, GString(), GString(), GString(), GString(), GString());

    RPC_REGISTER(call_base_entity, bool(), GString(), GString(), GArray());
    RPC_REGISTER(call_cell_entity, bool(), GString(), GString(), GArray());
}
