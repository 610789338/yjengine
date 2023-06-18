#pragma once

#include "engine/engine.h"

#include "../components/avatar_prop_test_component_client.h"
#include "../components/itembag_component_client.h"


class ClientAvatar : public ClientEntity {

    GENERATE_ENTITY_INNER(ClientAvatar);

    static void regist_components() {
#ifdef __PROP_SYNC_TEST__
        REGIST_COMPONENT(ClientAvatar, AvatarPropTestComponentClient);
#endif
        REGIST_COMPONENT(ClientAvatar, ItemBagComponentClient);
    }
    static void rpc_method_define() {
        RPC_METHOD(RpcType::CLIENT, msg_from_base);
        RPC_METHOD(RpcType::CLIENT, msg_from_cell);
    }
    static void property_define() {
    }
    static void migrate_timer_define() {}

public:
    ClientAvatar() {}
    ~ClientAvatar() {}

    void on_ready(); // call by engine

    void msg_from_base(const GString& msg);
    void msg_from_cell(const GString& msg);

    void on_prop_sync_from_server();

    // migrate
    void avatar_migrate_timer();
    void avatar_migrate_print_timer();

    void client_rpc_timer();

private:
    TimerID migrate_timer;
    TimerID migrate_print_timer;
};
