#include "entity_property_manager.h"


unordered_map<GString, unordered_map<GString, uint8_t> > g_all_prop_idxs;

void EntityPropertyBase::link_node(EntityPropertyBase* pre_node, GString self_name) {
    if (pre_node != nullptr) {
        EntityPropertyComplex* pre = (EntityPropertyComplex*)pre_node;
        auto pclass_name = pre->get_pclass_name();
        if (g_all_prop_idxs.find(pclass_name) == g_all_prop_idxs.end()) {
            pre->gen_prop_idxs();
        }

        auto prop_idxs = g_all_prop_idxs.at(pclass_name);
        EntityPropertyBase** propertys = pre->get_propertys();
        propertys[prop_idxs[self_name]] = this;
    }
}

void EntityPropertyBase::update(const char* v) {
    ASSERT_LOG(GString(typeid(GString).name()) == this->get_v_tstring(),
        "update type.%s error, should be %s\n", typeid(GString).name(), this->get_v_tstring().c_str());

    EntityPropertySimple<GString>* child = dynamic_cast<EntityPropertySimple<GString>*>(this);
    child->update(GString(v));
}

void EntityPropertyBase::push_back(char* v) {
    push_back((const char*)v);
}

void EntityPropertyBase::push_back(const char* v) {

    ASSERT_LOG(GString(typeid(GString).name()) == this->get_v_tstring(),
        "push_back type.%s error, should be %s\n", typeid(GString).name(), this->get_v_tstring().c_str());

    EntityPropertyArray<GString>* child = dynamic_cast<EntityPropertyArray<GString>*>(this);
    child->push_back(GString(v));
}

void EntityPropertyBase::update(const int32_t idx, char* v) {
    update(idx, (const char*)v);
}

void EntityPropertyBase::update(const int32_t idx, const char* v) {

    ASSERT_LOG(GString(typeid(GString).name()) == this->get_v_tstring(),
        "update type.%s error, should be %s\n", typeid(GString).name(), this->get_v_tstring().c_str());

    EntityPropertyArray<GString>* child = dynamic_cast<EntityPropertyArray<GString>*>(this);
    child->update(idx, GString(v));
}

void EntityPropertyBase::insert(GString k, char* v) {
    insert(k, (const char*)v);
}

void EntityPropertyBase::insert(GString k, const char* v) {

    ASSERT_LOG(GString(typeid(GString).name()) == this->get_v_tstring(),
        "insert type.%s error, should be %s\n", typeid(GString).name(), this->get_v_tstring().c_str());

    EntityPropertyMap<GString>* child = dynamic_cast<EntityPropertyMap<GString>*>(this);
    child->insert(k, GString(v));
}

void EntityPropertyBase::update(GString k, char* v) {
    update(k, (const char*)(v));
}

void EntityPropertyBase::update(GString k, const char* v) {
    ASSERT_LOG(GString(typeid(GString).name()) == this->get_v_tstring(),
        "insert type.%s error, should be %s\n", typeid(GString).name(), this->get_v_tstring().c_str());

    EntityPropertyMap<GString>* child = dynamic_cast<EntityPropertyMap<GString>*>(this);
    child->update(k, GString(v));
}
