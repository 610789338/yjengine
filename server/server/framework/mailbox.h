#pragma once

#include <stdint.h>

#include "engine/gvalue.h"
#include "engine/log.h"
#include "engine/rpc_manager.h"

#include "game_rpc.h"


class MailBox {

public:
    MailBox() = delete;
    MailBox(int64_t entity_id, const GString& ip, int16_t port) 
        : m_entity_id(entity_id), m_ip(ip), m_port(port) {}
    ~MailBox() {}

protected:
    int64_t     m_entity_id;
    GString     m_ip;
    int16_t     m_port;
};

class CellMailBox : public MailBox {

public:
    CellMailBox() = delete;
    CellMailBox(int64_t entity_id, const GString& ip, int16_t port) : MailBox(entity_id, ip, port) {}
    ~CellMailBox() {}

public:
    template<class... T>
    void call(GString rpc_name, T... args) {
        auto gate = g_gates.find(IPPORT_STRING(m_ip, m_port));
        if (gate == g_gates.end()) {
            ERROR_LOG("client mailbox address() error\n", IPPORT_STRING(m_ip, m_port).c_str());
            return;
        }

        REMOTE_RPC_CALL(gate->second->Session, "call_client", rpc_name, args...);
    }
};

class ClientMailBox : public MailBox {

public:
    ClientMailBox() = delete;
    ClientMailBox(int64_t entity_id, const GString& ip, int16_t port) : MailBox(entity_id, ip, port) {}
    ~ClientMailBox() {}

public:
    template<class... T>
    void call(GString rpc_name, T... args) {
        auto remote = g_remote_mgr.get_remote(IPPORT_STRING(m_ip, m_port));
        if (nullptr == remote) {
            ERROR_LOG("cell mailbox address() error\n", IPPORT_STRING(m_ip, m_port).c_str());
            return;
        }

        REMOTE_RPC_CALL(remote, "call_cell", rpc_name, args...);
    }
};
