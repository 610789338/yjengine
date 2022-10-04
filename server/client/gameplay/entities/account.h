#pragma once

#include "engine/engine.h"


class ClientAccount : public ClientEntity {

    GENERATE_ENTITY_INNER(ClientAccount)

public:
    ClientAccount() {}
    ~ClientAccount() {}

    static void regist_components();
    static void rpc_method_define();
    static void property_define();

    void on_ready(); // call by engine

    void msg_from_base(const GValue& msg);
    void msg_from_cell(const GValue& msg);
    void prop_sync_compare(const GValue& v);

    void on_prop_sync_from_server();

    // migrate
    void account_migrate_timer();

private:
    TimerID migrate_timer;
};
