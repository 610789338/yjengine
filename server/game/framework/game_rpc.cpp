#include <iostream>

#include "engine/engine.h"


using namespace std;

extern shared_ptr<RpcImp> g_cur_imp;

// ------------------------------  from gate ------------------------------ 

void connect_from_client() {
    auto session = g_cur_imp->get_session();
    g_session_mgr.on_session_connected(session);
    INFO_LOG("on_connect_from_gate %s\n", session->get_remote_addr().c_str());
}

void disconnect_from_client(const GString& session_addr) {
    g_session_mgr.on_session_disconnected(session_addr);
    INFO_LOG("on_disconnect_from_gate %s\n", session_addr.c_str());
}

void regist_from_gate(const GString& gate_listen_addr) {

    auto session = g_cur_imp->get_session();
    session->set_verify(true);

    g_session_mgr.add_gate(session->get_remote_addr(), gate_listen_addr);

    INFO_LOG("regist from gate@%s\n", session->get_remote_addr().c_str());

    REMOTE_RPC_CALL(session, "regist_ack_from_game", session->get_local_addr(), true, *get_local_entity_rpc_names());

    REMOTE_RPC_CALL(session, "get_client_entity_rpc_names_from_game", session->get_local_addr());
}

void get_client_entity_rpc_names_ack(const GArray& client_rpc_names) {

    if (client_rpc_names.empty()) {
        auto session = g_session_mgr.get_rand_session();
        REMOTE_RPC_CALL(session, "get_client_entity_rpc_names_from_game", session->get_local_addr());
        return;
    }

    g_entity_rpc_name_l2s.clear();
    g_entity_rpc_name_s2l.clear();

    for (size_t i = 0; i < get_local_entity_rpc_names()->size(); ++i) {
        g_entity_rpc_name_l2s.insert(make_pair((*get_local_entity_rpc_names())[i].as_string(), (uint16_t)g_entity_rpc_name_l2s.size()));
    }

    auto& client_rpc_name_array = client_rpc_names;
    for (size_t i = 0; i < client_rpc_name_array.size(); ++i) {
        g_entity_rpc_name_l2s.insert(make_pair(client_rpc_name_array[i].as_string(), (uint16_t)g_entity_rpc_name_l2s.size()));
    }

    for (auto iter = g_entity_rpc_name_l2s.begin(); iter != g_entity_rpc_name_l2s.end(); ++iter) {
        g_entity_rpc_name_s2l.insert(make_pair(iter->second, iter->first));
    }
}

void create_base_entity_new(const GString& entity_class_name, const GString& client_addr, const GString& gate_addr) {
    Entity* entity = create_local_base_entity(entity_class_name, gen_uuid());
    GDict create_data;
    create_data.insert(make_pair("cell_bin", GBin(nullptr, 0)));
    create_data.insert(make_pair("client_addr", client_addr));
    create_data.insert(make_pair("gate_addr", gate_addr));
    entity->on_create(create_data);;
}

void create_base_entity_fromdb(const GString& entity_class_name, const GString& client_addr, const GString& gate_addr, const GString& entity_uuid) {

    // load from db - TODO: move to child thread
    GString db_file_name = "./db/" + entity_uuid + ".bin";
    auto fp = fopen(db_file_name.c_str(), "rb");
    if (fp == nullptr) {
        ERROR_LOG("load - open db file %s error\n", db_file_name.c_str());
        return;
    }

    char buf[65535] = { 0 };
    fread(buf, sizeof(buf), 1, fp);

    Decoder db(buf, sizeof(buf));
    db.read_int16(); // skip pkg len offset
    
    const GBin& base_bin = db.read_bin(); // base_bin
    Decoder base_db(base_bin.buf, base_bin.size);
    base_db.read_int16(); // skip pkg len offset

    Entity* entity = create_local_base_entity(entity_class_name, entity_uuid);
    entity->propertys_unserialize(base_db);

    const GBin& cell_bin = db.read_bin(); // cell_bin

    GDict create_data;
    create_data.insert(make_pair("cell_bin", cell_bin));
    create_data.insert(make_pair("client_addr", client_addr));
    create_data.insert(make_pair("gate_addr", gate_addr));
    entity->on_create(create_data);
}

