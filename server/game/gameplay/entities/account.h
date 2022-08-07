#pragma once

#include "engine/engine.h"

#include "components/create_avatar_component.h"


class BaseAccount : public BaseEntityWithCellAndClient {
    
    GENERATE_ENTITY_INNER(BaseAccount)

public:
    BaseAccount() : BaseEntityWithCellAndClient("CellAccount", "ClientAccount") {}
    ~BaseAccount() {}

    static void regist_components() {
        REGIST_COMPONENT(BaseAccount, CreateAvatarComponent);
    }

    static void rpc_method_define() {
        RPC_METHOD(RpcType::SERVER_ONLY, msg_from_cell, GString());
        RPC_METHOD(RpcType::EXPOSED, msg_from_client, GString());
    }

    static void property_define() {
        PROPERTY(PropType::BASE_AND_CLIENT, "id", 1001);
        PROPERTY(PropType::BASE_AND_CLIENT, "name", "youjun");
    }

    // called by engine
    void on_ready() {
        INFO_LOG("BaseAccount on_ready\n");

        cell.call("msg_from_base", "hello, i am base");
        client.call("msg_from_base", "hello, i am base");

        RETIST_TIMER(0, 3600, true, &BaseAccount::account_timer_test, "60 seconds");
    }

    void msg_from_cell(const GValue& msg) { INFO_LOG("[base] msg.%s from cell\n", msg.as_string().c_str()); }
    void msg_from_client(const GValue& msg) { INFO_LOG("[base] msg.%s from client\n", msg.as_string().c_str()); }
    void account_timer_test(const GValue& arg1) { 
        INFO_LOG("this is timer test arg1.%s\n", arg1.as_string().c_str()); 
    }

    TimerID test_timer;
};

GENERATE_ENTITY_OUT(BaseAccount)

class CellAccount : public CellEntityWithClient {

    GENERATE_ENTITY_INNER(CellAccount)

public:
    CellAccount() {}
    ~CellAccount() {}

    static void regist_components() {}
    static void rpc_method_define() {
        RPC_METHOD(RpcType::SERVER_ONLY, msg_from_base, GString());
        RPC_METHOD(RpcType::EXPOSED, msg_from_client, GString());
    }
    static void property_define() {}

    void msg_from_base(const GValue& msg) { INFO_LOG("[cell] msg.%s from base\n", msg.as_string().c_str()); }
    void msg_from_client(const GValue& msg) { INFO_LOG("[cell] msg.%s from client\n", msg.as_string().c_str()); }

    void on_ready() {
        INFO_LOG("CellAccount on_ready\n");

        base.call("msg_from_cell", "hello, i am cell");
        client.call("msg_from_cell", "hello, i am cell");
    }
};

GENERATE_ENTITY_OUT(CellAccount)
