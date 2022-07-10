#pragma once

#include "engine/engine.h"



class BaseAccount : public BaseEntityWithCellAndClient {
    
    GENERATE_ENTITY_INNER(BaseAccount)

public:
    BaseAccount() : BaseEntityWithCellAndClient("CellAccount", "ClientAccount") {}
    ~BaseAccount() {}

    static void rpc_method_define();
    static void property_define();

    // called by engine
    void on_ready() {
        INFO_LOG("BaseAccount on_ready\n");

        cell.call("msg_from_base", "hello, i am client");
        client.call("msg_from_base", "hello, i am client");
    }

    void msg_from_cell(const GValue& msg) { INFO_LOG("msg.%s from cell\n", msg.as_string().c_str()); }
    void msg_from_client(const GValue& msg) { INFO_LOG("msg.%s from client\n", msg.as_string().c_str()); }
};

GENERATE_ENTITY_OUT(BaseAccount)


void BaseAccount::rpc_method_define() {
    RPC_METHOD(RpcType::SERVER_ONLY, msg_from_cell, GString());
    RPC_METHOD(RpcType::EXPOSED, msg_from_client, GString());
}

void BaseAccount::property_define() {
    PROPERTY(PropType::BASE_AND_CLIENT, "id", 1001);
    PROPERTY(PropType::BASE_AND_CLIENT, "name", "youjun");
}

class CellAccount : public CellEntityWithClient {

    GENERATE_ENTITY_INNER(CellAccount)

public:
    CellAccount() {}
    ~CellAccount() {}

    static void rpc_method_define();
    static void property_define();

    void msg_from_base(const GValue& msg) { INFO_LOG("msg.%s from base\n", msg.as_string().c_str()); }
    void msg_from_client(const GValue& msg) { INFO_LOG("msg.%s from client\n", msg.as_string().c_str()); }

    void on_ready() {
        INFO_LOG("CellAccount on_ready\n");

        base.call("msg_from_cell", "hello, i am client");
        client.call("msg_from_cell", "hello, i am client");
    }
};

void CellAccount::rpc_method_define() {
    RPC_METHOD(RpcType::SERVER_ONLY, msg_from_base, GString());
    RPC_METHOD(RpcType::EXPOSED, msg_from_client, GString());
}

void CellAccount::property_define() {

}

GENERATE_ENTITY_OUT(CellAccount)
