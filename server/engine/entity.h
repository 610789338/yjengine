#pragma once

#include <stdint.h>

int64_t g_entity_id = 0;


class Entity {

public:
    Entity() {}
    ~Entity() {}

};

class BaseEntity : public Entity {

public:
    BaseEntity() {}
    ~BaseEntity() {}
};

class BaseEntityWithClient : public BaseEntity {

public:
    BaseEntityWithClient() {}
    ~BaseEntityWithClient() {}
};

class CellEntity : public Entity {

public:
    CellEntity() {}
    ~CellEntity() {}
};

class CellEntityWithClient : public CellEntity {

public:
    CellEntityWithClient() {}
    ~CellEntityWithClient() {}
};
