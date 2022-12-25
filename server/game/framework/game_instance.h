#pragma once

#include "engine/entity.h"

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

    void on_gate_disappear(const GString& gate_addr);
};

extern BaseGameInstance* g_game_instance;

class HeatbeatThreadObj {
public:
    HeatbeatThreadObj() {}
    ~HeatbeatThreadObj(){}

    void operator()();
    void heart_beat_check();
};
