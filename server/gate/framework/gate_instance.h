#pragma once

#include "engine/engine.h"

#include "common/utils/utils.h"


class GateInstance : public BaseEntity {

    GENERATE_ENTITY_INNER(GateInstance);

    static void regist_components() {}
    static void rpc_method_define() {}
    static void property_define() {}
    static void migrate_timer_define() {}
    static void migrate_event_define() {}

public:
    GateInstance() {}
    ~GateInstance() {}

    void on_game_disappear(const GString& game_addr);
};

extern GateInstance* g_gate_instance;