void entity_reconnect_fromclient(Entity* entity, const GString& client_addr, const GString& gate_addr) {
    entity->on_reconnect_fromclient(client_addr, gate_addr);
}

void create_base_entity(const GString& entity_class_name, const GString& client_addr, const GString& gate_addr, const GString& entity_uuid) {
    if (entity_uuid.empty()) {
        create_base_entity_new(entity_class_name, client_addr, gate_addr);
    }
    else {
        auto iter = g_base_entities.find(entity_uuid);
        if (iter == g_base_entities.end()) {
            create_base_entity_fromdb(entity_class_name, client_addr, gate_addr, entity_uuid);
        }
        else {
            if (iter->second->is_ready) {
                entity_reconnect_fromclient(iter->second, client_addr, gate_addr);
            }
        }
    }
}

void create_cell_entity(const GString& entity_class_name, const GString& entity_uuid, const GString& base_addr, const GString& gate_addr, const GString& client_addr, const GBin& bin) {
    Entity* entity = create_local_cell_entity(entity_class_name, entity_uuid);

    const GBin& cell_bin = bin;
    if (cell_bin.size != 0) {
        Decoder cell_db(cell_bin.buf, cell_bin.size);
        cell_db.read_int16(); // skip pk len offset
        entity->propertys_unserialize(cell_db);
    }

    GDict create_data;
    create_data.insert(make_pair("base_addr", base_addr));
    create_data.insert(make_pair("gate_addr", gate_addr));
    create_data.insert(make_pair("client_addr", client_addr));
    entity->on_create(create_data);
}

void call_base_entity(bool from_client, const GString& entity_uuid, const GBin& inner_rpc) {

    auto iter = g_base_entities.find(entity_uuid);
    if (iter == g_base_entities.end()) {
        ERROR_LOG("call_base_entity entity.%s not exist\n", entity_uuid.c_str());
        return;
    }

    Entity* entity = iter->second;

    Decoder decoder(inner_rpc.buf, inner_rpc.size);
    auto const pkg_len = decoder.read_uint16();
    auto const rpc_imp = entity->rpc_mgr->rpc_decode(inner_rpc.buf + decoder.get_offset(), pkg_len);

    DEBUG_LOG("call_base_entity %s - %s\n", entity_uuid.c_str(), rpc_imp->get_rpc_name().c_str());
    entity->rpc_call(from_client, rpc_imp->get_rpc_name(), rpc_imp->get_rpc_method());
}

void call_cell_entity(bool from_client, const GString& entity_uuid, const GBin& inner_rpc) {

    auto iter = g_cell_entities.find(entity_uuid);
    if (iter == g_cell_entities.end()) {
        ERROR_LOG("call_cell_entity entity.%s not exist\n", entity_uuid.c_str());
        return;
    }

    Entity* entity = iter->second;

    Decoder decoder(inner_rpc.buf, inner_rpc.size);
    auto const pkg_len = decoder.read_uint16();
    auto const rpc_imp = entity->rpc_mgr->rpc_decode(inner_rpc.buf + decoder.get_offset(), pkg_len);

    DEBUG_LOG("call_cell_entity %s - %s\n", entity_uuid.c_str(), rpc_imp->get_rpc_name().c_str());
    entity->rpc_call(from_client, rpc_imp->get_rpc_name(), rpc_imp->get_rpc_method());
}

extern void migrate_rpc_handle_regist();
extern void heartbeat_from_gate();

void rpc_handle_regist() {

    RPC_REGISTER(connect_from_client);
    RPC_REGISTER(disconnect_from_client);
    RPC_REGISTER(regist_from_gate);

    RPC_REGISTER(get_client_entity_rpc_names_ack);

    RPC_REGISTER(create_base_entity);
    RPC_REGISTER(create_cell_entity);

    RPC_REGISTER(call_base_entity);
    RPC_REGISTER(call_cell_entity);

    RPC_REGISTER(heartbeat_from_gate);

    migrate_rpc_handle_regist();
}
