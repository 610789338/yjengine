#pragma once

#include "engine/engine.h"

#include "common/utils/utils.h"


class GameInstance : public BaseEntity {

    GENERATE_ENTITY_INNER(GameInstance);

    static void regist_components() {}
    static void rpc_method_define() {}
    static void property_define() {}
    static void migrate_timer_define() {}
    static void migrate_event_define() {}

public:
    GameInstance() {}
    ~GameInstance() {}

    void on_ready();
    void on_gate_disappear(const GString& gate_addr);
    void create_stubs();
    // void dump_profile();
};

extern GameInstance* g_game_instance;
