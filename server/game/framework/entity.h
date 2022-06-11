#pragma once

#include <stdint.h>
#include <unordered_map>

#include "engine/engine.h"

#include "mailbox.h"

using namespace std;

class Entity;
extern unordered_map<GString, Entity*> g_entities;
extern unordered_map<GString, function<Entity*()>> g_entity_creator;
extern void regist_entity_creator(const GString& entity, const function<Entity*()>& creator);
extern Entity* create_entity(const GString& entity_type, const GDict& create_data);

extern GString gen_uuid();

enum EntityType {
    EntityType_Base,
    EntityType_BaseWithCell,
    EntityType_BaseWithClient,
    EntityType_BaseWithCellAndClient,
    EntityType_Cell,
    EntityType_CellWithClient
};

class Entity {

public:
    Entity() {}
    virtual ~Entity() {}

    virtual void on_create(const GDict& create_data) = 0;
    virtual void on_destroy() = 0;

    GString id;
    EntityType type;
};

class BaseEntity : public Entity {

public:
    BaseEntity() {
        type = EntityType::EntityType_Base;
    }
    virtual ~BaseEntity() {}

    virtual void on_create(const GDict& create_data) {}
    virtual void on_destroy() {}
};

class BaseEntityWithCell : virtual public BaseEntity {

public:
    BaseEntityWithCell() {
        type = EntityType::EntityType_BaseWithCell;
    }
    virtual ~BaseEntityWithCell() {}

    virtual void on_create(const GDict& create_data);
    virtual void on_destroy();
    virtual void on_cell_create();

    void create_cell();

    //CellMailBox cell;
};

class BaseEntityWithClient : virtual public BaseEntity {

public:
    BaseEntityWithClient() {
        type = EntityType::EntityType_BaseWithClient;
    }
    virtual ~BaseEntityWithClient() {}

    virtual void on_create(const GDict& create_data);
    virtual void on_destroy();
    virtual void on_client_create();

    void create_client();

    //ClientMailBox client;
};

class BaseEntityWithCellAndClient : public BaseEntityWithCell, public BaseEntityWithClient {

public:
    BaseEntityWithCellAndClient() {
        type = EntityType::EntityType_BaseWithCellAndClient;
    }
    virtual ~BaseEntityWithCellAndClient() {}

    virtual void on_create(const GDict& create_data);
    virtual void on_destroy();
    virtual void on_cell_create();
    virtual void on_client_create();
};

class CellEntity : public Entity {

public:
    CellEntity() {
        type = EntityType::EntityType_Cell;
    }
    virtual ~CellEntity() {}

    virtual void on_create(const GDict& create_data) {}
    virtual void on_destroy() {}

    //BaseMailBox base;
};

class CellEntityWithClient : public CellEntity {

public:
    CellEntityWithClient() {
        type = EntityType::EntityType_CellWithClient;
    }
    virtual ~CellEntityWithClient() {}

    //ClientMailBox client;
};
