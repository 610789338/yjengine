#pragma once

#include "engine/engine.h"



class BaseAccount : public BaseEntityWithCellAndClient {
    
    GENERATE_BASE_ENTITY_INNER()

public:
    BaseAccount() : BaseEntityWithCellAndClient("CellAccount", "ClientAccount") {}
    ~BaseAccount() {}
};

class CellAccount : public CellEntityWithClient {

    GENERATE_CELL_ENTITY_INNER()

public:
    CellAccount() {}
    ~CellAccount() {}
};

GENERATE_BASE_ENTITY_OUT(BaseAccount)
GENERATE_CELL_ENTITY_OUT(CellAccount)
