#pragma once

#include <unordered_map>
#include <functional>
#include <typeinfo>

#include "gvalue.h"
#include "log.h"
#include "encode.h"
#include "decode.h"

using namespace std;

enum PropType : int8_t {
    NONE = 0x00,

    BASE_PRIVATE = 0x01,
    BASE_AND_CLIENT = 0x02,
    CELL_PRIVATE = 0x03,
    CELL_PUBLIC = 0x04,  // TODO
    CELL_AND_CLIENT = 0x05,
    ALL_CLIENT = 0x06,  // TODO
    OTHER_CLIENT = 0x07,  // TODO
    ALL = 0x08,
};

struct EntityPropertyBase {

    EntityPropertyBase() {}
    virtual ~EntityPropertyBase() {}

    virtual GString get_pclass_name() { ASSERT(false); return ""; }
    virtual EntityPropertyBase* create_self() = 0;
    virtual EntityPropertyBase* create_array_value() { ASSERT(false); return nullptr; }
    virtual EntityPropertyBase* create_map_value() { ASSERT(false); return nullptr; }

    // get value type string
    virtual GString get_v_tstring() { ASSERT(false); return ""; }
    virtual EntityPropertyBase** get_propertys() { ASSERT(false); return nullptr; }

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
    void update(int8_t v) { update_int(v); }
    void update(int16_t v) { update_int(v); }
    void update(int32_t v) { update_int(v); }
    void update(int64_t v) { update_int(v); }
    void update(uint8_t v) { update_int(v); }
    void update(uint16_t v) { update_int(v); }
    void update(uint32_t v) { update_int(v); }
    void update(uint64_t v) { update_int(v); }
    void update(float v) { update_float(v); }
    void update(double v) { update_float(v); }
    void update(const char* v) { update_cstr(v); }

    void update_int(uint64_t v);
    void update_float(double v);
    void update_cstr(const char*  v);

    // array property - crud
    template<class T>
    void push_back(T v);
    void push_back(int8_t v) { push_back_int(v); }
    void push_back(int16_t v) { push_back_int(v); }
    void push_back(int32_t v) { push_back_int(v); }
    void push_back(int64_t v) { push_back_int(v); }
    void push_back(uint8_t v) { push_back_int(v); }
    void push_back(uint16_t v) { push_back_int(v); }
    void push_back(uint32_t v) { push_back_int(v); }
    void push_back(uint64_t v) { push_back_int(v); }
    void push_back(float v) { push_back_float(v); }
    void push_back(double v) { push_back_float(v); }
    void push_back(const char* v) { push_back_cstr(v); }

    void push_back_int(uint64_t v);
    void push_back_float(double v);
    void push_back_cstr(const char*  v);

    virtual void pop_back() { ASSERT(false); }

    template<class T>
    void update(int32_t idx, T v);
    void update(int32_t idx, int8_t v) { update_int(idx, v); }
    void update(int32_t idx, int16_t v) { update_int(idx, v); }
    void update(int32_t idx, int32_t v) { update_int(idx, v); }
    void update(int32_t idx, int64_t v) { update_int(idx, v); }
    void update(int32_t idx, uint8_t v) { update_int(idx, v); }
    void update(int32_t idx, uint16_t v) { update_int(idx, v); }
    void update(int32_t idx, uint32_t v) { update_int(idx, v); }
    void update(int32_t idx, uint64_t v) { update_int(idx, v); }
    void update(int32_t idx, float v) { update_float(idx, v); }
    void update(int32_t idx, double v) { update_float(idx, v); }
    void update(int32_t idx, const char* v) { update_cstr(idx, v); }

    void update_int(int32_t idx, uint64_t v);
    void update_float(int32_t idx, double v);
    void update_cstr(int32_t idx, const char*  v);

    virtual EntityPropertyBase* get(const int32_t idx) const { ASSERT(false); return nullptr; }

    // map property - crud
    template<class T>
    void insert(GString k, T v);
    void insert(GString k, int8_t v) { insert_int(k, v); }
    void insert(GString k, int16_t v) { insert_int(k, v); }
    void insert(GString k, int32_t v) { insert_int(k, v); }
    void insert(GString k, int64_t v) { insert_int(k, v); }
    void insert(GString k, uint8_t v) { insert_int(k, v); }
    void insert(GString k, uint16_t v) { insert_int(k, v); }
    void insert(GString k, uint32_t v) { insert_int(k, v); }
    void insert(GString k, uint64_t v) { insert_int(k, v); }
    void insert(GString k, float v) { insert_float(k, v); }
    void insert(GString k, double v) { insert_float(k, v); }
    void insert(GString k, const char* v) { insert_cstr(k, v); }

    void insert_int(GString k, uint64_t v);
    void insert_float(GString k, double v);
    void insert_cstr(GString k, const char*  v);

    virtual void erase(GString k) {}

