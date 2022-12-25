#pragma once

#include "engine/entity.h"

#include "common/utils/utils.h"


class BaseGateInstance : public BaseEntity {

    GENERATE_ENTITY_INNER(BaseGateInstance);

    static void regist_components() {}
    static void rpc_method_define() {}
    static void property_define() {}
    static void migrate_timer_define() {}

public:
    BaseGateInstance() {}
    ~BaseGateInstance() {}

    void on_game_disappear(const GString& game_addr);
};

extern BaseGateInstance* g_gate_instance;

class HeatbeatThreadObj {
public:
    HeatbeatThreadObj() {}
    ~HeatbeatThreadObj() {}

    void operator()();
    void heart_beat_check();
};
