#pragma once

#include "framework/entity.h"

class BaseAccount : public BaseEntityWithCellAndClient {
public:
    BaseAccount() {}
    ~BaseAccount() {}

};

class CellAccount : public CellEntityWithClient {
public:
    CellAccount() {}
    ~CellAccount() {}
};


Entity* create_base_account() {
    return new BaseAccount;
}

Entity* create_cell_account() {
    return new CellAccount;
}
