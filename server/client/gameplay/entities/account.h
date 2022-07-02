#pragma once

#include "engine/engine.h"



class ClientAccount : public ClientEntity {

    GENERATE_ENTITY_INNER()

public:
    ClientAccount() {}
    ~ClientAccount() {}

    void on_ready() {
        INFO_LOG("ClientAccount on_ready\n");
    }
};

GENERATE_ENTITY_OUT(ClientAccount)
