#include "entity_component_manager.h"
#include "entity.h"


EntityComponentBase::~EntityComponentBase() {}

EntityPropertyBase* EntityComponentBase::get_prop(const GString& prop_name) const { 
    return owner->get_prop(prop_name);
}
