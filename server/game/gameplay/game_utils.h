#pragma once

#include "engine/engine.h"

#include "framework/entity.h"

#include "entities/Account.h"

void regist_game_entity_creator() {
    regist_entity_creator("BaseAccount", create_base_account);
    regist_entity_creator("CellAccount", create_cell_account);
}
