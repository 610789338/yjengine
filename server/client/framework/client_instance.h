#pragma once

#include "engine/engine.h"

#include "common/utils/utils.h"


class ClientGameInstance : public ClientEntity {

    GENERATE_ENTITY_INNER(ClientGameInstance);

    static void regist_components() {}
    static void rpc_method_define() {}
    static void property_define() {}
    static void migrate_timer_define() {}

public:
    ClientGameInstance() {}
    ~ClientGameInstance() {}

    void on_gate_disappear(const GString& gate_addr);
    void set_controller(Entity* entity);
    Entity* get_controller();

    void set_should_call_create(bool should_call_create);
    bool get_should_call_create();

private:
    Entity* m_controller = nullptr;
    bool m_should_call_create = true;
};

extern ClientGameInstance* g_client_instance;
