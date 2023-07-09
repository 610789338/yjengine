#pragma once

#include "engine/engine.h"

#include "game/framework/stub_base.h"
#include "common/prop_def/dungeon_manager_prop_def.h"

class DungeonManager : public StubBase {

    GENERATE_ENTITY_INNER(DungeonManager);

    static void regist_components() {
    }
    static void rpc_method_define() {
        RPC_METHOD(RpcType::SERVER_ONLY, create_dungeon);
        RPC_METHOD(RpcType::SERVER_ONLY, on_dungeon_create);
        RPC_METHOD(RpcType::SERVER_ONLY, enter_dungeon);
        RPC_METHOD(RpcType::SERVER_ONLY, enter_random_dungeon);
    }
    static void property_define() {
        _property_define<DungeonManager>();
    }
    template<class TEntity>
    static void _property_define() {
        PROPERTY_MAP(PropType::BASE_PRIVATE, all_dungeon_proxys, DungeonProxyContainer);
    }
    static void migrate_timer_define() {
    }

public:
    DungeonManager() {}
    ~DungeonManager() {}

    void on_ready();

    void create_dungeon(int32_t dungeon_id);
    void on_dungeon_create(int32_t dungeon_id, const MailBox& dungeon_mailbox);
    void enter_random_dungeon(const MailBox& avatar);
    void enter_dungeon(const MailBox& avatar, int32_t dungeon_id);
};
