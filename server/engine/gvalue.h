﻿#pragma once

#include <stdint.h>
#include <cstring>
#include <string>
#include <vector>
#include <unordered_map>
#include <map>
#include <memory>  // shared_ptr need include memory, omg~~~

#include "utils.h"
#include "log.h"

using namespace std;

#define RMCVR(T) typename std::remove_cv<typename std::remove_reference<T>::type>::type 
#define RMP(T) std::remove_pointer<T>::type

class GValue;
typedef string GString;
typedef vector<GValue> GArray;
typedef unordered_map<GString, GValue> GDict;


class Entity;
class Session;
class Remote;
class MailBoxProxy;

class MailBox {

public:
    MailBox();
    MailBox(const MailBox& other)
        : m_entity_uuid(other.m_entity_uuid)
        , m_addr(other.m_addr)
        , m_flag(other.m_flag)
    {}
    ~MailBox();

    bool is_valid();

    void set_entity_and_addr(const GString& entity_uuid, const GString& addr);
    void set_owner(Entity* owner) { m_owner = owner; }
    void set_flag(int8_t flag);

    GString get_entity_uuid() const;
    GString get_addr() const;
    int8_t get_flag() const;
    Entity* get_owner();

    void set_base() { m_flag |= 0x01; }
    bool is_base() { return m_flag & 0x01; }
    bool is_cell() { return !is_base(); }
    void set_b2c() { m_flag |= 0x02; }
    bool is_b2c() { return m_flag & 0x02; }

    MailBoxProxy& to_proxy() const;
    //BaseMailBox& to_base();
    //CellMailBox& to_cell();

protected:
    GString     m_entity_uuid = "";
    GString     m_addr = "";

    // bit 0: 1 is base, 0 is cell
    // bit 1: is b2c
    int8_t      m_flag = 0x00;

public:
    Entity*     m_owner = nullptr;
    shared_ptr<Session> m_session_cache = nullptr;
    shared_ptr<Remote> m_remote_cache = nullptr;
};

struct GBin {
public:
    GBin() = default;
    GBin(const char* _buf, int16_t _size) : size(_size) { 
        if (size == 0) {
            return;
        }
        buf = new char[size];
        memmove(buf, _buf, size);
    }
    GBin(const GBin& other) : size(other.size) {
        if (size == 0) {
            return;
        }
        buf = new char[other.size];
        memmove(buf, other.buf, size);
    }
    GBin(GBin&& other) : buf(other.buf), size(other.size) {
        other.buf = nullptr;
        other.size = 0;
    }

    GBin& operator=(const GBin& other) {
        release();

        size = other.size;
        if (size == 0) {
            return *this;
        }
        buf = new char[size];
        memmove(buf, other.buf, size);
        return *this;
    }

    GBin& operator=(GBin&& other) {
        release();

        buf = other.buf;
        size = other.size;
        other.buf = nullptr;
        other.size = 0;
        return *this;
    }

    ~GBin() { release(); }

    void release() {
        if (buf != nullptr) {
            delete[]buf;
            buf = nullptr;
            size = 0;
        }
    }

    char* buf = nullptr;
    uint16_t size = 0;
};

enum GType : uint8_t {BOOL_T, INT8_T, INT16_T, INT32_T, INT64_T, UINT8_T, UINT16_T, UINT32_T, UINT64_T, FLOAT_T, DOUBLE_T, STRING_T, ARRAY_T, DICT_T, BIN_T, MAILBOX_T, NONE_T};

class GValue {

    union GV {
        GV() {}
        ~GV() {}

        bool    b = false;
        int8_t  i8;
        int16_t i16;
        int32_t i32;
        int64_t i64;
        uint8_t  ui8;
        uint16_t ui16;
        uint32_t ui32;
        uint64_t ui64;
        float f;
        double d;
        GString* s;
        //GString s = "";  // 必须显示初始化，否则是invalid string
        GArray* array;
        GDict* dict;
        GBin* bin;
        MailBox* mailbox;
    };

public:
    GValue() = delete;
    ~GValue() { release(); };
    void release();