    template<class T>
    void update(GString k, T v);
    void update(GString k, int8_t v) { update_int(k, v); }
    void update(GString k, int16_t v) { update_int(k, v); }
    void update(GString k, int32_t v) { update_int(k, v); }
    void update(GString k, int64_t v) { update_int(k, v); }
    void update(GString k, uint8_t v) { update_int(k, v); }
    void update(GString k, uint16_t v) { update_int(k, v); }
    void update(GString k, uint32_t v) { update_int(k, v); }
    void update(GString k, uint64_t v) { update_int(k, v); }
    void update(GString k, float v) { update_float(k, v); }
    void update(GString k, double v) { update_float(k, v); }
    void update(GString k, const char* v) { update_cstr(k, v); }

    void update_int(GString k, uint64_t v);
    void update_float(GString k, double v);
    void update_cstr(GString k, const char*  v);

    virtual EntityPropertyBase* get(const GString& prop_name) const { ASSERT(false); return nullptr; }
    virtual vector<GString> keys() { ASSERT(false); return vector<GString>(); }

    virtual int32_t size() { ASSERT(false); return 0; }

    virtual void encode() { ASSERT(false); }
    virtual void decode() { ASSERT(false); }

    // property tree
    virtual bool is_complex() { return false; }
    virtual bool is_array() { return false; }
    virtual bool is_map() { return false; }
    virtual bool is_value_complex() { return false; }

    void set_parent(EntityPropertyBase* _parent) { parent = _parent; }
    EntityPropertyBase* parent = nullptr;

    bool need_sync2client();
    virtual void serialize(Encoder& encoder) { ASSERT(false); }
    virtual void serialize_all(Encoder& encoder) { ASSERT(false); }
    virtual void serialize_dirty(Encoder& encoder) { ASSERT(false); }
    virtual void unserialize(Decoder& decoder) { ASSERT(false); }

    // flag
    void set_prop_type(const PropType& t) { flag |= (t & 0x0F); }
    PropType get_prop_type() const { return PropType(flag & 0x0F); }

    void set_first_level() { flag |= 0x20; }
    bool is_first_level() const { return bool(flag & 0x20); }

    void set_all_dirty();
    bool is_all_dirty() const { return bool(flag & 0x40); }

    void set_dirty();
    bool is_dirty() const { return bool(flag & 0x80); }

    void clean_dirty() { flag &= 0x3F; }

    // void copy_flag(int8_t _flag) { flag &= 0xF0; flag |= (_flag & 0x0F); }

    // bit [0, 3] means PropType, only useful with first level property, immutable
    // bit 4 reserved
    // bit 5 means whether this is a first level property, immutable
    // bit 6 means all dirty, mutable
    // bit 7 means dirty, mutable
    int8_t flag = 0x00;
};

typedef function<void(EntityPropertyBase* mem)> TCallBack;

template<class T>
struct EntityPropertySimple : public EntityPropertyBase {

    EntityPropertySimple() : v(T()) {}
    EntityPropertySimple(T _v) : v(_v) {}
    EntityPropertySimple(const TCallBack& cb, T _v) : v(_v) { cb(this); }
    EntityPropertySimple(const EntityPropertySimple& other) : v(other.v) {};

    EntityPropertySimple<T>& operator=(const EntityPropertySimple<T>& other) { 
        v = other.v; 
        set_dirty();
        return *this;
    }
    EntityPropertySimple<T>& operator=(const T& _v) { 
        v = _v; 
        set_dirty(); 
        return *this; 
    }

    EntityPropertyBase* create_self() { return new EntityPropertySimple<T>(*this); }

    GString get_v_tstring() { return typeid(T).name(); }
    void update(T _v) { set_dirty(); v = _v; }

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

    void serialize(Encoder& encoder) { encoder.write_gvalue(v); }
    void serialize_all(Encoder& encoder) { encoder.write_gvalue(v); }
    void serialize_dirty(Encoder& encoder) { encoder.write_gvalue(v); }
    void unserialize(Decoder& decoder) { v = decoder.read_gvalue(); }

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

    EntityPropertyComplex() = default;
    EntityPropertyComplex(const TCallBack& cb) { cb(this); }

    virtual EntityPropertyBase* create_self() { ASSERT(false);  return nullptr; };
    virtual int8_t get_propertys_len() = 0;
    virtual void gen_prop_idxs() = 0;
    virtual EntityPropertyBase** get_propertys() { return nullptr; }

    bool is_complex() { return true; }

    void serialize(Encoder& encoder);
    void serialize_all(Encoder& encoder);
    void serialize_dirty(Encoder& encoder);
    void unserialize(Decoder& decoder);
};

template<class TValue>
struct EntityPropertyArray : public EntityPropertyBase {

    EntityPropertyArray() = default;
    EntityPropertyArray(const TCallBack& cb) { cb(this); }

    GString get_v_tstring() { return typeid(TValue).name(); }
    EntityPropertyBase* create_self() { return new EntityPropertyArray<TValue>(); }
    EntityPropertyBase* create_array_value() { return new TValue(); }

    EntityPropertyArray<TValue>& operator=(const EntityPropertyArray<TValue>& other) {
        propertys = other.propertys;
        set_all_dirty();
        return *this;
    }

