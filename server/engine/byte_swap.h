#pragma once

#include <stdint.h>

inline uint16_t byte_swap2(uint16_t in_data) {
    return (in_data >> 8) | (in_data << 8);
}

inline uint32_t byte_swap4(uint32_t in_data) {
    return  ((in_data >> 24) & 0x000000ff) |
        ((in_data >> 8) & 0x0000ff00) |
        ((in_data << 8) & 0x00ff0000) |
        ((in_data << 24) & 0xff000000);
}

inline uint64_t byte_swap8(uint64_t in_data) {
    return  ((in_data >> 56) & 0x00000000000000ff) |
        ((in_data >> 40) & 0x000000000000ff00) |
        ((in_data >> 24) & 0x0000000000ff0000) |
        ((in_data >> 8) & 0x00000000ff000000) |
        ((in_data << 8) & 0x000000ff00000000) |
        ((in_data << 24) & 0x0000ff0000000000) |
        ((in_data << 40) & 0x00ff000000000000) |
        ((in_data << 56) & 0xff00000000000000);
}


template<class t_from, class t_to >
class TypeAliaser {
public:
    TypeAliaser(t_from value) :
        m_from(value) {}
    t_to& get() { return m_to; }

    union {
        t_from   m_from;
        t_to     m_to;
    };
};


template<class T, size_t size > class ByteSwapper;

//specialize for 1...
template<class T>
class ByteSwapper< T, 1 > {
public:
    T swap(T in_data) const {
        return in_data;
    }
};


//specialize for 2...
template<class T>
class ByteSwapper< T, 2 > {
public:
    T swap(T in_data) const {
        uint16_t result =
            byte_swap2(TypeAliaser< T, uint16_t >(in_data).get());
        return TypeAliaser< uint16_t, T >(result).get();
    }
};

//specialize for 4...
template<class T>
class ByteSwapper< T, 4 > {
public:
    T swap(T in_data) const {
        uint32_t result =
            byte_swap4(TypeAliaser< T, uint32_t >(in_data).get());
        return TypeAliaser< uint32_t, T >(result).get();
    }
};

//specialize for 8...
template<class T>
class ByteSwapper< T, 8 > {
public:
    T swap(T in_data) const {
        uint64_t result =
            byte_swap8(TypeAliaser< T, uint64_t >(in_data).get());
        return TypeAliaser< uint64_t, T >(result).get();
    }
};

union EndianDef {
    int i;
    char c;
};

template<class T>
T byte_swap(T in_data) {
    static EndianDef un;
    un.i = 1;

    if (un.c == 1) {
        return ByteSwapper<T, sizeof(T)>().swap(in_data);
    }
    else {
        return in_data;
    }
}
