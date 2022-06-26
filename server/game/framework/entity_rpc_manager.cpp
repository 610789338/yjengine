#include "entity_rpc_manager.h"
#include "entity.h"


EntityRpcManager::EntityRpcManager(const GString& belong, const GString& entity_class_name, const function<Entity*()>& creator) {
    m_belong = belong;

    if (m_belong == "base") {
        entity_rpc_regist("on_cell_create", &BaseEntityWithCell::on_cell_create, GString(), GString());
    }
    else if (m_belong == "cell") {

    }
    
    regist_entity_creator(entity_class_name, creator);
}
