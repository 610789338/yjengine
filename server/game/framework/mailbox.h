#pragma once

#include <stdint.h>

#include "engine/gvalue.h"
#include "engine/log.h"
#include "engine/rpc_manager.h"
#include "engine/boost_asio.h"
#include "engine/utils.h"


class MailBox {

public:
    MailBox() {}
    ~MailBox() {}

    void set_entity_and_addr(const GString& entity_uuid, const GString& addr) { m_entity_uuid = entity_uuid; m_addr = addr; }

protected:
    GString     m_entity_uuid = "";
    GString     m_addr = "";
};

class BaseMailBox : public MailBox {

public:
    BaseMailBox() {}
    ~BaseMailBox() {}

public:
    template<class... T>
    void call(GString rpc_name, T... args) {
        auto gate = g_session_mgr.get_rand_session();
        if (nullptr == gate) {
            WARN_LOG("gate empty\n");
            return;
        }

        GArray rpc_params;
        args2array(rpc_params, args...);

        REMOTE_RPC_CALL(gate, "call_base_entity", m_addr, m_entity_uuid, rpc_name, rpc_params);
    }
};

class CellMailBox : public MailBox {

public:
    CellMailBox() {}
    ~CellMailBox() {}

public:
    template<class... T>
    void call(GString rpc_name, T... args) {
        auto gate = g_session_mgr.get_rand_session();
        if (nullptr == gate) {
            WARN_LOG("gate empty\n");
            return;
        }

        GArray rpc_params;
        args2array(rpc_params, args...);

        REMOTE_RPC_CALL(gate, "call_cell_entity", m_addr, m_entity_uuid, rpc_name, rpc_params);
    }
};

class ClientMailBox : public MailBox {

public:
    ClientMailBox() {}
    ~ClientMailBox() {}

public:
    template<class... T>
    void call(GString rpc_name, T... args) {
        auto gate = g_session_mgr.get_session(m_addr);
        if (nullptr == gate) {
            ERROR_LOG("client mailbox address(%s) error\n", m_addr.c_str());
            return;
        }

        GArray rpc_params;
        args2array(rpc_params, args...);

        REMOTE_RPC_CALL(gate, "call_client_entity", m_entity_uuid, rpc_name, rpc_params);
    }

    void set_gate_addr(const GString& gate_addr) { m_gate_addr = gate_addr; }

    GString m_gate_addr;
};
