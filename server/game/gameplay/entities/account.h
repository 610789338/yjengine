#pragma once

#include "engine/engine.h"



class BaseAccount : public BaseEntityWithCellAndClient {
    
    GENERATE_ENTITY_INNER()

public:
    BaseAccount() : BaseEntityWithCellAndClient("CellAccount", "ClientAccount") {}
    ~BaseAccount() {}

    void on_ready() {
        INFO_LOG("BaseAccount on_ready\n");
    }
};

class CellAccount : public CellEntityWithClient {

    GENERATE_ENTITY_INNER()

public:
    CellAccount() {}
    ~CellAccount() {}

    void on_ready() {
        INFO_LOG("CellAccount on_ready\n");
    }
};

GENERATE_ENTITY_OUT(BaseAccount)
GENERATE_ENTITY_OUT(CellAccount)
