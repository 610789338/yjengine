#include "entity_property_manager.h"
#include "entity.h"


PropIdxType* g_all_prop_idxs = nullptr;

PropIdxType& get_all_prop_idxs() {
    if (g_all_prop_idxs == nullptr) {
        g_all_prop_idxs = new PropIdxType;
    }

    return *g_all_prop_idxs;
}

bool prop_regist_check(int8_t t, enum PropType type) {

    switch (EntityType(t)) {
    case EntityType::EntityType_Base:
    case EntityType::EntityType_BaseWithCell:
    case EntityType::EntityType_BaseWithCellAndClient: {
        if (type == PropType::ALL ||
            type == PropType::BASE_PRIVATE ||
            type == PropType::BASE_AND_CLIENT ||
            type == PropType::ALL_CLIENT) {
            return true;
        }

        break;
    }

    case EntityType::EntityType_Cell:
    case EntityType::EntityType_CellWithClient: {
        if (type == PropType::ALL ||
            type == PropType::CELL_PRIVATE ||
            type == PropType::CELL_PUBLIC ||
            type == PropType::CELL_AND_CLIENT ||
            type == PropType::ALL_CLIENT) {
            return true;
        }

        break;
    }

    case EntityType::EntityType_Client: {
        if (type == PropType::ALL ||
            type == PropType::BASE_AND_CLIENT ||
            type == PropType::CELL_AND_CLIENT ||
            type == PropType::ALL_CLIENT) {
            return true;
        }

        break;
    }
    }

    return false;
}

void EntityPropertyBase::link_node(EntityPropertyBase* pre_node, const GString& self_name) {
    if (pre_node != nullptr) {
        EntityPropertyComplex* parent = (EntityPropertyComplex*)pre_node;
        parent->gen_prop_idxs();

        PropIdxType& all_prop_idxs = get_all_prop_idxs();
        auto pclass_name = parent->get_pclass_name();
        auto prop_idxs = all_prop_idxs.at(pclass_name);
        EntityPropertyBase** propertys = parent->get_propertys();
        propertys[prop_idxs[self_name]] = this;

        set_parent(pre_node);
    }
}

void EntityPropertyBase::set_dirty() {
    if (is_dirty()) {
        return;
    }

    flag |= 0x80;

    if (parent == nullptr) {
        return;
    }

    if (is_all_dirty()) {
        return;
    }

    if (!is_first_level()) {
        if (!parent->is_dirty()) {
            parent->set_dirty();
        }
    }
    else {
        Entity* owner = (Entity*)parent;
        owner->dirty_propertys.insert(make_pair(owner->propertys_turn.at(this), this));
    }
}

void EntityPropertyBase::set_all_dirty() {
    if (is_all_dirty()) {
        return;
    }

    flag |= 0x40;

    if (parent == nullptr) {
        return;
    }

    if (is_dirty()) {
        return;
    }

    if (!is_first_level()) {
        if (!parent->is_dirty()) {
            parent->set_dirty();
        }
    }
    else {
        Entity* owner = (Entity*)parent;
        owner->dirty_propertys.insert(make_pair(owner->propertys_turn.at(this), this));
    }
}

bool EntityPropertyBase::need_sync2client() {

    const auto& prop_type = get_prop_type();

    if (prop_type == PropType::BASE_AND_CLIENT ||
        prop_type == PropType::CELL_AND_CLIENT ||
        prop_type == PropType::ALL_CLIENT ||
        prop_type == PropType::OTHER_CLIENT ||
        prop_type == PropType::ALL) {
        return true;
    }

    return false;
}

bool EntityPropertyBase::need_sync2otherclient() {

    const auto& prop_type = get_prop_type();

    if (prop_type == PropType::ALL_CLIENT ||
        prop_type == PropType::OTHER_CLIENT ||
        prop_type == PropType::ALL) {
        return true;
    }

    return false;
}

bool EntityPropertyBase::need_sync2db() {
    // TODO
    return true;
}

// ------------------------------------------------------------------------------- //
// ------------------------------------ simple ----------------------------------- //
// ------------------------------------------------------------------------------- //

