#pragma once

#include <stdint.h>

#include "engine/gvalue.h"
#include "engine/log.h"
#include "engine/rpc_manager.h"
#include "engine/boost_asio.h"
#include "engine/utils.h"


class MailBox {

public:
    MailBox() = delete;
    MailBox(int64_t entity_id, const GString& ip, int16_t port) 
        : m_entityid(entity_id), m_ip(ip), m_port(port) {}
    ~MailBox() {}

protected:
    int64_t     m_entityid;
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
        auto gate = g_session_mgr.get_rand_session();
        if (nullptr == gate) {
            WARN_LOG("gate empty\n");
            return;
        }

        REMOTE_RPC_CALL(gate, "call_cell", m_ip, m_port, m_entityid, rpc_name, args...);
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
        auto gate = g_session_mgr.get_session(IPPORT_STRING(m_ip, m_port));
        if (nullptr == gate) {
            ERROR_LOG("client mailbox address(%s) error\n", IPPORT_STRING(m_ip, m_port).c_str());
            return;
        }

        REMOTE_RPC_CALL(gate, "call_client", m_entityid, rpc_name, args...);
    }
};
