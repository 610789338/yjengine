#pragma once

#include "engine/engine.h"

#include "../components/avatar_prop_test_component.h"
#include "../components/itembag_component.h"
#include "../components/dungeon_component.h"


class BaseAvatar : public BaseEntityWithCellAndClient {

    GENERATE_ENTITY_INNER(BaseAvatar);

    static void regist_components() {
        //REGIST_COMPONENT(BaseAvatar, AvatarPropTestComponent);
        //REGIST_COMPONENT(BaseAvatar, ItemBagComponent);
    }
    static void rpc_method_define() {
        RPC_METHOD(RpcType::SERVER_ONLY, msg_from_cell);
        RPC_METHOD(RpcType::EXPOSED, msg_from_client);
        RPC_METHOD(RpcType::EXPOSED, byte_swap_test);
        RPC_METHOD(RpcType::EXPOSED, add_migrate_int_from_client);
    }
    static void property_define() {
    }
    static void migrate_timer_define() {
        MIGRATE_TIMER_DEF(avatar_migrate_timer);
        MIGRATE_TIMER_DEF(base_rpc_timer);
    }
    static void migrate_event_define() {}

public:
    BaseAvatar() {}
    ~BaseAvatar() {}

    void on_ready(); // call by engine

    void msg_from_cell(const GString& msg);
    void msg_from_client(const GString& msg);
    void byte_swap_test(const GString& msg, int16_t arg1, int32_t arg2, int64_t arg3, float arg4, uint16_t arg5, uint32_t arg6, uint64_t arg7, double arg8);
    void avatar_timer_test(const GString& arg1);

    // migrate
    void avatar_migrate_timer();
    void add_migrate_int_from_client();

    void base_rpc_timer();

private:
    TimerID test_timer;
    TimerID migrate_timer;
};

class CellAvatar : public CellEntityWithClient {

    GENERATE_ENTITY_INNER(CellAvatar);

    static void regist_components() {
#ifdef __PROP_SYNC_TEST__
        REGIST_COMPONENT(CellAvatar, AvatarPropTestComponent);
#endif
        REGIST_COMPONENT(CellAvatar, ItemBagComponent);
        REGIST_COMPONENT(CellAvatar, DungeonComponent);
    }
    static void rpc_method_define() {
        RPC_METHOD(RpcType::SERVER_ONLY, msg_from_base);
        RPC_METHOD(RpcType::EXPOSED, msg_from_client);

        RPC_METHOD(RpcType::SERVER_ONLY, add_migrate_int_from_base);
        RPC_METHOD(RpcType::EXPOSED, add_migrate_int_from_client);
    }
    static void property_define() {
    }
    static void migrate_timer_define() {
        MIGRATE_TIMER_DEF(avatar_timer_test);
        MIGRATE_TIMER_DEF(cell_rpc_timer);
    }
    static void migrate_event_define() {
        MIGRATE_EVENT_DEF(begin_migrate);
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

    void begin_migrate(const GString& new_addr);

    void cell_rpc_timer();
};
