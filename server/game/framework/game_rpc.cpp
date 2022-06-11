#include <iostream>
#include <map>

#include "engine/rpc_manager.h"
#include "engine/boost_asio.h"

#include "entity.h"

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

void register_from_gate() {
    
    auto session = g_cur_imp->get_session();
    session->set_verify(true);
    INFO_LOG("register from gate@%s\n", session->get_remote_addr().c_str());
    
    REMOTE_RPC_CALL(session, "register_ack_from_game", session->get_local_addr(), true);
}

void create_entity_from_client(GValue entity_type, GValue client_addr) {

    auto create_data = GDict();
    create_data.insert(make_pair("client_addr", client_addr));
    auto entity = create_entity(entity_type.as_string(), create_data);
}

void rpc_handle_register() {

    RPC_REGISTER(connect_from_client);
    RPC_REGISTER(disconnect_from_client, GString());
    RPC_REGISTER(register_from_gate);

    RPC_REGISTER(create_entity_from_client, GString(), GString());
}
