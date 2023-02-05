#pragma once

#include "engine/engine.h"

#include "../components/create_avatar_component.h"
#include "common/prop_def/account_prop_def.h"
#include "common/utils/utils.h"


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
    static void property_define() {
        account_property_define<BaseAccount>();
    }
    static void migrate_timer_define() {
        MIGRATE_TIMER_DEF(account_migrate_timer);
        MIGRATE_TIMER_DEF(base_rpc_timer);
    }

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

    void base_rpc_timer();

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

    static void regist_components() {}
    static void rpc_method_define() {
        RPC_METHOD(RpcType::SERVER_ONLY, msg_from_base);
        RPC_METHOD(RpcType::EXPOSED, msg_from_client);

        RPC_METHOD(RpcType::SERVER_ONLY, add_migrate_int_from_base);
        RPC_METHOD(RpcType::EXPOSED, add_migrate_int_from_client);
    }
    static void property_define() {
        account_property_define<CellAccount>();
    }
    static void migrate_timer_define() {
        MIGRATE_TIMER_DEF(account_timer_test);
        MIGRATE_TIMER_DEF(cell_rpc_timer);
    }

public:
    CellAccount() {}
    ~CellAccount() {}

    void on_ready(); // call by engine

    void msg_from_base(const GString& msg);
    void msg_from_client(const GString& msg);
    void account_timer_test(const GString& arg1);

    void add_migrate_int_from_base();
    void add_migrate_int_from_client();

    void cell_rpc_timer();
};
