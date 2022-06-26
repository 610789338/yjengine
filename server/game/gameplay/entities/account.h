#pragma once

#include "framework/entity.h"


class BaseAccount : public BaseEntityWithCellAndClient {
    
    GENERATE_BASE_ENTITY_INNER()

public:
    BaseAccount() : BaseEntityWithCellAndClient("CellAccount") {}
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
