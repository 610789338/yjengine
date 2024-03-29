#pragma once

#include "engine/engine.h"


class CreateAvatarComponent : public EntityComponentBase {

    GENERATE_COMPONENT_INNER(CreateAvatarComponent);

    COMP_RPC_DEFINE() {
        COMP_RPC_METHOD(RpcType::EXPOSED, component_rpc_test);
    }

    COMP_MIGRATE_TIMER_DEFINE() {
        COMP_MIGRATE_TIMER_DEF(component_timer_test);
    }
    COMP_MIGRATE_EVENT_DEFINE() {}

    COMP_PROPERTY_DEFINE() {}

public:
    CreateAvatarComponent() {}
    ~CreateAvatarComponent() {}

    virtual void on_ready() {
        //COMP_REGIST_TIMER(0, 5, true, component_timer_test, "args1");
        COMP_REGIST_EVENT(event_test);

        COMP_SEND_EVENT("event_test", "lalalala");
    }

    void component_rpc_test(const GString& msg);
    void component_timer_test(const GString& msg);
    void event_test(const GString& msg);
};
