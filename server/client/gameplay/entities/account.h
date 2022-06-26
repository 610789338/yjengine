#pragma once

#include "engine/engine.h"



class ClientAccount : public ClientEntity {

    GENERATE_CLIENT_ENTITY_INNER()

public:
    ClientAccount() {}
    ~ClientAccount() {}
};

GENERATE_CLIENT_ENTITY_OUT(ClientAccount)
