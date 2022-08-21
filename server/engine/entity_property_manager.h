#pragma once

#include <unordered_map>
#include <functional>
#include <typeinfo>

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

static GValue yj(true);

struct EntityPropertyBase {

    EntityPropertyBase() = delete;
    EntityPropertyBase(const PropType& t) : type(t) {}
    virtual ~EntityPropertyBase() {}

    // get container value type string
    virtual GString get_v_tstring() { ASSERT(false); return ""; }

    void link_node(EntityPropertyBase* pre_node, GString self_name);

    // simple property
    virtual bool as_bool() const { ASSERT(false); return bool(); }
    virtual int8_t as_int8() const { ASSERT(false); return int8_t(); }
    virtual int16_t as_int16() const { ASSERT(false); return int16_t(); }
    virtual int32_t as_int32() const { ASSERT(false); return int32_t(); }
    virtual int64_t as_int64() const { ASSERT(false); return int64_t(); }
    virtual uint8_t as_uint8() const { ASSERT(false); return uint8_t(); }
    virtual uint16_t as_uint16() const { ASSERT(false); return uint16_t(); }
    virtual uint32_t as_uint32() const { ASSERT(false); return uint32_t(); }
    virtual uint64_t as_uint64() const { ASSERT(false); return uint64_t(); }
    virtual float as_float() const { ASSERT(false); return float(); }
    virtual double as_double() const { ASSERT(false); return double(); }
    virtual GString& as_string() const { ASSERT(false); static GString ret; return ret; }

    template<class T>
    void update(T v);
    void update(const char* v);

    // array property - crud
    template<class T>
    void push_back(T v);
    void push_back(char* v);
    void push_back(const char* v);

    virtual void pop_back() { ASSERT(false); }

    template<class T>
    void update(const int32_t idx, T v);
    void update(const int32_t idx, char* v);
    void update(const int32_t idx, const char* v);

    virtual EntityPropertyBase* get(const int32_t idx) const { ASSERT(false); return nullptr; }

    // map property - crud
    template<class T>
    void insert(GString k, T v);
    void insert(GString k, char* v);
    void insert(GString k, const char* v);

    virtual void erase(GString k) {}

    template<class T>
    void update(GString k, T v);
    void update(GString k, char* v);
    void update(GString k, const char* v);

    virtual EntityPropertyBase* get(const GString& prop_name) const { ASSERT(false); return nullptr; }
    virtual vector<GString> keys() { ASSERT(false); return vector<GString>(); }

    virtual int32_t size() { ASSERT(false); return 0; }

    virtual void encode() {}
    virtual void decode() {}

    PropType type = PropType::BASE;
    bool dirty = false;
};

typedef function<void(EntityPropertyBase* mem)> TCallBack;

template<class T>
struct EntityPropertySimple : public EntityPropertyBase {

    EntityPropertySimple() : EntityPropertyBase(PropType::NONE), v(T()) {}
    EntityPropertySimple(const EntityPropertySimple& other) : EntityPropertyBase(other.type), v(other.v) {};
    EntityPropertySimple(T _v) : EntityPropertyBase(PropType::NONE), v(_v) {}
    EntityPropertySimple(const TCallBack& cb, const PropType& t, T _v) : EntityPropertyBase(t), v(_v) { cb(this); }

    EntityPropertySimple& operator=(T _v) { type = PropType::NONE; v = _v; return *this; }

    GString get_v_tstring() { return typeid(T).name(); }
    void update(T _v) { v = _v; }

    bool as_bool() const { return v.as_bool(); }
    int8_t as_int8() const { return v.as_int8(); }
    int16_t as_int16() const { return v.as_int16(); }
    int32_t as_int32() const { return v.as_int32(); }
    int64_t as_int64() const { return v.as_int64(); }
    uint8_t as_uint8() const { return v.as_uint8(); }
    uint16_t as_uint16() const { return v.as_uint16(); }
    uint32_t as_uint32() const { return v.as_uint32(); }
    uint64_t as_uint64() const { return v.as_uint64(); }
    float as_float() const { return v.as_float(); }
    double as_double() const { return v.as_double(); }
    GString& as_string() const { return v.as_string(); }

    GValue v;
};

template<class T>
void EntityPropertyBase::update(T v) {

    ASSERT_LOG(GString(typeid(T).name()) == this->get_v_tstring(),
        "update type.%s error, should be %s\n", typeid(T).name(), this->get_v_tstring().c_str());

    EntityPropertySimple<T>* child = dynamic_cast<EntityPropertySimple<T>*>(this);
    child->update(v);
}