    // crud
    void push_back(TValue v) {
        propertys.push_back(v);
        propertys[propertys.size() - 1].set_parent(this);
        propertys[propertys.size() - 1].set_dirty();
    }
    void pop_back() { propertys.pop_back(); set_all_dirty(); }
    void update(const int32_t idx, TValue v) { 
        propertys[idx] = v;
        propertys[idx].set_parent(this);
        propertys[idx].set_dirty();
    }
    EntityPropertyBase* get(const int32_t idx) const { return (EntityPropertyBase*)&propertys[idx]; }

    int32_t size() { return (int32_t)propertys.size(); }

    bool is_array() { return true; }
    bool is_value_complex() { return true; }

    void serialize(Encoder& encoder) { 
        if (is_all_dirty()) 
            serialize_all(encoder); 
        else 
            serialize_dirty(encoder); 
    }

    void serialize_all(Encoder& encoder) {
        encoder.write_string("y");
        encoder.write_int16((int16_t)propertys.size());
        for (int16_t i = 0; i < propertys.size(); ++i) {
            propertys[i].serialize_all(encoder);
        }
    }

    void serialize_dirty(Encoder& encoder) {
        encoder.write_string("j");
        int16_t num = 0;
        for (int16_t i = 0; i < propertys.size(); ++i) {
            if (!propertys[i].is_dirty() && !propertys[i].is_all_dirty()) {
                continue;
            }
            ++num;
        }
        encoder.write_int16(num);

        for (int16_t i = 0; i < propertys.size(); ++i) {
            if (!propertys[i].is_dirty() && !propertys[i].is_all_dirty()) {
                continue;
            }
            encoder.write_int16(i);
            propertys[i].serialize(encoder);
        }
    }

    void unserialize(Decoder& decoder) {
        bool is_unserialize_all = decoder.read_string() == "y";
        int16_t num = decoder.read_int16();
        if (is_unserialize_all) {
            int16_t i = 0;
            while (num > 0) {
                if (i >= propertys.size()) {
                    push_back(TValue());
                }
                propertys[i].unserialize(decoder);
                --num;
                ++i;
            }
        }
        else {
            while (num > 0) {
                int16_t i = decoder.read_int16();
                if (i >= propertys.size()) {
                    push_back(TValue());
                }
                propertys[i].unserialize(decoder);
                --num;
            }
        }
    }

    vector<TValue> propertys;
};

#define ENTITY_PROPERTY_ARRAY_PARTIAL(T) \
template<> \
struct EntityPropertyArray<T> : public EntityPropertyBase { \
\
    EntityPropertyArray<T>() = default; \
    EntityPropertyArray<T>(const TCallBack& cb) { cb(this); } \
\
    GString get_v_tstring() { return typeid(T).name(); } \
    EntityPropertyBase* create_self() { return new EntityPropertyArray<T>(); } \
\
    EntityPropertyArray<T>& operator=(const EntityPropertyArray<T>& other) { \
        propertys = other.propertys; \
        set_all_dirty(); \
        return *this; \
    } \
\
    /* crud */ \
    void push_back(T _v) { \
        propertys.push_back(EntityPropertySimple<T>(_v)); \
        propertys[propertys.size() - 1].set_parent(this); \
        propertys[propertys.size() - 1].set_dirty(); \
    } \
    void pop_back() { propertys.pop_back(); set_all_dirty(); } \
    void update(const int32_t idx, T _v) { \
        propertys[idx] = EntityPropertySimple<T>(_v); \
        propertys[idx].set_parent(this); \
        propertys[idx].set_dirty(); \
    } \
    EntityPropertyBase* get(const int32_t idx) const { return (EntityPropertyBase*)&propertys[idx]; } \
\
    int32_t size() { return (int32_t)propertys.size(); } \
    bool is_array() { return true; } \
\
    void serialize(Encoder& encoder) { \
        if (is_all_dirty()) \
            serialize_all(encoder); \
        else \
            serialize_dirty(encoder); \
    }\
\
    void serialize_all(Encoder& encoder) { \
        encoder.write_string("y"); \
        encoder.write_int16((int16_t)propertys.size()); \
        for (int16_t i = 0; i < propertys.size(); ++i) { \
            propertys[i].serialize_all(encoder); \
        } \
    } \
\
    void serialize_dirty(Encoder& encoder) { \
        encoder.write_string("j"); \
        int16_t num = 0; \
        for (int16_t i = 0; i < propertys.size(); ++i) { \
            if (!propertys[i].is_dirty() && !propertys[i].is_all_dirty()) { \
                continue; \
            } \
            ++num; \
        } \
        encoder.write_int16(num); \
\
        for (int16_t i = 0; i < propertys.size(); ++i) { \
            if (!propertys[i].is_dirty() && !propertys[i].is_all_dirty()) { \
                continue; \
            } \
            encoder.write_int16(i); \
            propertys[i].serialize(encoder); \
        } \
    } \
\
    void unserialize(Decoder& decoder) { \
        bool is_unserialize_all = decoder.read_string() == "y"; \
        int16_t num = decoder.read_int16(); \
        if (is_unserialize_all) { \
            int16_t i = 0; \
            while (num > 0) { \
                if (i >= propertys.size()) { \
                    push_back(T()); \
                } \
                propertys[i].unserialize(decoder); \
                --num; \
                ++i; \
            } \
        } \
        else { \
            while (num > 0) { \
                int16_t i = decoder.read_int16(); \
                if (i >= propertys.size()) { \
                    push_back(T()); \
                } \
                propertys[i].unserialize(decoder); \
                --num; \
            } \
        } \
    } \
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

