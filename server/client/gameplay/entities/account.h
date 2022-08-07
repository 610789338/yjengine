#pragma once

#include "engine/engine.h"



class ClientAccount : public ClientEntity {

    GENERATE_ENTITY_INNER(ClientAccount)

public:
    ClientAccount() {}
    ~ClientAccount() {}

    static void regist_components() {}
    static void rpc_method_define() {
        RPC_METHOD(RpcType::CLIENT, msg_from_base, GString());
        RPC_METHOD(RpcType::CLIENT, msg_from_cell, GString());
    }
    static void property_define() {}

    void msg_from_base(const GValue& msg) { INFO_LOG("[client] msg.%s from base\n", msg.as_string().c_str()); }
    void msg_from_cell(const GValue& msg) { INFO_LOG("[client] msg.%s from cell\n", msg.as_string().c_str()); }

    void on_ready() {
        INFO_LOG("ClientAccount on_ready\n");

        base.call("msg_from_client", "hello, i am client");
        cell.call("msg_from_client", "hello, i am client");
    }
};

GENERATE_ENTITY_OUT(ClientAccount)
