#pragma once

#include "engine/engine.h"

#include "game/framework/stub_base.h"
#include "common/prop_def/dungeon_manager_prop_def.h"

class DungeonMananger : public StubBase {

    GENERATE_ENTITY_INNER(DungeonMananger);

    static void regist_components() {
    }
    static void rpc_method_define() {
        RPC_METHOD(RpcType::SERVER_ONLY, enter_dungeon);
        RPC_METHOD(RpcType::SERVER_ONLY, req_create_dungeon);
        RPC_METHOD(RpcType::SERVER_ONLY, on_dungeon_create);
    }
    static void property_define() {
        _property_define<DungeonMananger>();
    }
    template<class TEntity>
    static void _property_define() {
        PROPERTY_MAP(PropType::BASE_PRIVATE, dungeon_proxys, DungeonProxy);
    }
    static void migrate_timer_define() {
    }

public:
    DungeonMananger() {}
    ~DungeonMananger() {}

    void enter_dungeon(const MailBox& avatar, int32_t dungeon_id);
    void req_create_dungeon(int32_t dungeon_id);
    void on_dungeon_create(const GString& dungeon_uuid, int32_t dungeon_id, const MailBox& dungeon_mailbox);
};