void EntityPropertyBase::update_int(uint64_t v) {

    if (this->get_v_tstring() == GString(typeid(int8_t).name())) {
        EntityPropertySimple<int8_t>* child = dynamic_cast<EntityPropertySimple<int8_t>*>(this);
        child->update((int8_t)v);
    }
    else if (this->get_v_tstring() == GString(typeid(int16_t).name())) {
        EntityPropertySimple<int16_t>* child = dynamic_cast<EntityPropertySimple<int16_t>*>(this);
        child->update((int16_t)v);
    }
    else if (this->get_v_tstring() == GString(typeid(int32_t).name())) {
        EntityPropertySimple<int32_t>* child = dynamic_cast<EntityPropertySimple<int32_t>*>(this);
        child->update((int32_t)v);
    }
    else if (this->get_v_tstring() == GString(typeid(int64_t).name())) {
        EntityPropertySimple<int64_t>* child = dynamic_cast<EntityPropertySimple<int64_t>*>(this);
        child->update((int64_t)v);
    }
    else if (this->get_v_tstring() == GString(typeid(uint8_t).name())) {
        EntityPropertySimple<uint8_t>* child = dynamic_cast<EntityPropertySimple<uint8_t>*>(this);
        child->update((uint8_t)v);
    }
    else if (this->get_v_tstring() == GString(typeid(uint16_t).name())) {
        EntityPropertySimple<uint16_t>* child = dynamic_cast<EntityPropertySimple<uint16_t>*>(this);
        child->update((uint16_t)v);
    }
    else if (this->get_v_tstring() == GString(typeid(uint32_t).name())) {
        EntityPropertySimple<uint32_t>* child = dynamic_cast<EntityPropertySimple<uint32_t>*>(this);
        child->update((uint32_t)v);
    }
    else if (this->get_v_tstring() == GString(typeid(uint64_t).name())) {
        EntityPropertySimple<uint64_t>* child = dynamic_cast<EntityPropertySimple<uint64_t>*>(this);
        child->update((uint64_t)v);
    }
    else {
        ASSERT_LOG(false, "update type.%s error, should be %s\n", typeid(int).name(), this->get_v_tstring().c_str());
    }
}

void EntityPropertyBase::update_float(double v) {

    if (this->get_v_tstring() == GString(typeid(float).name())) {
        EntityPropertySimple<float>* child = dynamic_cast<EntityPropertySimple<float>*>(this);
        child->update((float)v);
    }
    else if (this->get_v_tstring() == GString(typeid(double).name())) {
        EntityPropertySimple<double>* child = dynamic_cast<EntityPropertySimple<double>*>(this);
        child->update(v);
    }
    else {
        ASSERT_LOG(false, "update type.%s error, should be %s\n", typeid(float).name(), this->get_v_tstring().c_str());
    }
}

void EntityPropertyBase::update_cstr(const char*  v) {

    ASSERT_LOG(GString(typeid(GString).name()) == this->get_v_tstring(),
        "update type.%s error, should be %s\n", typeid(GString).name(), this->get_v_tstring().c_str());

    EntityPropertySimple<GString>* child = dynamic_cast<EntityPropertySimple<GString>*>(this);
    child->update(GString(v));
}

// ------------------------------------------------------------------------------- //
// ------------------------------------ array ------------------------------------ //
// ------------------------------------------------------------------------------- //

void EntityPropertyBase::push_back_int(uint64_t v) {

    if (this->get_v_tstring() == GString(typeid(int8_t).name())) {
        EntityPropertyArray<int8_t>* child = dynamic_cast<EntityPropertyArray<int8_t>*>(this);
        child->push_back((int8_t)v);
    }
    else if (this->get_v_tstring() == GString(typeid(int16_t).name())) {
        EntityPropertyArray<int16_t>* child = dynamic_cast<EntityPropertyArray<int16_t>*>(this);
        child->push_back((int16_t)v);
    }
    else if (this->get_v_tstring() == GString(typeid(int32_t).name())) {
        EntityPropertyArray<int32_t>* child = dynamic_cast<EntityPropertyArray<int32_t>*>(this);
        child->push_back((int32_t)v);
    }
    else if (this->get_v_tstring() == GString(typeid(int64_t).name())) {
        EntityPropertyArray<int64_t>* child = dynamic_cast<EntityPropertyArray<int64_t>*>(this);
        child->push_back((int64_t)v);
    }
    else if (this->get_v_tstring() == GString(typeid(uint8_t).name())) {
        EntityPropertyArray<uint8_t>* child = dynamic_cast<EntityPropertyArray<uint8_t>*>(this);
        child->push_back((uint8_t)v);
    }
    else if (this->get_v_tstring() == GString(typeid(uint16_t).name())) {
        EntityPropertyArray<uint16_t>* child = dynamic_cast<EntityPropertyArray<uint16_t>*>(this);
        child->push_back((uint16_t)v);
    }
    else if (this->get_v_tstring() == GString(typeid(uint32_t).name())) {
        EntityPropertyArray<uint32_t>* child = dynamic_cast<EntityPropertyArray<uint32_t>*>(this);
        child->push_back((uint32_t)v);
    }
    else if (this->get_v_tstring() == GString(typeid(uint64_t).name())) {
        EntityPropertyArray<uint64_t>* child = dynamic_cast<EntityPropertyArray<uint64_t>*>(this);
        child->push_back((uint64_t)v);
    }
    else {
        ASSERT_LOG(false, "push_back type.%s error, should be %s\n", typeid(int).name(), this->get_v_tstring().c_str());
    }
}

