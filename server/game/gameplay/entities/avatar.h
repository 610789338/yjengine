#pragma once

#include "engine/engine.h"

#include "../components/itembag_component.h"

#include "common/prop_def/avatar_prop_def.h"
#include "common/utils/utils.h"


class BaseAvatar : public BaseEntityWithCellAndClient {

    GENERATE_ENTITY_INNER(BaseAvatar);

    static void regist_components() {
        REGIST_COMPONENT(BaseAvatar, ItemBagComponent);
    }
    static void rpc_method_define() {
        RPC_METHOD(RpcType::SERVER_ONLY, msg_from_cell);
        RPC_METHOD(RpcType::EXPOSED, msg_from_client);
        RPC_METHOD(RpcType::EXPOSED, byte_swap_test);
        RPC_METHOD(RpcType::EXPOSED, add_migrate_int_from_client);
    }
    static void property_define() {
        avatar_property_define<BaseAvatar>();
    }
    static void migrate_timer_define() {
        MIGRATE_TIMER_DEF(avatar_migrate_timer);
        MIGRATE_TIMER_DEF(base_rpc_timer);
    }

public:
    BaseAvatar() {}
    ~BaseAvatar() {}

    void on_ready(); // call by engine

    void msg_from_cell(const GString& msg);
    void msg_from_client(const GString& msg);
    void byte_swap_test(const GString& msg, int16_t arg1, int32_t arg2, int64_t arg3, float arg4, uint16_t arg5, uint32_t arg6, uint64_t arg7, double arg8);
    void avatar_timer_test(const GString& arg1);

    // property
    void property_test();
    void property_create();
    void property_delete();
    void property_update();

    // property sync
    void avatar_timer_prop_sync();
    void property_sync_test();
    void property_sync_test_create();
    void property_sync_test_clear();
    void property_sync_test_add();
    void property_sync_test_del();
    void property_sync_test_update();

    // migrate
    void avatar_migrate_timer();
    void add_migrate_int_from_client();

    // event 
    void entity_event_test(const GString& msg);
    void avatar_event_timer();

    void base_rpc_timer();

private:
    int32_t sync_count = 0;
    int32_t incr = 0;

private:
    TimerID test_timer;
    TimerID prop_timer;
    TimerID migrate_timer;
};

class CellAvatar : public CellEntityWithClient {

    GENERATE_ENTITY_INNER(CellAvatar);

    static void regist_components() {}
    static void rpc_method_define() {
        RPC_METHOD(RpcType::SERVER_ONLY, msg_from_base);
        RPC_METHOD(RpcType::EXPOSED, msg_from_client);

        RPC_METHOD(RpcType::SERVER_ONLY, add_migrate_int_from_base);
        RPC_METHOD(RpcType::EXPOSED, add_migrate_int_from_client);
    }
    static void property_define() {
        avatar_property_define<CellAvatar>();
    }
    static void migrate_timer_define() {
        MIGRATE_TIMER_DEF(avatar_timer_test);
        MIGRATE_TIMER_DEF(cell_rpc_timer);
    }

public:
    CellAvatar() {}
    ~CellAvatar() {}

    void on_ready(); // call by engine

    void msg_from_base(const GString& msg);
    void msg_from_client(const GString& msg);
    void avatar_timer_test(const GString& arg1);

    void add_migrate_int_from_base();
    void add_migrate_int_from_client();

    void cell_rpc_timer();
};
