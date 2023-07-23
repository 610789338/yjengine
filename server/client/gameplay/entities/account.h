#pragma once

#include "engine/engine.h"
#include "common/prop_def/account_prop_def.h"
#include "common/utils/utils.h"


class ClientAccount : public ClientEntity {

    GENERATE_ENTITY_INNER(ClientAccount);

    static void regist_components() {}
    static void rpc_method_define() {
        RPC_METHOD(RpcType::CLIENT, msg_from_base);
    }
    static void property_define() {
        account_property_define<ClientAccount>();
    }
    static void migrate_timer_define() {}
    static void migrate_event_define() {}

public:
    ClientAccount() {}
    ~ClientAccount() {}

    void on_ready(); // call by engine

    void msg_from_base(const GString& msg);

    void client_rpc_timer();
};
