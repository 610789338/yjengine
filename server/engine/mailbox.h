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

class MailBoxProxy : public MailBox {
public:
    MailBoxProxy() {}
    ~MailBoxProxy() {}

    //template<class... T>
    //void local_call(const GString& rpc_name, const T&... args) {
    //    const Encoder& encoder = g_rpc_manager.rpc_encode(rpc_name, args...);
    //    GBin inner_rpc_bin(encoder.get_buf(), encoder.get_offset());
    //    Decoder inner_decoder(inner_rpc_bin.buf, inner_rpc_bin.size);
    //    auto const pkg_len = inner_decoder.read_uint16();

    //    RpcManagerBase* rpcmgr = nullptr;
    //    if (is_base()) {
    //        rpcmgr = thread_safe_get_base_entity_rpcmgr(get_entity_uuid());
    //        if (!rpcmgr) {
    //            ERROR_LOG("call_base_entity entity.%s not exist\n", entity_uuid.c_str());
    //            return;
    //        }
    //    }
    //    else {
    //        rpcmgr = thread_safe_get_cell_entity_rpcmgr(get_entity_uuid());
    //        if (!rpcmgr) {
    //            ERROR_LOG("call_cell_entity entity.%s not exist\n", entity_uuid.c_str());
    //            return;
    //        }
    //    }
    //    else {
    //        ASSERT_LOG("error special rpc.%s\n", rpc_name.c_str());
    //    }

    //    auto rpc_imp = rpcmgr->rpc_decode(inner_rpc_bin.buf + inner_decoder.get_offset(), pkg_len);

    //    if (g_is_server) {
    //        if (m_session_cache == nullptr || !g_session_mgr.is_valid_session(m_session_cache)) {
    //            m_session_cache = g_session_mgr.get_fixed_session(m_addr);
    //        }
    //        auto gate = m_session_cache;
    //        if (nullptr == gate) {
    //            WARN_LOG("gate empty\n");
    //            return;
    //        }
    //        if (is_base()) {
    //            LOCAL_RPC_CALL(gate, "call_base_entity", false, m_entity_uuid, rpc_imp);
    //        }
    //        else {
    //            LOCAL_RPC_CALL(gate, "call_cell_entity", false, m_entity_uuid, rpc_imp);
    //        }
    //    }
    //    else {
    //        if (m_remote_cache == nullptr || !g_remote_mgr.is_valid_remote(m_remote_cache)) {
    //            m_remote_cache = g_remote_mgr.get_fixed_remote(m_addr);
    //        }
    //        auto gate = m_remote_cache;
    //        if (nullptr == gate) {
    //            WARN_LOG("gate empty\n");
    //            return;
    //        }
    //        if (is_base()) {
    //            LOCAL_RPC_CALL(gate, "call_base_entity", false, m_entity_uuid, rpc_imp);
    //        }
    //        else {
    //            LOCAL_RPC_CALL(gate, "call_cell_entity", false, m_entity_uuid, rpc_imp);
    //        }
    //    }
    //}

    template<class... T>
    void call(const GString& rpc_name, const T&... args) {
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
            if (is_base()) {
                if (is_b2c()) {
                    REMOTE_RPC_CALL(gate, "call_base_entity_b2c", m_addr, m_entity_uuid, inner_rpc);
                }
                else {
                    REMOTE_RPC_CALL(gate, "call_base_entity", m_addr, m_entity_uuid, inner_rpc);
                }
            }
            else {
                REMOTE_RPC_CALL(gate, "call_cell_entity", m_addr, m_entity_uuid, inner_rpc);
            }
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
            if (is_base()) {
                REMOTE_RPC_CALL(gate, "call_base_entity", m_addr, m_entity_uuid, inner_rpc);
            }
            else {
                REMOTE_RPC_CALL(gate, "call_cell_entity", m_addr, m_entity_uuid, inner_rpc);
            }
        }
    }
};

class BaseMailBox : public MailBox {

public:
    BaseMailBox() { set_base(); }
    ~BaseMailBox() {}
    template<class... T>
    void call(const GString& rpc_name, const T&... args) {
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

    struct _QueueElement {
        _QueueElement(bool _is_pure_bin, void* _p)
            : is_pure_bin(_is_pure_bin)
            , p(_p)
        {}
        bool is_pure_bin = false;
        void* p = nullptr;
    };

    void call(const GBin& inner_rpc) {
        _QueueElement ele(true, (void*)(&inner_rpc));
        if (is_rpc_cache) {
            m_inner_rpc_cache.push(ele);
        }
        else {
            send_rpc(ele);
        }
    }

    template<class... T>
    void call(const GString& rpc_name, const T&... args) {
        auto inner_rpc = GEN_INNER_RPC(rpc_name, args...);
        _QueueElement ele(false, (void*)(&inner_rpc));

        if (is_rpc_cache) {
            m_inner_rpc_cache.push(ele);
        }
        else {
            send_rpc(ele);
        }
    }

    void send_rpc(_QueueElement& queue_ele) {
        if (g_is_server) {
            if (m_session_cache == nullptr || !g_session_mgr.is_valid_session(m_session_cache)) {
                m_session_cache = g_session_mgr.get_fixed_session(m_addr);
            }

            auto gate = m_session_cache;
            if (nullptr == gate) {
                WARN_LOG("gate empty\n");
                return;
            }

            if (queue_ele.is_pure_bin) {
                GBin inner_rpc = std::move(*(GBin*)(queue_ele.p));
                REMOTE_RPC_CALL(gate, "call_cell_entity", m_addr, m_entity_uuid, inner_rpc);
            }
            else {
                InnerRpcPtr_Encode inner_rpc = *(InnerRpcPtr_Encode*)(queue_ele.p);
                REMOTE_RPC_CALL(gate, "call_cell_entity", m_addr, m_entity_uuid, inner_rpc);
            }
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

            if (queue_ele.is_pure_bin) {
                GBin inner_rpc = std::move(*(GBin*)(queue_ele.p));
                REMOTE_RPC_CALL(gate, "call_cell_entity", m_addr, m_entity_uuid, inner_rpc);
            }
            else {
                InnerRpcPtr_Encode inner_rpc = *(InnerRpcPtr_Encode*)(queue_ele.p);
                REMOTE_RPC_CALL(gate, "call_cell_entity", m_addr, m_entity_uuid, inner_rpc);
            }

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
    queue<_QueueElement> m_inner_rpc_cache;
};

class ClientMailBox : public MailBox {

public:
    ClientMailBox() {}
    ~ClientMailBox() {}

public:
    template<class... T>
    void call(const GString& rpc_name, const T&... args) {
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