void EntityPropertyBase::push_back_float(double v) {

    if (this->get_v_tstring() == GString(typeid(float).name())) {
        EntityPropertyArray<float>* child = dynamic_cast<EntityPropertyArray<float>*>(this);
        child->push_back((float)v);
    }
    else if (this->get_v_tstring() == GString(typeid(double).name())) {
        EntityPropertyArray<double>* child = dynamic_cast<EntityPropertyArray<double>*>(this);
        child->push_back(v);
    }
    else {
        ASSERT_LOG(false, "push_back type.%s error, should be %s\n", typeid(float).name(), this->get_v_tstring().c_str());
    }
}

void EntityPropertyBase::push_back_cstr(const char*  v) {

    ASSERT_LOG(GString(typeid(GString).name()) == this->get_v_tstring(),
        "push_back type.%s error, should be %s\n", typeid(GString).name(), this->get_v_tstring().c_str());

    EntityPropertyArray<GString>* child = dynamic_cast<EntityPropertyArray<GString>*>(this);
    child->push_back(GString(v));
}

void EntityPropertyBase::update_int(int32_t idx, uint64_t v) {

    if (this->get_v_tstring() == GString(typeid(int8_t).name())) {
        EntityPropertyArray<int8_t>* child = dynamic_cast<EntityPropertyArray<int8_t>*>(this);
        child->update(idx, (int8_t)v);
    }
    else if (this->get_v_tstring() == GString(typeid(int16_t).name())) {
        EntityPropertyArray<int16_t>* child = dynamic_cast<EntityPropertyArray<int16_t>*>(this);
        child->update(idx, (int16_t)v);
    }
    else if (this->get_v_tstring() == GString(typeid(int32_t).name())) {
        EntityPropertyArray<int32_t>* child = dynamic_cast<EntityPropertyArray<int32_t>*>(this);
        child->update(idx, (int32_t)v);
    }
    else if (this->get_v_tstring() == GString(typeid(int64_t).name())) {
        EntityPropertyArray<int64_t>* child = dynamic_cast<EntityPropertyArray<int64_t>*>(this);
        child->update(idx, (int64_t)v);
    }
    else if (this->get_v_tstring() == GString(typeid(uint8_t).name())) {
        EntityPropertyArray<uint8_t>* child = dynamic_cast<EntityPropertyArray<uint8_t>*>(this);
        child->update(idx, (uint8_t)v);
    }
    else if (this->get_v_tstring() == GString(typeid(uint16_t).name())) {
        EntityPropertyArray<uint16_t>* child = dynamic_cast<EntityPropertyArray<uint16_t>*>(this);
        child->update(idx, (uint16_t)v);
    }
    else if (this->get_v_tstring() == GString(typeid(uint32_t).name())) {
        EntityPropertyArray<uint32_t>* child = dynamic_cast<EntityPropertyArray<uint32_t>*>(this);
        child->update(idx, (uint32_t)v);
    }
    else if (this->get_v_tstring() == GString(typeid(uint64_t).name())) {
        EntityPropertyArray<uint64_t>* child = dynamic_cast<EntityPropertyArray<uint64_t>*>(this);
        child->update(idx, (uint64_t)v);
    }
    else {
        ASSERT_LOG(false, "update type.%s error, should be %s\n", typeid(int).name(), this->get_v_tstring().c_str());
    }
}

void EntityPropertyBase::update_float(int32_t idx, double v) {

    if (this->get_v_tstring() == GString(typeid(float).name())) {
        EntityPropertyArray<float>* child = dynamic_cast<EntityPropertyArray<float>*>(this);
        child->update(idx, (float)v);
    }
    else if (this->get_v_tstring() == GString(typeid(double).name())) {
        EntityPropertyArray<double>* child = dynamic_cast<EntityPropertyArray<double>*>(this);
        child->update(idx, v);
    }
    else {
        ASSERT_LOG(false, "update type.%s error, should be %s\n", typeid(float).name(), this->get_v_tstring().c_str());
    }
}

