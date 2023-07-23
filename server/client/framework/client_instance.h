#pragma once

#include "engine/engine.h"

#include "common/utils/utils.h"


class ClientInstance : public BaseEntity {

    GENERATE_ENTITY_INNER(ClientInstance);

    static void regist_components() {}
    static void rpc_method_define() {}
    static void property_define() {}
    static void migrate_timer_define() {}
    static void migrate_event_define() {}

public:
    ClientInstance() {}
    ~ClientInstance() {}

    void on_gate_disappear(const GString& gate_addr);

    void set_should_call_create(bool should_call_create);
    bool get_should_call_create();

private:
    bool m_should_call_create = true;
};

extern ClientInstance* g_client_instance;
