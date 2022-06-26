#include "entity_rpc_manager.h"
#include "entity.h"


EntityRpcManager::EntityRpcManager(const GString& belong, const GString& entity_class_name, const function<Entity*()>& creator) {
    m_belong = belong;

    if (m_belong == "base") {
        entity_rpc_regist("on_cell_create", &BaseEntityWithCellAndClient::on_cell_create, GString(), GString());
        entity_rpc_regist("on_client_create", &BaseEntityWithCellAndClient::on_client_create, GString());
    }
    else if (m_belong == "cell") {
        entity_rpc_regist("on_client_create", &CellEntityWithClient::on_client_create, GString());
    }
    
    regist_entity_creator(entity_class_name, creator);
}
