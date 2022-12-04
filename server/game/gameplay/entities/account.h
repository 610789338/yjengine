#pragma once

#include "engine/engine.h"

#include "../components/create_avatar_component.h"


class BaseAccount : public BaseEntityWithCellAndClient {

    GENERATE_ENTITY_INNER(BaseAccount);

    static void regist_components() {
        REGIST_COMPONENT(BaseAccount, CreateAvatarComponent);
    }
    static void rpc_method_define() {
        RPC_METHOD(RpcType::SERVER_ONLY, msg_from_cell);
        RPC_METHOD(RpcType::EXPOSED, msg_from_client);
        RPC_METHOD(RpcType::EXPOSED, add_migrate_int_from_client);
    }
    static void property_define();
    static void timer_cb_store() {}

public:
    BaseAccount() {}
    ~BaseAccount() {}

    void on_ready(); // call by engine

    void msg_from_cell(const GString& msg);
    void msg_from_client(const GString& msg);
    void account_timer_test(const GString& arg1);

    // property
    void property_test();
    void property_create();
    void property_delete();
    void property_update();

    // property sync
    void account_timer_prop_sync();
    void property_sync_test();
    void property_sync_test_create();
    void property_sync_test_clear();
    void property_sync_test_add();
    void property_sync_test_del();
    void property_sync_test_update();

    // migrate
    void account_migrate_timer();
    void add_migrate_int_from_client();

    // event 
    void entity_event_test(const GString& msg);
    void account_event_timer();

private:
    int32_t sync_count = 0;
    int32_t incr = 0;

private:
    TimerID test_timer;
    TimerID prop_timer;
    TimerID migrate_timer;
};

class CellAccount : public CellEntityWithClient {

    GENERATE_ENTITY_INNER(CellAccount);

public:
    CellAccount() {}
    ~CellAccount() {}

    static void regist_components();
    static void rpc_method_define();
    static void property_define();
    static void timer_cb_store();

    void on_ready(); // call by engine

    void msg_from_base(const GString& msg);
    void msg_from_client(const GString& msg);
    void account_timer_test(const GString& arg1);

    void add_migrate_int_from_base();
    void add_migrate_int_from_client();
};
