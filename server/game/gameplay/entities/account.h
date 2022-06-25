#pragma once

#include "framework/entity.h"

#define BEGIN_GENERATE_BASE_BODY() \
public:\
    static EntityRpcManager rpc_manager;\
    EntityRpcManager* get_rpc_mgr() { return &rpc_manager; }

#define END_GENERATE_BASE_BODY(TCLASS) \
EntityRpcManager TCLASS::rpc_manager("base");

class BaseAccount : public BaseEntityWithCellAndClient {
    
    BEGIN_GENERATE_BASE_BODY()

public:
    BaseAccount() : BaseEntityWithCellAndClient("CellAccount") {}
    ~BaseAccount() {}
};

END_GENERATE_BASE_BODY(BaseAccount)

class CellAccount : public CellEntityWithClient {
public:
    CellAccount() {}
    ~CellAccount() {}
};


Entity* create_base_account() {
    return new BaseAccount();
}

Entity* create_cell_account() {
    return new CellAccount;
}