struct EntityPropertyComplex : public EntityPropertyBase {

    EntityPropertyComplex() = delete;
    EntityPropertyComplex(const TCallBack& cb, const PropType& t) : EntityPropertyBase(t) { cb(this); }
    
    EntityPropertyBase* get(const GString& prop_name) const { return propertys.at(prop_name); }

    unordered_map<GString, EntityPropertyBase*> propertys;
};

template<class TValue>
struct EntityPropertyArray : public EntityPropertyBase {

    EntityPropertyArray() = delete;
    EntityPropertyArray(const TCallBack& cb, const PropType& t) : EntityPropertyBase(t) { cb(this); }

    GString get_v_tstring() { return typeid(TValue).name(); }

    // crud
    void push_back(TValue v) { propertys.push_back(v); }
    void pop_back() { propertys.pop_back(); }
    void update(const int32_t idx, TValue v) { propertys[idx] = v; }
    EntityPropertyBase* get(const int32_t idx) const { return (EntityPropertyBase*)&propertys[idx]; }

    int32_t size() { return (int32_t)propertys.size(); }

    vector<TValue> propertys;
};

#define ENTITY_PROPERTY_ARRAY_PARTIAL(T) \
template<> \
struct EntityPropertyArray<T> : public EntityPropertyBase { \
\
    EntityPropertyArray<T>() = delete; \
    EntityPropertyArray<T>(const TCallBack& cb, const PropType& t) : EntityPropertyBase(t) { cb(this); } \
\
    GString get_v_tstring() { return typeid(T).name(); } \
\
    /* crud */ \
    void push_back(T v) { propertys.push_back(EntityPropertySimple<T>(v)); } \
    void pop_back() { propertys.pop_back(); } \
    EntityPropertyBase* get(const int32_t idx) const { return (EntityPropertyBase*)&propertys[idx]; } \
    void update(const int32_t idx, T v) { propertys[idx] = EntityPropertySimple<T>(v); } \
\
    int32_t size() { return (int32_t)propertys.size(); } \
\
    vector<EntityPropertySimple<T>> propertys; \
};

ENTITY_PROPERTY_ARRAY_PARTIAL(bool)
ENTITY_PROPERTY_ARRAY_PARTIAL(int8_t)
ENTITY_PROPERTY_ARRAY_PARTIAL(int16_t)
ENTITY_PROPERTY_ARRAY_PARTIAL(int32_t)
ENTITY_PROPERTY_ARRAY_PARTIAL(int64_t)
ENTITY_PROPERTY_ARRAY_PARTIAL(uint8_t)
ENTITY_PROPERTY_ARRAY_PARTIAL(uint16_t)
ENTITY_PROPERTY_ARRAY_PARTIAL(uint32_t)
ENTITY_PROPERTY_ARRAY_PARTIAL(uint64_t)
ENTITY_PROPERTY_ARRAY_PARTIAL(GString)
ENTITY_PROPERTY_ARRAY_PARTIAL(float)
ENTITY_PROPERTY_ARRAY_PARTIAL(double)
ENTITY_PROPERTY_ARRAY_PARTIAL(GBin)

template<class T>
void EntityPropertyBase::push_back(T v) {

    ASSERT_LOG(GString(typeid(T).name()) == this->get_v_tstring(), 
        "push_back type.%s error, should be %s\n", typeid(T).name(), this->get_v_tstring().c_str());

    EntityPropertyArray<T>* child = dynamic_cast<EntityPropertyArray<T>*>(this);
    child->push_back(v);
}

template<class T>
void EntityPropertyBase::update(const int32_t idx, T v) {

    ASSERT_LOG(GString(typeid(T).name()) == this->get_v_tstring(),
        "update type.%s error, should be %s\n", typeid(T).name(), this->get_v_tstring().c_str());

    EntityPropertyArray<T>* child = dynamic_cast<EntityPropertyArray<T>*>(this);
    child->update(idx, v);
}

template<class TValue>
struct EntityPropertyMap : public EntityPropertyBase {

    EntityPropertyMap() = delete;
    EntityPropertyMap(const TCallBack& cb, const PropType& t) : EntityPropertyBase(t) { cb(this); }

    GString get_v_tstring() { return typeid(TValue).name(); }

    // crud
    void insert(GString k, TValue v) { propertys.insert(make_pair(k, v)); }
    void erase(GString k) { propertys.erase(k); }
    void update(GString k, TValue v) { propertys[k] = v; }
    EntityPropertyBase* get(const GString& prop_name) const { return (EntityPropertyBase*)&propertys.at(prop_name); }

