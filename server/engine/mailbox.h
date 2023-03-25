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

class MailBox {

public:
    MailBox();
    ~MailBox();

    void set_entity_and_addr(const GString& entity_uuid, const GString& addr) { 
        m_entity_uuid = entity_uuid; 
        m_addr = addr;
    }
    GString& get_entity_uuid() { return m_entity_uuid; }
    GString& get_addr() { return m_addr; }
    void set_side(const GString& side) { m_side = side; }
    void set_owner(Entity* owner) { m_owner = owner; }

protected:
    GString     m_entity_uuid = "";
    GString     m_addr = "";
    GString     m_side = "server";
    Entity*     m_owner;
};

class BaseMailBox : public MailBox {

public:
    BaseMailBox() {}
    ~BaseMailBox() {}

public:
    template<class... T>
    void call(GString rpc_name, T... args) {

        //const Encoder& encoder = g_rpc_manager.rpc_encode(rpc_name, args...);
        //GBin inner_rpc(encoder.get_buf(), encoder.get_offset());

        auto inner_rpc = GEN_INNER_RPC(rpc_name, args...);

        if (m_side == "server") {
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
        else if (m_side == "client") {
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

private:
    shared_ptr<Session> m_session_cache = nullptr;
    shared_ptr<Remote> m_remote_cache = nullptr;
};

class CellMailBox : public MailBox {

public:
    CellMailBox() {}
    ~CellMailBox() {}

public:
    template<class... T>
    void call(GString rpc_name, T... args) {

        //const Encoder& encoder = g_rpc_manager.rpc_encode(rpc_name, args...);
        //GBin* inner_rpc = new GBin(encoder.get_buf(), encoder.get_offset());

        auto inner_rpc = GEN_INNER_RPC(rpc_name, args...);

        if (is_rpc_cache) {
            m_inner_rpc_cache.push(inner_rpc);
        }
        else {
            send_rpc(inner_rpc);
            //delete inner_rpc;
        }
    }

    void send_rpc(InnerRpcPtr inner_rpc) {
        if (m_side == "server") {
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
        else if (m_side == "client") {
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
    shared_ptr<Session> m_session_cache = nullptr;
    shared_ptr<Remote> m_remote_cache = nullptr;

    bool is_rpc_cache = false;
    queue<InnerRpcPtr> m_inner_rpc_cache;
};

class ClientMailBox : public MailBox {

public:
    ClientMailBox() {}
    ~ClientMailBox() {}

public:
    template<class... T>
    void call(GString rpc_name, T... args) {

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
