#pragma once

#include <stdint.h>

#include "gvalue.h"
#include "log.h"
#include "rpc_manager.h"
#include "utils.h"
#include "remote_manager.h"

class Entity;

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

        Encoder encoder;
        g_rpc_manager.rpc_params_encode(encoder, args...);
        GBin rpc_params(encoder.get_buf(), encoder.get_offset());

        if (m_side == "server") {
            auto gate = g_session_mgr.get_rand_session();
            if (nullptr == gate) {
                WARN_LOG("gate empty\n");
                return;
            }

            REMOTE_RPC_CALL(gate, "call_base_entity", m_addr, m_entity_uuid, rpc_name, rpc_params);
        } 
        else if (m_side == "client") {
            auto gate = g_remote_mgr.get_rand_remote();
            if (nullptr == gate) {
                WARN_LOG("gate empty\n");
                return;
            }

            REMOTE_RPC_CALL(gate, "call_base_entity", m_addr, m_entity_uuid, rpc_name, rpc_params);
        }
    }
};

class CellMailBox : public MailBox {

public:
    CellMailBox() {}
    ~CellMailBox() {}

public:
    template<class... T>
    void call(GString rpc_name, T... args) {

        Encoder encoder;
        g_rpc_manager.rpc_params_encode(encoder, args...);
        GBin rpc_params(encoder.get_buf(), encoder.get_offset());

        if (m_side == "server") {
            auto gate = g_session_mgr.get_rand_session();
            if (nullptr == gate) {
                WARN_LOG("gate empty\n");
                return;
            }

            REMOTE_RPC_CALL(gate, "call_cell_entity", m_addr, m_entity_uuid, rpc_name, rpc_params);
        }
        else if (m_side == "client") {
            auto gate = g_remote_mgr.get_rand_remote();
            if (nullptr == gate) {
                WARN_LOG("gate empty\n");
                return;
            }

            REMOTE_RPC_CALL(gate, "call_cell_entity", m_addr, m_entity_uuid, rpc_name, rpc_params);
        }
    }
};

class ClientMailBox : public MailBox {

public:
    ClientMailBox() {}
    ~ClientMailBox() {}

public:
    template<class... T>
    void call(GString rpc_name, T... args) {

        Encoder encoder;
        g_rpc_manager.rpc_params_encode(encoder, args...);
        GBin rpc_params(encoder.get_buf(), encoder.get_offset());

        auto gate = g_session_mgr.get_session(m_gate_addr);
        if (nullptr == gate) {
            ERROR_LOG("client mailbox address(%s) error\n", m_gate_addr.c_str());
            return;
        }

        REMOTE_RPC_CALL(gate, "call_client_entity", m_addr, m_entity_uuid, rpc_name, rpc_params);
    }

    void set_gate_addr(const GString& gate_addr) { m_gate_addr = gate_addr; }

    GString m_gate_addr;
};