    int32_t size() { return (int32_t)propertys.size(); }
    vector<GString> keys() { 
        vector<GString> ret; 
        for (auto iter = propertys.begin(); iter != propertys.end(); ++iter)
            ret.push_back(iter->first);
        return ret;
    }

    unordered_map<GString, TValue> propertys;
};

#define ENTITY_PROPERTY_MAP_PARTIAL(T) \
template<> \
struct EntityPropertyMap<T> : public EntityPropertyBase { \
\
    EntityPropertyMap<T>() = delete; \
    EntityPropertyMap<T>(const TCallBack& cb, const PropType& t) : EntityPropertyBase(t) { cb(this); } \
\
    GString get_v_tstring() { return typeid(T).name(); } \
\
    /* crud */ \
    void insert(GString k, T v) { propertys.insert(make_pair(k, EntityPropertySimple<T>(v))); } \
    void erase(GString k) { propertys.erase(k); } \
    void update(GString k, T v) { propertys[k] = EntityPropertySimple<T>(v); } \
    EntityPropertyBase* get(const GString& prop_name) const { return (EntityPropertyBase*)&propertys.at(prop_name); } \
\
    int32_t size() { return (int32_t)propertys.size(); } \
    vector<GString> keys() { \
        vector<GString> ret; \
        for (auto iter = propertys.begin(); iter != propertys.end(); ++iter) \
            ret.push_back(iter->first); \
        return ret; \
    } \
\
    unordered_map<GString, EntityPropertySimple<T>> propertys; \
};

ENTITY_PROPERTY_MAP_PARTIAL(bool)
ENTITY_PROPERTY_MAP_PARTIAL(int8_t)
ENTITY_PROPERTY_MAP_PARTIAL(int16_t)
ENTITY_PROPERTY_MAP_PARTIAL(int32_t)
ENTITY_PROPERTY_MAP_PARTIAL(int64_t)
ENTITY_PROPERTY_MAP_PARTIAL(uint8_t)
ENTITY_PROPERTY_MAP_PARTIAL(uint16_t)
ENTITY_PROPERTY_MAP_PARTIAL(uint32_t)
ENTITY_PROPERTY_MAP_PARTIAL(uint64_t)
ENTITY_PROPERTY_MAP_PARTIAL(GString)
ENTITY_PROPERTY_MAP_PARTIAL(float)
ENTITY_PROPERTY_MAP_PARTIAL(double)
ENTITY_PROPERTY_MAP_PARTIAL(GBin)


template<class T>
void EntityPropertyBase::insert(GString k, T v) {

    ASSERT_LOG(GString(typeid(T).name()) == this->get_v_tstring(),
        "insert type.%s error, should be %s\n", typeid(T).name(), this->get_v_tstring().c_str());

    EntityPropertyMap<T>* child = dynamic_cast<EntityPropertyMap<T>*>(this);
    child->insert(k, v);
}

template<class T>
void EntityPropertyBase::update(GString k, T v) {

    ASSERT_LOG(GString(typeid(T).name()) == this->get_v_tstring(),
        "update type.%s error, should be %s\n", typeid(T).name(), this->get_v_tstring().c_str());

    EntityPropertyMap<T>* child = dynamic_cast<EntityPropertyMap<T>*>(this);
    child->update(k, v);
}

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
        propertys[property_name] = new T([](EntityPropertyBase* mem) {}, type);
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