void EntityPropertyBase::update_cstr(int32_t idx, const char*  v) {

    ASSERT_LOG(GString(typeid(GString).name()) == this->get_v_tstring(),
        "update type.%s error, should be %s\n", typeid(GString).name(), this->get_v_tstring().c_str());

    EntityPropertyArray<GString>* child = dynamic_cast<EntityPropertyArray<GString>*>(this);
    child->update(idx, GString(v));
}

// ------------------------------------------------------------------------------- //
// ------------------------------------- map ------------------------------------- //
// ------------------------------------------------------------------------------- //

void EntityPropertyBase::insert_int(const GString& k, uint64_t v) {

    if (this->get_v_tstring() == GString(typeid(int8_t).name())) {
        EntityPropertyMap<int8_t>* child = dynamic_cast<EntityPropertyMap<int8_t>*>(this);
        child->insert(k, (int8_t)v);
    }
    else if (this->get_v_tstring() == GString(typeid(int16_t).name())) {
        EntityPropertyMap<int16_t>* child = dynamic_cast<EntityPropertyMap<int16_t>*>(this);
        child->insert(k, (int16_t)v);
    }
    else if (this->get_v_tstring() == GString(typeid(int32_t).name())) {
        EntityPropertyMap<int32_t>* child = dynamic_cast<EntityPropertyMap<int32_t>*>(this);
        child->insert(k, (int32_t)v);
    }
    else if (this->get_v_tstring() == GString(typeid(int64_t).name())) {
        EntityPropertyMap<int64_t>* child = dynamic_cast<EntityPropertyMap<int64_t>*>(this);
        child->insert(k, (int64_t)v);
    }
    else if (this->get_v_tstring() == GString(typeid(uint8_t).name())) {
        EntityPropertyMap<uint8_t>* child = dynamic_cast<EntityPropertyMap<uint8_t>*>(this);
        child->insert(k, (uint8_t)v);
    }
    else if (this->get_v_tstring() == GString(typeid(uint16_t).name())) {
        EntityPropertyMap<uint16_t>* child = dynamic_cast<EntityPropertyMap<uint16_t>*>(this);
        child->insert(k, (uint16_t)v);
    }
    else if (this->get_v_tstring() == GString(typeid(uint32_t).name())) {
        EntityPropertyMap<uint32_t>* child = dynamic_cast<EntityPropertyMap<uint32_t>*>(this);
        child->insert(k, (uint32_t)v);
    }
    else if (this->get_v_tstring() == GString(typeid(uint64_t).name())) {
        EntityPropertyMap<uint64_t>* child = dynamic_cast<EntityPropertyMap<uint64_t>*>(this);
        child->insert(k, (uint64_t)v);
    }
    else {
        ASSERT_LOG(false, "insert type.%s error, should be %s\n", typeid(int).name(), this->get_v_tstring().c_str());
    }
}

void EntityPropertyBase::insert_float(const GString& k, double v) {

    if (this->get_v_tstring() == GString(typeid(float).name())) {
        EntityPropertyMap<float>* child = dynamic_cast<EntityPropertyMap<float>*>(this);
        child->insert(k, (float)v);
    }
    else if (this->get_v_tstring() == GString(typeid(double).name())) {
        EntityPropertyMap<double>* child = dynamic_cast<EntityPropertyMap<double>*>(this);
        child->insert(k, v);
    }
    else {
        ASSERT_LOG(false, "insert type.%s error, should be %s\n", typeid(float).name(), this->get_v_tstring().c_str());
    }
}

void EntityPropertyBase::insert_cstr(const GString& k, const char*  v) {

    ASSERT_LOG(GString(typeid(GString).name()) == this->get_v_tstring(),
        "insert type.%s error, should be %s\n", typeid(GString).name(), this->get_v_tstring().c_str());

    EntityPropertyMap<GString>* child = dynamic_cast<EntityPropertyMap<GString>*>(this);
    child->insert(k, GString(v));
}