    template<class T>
    GValue(T v) {ASSERT_LOG(false, "error type %s\n", typeid(v).name())}

    GValue(bool v) : m_t(GType::BOOL_T) { m_v.b = v; }
    GValue(int8_t v) : m_t(GType::INT8_T) { m_v.i8 = v;}
    GValue(int16_t v) : m_t(GType::INT16_T) { m_v.i16 = v; }
    GValue(int32_t v) : m_t(GType::INT32_T) { m_v.i32 = v; }
    GValue(int64_t v) : m_t(GType::INT64_T) { m_v.i64 = v; }
    GValue(uint8_t v) : m_t(GType::UINT8_T) { m_v.ui8 = v; }
    GValue(uint16_t v) : m_t(GType::UINT16_T) { m_v.ui16 = v; }
    GValue(uint32_t v) : m_t(GType::UINT32_T) { m_v.ui32 = v; }
    GValue(uint64_t v) : m_t(GType::UINT64_T) { m_v.ui64 = v; }
    GValue(float v) : m_t(GType::FLOAT_T) { m_v.f = v; }
    GValue(double v) : m_t(GType::DOUBLE_T) { m_v.d = v; }
    GValue(const char* v) : m_t(GType::STRING_T) { m_v.s = new GString(v); }
    GValue(const GString& v) : m_t(GType::STRING_T) { m_v.s = new GString(v); }
    GValue(const GArray& v) : m_t(GType::ARRAY_T) { m_v.array = new GArray(v); }
    GValue(const GDict& v) : m_t(GType::DICT_T) { m_v.dict = new GDict(v); }
    GValue(const GBin& v) : m_t(GType::BIN_T) { m_v.bin = new GBin(v); }
    GValue(const MailBox& v) : m_t(GType::MAILBOX_T) { m_v.mailbox = new MailBox(v); }

    GValue(const GValue& rs);
    GValue& operator=(const GValue& rs);

    GValue(GValue&& rs);
    GValue& operator=(GValue&& rs);
    
    bool as_bool() const { ASSERT(m_t == GType::BOOL_T); return m_v.b; }
    int8_t as_int8() const { ASSERT(m_t == GType::INT8_T); return m_v.i8; }
    int16_t as_int16() const { ASSERT(m_t == GType::INT16_T); return m_v.i16; }
    int32_t as_int32() const { ASSERT(m_t == GType::INT32_T); return m_v.i32; }
    int64_t as_int64() const { ASSERT(m_t == GType::INT64_T); return m_v.i64; }
    uint8_t as_uint8() const { ASSERT(m_t == GType::UINT8_T); return m_v.ui8; }
    uint16_t as_uint16() const { ASSERT(m_t == GType::UINT16_T); return m_v.ui16; }
    uint32_t as_uint32() const { ASSERT(m_t == GType::UINT32_T); return m_v.ui32; }
    uint64_t as_uint64() const { ASSERT(m_t == GType::UINT64_T); return m_v.ui64; }
    float as_float() const { ASSERT(m_t == GType::FLOAT_T); return m_v.f; }
    double as_double() const { ASSERT(m_t == GType::DOUBLE_T); return m_v.d; }
    GString& as_string() const { ASSERT(m_t == GType::STRING_T); return *(m_v.s); }
    GArray& as_array() const { ASSERT(m_t == GType::ARRAY_T); return *(m_v.array); }
    GDict& as_dict() const { ASSERT(m_t == GType::DICT_T); return *(m_v.dict); }
    GBin& as_bin() const { ASSERT(m_t == GType::BIN_T); return *(m_v.bin); }
    MailBox& as_mailbox() const { ASSERT(m_t == GType::MAILBOX_T); return *(m_v.mailbox); }

    GType type() const { return m_t; }
private:
    GType m_t = GType::NONE_T;
    GV m_v;
};