    EntityPropertyMap() = default;
    EntityPropertyMap(const TCallBack& cb) { cb(this); }

    GString get_v_tstring() { return typeid(TValue).name(); }
    EntityPropertyBase* create_self() { return new EntityPropertyMap<TValue>(); }
    EntityPropertyBase* create_map_value() { return new TValue(); }

    EntityPropertyMap<TValue>& operator=(const EntityPropertyMap<TValue>& other) {
        propertys = other.propertys;
        set_all_dirty();
        return *this;
    }

    // crud
    void insert(GString k, TValue v) { propertys.insert(make_pair(k, v)); propertys.at(k).set_parent(this); propertys.at(k).set_dirty(); }
    void erase(GString k) { propertys.erase(k); set_all_dirty(); }
    void update(GString k, TValue v) { propertys[k] = v; propertys.at(k).set_parent(this); propertys.at(k).set_dirty(); }
    EntityPropertyBase* get(const GString& prop_name) const { return (EntityPropertyBase*)&propertys.at(prop_name); }

    int32_t size() { return (int32_t)propertys.size(); }
    vector<GString> keys() { 
        vector<GString> ret; 
        for (auto iter = propertys.begin(); iter != propertys.end(); ++iter)
            ret.push_back(iter->first);
        return ret;
    }

    bool is_map() { return true; }
    bool is_value_complex() { return true; }

    void serialize(Encoder& encoder) {
        if (is_all_dirty())
            serialize_all(encoder);
        else
            serialize_dirty(encoder);
    }

    void serialize_all(Encoder& encoder) {
        encoder.write_int16((int16_t)propertys.size());

        for (auto iter = propertys.begin(); iter != propertys.end(); ++iter) {
            encoder.write_string(iter->first);
            iter->second.serialize_all(encoder);
        }
    }

    void serialize_dirty(Encoder& encoder) {
        int16_t num = 0;
        for (auto iter = propertys.begin(); iter != propertys.end(); ++iter) {
            if (!iter->second.is_dirty() && !iter->second.is_all_dirty()) {
                continue;
            }
            ++num;
        }
        encoder.write_int16(num);

        for (auto iter = propertys.begin(); iter != propertys.end(); ++iter) {
            if (!iter->second.is_dirty() && !iter->second.is_all_dirty()) {
                continue;
            }
            encoder.write_string(iter->first);
            iter->second.serialize(encoder);
        }
    }

    void unserialize(Decoder& decoder) {
        int16_t num = decoder.read_int16();
        while (num > 0) {
            const GString& prop_name = decoder.read_string();
            if (propertys.find(prop_name) == propertys.end()) {
                insert(prop_name, TValue());
            }
            propertys.at(prop_name).unserialize(decoder);
            --num;
        }
    }

    unordered_map<GString, TValue> propertys;
};

