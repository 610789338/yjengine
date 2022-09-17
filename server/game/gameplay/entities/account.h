#pragma once

#include "engine/engine.h"


class BaseAccount : public BaseEntityWithCellAndClient {
    
    GENERATE_ENTITY_INNER(BaseAccount)

public:
    BaseAccount() : BaseEntityWithCellAndClient("CellAccount", "ClientAccount") {}
    ~BaseAccount() {}

    static void regist_components();
    static void rpc_method_define();
    static void property_define();

    void on_tick();

    void on_ready(); // call by engine

    void msg_from_cell(const GValue& msg);
    void msg_from_client(const GValue& msg);
    void account_timer_test(const GValue& arg1);

    void property_test();
    void property_create();
    void property_delete();
    void property_update();

    void property_sync_test();
    void property_sync_test_create();
    void property_sync_test_clear();
    void property_sync_test_add();
    void property_sync_test_del();
    void property_sync_test_update();

private:
    TimerID test_timer;
};

class CellAccount : public CellEntityWithClient {

    GENERATE_ENTITY_INNER(CellAccount)

public:
    CellAccount() {}
    ~CellAccount() {}

    static void regist_components();
    static void rpc_method_define();
    static void property_define();

    void on_ready(); // call by engine

    void msg_from_base(const GValue& msg);
    void msg_from_client(const GValue& msg);
};