#define MEM_PROP_BEGIN_Zero_MEM(TComplex) MEM_PROP_BEGIN(TComplex, 0) COPY_CONSTRUCT_0(TComplex)
#define MEM_PROP_BEGIN_One_MEM(TComplex, Mem1) MEM_PROP_BEGIN(TComplex, 1) COPY_CONSTRUCT_1(TComplex, Mem1)
#define MEM_PROP_BEGIN_Two_MEM(TComplex, Mem1, Mem2) MEM_PROP_BEGIN(TComplex, 2) COPY_CONSTRUCT_2(TComplex, Mem1, Mem2)
#define MEM_PROP_BEGIN_Three_MEM(TComplex, Mem1, Mem2, Mem3) MEM_PROP_BEGIN(TComplex, 3) COPY_CONSTRUCT_3(TComplex, Mem1, Mem2, Mem3)
#define MEM_PROP_BEGIN_Four_MEM(TComplex, Mem1, Mem2, Mem3, Mem4) MEM_PROP_BEGIN(TComplex, 4) COPY_CONSTRUCT_4(TComplex, Mem1, Mem2, Mem3, Mem4)
#define MEM_PROP_BEGIN_Five_MEM(TComplex, Mem1, Mem2, Mem3, Mem4, Mem5) MEM_PROP_BEGIN(TComplex, 5) COPY_CONSTRUCT_5(TComplex, Mem1, Mem2, Mem3, Mem4, Mem5)
#define MEM_PROP_BEGIN_Six_MEM(TComplex, Mem1, Mem2, Mem3, Mem4, Mem5, Mem6) MEM_PROP_BEGIN(TComplex, 6) COPY_CONSTRUCT_6(TComplex, Mem1, Mem2, Mem3, Mem4, Mem5, Mem6)
#define MEM_PROP_BEGIN_Seven_MEM(TComplex, Mem1, Mem2, Mem3, Mem4, Mem5, Mem6, Mem7) MEM_PROP_BEGIN(TComplex, 7) COPY_CONSTRUCT_7(TComplex, Mem1, Mem2, Mem3, Mem4, Mem5, Mem6, Mem7)
#define MEM_PROP_BEGIN_Eight_MEM(TComplex, Mem1, Mem2, Mem3, Mem4, Mem5, Mem6, Mem7, Mem8) MEM_PROP_BEGIN(TComplex, 8) COPY_CONSTRUCT_8(TComplex, Mem1, Mem2, Mem3, Mem4, Mem5, Mem6, Mem7, Mem8)
#define MEM_PROP_BEGIN_Nine_MEM(TComplex, Mem1, Mem2, Mem3, Mem4, Mem5, Mem6, Mem7, Mem8, Mem9) MEM_PROP_BEGIN(TComplex, 9) COPY_CONSTRUCT_9(TComplex, Mem1, Mem2, Mem3, Mem4, Mem5, Mem6, Mem7, Mem8, Mem9)

