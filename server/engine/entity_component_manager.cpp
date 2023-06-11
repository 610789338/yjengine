#include "entity_component_manager.h"
#include "entity.h"


EntityComponentBase::~EntityComponentBase() {}

EntityPropertyBase* EntityComponentBase::get_prop(const GString& prop_name) const { 
    return owner->get_prop(prop_name);
}

EntityPropertyBase& EntityComponentBase::get_prop_ref(const GString& prop_name) const {
    EntityPropertyBase* prop = get_prop(prop_name);
    return *prop;
}

void ComponentManagerBase::component_regist(EntityComponentBase* component) {

    if (components.find(component->get_name()) != components.end()) {
        WARN_LOG("component.%s exist\n", component->get_name().c_str());
        return;
    }

    components[component->get_name()] = component;
}

void ComponentManagerBase::generate_entity_components(Entity* owner) {
    owner->components.clear();
    for (auto iter = components.begin(); iter != components.end(); ++iter) {
        owner->components[iter->first] = iter->second->create_self(owner);
    }
}

void ComponentManagerBase::rpc_call(Entity* entity, bool from_client, const GString& rpc_name, RpcMethodBase* rpc_method) {

    RpcMethodBase* method = get_rpc_mgr()->find_rpc_method(rpc_name);
    const auto& comp_name = method->get_comp_name();

    auto iter = entity->components.find(comp_name);
    if (iter == entity->components.end()) {
        return;
    }

    iter->second->rpc_call(from_client, rpc_name, rpc_method);
}
