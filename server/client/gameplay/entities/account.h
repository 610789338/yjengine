#pragma once

#include "engine/engine.h"
#include "common/prop_def/account_prop_def.h"
#include "common/utils/utils.h"


class ClientAccount : public ClientEntity {

    GENERATE_ENTITY_INNER(ClientAccount);

    static void regist_components() {}
    static void rpc_method_define() {
        RPC_METHOD(RpcType::CLIENT, msg_from_base);
        RPC_METHOD(RpcType::CLIENT, msg_from_cell);
        RPC_METHOD(RpcType::CLIENT, prop_sync_compare);
    }
    static void property_define() {
        account_property_define<ClientAccount>();
    }
    static void migrate_timer_define() {}

public:
    ClientAccount() {}
    ~ClientAccount() {}

    void on_ready(); // call by engine

    void msg_from_base(const GString& msg);
    void msg_from_cell(const GString& msg);
    void prop_sync_compare(const GBin& v);

    void on_prop_sync_from_server();

    // migrate
    void account_migrate_timer();
    void account_migrate_print_timer();

    void client_rpc_timer();

private:
    TimerID migrate_timer;
    TimerID migrate_print_timer;
};
