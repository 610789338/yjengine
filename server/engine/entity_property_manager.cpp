#include "entity_property_manager.h"


void EntityPropertyBase::link_node(EntityPropertyBase* pre_node, GString self_name) {
    if (pre_node != nullptr) {
        EntityPropertyComplex* pre = (EntityPropertyComplex*)pre_node;
        pre->propertys[self_name] = this;
    }
}
