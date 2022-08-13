#pragma once

#include <unordered_map>
#include <functional>

#include "gvalue.h"
#include "log.h"

using namespace std;

enum PropType {
    BASE,
    BASE_AND_CLIENT,
    CELL_PRIVATE,
    CELL_PUBLIC,
    CELL_AND_CLIENT,
    ALL_CLIENT,
    OTHER_CLIENT,

    NONE
};


struct EntityPropertyBase {

    EntityPropertyBase() = delete;
    EntityPropertyBase(const PropType& t) : type(t) {}
    ~EntityPropertyBase() {}

    void link_node(EntityPropertyBase* pre_node, GString self_name);

    virtual void encode() {}
    virtual void decode() {}

    PropType type = PropType::BASE;
    bool dirty = false;
};

typedef function<void(EntityPropertyBase* mem)> TCallBack;

template<class T>
struct EntityPropertySimple : public EntityPropertyBase {

    EntityPropertySimple() = delete;
    EntityPropertySimple(const TCallBack& cb, const PropType& t, T _v) : EntityPropertyBase(t), v(_v) { cb(this); }

    GValue v;
};

struct EntityPropertyComplex : public EntityPropertyBase {

    EntityPropertyComplex() : EntityPropertyBase(PropType::NONE) {}
    //EntityPropertyComplex(const TCallBack& cb, const PropType& t) : EntityPropertyBase(t) { cb(this); }
    void excute(const TCallBack& cb, const PropType& t) { type = t; cb(this); }

    unordered_map<GString, EntityPropertyBase*> propertys;
};

template<class TValue>
struct EntityPropertyArray : public EntityPropertyBase {

    EntityPropertyArray() = delete;
    EntityPropertyArray(const TCallBack& cb, const PropType& t) : EntityPropertyBase(t) { cb(this); }

    vector<TValue*> propertys;
};

template<class TValue>
struct EntityPropertyMap : public EntityPropertyBase {

    EntityPropertyMap() = delete;
    EntityPropertyMap(const TCallBack& cb, const PropType& t) : EntityPropertyBase(t) { cb(this); }

    unordered_map<GString, TValue*> propertys;
};


// entity property manager
template<class EntityClassType>
class EntityPropertyManager {

public:
    EntityPropertyManager() {
        EntityClassType::property_define();
    }

    template<class T>
    void regist_simple_property(enum PropType type, const GString& property_name, T _default) {
        ASSERT_LOG(propertys.find(property_name) == propertys.end(), "prop.%s exist\n", property_name.c_str());
        propertys[property_name] = new EntityPropertySimple<T>([](EntityPropertyBase* mem) {}, type, _default);
    }

    template<class T>
    void regist_complex_property(enum PropType type, const GString& property_name) {
        ASSERT_LOG(propertys.find(property_name) == propertys.end(), "prop.%s exist\n", property_name.c_str());
        EntityPropertyComplex* v = new T();
        v->excute([](EntityPropertyBase* mem) {}, type);
        propertys[property_name] = v;
    }

    unordered_map<GString, EntityPropertyBase*> propertys;
};

#define PROPERTY_SIMPLE(prop_type, property_name, prop_class, default_value) \
    property_manager.regist_simple_property<prop_class>(prop_type, #property_name, default_value)
#define PROPERTY_COMPLEX(prop_type, property_name, prop_class) \
    property_manager.regist_complex_property<prop_class>(prop_type, #property_name)
#define PROPERTY_ARRAY(prop_type, property_name, prop_class) \
    property_manager.regist_complex_property<EntityPropertyArray<prop_class>>(prop_type, #property_name)
#define PROPERTY_MAP(prop_type, property_name, prop_class) \
    property_manager.regist_complex_property<EntityPropertyMap<prop_class>>(prop_type, #property_name)


#define MEM_PROPERTY_SIMPLE(property_name, prop_class, default_value) \
    EntityPropertySimple<prop_class> property_name = EntityPropertySimple<prop_class>([this](EntityPropertyBase* mem){ mem->link_node(this, #property_name);}, PropType::NONE, default_value)
#define MEM_PROPERTY_COMPLEX(property_name, prop_class) \
    prop_class property_name = prop_class([this](EntityPropertyBase* mem){ mem->link_node(this, #property_name);}, PropType::NONE)
#define MEM_PROPERTY_ARRAY(property_name, prop_class) \
    EntityPropertyArray<prop_class> property_name = EntityPropertyArray<prop_class>([this](EntityPropertyBase* mem){ mem->link_node(this, #property_name);}, PropType::BASE)
#define MEM_PROPERTY_MAP(property_name, prop_class) \
    EntityPropertyMap<prop_class> property_name = EntityPropertyMap<prop_class>([this](EntityPropertyBase* mem){ mem->link_node(this, #property_name);}, PropType::NONE)
