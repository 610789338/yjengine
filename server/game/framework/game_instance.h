#pragma once

#include "engine/engine.h"

#include "common/utils/utils.h"


class BaseGameInstance : public BaseEntity {

    GENERATE_ENTITY_INNER(BaseGameInstance);

    static void regist_components() {}
    static void rpc_method_define() {}
    static void property_define() {}
    static void migrate_timer_define() {}

public:
    BaseGameInstance() {}
    ~BaseGameInstance() {}

    void on_ready();
    void on_gate_disappear(const GString& gate_addr);
    void create_stubs();
};

extern BaseGameInstance* g_game_instance;
