#pragma once

#include <stdint.h>
#include <queue>

#include "gvalue.h"
#include "log.h"
#include "rpc_manager.h"
#include "utils.h"
#include "remote_manager.h"

class Entity;

extern RpcManagerBase* get_entity_rpc_mgr(Entity* entity);
extern bool g_is_server;


class BaseMailBox : public MailBox {

public:
    BaseMailBox() {}
    ~BaseMailBox() {}

    template<class... T>
    void call(const GString& rpc_name, T... args) {
        auto inner_rpc = GEN_INNER_RPC(rpc_name, args...);
        if (g_is_server) {
            if (m_session_cache == nullptr || !g_session_mgr.is_valid_session(m_session_cache)) {
                m_session_cache = g_session_mgr.get_fixed_session(m_addr);
            }
            auto gate = m_session_cache;
            if (nullptr == gate) {
                WARN_LOG("gate empty\n");
                return;
            }
            REMOTE_RPC_CALL(gate, "call_base_entity", m_addr, m_entity_uuid, inner_rpc);
        }
        else {
            if (m_remote_cache == nullptr || !g_remote_mgr.is_valid_remote(m_remote_cache)) {
                m_remote_cache = g_remote_mgr.get_fixed_remote(m_addr);
            }
            auto gate = m_remote_cache;
            if (nullptr == gate) {
                WARN_LOG("gate empty\n");
                return;
            }
            REMOTE_RPC_CALL(gate, "call_base_entity", m_addr, m_entity_uuid, inner_rpc);
        }
    }
};

class CellMailBox : public MailBox {

public:
    CellMailBox() {}
    ~CellMailBox() {}

public:
    template<class... T>
    void call(const GString& rpc_name, T... args) {
        auto inner_rpc = GEN_INNER_RPC(rpc_name, args...);

        if (is_rpc_cache) {
            m_inner_rpc_cache.push(inner_rpc);
        }
        else {
            send_rpc(inner_rpc);
        }
    }

    void send_rpc(InnerRpcPtr_Encode inner_rpc) {
        if (g_is_server) {
            if (m_session_cache == nullptr || !g_session_mgr.is_valid_session(m_session_cache)) {
                m_session_cache = g_session_mgr.get_fixed_session(m_addr);
            }

            auto gate = m_session_cache;
            if (nullptr == gate) {
                WARN_LOG("gate empty\n");
                return;
            }

            REMOTE_RPC_CALL(gate, "call_cell_entity", m_addr, m_entity_uuid, inner_rpc);
        }
        else {
            if (m_remote_cache == nullptr || !g_remote_mgr.is_valid_remote(m_remote_cache)) {
                m_remote_cache = g_remote_mgr.get_fixed_remote(m_addr);
            }

            auto gate = m_remote_cache;
            if (nullptr == gate) {
                WARN_LOG("gate empty\n");
                return;
            }

            REMOTE_RPC_CALL(gate, "call_cell_entity", m_addr, m_entity_uuid, inner_rpc);
        }
    }

    void start_cache_rpc() {
        is_rpc_cache = true;
    }

    void stop_cache_rpc() {
        // send cached rpc
        while (!m_inner_rpc_cache.empty()) {
            auto inner_rpc = m_inner_rpc_cache.front();
            send_rpc(inner_rpc);
            //delete inner_rpc;
            m_inner_rpc_cache.pop();
        }
        is_rpc_cache = false;
    }

    void clear_cache_rpc() {
        while (!m_inner_rpc_cache.empty()) {
            auto inner_rpc = m_inner_rpc_cache.front();
            //delete inner_rpc;
            m_inner_rpc_cache.pop();
        }
        is_rpc_cache = false;
    }

private:
    bool is_rpc_cache = false;
    queue<InnerRpcPtr_Encode> m_inner_rpc_cache;
};

class ClientMailBox : public MailBox {

public:
    ClientMailBox() {}
    ~ClientMailBox() {}

public:
    template<class... T>
    void call(const GString& rpc_name, T... args) {

        //const Encoder& encoder = g_rpc_manager.rpc_encode(rpc_name, args...);
        //GBin inner_rpc(encoder.get_buf(), encoder.get_offset());

        auto inner_rpc = GEN_INNER_RPC(rpc_name, args...);

        auto gate = g_session_mgr.get_gate(m_gate_addr);
        if (nullptr == gate) {
            ERROR_LOG("client mailbox address(%s) error\n", m_gate_addr.c_str());
            return;
        }

        REMOTE_RPC_CALL(gate, "call_client_entity", m_addr, m_entity_uuid, inner_rpc);
    }

    void set_gate_addr(const GString& gate_addr) { 
        m_gate_addr = gate_addr; 
    }
    GString& get_gate_addr() { return m_gate_addr; }

    GString m_gate_addr;
};
