#pragma once

#include "engine/engine.h"

#include "common/utils/utils.h"


//class BaseGameInstance : public BaseEntity {
//
//    GENERATE_ENTITY_INNER(BaseGameInstance);
//
//    static void regist_components() {}
//    static void rpc_method_define() {}
//    static void property_define() {}
//    static void migrate_timer_define() {}
//
//public:
//    BaseGameInstance() {}
//    ~BaseGameInstance() {}
//
//    void on_ready(); // call by engine
//    void heartbeat_check_timer();
//};
//
//extern Entity* g_game_instance = nullptr;

class HeatbeatThreadObj {
public:
    HeatbeatThreadObj() {}
    ~HeatbeatThreadObj(){}

    void operator()();
    void heart_beat_check();
};