#define ENTITY_PROPERTY_MAP_PARTIAL(T) \
template<> \
struct EntityPropertyMap<T> : public EntityPropertyBase { \
\
    EntityPropertyMap<T>() = default; \
    EntityPropertyMap<T>(const TCallBack& cb) { cb(this); } \
\
    GString get_v_tstring() { return typeid(T).name(); } \
    EntityPropertyBase* create_self() { return new EntityPropertyMap<T>(); } \
\
    EntityPropertyMap<T>& operator=(const EntityPropertyMap<T>& other) { \
        propertys = other.propertys; \
        set_all_dirty(); \
        return *this; \
    } \
\
    /* crud */ \
    void insert(GString k, T _v) { propertys.insert(make_pair(k, EntityPropertySimple<T>(_v))); propertys.at(k).set_parent(this); propertys.at(k).set_dirty(); } \
    void erase(GString k) { propertys.erase(k); set_all_dirty(); } \
    void update(GString k, T _v) { propertys[k] = EntityPropertySimple<T>(_v); propertys.at(k).set_parent(this); propertys.at(k).set_dirty(); } \
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
    bool is_map() { return true; } \
\
    void serialize(Encoder& encoder) { \
        if (is_all_dirty()) \
            serialize_all(encoder); \
        else \
            serialize_dirty(encoder); \
    } \
\
    void serialize_all(Encoder& encoder) { \
        encoder.write_int16((int16_t)propertys.size()); \
        for (auto iter = propertys.begin(); iter != propertys.end(); ++iter) { \
            encoder.write_string(iter->first); \
            iter->second.serialize_all(encoder); \
        } \
    } \
\
    void serialize_dirty(Encoder& encoder) { \
        int16_t num = 0; \
        for (auto iter = propertys.begin(); iter != propertys.end(); ++iter) { \
            if (!iter->second.is_dirty() && !iter->second.is_all_dirty()) { \
                continue; \
            } \
            ++num; \
        } \
        encoder.write_int16(num); \
\
        for (auto iter = propertys.begin(); iter != propertys.end(); ++iter) { \
            if (!iter->second.is_dirty() && !iter->second.is_all_dirty()) { \
                continue; \
            } \
            encoder.write_string(iter->first); \
            iter->second.serialize(encoder); \
        } \
    } \
\
    void unserialize(Decoder& decoder) { \
        int16_t num = decoder.read_int16(); \
        while (num > 0) { \
            const GString& prop_name = decoder.read_string(); \
            if (propertys.find(prop_name) == propertys.end()) { \
                insert(prop_name, T()); \
            } \
            propertys.at(prop_name).unserialize(decoder); \
            --num; \
        } \
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

extern bool prop_regist_check(int8_t t, enum PropType type);

// entity property manager
template<class TEntity>
class EntityPropertyManager {

public:
    EntityPropertyManager() {
        TEntity::property_define();
    }

    template<class TProp>
    void regist_simple_property(enum PropType type, const GString& property_name, TProp _default) {
        _gen_map(property_name);
        if (!prop_regist_check(TEntity::ENTITY_TYPE, type)) {
            return;
        }

        auto prop = new EntityPropertySimple<TProp>(_default);
        prop->set_prop_type(type);
        propertys[property_name] = prop;
    }

    template<class TProp>
    void regist_complex_property(enum PropType type, const GString& property_name) {
        _gen_map(property_name);
        if (!prop_regist_check(TEntity::ENTITY_TYPE, type)) {
            return;
        }

        auto prop = new TProp();
        prop->set_prop_type(type);
        propertys[property_name] = prop;
    }

    template<class TProp>
    void regist_array_property(enum PropType type, const GString& property_name) {
        _gen_map(property_name);
        if (!prop_regist_check(TEntity::ENTITY_TYPE, type)) {
            return;
        }

        auto prop = new EntityPropertyArray<TProp>();
        prop->set_prop_type(type);
        propertys[property_name] = prop;
    }

    template<class TProp>
    void regist_map_property(enum PropType type, const GString& property_name) {
        _gen_map(property_name);
        if (!prop_regist_check(TEntity::ENTITY_TYPE, type)) {
            return;
        }

        auto prop = new EntityPropertyMap<TProp>();
        prop->set_prop_type(type);
        propertys[property_name] = prop;
    }

    void _gen_map(const GString& property_name) {
        ASSERT_LOG(s2i_map.find(property_name) == s2i_map.end(), "repeated property.%s", property_name.c_str());

        int16_t idx = (int16_t)s2i_map.size();
        s2i_map.insert(make_pair(property_name, idx));
        i2s_map.insert(make_pair(idx, property_name));
    }

    unordered_map<GString, EntityPropertyBase*> propertys;
    unordered_map<GString, int16_t> s2i_map;
    unordered_map<int16_t, GString> i2s_map;
};

#define PROPERTY_SIMPLE(prop_type, property_name, prop_class, default_value) \
    TEntity::property_manager.regist_simple_property<prop_class>(prop_type, #property_name, default_value)
#define PROPERTY_COMPLEX(prop_type, property_name, prop_class) \
    TEntity::property_manager.regist_complex_property<prop_class>(prop_type, #property_name)
#define PROPERTY_ARRAY(prop_type, property_name, prop_class) \
    TEntity::property_manager.regist_array_property<prop_class>(prop_type, #property_name)
#define PROPERTY_MAP(prop_type, property_name, prop_class) \
    TEntity::property_manager.regist_map_property<prop_class>(prop_type, #property_name)


#define MEM_PROPERTY_SIMPLE(property_name, prop_class, default_value) \
    EntityPropertySimple<prop_class> property_name = EntityPropertySimple<prop_class>([this](EntityPropertyBase* mem){ mem->link_node(this, #property_name);}, default_value)
#define MEM_PROPERTY_COMPLEX(property_name, prop_class) \
    prop_class property_name = prop_class([this](EntityPropertyBase* mem){ mem->link_node(this, #property_name);})
#define MEM_PROPERTY_ARRAY(property_name, prop_class) \
    EntityPropertyArray<prop_class> property_name = EntityPropertyArray<prop_class>([this](EntityPropertyBase* mem){ mem->link_node(this, #property_name);})
#define MEM_PROPERTY_MAP(property_name, prop_class) \
    EntityPropertyMap<prop_class> property_name = EntityPropertyMap<prop_class>([this](EntityPropertyBase* mem){ mem->link_node(this, #property_name);})

#define MEM_PROP_BEGIN_One_MEM(TComplex, Mem1) MEM_PROP_BEGIN(TComplex, 1) COPY_CONSTRUCT_1(TComplex, Mem1)
#define MEM_PROP_BEGIN_Two_MEM(TComplex, Mem1, Mem2) MEM_PROP_BEGIN(TComplex, 2) COPY_CONSTRUCT_2(TComplex, Mem1, Mem2)
#define MEM_PROP_BEGIN_Three_MEM(TComplex, Mem1, Mem2, Mem3) MEM_PROP_BEGIN(TComplex, 3) COPY_CONSTRUCT_3(TComplex, Mem1, Mem2, Mem3)
#define MEM_PROP_BEGIN_Four_MEM(TComplex, Mem1, Mem2, Mem3, Mem4) MEM_PROP_BEGIN(TComplex, 4) COPY_CONSTRUCT_4(TComplex, Mem1, Mem2, Mem3, Mem4)
#define MEM_PROP_BEGIN_Five_MEM(TComplex, Mem1, Mem2, Mem3, Mem4, Mem5) MEM_PROP_BEGIN(TComplex, 5) COPY_CONSTRUCT_5(TComplex, Mem1, Mem2, Mem3, Mem4, Mem5)
#define MEM_PROP_BEGIN_Six_MEM(TComplex, Mem1, Mem2, Mem3, Mem4, Mem5, Mem6) MEM_PROP_BEGIN(TComplex, 6) COPY_CONSTRUCT_6(TComplex, Mem1, Mem2, Mem3, Mem4, Mem5, Mem6)
#define MEM_PROP_BEGIN_Seven_MEM(TComplex, Mem1, Mem2, Mem3, Mem4, Mem5, Mem6, Mem7) MEM_PROP_BEGIN(TComplex, 7) COPY_CONSTRUCT_7(TComplex, Mem1, Mem2, Mem3, Mem4, Mem5, Mem6, Mem7)
#define MEM_PROP_BEGIN_Eight_MEM(TComplex, Mem1, Mem2, Mem3, Mem4, Mem5, Mem6, Mem7, Mem8) MEM_PROP_BEGIN(TComplex, 8) COPY_CONSTRUCT_8(TComplex, Mem1, Mem2, Mem3, Mem4, Mem5, Mem6, Mem7, Mem8)
#define MEM_PROP_BEGIN_Nine_MEM(TComplex, Mem1, Mem2, Mem3, Mem4, Mem5, Mem6, Mem7, Mem8, Mem9) MEM_PROP_BEGIN(TComplex, 9) COPY_CONSTRUCT_9(TComplex, Mem1, Mem2, Mem3, Mem4, Mem5, Mem6, Mem7, Mem8, Mem9)


typedef unordered_map<GString, unordered_map<GString, uint8_t> > PropIdxType;
extern PropIdxType* g_all_prop_idxs;
extern PropIdxType& get_all_prop_idxs();

#define MEM_PROP_BEGIN(TComplex, Num) \
    TComplex() { num_check(); } \
    TComplex(const TCallBack& cb) : EntityPropertyComplex(cb) { num_check(); } \
    void num_check() { \
        PropIdxType& all_prop_idxs = get_all_prop_idxs(); \
        ASSERT_LOG(all_prop_idxs[#TComplex].size() == (size_t)get_propertys_len(), \
            "error macro def - %s Mem num is %ld\n", #TComplex, all_prop_idxs[#TComplex].size()); \
    } \
    GString get_pclass_name() { return #TComplex; } \
    virtual uint8_t get_prop_idx(const GString& prop_name) const { \
        PropIdxType& all_prop_idxs = get_all_prop_idxs(); \
        return all_prop_idxs.at(#TComplex).at(prop_name); \
    } \
    EntityPropertyBase* get(const GString& prop_name) const { auto idx = get_prop_idx(prop_name); return propertys[idx]; } \
    EntityPropertyBase** get_propertys() { return propertys; } \
    int8_t get_propertys_len() { return Num; } \
    EntityPropertyBase* propertys[Num] = {0}; \
    EntityPropertyBase* create_self() { return new TComplex(); };

#define COPY_CONSTRUCT_1(TComplex, Mem1) \
    TComplex(const TComplex& other) { \
        Mem1 = other.Mem1; \
    } \
    TComplex& operator=(const TComplex& other) { \
        Mem1 = other.Mem1; \
        set_all_dirty(); \
        return *this; \
    } \
    void gen_prop_idxs() { \
        PropIdxType& all_prop_idxs = get_all_prop_idxs(); \
        if (all_prop_idxs.find(#TComplex) != all_prop_idxs.end()) \
            return; \
        all_prop_idxs.insert(make_pair(#TComplex, unordered_map<GString, uint8_t>())); \
        all_prop_idxs[#TComplex].insert(make_pair(#Mem1, (uint8_t)all_prop_idxs[#TComplex].size())); \
    }

#define COPY_CONSTRUCT_2(TComplex, Mem1, Mem2) \
    TComplex(const TComplex& other) { \
        Mem1 = other.Mem1; \
        Mem2 = other.Mem2; \
    } \
    TComplex& operator=(const TComplex& other) { \
        Mem1 = other.Mem1; \
        Mem2 = other.Mem2; \
        set_all_dirty(); \
        return *this; \
    } \
    void gen_prop_idxs() { \
        PropIdxType& all_prop_idxs = get_all_prop_idxs(); \
        if (all_prop_idxs.find(#TComplex) != all_prop_idxs.end()) \
            return; \
        all_prop_idxs.insert(make_pair(#TComplex, unordered_map<GString, uint8_t>())); \
        all_prop_idxs[#TComplex].insert(make_pair(#Mem1, (uint8_t)all_prop_idxs[#TComplex].size())); \
        all_prop_idxs[#TComplex].insert(make_pair(#Mem2, (uint8_t)all_prop_idxs[#TComplex].size())); \
    }

#define COPY_CONSTRUCT_3(TComplex, Mem1, Mem2, Mem3) \
    TComplex(const TComplex& other) { \
        Mem1 = other.Mem1; \
        Mem2 = other.Mem2; \
        Mem3 = other.Mem3; \
    } \
    TComplex& operator=(const TComplex& other) { \
        Mem1 = other.Mem1; \
        Mem2 = other.Mem2; \
        Mem3 = other.Mem3; \
        set_all_dirty(); \
        return *this; \
    } \
    void gen_prop_idxs() { \
        PropIdxType& all_prop_idxs = get_all_prop_idxs(); \
        if (all_prop_idxs.find(#TComplex) != all_prop_idxs.end()) \
            return; \
        all_prop_idxs.insert(make_pair(#TComplex, unordered_map<GString, uint8_t>())); \
        all_prop_idxs[#TComplex].insert(make_pair(#Mem1, (uint8_t)all_prop_idxs[#TComplex].size())); \
        all_prop_idxs[#TComplex].insert(make_pair(#Mem2, (uint8_t)all_prop_idxs[#TComplex].size())); \
        all_prop_idxs[#TComplex].insert(make_pair(#Mem3, (uint8_t)all_prop_idxs[#TComplex].size())); \
    }

#define COPY_CONSTRUCT_4(TComplex, Mem1, Mem2, Mem3, Mem4) \
    TComplex(const TComplex& other) { \
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
        set_all_dirty(); \
        return *this; \
    } \
    void gen_prop_idxs() { \
        PropIdxType& all_prop_idxs = get_all_prop_idxs(); \
        if (all_prop_idxs.find(#TComplex) != all_prop_idxs.end()) \
            return; \
        all_prop_idxs.insert(make_pair(#TComplex, unordered_map<GString, uint8_t>())); \
        all_prop_idxs[#TComplex].insert(make_pair(#Mem1, (uint8_t)all_prop_idxs[#TComplex].size())); \
        all_prop_idxs[#TComplex].insert(make_pair(#Mem2, (uint8_t)all_prop_idxs[#TComplex].size())); \
        all_prop_idxs[#TComplex].insert(make_pair(#Mem3, (uint8_t)all_prop_idxs[#TComplex].size())); \
        all_prop_idxs[#TComplex].insert(make_pair(#Mem4, (uint8_t)all_prop_idxs[#TComplex].size())); \
    }

#define COPY_CONSTRUCT_5(TComplex, Mem1, Mem2, Mem3, Mem4, Mem5) \
    TComplex(const TComplex& other) { \
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
        set_all_dirty(); \
        return *this; \
    } \
    void gen_prop_idxs() { \
        PropIdxType& all_prop_idxs = get_all_prop_idxs(); \
        if (all_prop_idxs.find(#TComplex) != all_prop_idxs.end()) \
            return; \
        all_prop_idxs.insert(make_pair(#TComplex, unordered_map<GString, uint8_t>())); \
        all_prop_idxs[#TComplex].insert(make_pair(#Mem1, (uint8_t)all_prop_idxs[#TComplex].size())); \
        all_prop_idxs[#TComplex].insert(make_pair(#Mem2, (uint8_t)all_prop_idxs[#TComplex].size())); \
        all_prop_idxs[#TComplex].insert(make_pair(#Mem3, (uint8_t)all_prop_idxs[#TComplex].size())); \
        all_prop_idxs[#TComplex].insert(make_pair(#Mem4, (uint8_t)all_prop_idxs[#TComplex].size())); \
        all_prop_idxs[#TComplex].insert(make_pair(#Mem5, (uint8_t)all_prop_idxs[#TComplex].size())); \
    }

#define COPY_CONSTRUCT_6(TComplex, Mem1, Mem2, Mem3, Mem4, Mem5, Mem6) \
    TComplex(const TComplex& other) { \
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
        set_all_dirty(); \
        return *this; \
    } \
    void gen_prop_idxs() { \
        PropIdxType& all_prop_idxs = get_all_prop_idxs(); \
        if (all_prop_idxs.find(#TComplex) != all_prop_idxs.end()) \
            return; \
        all_prop_idxs.insert(make_pair(#TComplex, unordered_map<GString, uint8_t>())); \
        all_prop_idxs[#TComplex].insert(make_pair(#Mem1, (uint8_t)all_prop_idxs[#TComplex].size())); \
        all_prop_idxs[#TComplex].insert(make_pair(#Mem2, (uint8_t)all_prop_idxs[#TComplex].size())); \
        all_prop_idxs[#TComplex].insert(make_pair(#Mem3, (uint8_t)all_prop_idxs[#TComplex].size())); \
        all_prop_idxs[#TComplex].insert(make_pair(#Mem4, (uint8_t)all_prop_idxs[#TComplex].size())); \
        all_prop_idxs[#TComplex].insert(make_pair(#Mem5, (uint8_t)all_prop_idxs[#TComplex].size())); \
        all_prop_idxs[#TComplex].insert(make_pair(#Mem6, (uint8_t)all_prop_idxs[#TComplex].size())); \
    }

#define COPY_CONSTRUCT_7(TComplex, Mem1, Mem2, Mem3, Mem4, Mem5, Mem6, Mem7) \
    TComplex(const TComplex& other) { \
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
        set_all_dirty(); \
        return *this; \
    } \
    void gen_prop_idxs() { \
        PropIdxType& all_prop_idxs = get_all_prop_idxs(); \
        if (all_prop_idxs.find(#TComplex) != all_prop_idxs.end()) \
            return; \
        all_prop_idxs.insert(make_pair(#TComplex, unordered_map<GString, uint8_t>())); \
        all_prop_idxs[#TComplex].insert(make_pair(#Mem1, (uint8_t)all_prop_idxs[#TComplex].size())); \
        all_prop_idxs[#TComplex].insert(make_pair(#Mem2, (uint8_t)all_prop_idxs[#TComplex].size())); \
        all_prop_idxs[#TComplex].insert(make_pair(#Mem3, (uint8_t)all_prop_idxs[#TComplex].size())); \
        all_prop_idxs[#TComplex].insert(make_pair(#Mem4, (uint8_t)all_prop_idxs[#TComplex].size())); \
        all_prop_idxs[#TComplex].insert(make_pair(#Mem5, (uint8_t)all_prop_idxs[#TComplex].size())); \
        all_prop_idxs[#TComplex].insert(make_pair(#Mem6, (uint8_t)all_prop_idxs[#TComplex].size())); \
        all_prop_idxs[#TComplex].insert(make_pair(#Mem7, (uint8_t)all_prop_idxs[#TComplex].size())); \
    }

#define COPY_CONSTRUCT_8(TComplex, Mem1, Mem2, Mem3, Mem4, Mem5, Mem6, Mem7, Mem8) \
    TComplex(const TComplex& other) { \
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
        set_all_dirty(); \
        return *this; \
    } \
    void gen_prop_idxs() { \
        PropIdxType& all_prop_idxs = get_all_prop_idxs(); \
        if (all_prop_idxs.find(#TComplex) != all_prop_idxs.end()) \
            return; \
        all_prop_idxs.insert(make_pair(#TComplex, unordered_map<GString, uint8_t>())); \
        all_prop_idxs[#TComplex].insert(make_pair(#Mem1, (uint8_t)all_prop_idxs[#TComplex].size())); \
        all_prop_idxs[#TComplex].insert(make_pair(#Mem2, (uint8_t)all_prop_idxs[#TComplex].size())); \
        all_prop_idxs[#TComplex].insert(make_pair(#Mem3, (uint8_t)all_prop_idxs[#TComplex].size())); \
        all_prop_idxs[#TComplex].insert(make_pair(#Mem4, (uint8_t)all_prop_idxs[#TComplex].size())); \
        all_prop_idxs[#TComplex].insert(make_pair(#Mem5, (uint8_t)all_prop_idxs[#TComplex].size())); \
        all_prop_idxs[#TComplex].insert(make_pair(#Mem6, (uint8_t)all_prop_idxs[#TComplex].size())); \
        all_prop_idxs[#TComplex].insert(make_pair(#Mem7, (uint8_t)all_prop_idxs[#TComplex].size())); \
        all_prop_idxs[#TComplex].insert(make_pair(#Mem8, (uint8_t)all_prop_idxs[#TComplex].size())); \
    }

#define COPY_CONSTRUCT_9(TComplex, Mem1, Mem2, Mem3, Mem4, Mem5, Mem6, Mem7, Mem8, Mem9) \
    TComplex(const TComplex& other) { \
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
        set_all_dirty(); \
        return *this; \
    } \
    void gen_prop_idxs() { \
        PropIdxType& all_prop_idxs = get_all_prop_idxs(); \
        if (all_prop_idxs.find(#TComplex) != all_prop_idxs.end()) \
            return; \
        all_prop_idxs.insert(make_pair(#TComplex, unordered_map<GString, uint8_t>())); \
        all_prop_idxs[#TComplex].insert(make_pair(#Mem1, (uint8_t)all_prop_idxs[#TComplex].size())); \
        all_prop_idxs[#TComplex].insert(make_pair(#Mem2, (uint8_t)all_prop_idxs[#TComplex].size())); \
        all_prop_idxs[#TComplex].insert(make_pair(#Mem3, (uint8_t)all_prop_idxs[#TComplex].size())); \
        all_prop_idxs[#TComplex].insert(make_pair(#Mem4, (uint8_t)all_prop_idxs[#TComplex].size())); \
        all_prop_idxs[#TComplex].insert(make_pair(#Mem5, (uint8_t)all_prop_idxs[#TComplex].size())); \
        all_prop_idxs[#TComplex].insert(make_pair(#Mem6, (uint8_t)all_prop_idxs[#TComplex].size())); \
        all_prop_idxs[#TComplex].insert(make_pair(#Mem7, (uint8_t)all_prop_idxs[#TComplex].size())); \
        all_prop_idxs[#TComplex].insert(make_pair(#Mem8, (uint8_t)all_prop_idxs[#TComplex].size())); \
        all_prop_idxs[#TComplex].insert(make_pair(#Mem9, (uint8_t)all_prop_idxs[#TComplex].size())); \
    }
