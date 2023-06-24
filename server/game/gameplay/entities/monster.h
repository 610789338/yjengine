#pragma once

#include "engine/engine.h"

#include "common/utils/utils.h"


class BaseMonster : public BaseEntityWithCell {

    GENERATE_ENTITY_INNER(BaseMonster);

    static void regist_components() {
    }
    static void rpc_method_define() {
        RPC_METHOD(RpcType::SERVER_ONLY, msg_from_cell);
    }
    static void property_define() {
        _property_define<BaseMonster>();
    }
    template<class TEntity>
    static void _property_define() {
        PROPERTY_SIMPLE(PropType::ALL, monster_id, int32_t, 1001);
    }

    static void migrate_timer_define() {
        MIGRATE_TIMER_DEF(monster_migrate_timer);
        MIGRATE_TIMER_DEF(base_rpc_timer);
    }

public:
    BaseMonster() {}
    ~BaseMonster() {}

    void on_ready(); // call by engine

    void msg_from_cell(const GString& msg);

    void monster_migrate_timer();

    void base_rpc_timer();

private:
    TimerID migrate_timer;
};

class CellMonster : public CellEntity {

    GENERATE_ENTITY_INNER(CellMonster);

    static void regist_components() {}
    static void rpc_method_define() {
        RPC_METHOD(RpcType::SERVER_ONLY, msg_from_base);
    }
    static void property_define() {
    }
    static void migrate_timer_define() {
        MIGRATE_TIMER_DEF(cell_rpc_timer);
    }

public:
    CellMonster() {}
    ~CellMonster() {}

    void on_ready(); // call by engine

    void msg_from_base(const GString& msg);

    void cell_rpc_timer();
};