#define MEM_PROP_BEGIN(TComplex, Num) \
    TComplex() : EntityPropertyComplex([](EntityPropertyBase* mem) {}, PropType::NONE) { num_check(Num); } \
    TComplex(const TCallBack& cb, const PropType& t) : EntityPropertyComplex(cb, t) { num_check(Num); } \
    void num_check(size_t num) { ASSERT_LOG(propertys.size() == num, "error macro def - %s Mem num is %ld\n", #TComplex, propertys.size()); }

#define COPY_CONSTRUCT_0(TComplex) \
    TComplex(const TComplex& other) : EntityPropertyComplex([](EntityPropertyBase* mem) {}, PropType::NONE) { \
    } \
    TComplex& operator=(const TComplex& other) { \
        return *this; \
    } 

#define COPY_CONSTRUCT_1(TComplex, Mem1) \
    TComplex(const TComplex& other) : EntityPropertyComplex([](EntityPropertyBase* mem) {}, PropType::NONE) { \
        Mem1 = other.Mem1; \
    } \
    TComplex& operator=(const TComplex& other) { \
        Mem1 = other.Mem1; \
        return *this; \
    } 

#define COPY_CONSTRUCT_2(TComplex, Mem1, Mem2) \
    TComplex(const TComplex& other) : EntityPropertyComplex([](EntityPropertyBase* mem) {}, PropType::NONE) { \
        Mem1 = other.Mem1; \
        Mem2 = other.Mem2; \
    } \
    TComplex& operator=(const TComplex& other) { \
        Mem1 = other.Mem1; \
        Mem2 = other.Mem2; \
        return *this; \
    } 

#define COPY_CONSTRUCT_3(TComplex, Mem1, Mem2, Mem3) \
    TComplex(const TComplex& other) : EntityPropertyComplex([](EntityPropertyBase* mem) {}, PropType::NONE) { \
        Mem1 = other.Mem1; \
        Mem2 = other.Mem2; \
        Mem3 = other.Mem3; \
    } \
    TComplex& operator=(const TComplex& other) { \
        Mem1 = other.Mem1; \
        Mem2 = other.Mem2; \
        Mem3 = other.Mem3; \
        return *this; \
    } 

#define COPY_CONSTRUCT_4(TComplex, Mem1, Mem2, Mem3, Mem4) \
    TComplex(const TComplex& other) : EntityPropertyComplex([](EntityPropertyBase* mem) {}, PropType::NONE) { \
        Mem1 = other.Mem1; \
        Mem2 = other.Mem2; \
        Mem3 = other.Mem3; \
        Mem4 = other.Mem4; \
    } \
    TComplex& operator=(const TComplex& other) { \
        Mem1 = other.Mem1; \
        Mem2 = other.Mem2; \
        Mem3 = other.Mem3; \
        Mem4 = other.Mem4; \
        return *this; \
    } 

#define COPY_CONSTRUCT_5(TComplex, Mem1, Mem2, Mem3, Mem4, Mem5) \
    TComplex(const TComplex& other) : EntityPropertyComplex([](EntityPropertyBase* mem) {}, PropType::NONE) { \
        Mem1 = other.Mem1; \
        Mem2 = other.Mem2; \
        Mem3 = other.Mem3; \
        Mem4 = other.Mem4; \
        Mem5 = other.Mem5; \
    } \
    TComplex& operator=(const TComplex& other) { \
        Mem1 = other.Mem1; \
        Mem2 = other.Mem2; \
        Mem3 = other.Mem3; \
        Mem4 = other.Mem4; \
        Mem5 = other.Mem5; \
        return *this; \
    } 

#define COPY_CONSTRUCT_6(TComplex, Mem1, Mem2, Mem3, Mem4, Mem5, Mem6) \
    TComplex(const TComplex& other) : EntityPropertyComplex([](EntityPropertyBase* mem) {}, PropType::NONE) { \
        Mem1 = other.Mem1; \
        Mem2 = other.Mem2; \
        Mem3 = other.Mem3; \
        Mem4 = other.Mem4; \
        Mem5 = other.Mem5; \
        Mem6 = other.Mem6; \
    } \
    TComplex& operator=(const TComplex& other) { \
        Mem1 = other.Mem1; \
        Mem2 = other.Mem2; \
        Mem3 = other.Mem3; \
        Mem4 = other.Mem4; \
        Mem5 = other.Mem5; \
        Mem6 = other.Mem6; \
        return *this; \
    } 

#define COPY_CONSTRUCT_7(TComplex, Mem1, Mem2, Mem3, Mem4, Mem5, Mem6, Mem7) \
    TComplex(const TComplex& other) : EntityPropertyComplex([](EntityPropertyBase* mem) {}, PropType::NONE) { \
        Mem1 = other.Mem1; \
        Mem2 = other.Mem2; \
        Mem3 = other.Mem3; \
        Mem4 = other.Mem4; \
        Mem5 = other.Mem5; \
        Mem6 = other.Mem6; \
        Mem7 = other.Mem7; \
    } \
    TComplex& operator=(const TComplex& other) { \
        Mem1 = other.Mem1; \
        Mem2 = other.Mem2; \
        Mem3 = other.Mem3; \
        Mem4 = other.Mem4; \
        Mem5 = other.Mem5; \
        Mem6 = other.Mem6; \
        Mem7 = other.Mem7; \
        return *this; \
    } 

#define COPY_CONSTRUCT_8(TComplex, Mem1, Mem2, Mem3, Mem4, Mem5, Mem6, Mem7, Mem8) \
    TComplex(const TComplex& other) : EntityPropertyComplex([](EntityPropertyBase* mem) {}, PropType::NONE) { \
        Mem1 = other.Mem1; \
        Mem2 = other.Mem2; \
        Mem3 = other.Mem3; \
        Mem4 = other.Mem4; \
        Mem5 = other.Mem5; \
        Mem6 = other.Mem6; \
        Mem7 = other.Mem7; \
        Mem8 = other.Mem8; \
    } \
    TComplex& operator=(const TComplex& other) { \
        Mem1 = other.Mem1; \
        Mem2 = other.Mem2; \
        Mem3 = other.Mem3; \
        Mem4 = other.Mem4; \
        Mem5 = other.Mem5; \
        Mem6 = other.Mem6; \
        Mem7 = other.Mem7; \
        Mem8 = other.Mem8; \
        return *this; \
    } 

#define COPY_CONSTRUCT_9(TComplex, Mem1, Mem2, Mem3, Mem4, Mem5, Mem6, Mem7, Mem8, Mem9) \
    TComplex(const TComplex& other) : EntityPropertyComplex([](EntityPropertyBase* mem) {}, PropType::NONE) { \
        Mem1 = other.Mem1; \
        Mem2 = other.Mem2; \
        Mem3 = other.Mem3; \
        Mem4 = other.Mem4; \
        Mem5 = other.Mem5; \
        Mem6 = other.Mem6; \
        Mem7 = other.Mem7; \
        Mem8 = other.Mem8; \
        Mem9 = other.Mem9; \
    } \
    TComplex& operator=(const TComplex& other) { \
        Mem1 = other.Mem1; \
        Mem2 = other.Mem2; \
        Mem3 = other.Mem3; \
        Mem4 = other.Mem4; \
        Mem5 = other.Mem5; \
        Mem6 = other.Mem6; \
        Mem7 = other.Mem7; \
        Mem8 = other.Mem8; \
        Mem9 = other.Mem9; \
        return *this; \
    } 
