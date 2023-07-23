#pragma once

#include "engine/engine.h"

#include "../components/create_avatar_component.h"
#include "common/prop_def/account_prop_def.h"
#include "common/utils/utils.h"


class BaseAccount : public BaseEntityWithClient {

    GENERATE_ENTITY_INNER(BaseAccount);

    static void regist_components() {
        REGIST_COMPONENT(BaseAccount, CreateAvatarComponent);
    }
    static void rpc_method_define() {
        RPC_METHOD(RpcType::EXPOSED, create_avatar);
        RPC_METHOD(RpcType::EXPOSED, create_monster);
        RPC_METHOD(RpcType::EXPOSED, msg_from_client);
    }
    static void property_define() {
        account_property_define<BaseAccount>();
    }
    static void migrate_timer_define() {}
    static void migrate_event_define() {}

public:
    BaseAccount() {}
    ~BaseAccount() {}

    void on_ready(); // call by engine

    void create_avatar();
    void create_monster();

    void msg_from_client(const GString& msg);

    // event 
    void event_test(const GString& msg);
    void account_event_timer();

    void base_rpc_timer();
};
