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
    static void timer_cb_store() {}

    void on_ready(); // call by engine

    void msg_from_base(const GString& msg);
    void msg_from_cell(const GString& msg);
    void prop_sync_compare(const GBin& v);

    void on_prop_sync_from_server();

    // migrate
    void account_migrate_timer();
    void account_migrate_print_timer();

private:
    TimerID migrate_timer;
    TimerID migrate_print_timer;
};
