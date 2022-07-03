#include "entity_rpc_manager.h"

//
//EntityRpcManager::EntityRpcManager(enum EntityType entity_type, const GString& entity_class_name, const function<Entity*()>& creator) {
//
//    // entity base rpc
//    if (entity_type == EntityType::EntityType_Base) {
//    }
//    else if (entity_type == EntityType::EntityType_BaseWithCell) {
//        entity_rpc_regist("on_cell_create", &BaseEntityWithCell::on_cell_create, GString(), GString());
//    }
//    else if (entity_type == EntityType::EntityType_BaseWithClient) {
//        entity_rpc_regist("on_client_create", &BaseEntityWithClient::on_client_create, GString());
//    }
//    else if (entity_type == EntityType::EntityType_BaseWithCellAndClient) {
//        entity_rpc_regist("on_cell_create", &BaseEntityWithCellAndClient::on_cell_create, GString(), GString());
//        entity_rpc_regist("on_client_create", &BaseEntityWithCellAndClient::on_client_create, GString());
//        entity_rpc_regist("on_ready", &BaseEntityWithCellAndClient::on_ready);
//    }
//    else if (entity_type == EntityType::EntityType_CellWithClient) {
//        entity_rpc_regist("on_client_create", &CellEntityWithClient::on_client_create, GString());
//    }
//    else if (entity_type == EntityType::EntityType_Client) {
//        entity_rpc_regist("on_ready", &ClientEntity::on_ready);
//    }
//    
//    regist_entity_creator(entity_class_name, creator);
//
//    EntityClassType::rpc_method_define();
//    EntityClassType::property_define();
//}