void EntityPropertyBase::update_int(const GString& k, uint64_t v) {

    if (this->get_v_tstring() == GString(typeid(int8_t).name())) {
        EntityPropertyMap<int8_t>* child = dynamic_cast<EntityPropertyMap<int8_t>*>(this);
        child->update(k, (int8_t)v);
    }
    else if (this->get_v_tstring() == GString(typeid(int16_t).name())) {
        EntityPropertyMap<int16_t>* child = dynamic_cast<EntityPropertyMap<int16_t>*>(this);
        child->update(k, (int16_t)v);
    }
    else if (this->get_v_tstring() == GString(typeid(int32_t).name())) {
        EntityPropertyMap<int32_t>* child = dynamic_cast<EntityPropertyMap<int32_t>*>(this);
        child->update(k, (int32_t)v);
    }
    else if (this->get_v_tstring() == GString(typeid(int64_t).name())) {
        EntityPropertyMap<int64_t>* child = dynamic_cast<EntityPropertyMap<int64_t>*>(this);
        child->update(k, (int64_t)v);
    }
    else if (this->get_v_tstring() == GString(typeid(uint8_t).name())) {
        EntityPropertyMap<uint8_t>* child = dynamic_cast<EntityPropertyMap<uint8_t>*>(this);
        child->update(k, (uint8_t)v);
    }
    else if (this->get_v_tstring() == GString(typeid(uint16_t).name())) {
        EntityPropertyMap<uint16_t>* child = dynamic_cast<EntityPropertyMap<uint16_t>*>(this);
        child->update(k, (uint16_t)v);
    }
    else if (this->get_v_tstring() == GString(typeid(uint32_t).name())) {
        EntityPropertyMap<uint32_t>* child = dynamic_cast<EntityPropertyMap<uint32_t>*>(this);
        child->update(k, (uint32_t)v);
    }
    else if (this->get_v_tstring() == GString(typeid(uint64_t).name())) {
        EntityPropertyMap<uint64_t>* child = dynamic_cast<EntityPropertyMap<uint64_t>*>(this);
        child->update(k, (uint64_t)v);
    }
    else {
        ASSERT_LOG(false, "update type.%s error, should be %s\n", typeid(int).name(), this->get_v_tstring().c_str());
    }
}

void EntityPropertyBase::update_float(const GString& k, double v) {

    if (this->get_v_tstring() == GString(typeid(float).name())) {
        EntityPropertyMap<float>* child = dynamic_cast<EntityPropertyMap<float>*>(this);
        child->update(k, (float)v);
    }
    else if (this->get_v_tstring() == GString(typeid(double).name())) {
        EntityPropertyMap<double>* child = dynamic_cast<EntityPropertyMap<double>*>(this);
        child->update(k, v);
    }
    else {
        ASSERT_LOG(false, "update type.%s error, should be %s\n", typeid(float).name(), this->get_v_tstring().c_str());
    }
}

void EntityPropertyBase::update_cstr(const GString& k, const char*  v) {

    ASSERT_LOG(GString(typeid(GString).name()) == this->get_v_tstring(),
        "update type.%s error, should be %s\n", typeid(GString).name(), this->get_v_tstring().c_str());

    EntityPropertyMap<GString>* child = dynamic_cast<EntityPropertyMap<GString>*>(this);
    child->update(k, GString(v));
}

void EntityPropertyComplex::serialize(Encoder& encoder, bool need_clean_dirty) {
    if (is_all_dirty())
        serialize_all(encoder, need_clean_dirty);
    else
        serialize_dirty(encoder, need_clean_dirty);
}

void EntityPropertyComplex::serialize_all(Encoder& encoder, bool need_clean_dirty) {
    encoder.write_string("y");
    auto const& propertys = get_propertys();
    for (int8_t i = 0; i < get_propertys_len(); ++i) {
        propertys[i]->serialize_all(encoder, need_clean_dirty);
    }
    if(need_clean_dirty) clean_dirty();
}

void EntityPropertyComplex::serialize_dirty(Encoder& encoder, bool need_clean_dirty) {
    encoder.write_string("j");
    auto const& propertys = get_propertys();

    int8_t num = 0;
    for (int8_t i = 0; i < get_propertys_len(); ++i) {
        if (!propertys[i]->is_dirty() && !propertys[i]->is_all_dirty()) {
            continue;
        }
        ++num;
    }
    encoder.write_int8(num);

    for (int8_t i = 0; i < get_propertys_len(); ++i) {
        if (!propertys[i]->is_dirty() && !propertys[i]->is_all_dirty()) {
            continue;
        }
        encoder.write_int8(i);
        propertys[i]->serialize(encoder, need_clean_dirty);
    }
    if(need_clean_dirty) clean_dirty();
}

void EntityPropertyComplex::unserialize(Decoder& decoder) {
    bool is_unserialize_all = decoder.read_string() == "y";
    auto const& propertys = get_propertys();

    if (is_unserialize_all) {
        for (int8_t i = 0; i < get_propertys_len(); ++i) {
            propertys[i]->unserialize(decoder);
        }
    }
    else {
        int8_t num = decoder.read_int8();
        while (num > 0) {
            int8_t i = decoder.read_int8();
            propertys[i]->unserialize(decoder);
            --num;
        }
    }
}
