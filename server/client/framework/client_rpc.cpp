#include <iostream>

#include "engine/gvalue.h"
#include "engine/ini.h"

#include "client_instance.h"

using namespace std;

extern shared_ptr<RpcImp> g_cur_imp;

// ------------------------------  to && from gate ------------------------------ 

void on_remote_connected() {
    auto remote = g_cur_imp->get_remote();
    g_remote_mgr.on_remote_connected(remote);
    INFO_LOG("on gate connected %s\n", remote->get_remote_addr().c_str());

    REMOTE_RPC_CALL(remote, "regist_from_client", ini_get_string("Identity", "md5"), *get_local_entity_rpc_names());
}

void on_remote_disconnected(const GString& remote_addr) {
    g_remote_mgr.on_remote_disconnected(remote_addr);
    g_client_instance->on_gate_disappear(remote_addr);
    INFO_LOG("on gate disconnected %s\n", remote_addr.c_str());
}

void regist_ack_from_gate(const GString& gate_addr, bool result) {
    if (result) {
        INFO_LOG("regist ack from gate@%s\n", gate_addr.c_str());
    }

    auto remote = g_remote_mgr.get_remote(gate_addr);
    REMOTE_RPC_CALL(remote, "get_game_entity_rpc_names_from_client", remote->get_local_addr());
}

void get_game_entity_rpc_names_ack(const GArray& game_entity_rpc_names) {

    auto remote = g_remote_mgr.get_rand_remote();
    if (game_entity_rpc_names.empty()) {
        REMOTE_RPC_CALL(remote, "get_game_entity_rpc_names_from_client", remote->get_local_addr());
        return;
    }

    {
        unique_lock<boost::shared_mutex> lock(g_rpc_name_turn_mutex);

        all_rpc_names_l2s.clear();
        all_rpc_names_s2l.clear();

        auto& game_entity_rpc_name_array = game_entity_rpc_names;
        for (size_t i = 0; i < game_entity_rpc_name_array.size(); ++i) {
            all_rpc_names_l2s.insert(make_pair(game_entity_rpc_name_array[i].as_string(), (uint16_t)(g_rpc_names_l2s.size() + all_rpc_names_l2s.size())));
        }

        for (size_t i = 0; i < get_local_entity_rpc_names()->size(); ++i) {
            all_rpc_names_l2s.insert(make_pair((*get_local_entity_rpc_names())[i].as_string(), (uint16_t)(g_rpc_names_l2s.size() + all_rpc_names_l2s.size())));
        }

        for (auto iter = all_rpc_names_l2s.begin(); iter != all_rpc_names_l2s.end(); ++iter) {
            all_rpc_names_s2l.insert(make_pair(iter->second, iter->first));
        }
    }

    if (g_client_instance->get_should_call_create()) {
        const GString& base_uuid = ini_get_string("Entity", "uuid", "empty");
        if (base_uuid == "empty") {
            REMOTE_RPC_CALL(remote, "create_base_entity", "Account", "");
        }
        else {
            REMOTE_RPC_CALL(remote, "create_base_entity", "Account", base_uuid);
        }
        g_client_instance->set_should_call_create(false);
    }
}

void create_client_entity(const GString& entity_class_name, const GString& entity_uuid, const GString& base_addr, const GString& cell_addr) {
    Entity* entity = create_local_client_entity(entity_class_name, entity_uuid);

    GDict create_data;
    create_data.insert(make_pair("base_addr", base_addr));
    create_data.insert(make_pair("cell_addr", cell_addr));
    entity->on_create(create_data);

    if (entity_class_name == "ClientAccount" || entity_class_name == "ClientPlayer") {
        g_client_instance->set_controller(entity);
    }
}

void create_client_entity_onreconnect(const GString& entity_class_name, const GString& entity_uuid, const GString& base_addr, const GString& cell_addr) {
    Entity* entity = nullptr;

    auto iter = g_client_entities.find(entity_uuid);
    if (iter == g_client_entities.end()) {
        entity = create_local_client_entity(entity_class_name, entity_uuid);
    }
    else {
        entity = iter->second;
    }

    GDict create_data;
    create_data.insert(make_pair("base_addr", base_addr));
    create_data.insert(make_pair("cell_addr", cell_addr));
    entity->on_reconnect_success(create_data);

    if (entity_class_name == "ClientAccount" || entity_class_name == "ClientPlayer") {
        g_client_instance->set_controller(entity);
    }
}

//void call_client_entity(const GString& entity_uuid, const GBin& inner_rpc) {
//
//    auto iter = g_client_entities.find(entity_uuid);
//    if (iter == g_client_entities.end()) {
//        ERROR_LOG("call_client_entity entity.%s not exist\n", entity_uuid.c_str());
//        return;
//    }
//
//    Entity* entity = iter->second;
//
//    Decoder decoder(inner_rpc.buf, inner_rpc.size);
//    auto const pkg_len = decoder.read_uint16();
//    auto const rpc_imp = entity->rpc_mgr->rpc_decode(inner_rpc.buf + decoder.get_offset(), pkg_len);
//
//    DEBUG_LOG("call_client_entity %s - %s\n", entity_uuid.c_str(), rpc_imp->get_rpc_name().c_str());
//    entity->rpc_call(false, rpc_imp->get_rpc_name(), rpc_imp->get_rpc_method());
//}

void call_client_entity(bool placeholder, const GString& entity_uuid, InnerRpcPtr_Decode rpc_imp) {

    auto iter = g_client_entities.find(entity_uuid);
    if (iter == g_client_entities.end()) {
        ERROR_LOG("call_client_entity entity.%s not exist\n", entity_uuid.c_str());
        return;
    }

    Entity* entity = iter->second;
    DEBUG_LOG("call_client_entity %s - %s\n", entity_uuid.c_str(), rpc_imp->get_rpc_name().c_str());
    entity->rpc_call(placeholder, rpc_imp->get_rpc_name(), rpc_imp->get_rpc_method());
}


void rpc_handle_regist() {
    RPC_REGISTER(on_remote_connected);
    RPC_REGISTER(on_remote_disconnected);
    RPC_REGISTER(regist_ack_from_gate);
    RPC_REGISTER(create_client_entity);
    RPC_REGISTER(create_client_entity_onreconnect);

    RPC_REGISTER(get_game_entity_rpc_names_ack);

    RPC_REGISTER(call_client_entity);
}
