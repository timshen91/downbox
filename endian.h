#ifndef __ENDIAN_H__
#define __ENDIAN_H__

#include <stdint.h>
#include <endian.h>

namespace {

template<int>
struct uint;

template<>
struct uint<1> {
    typedef uint8_t type;
};

template<>
struct uint<2> {
    typedef uint16_t type;
};

template<>
struct uint<4> {
    typedef uint32_t type;
};

template<>
struct uint<8> {
    typedef uint64_t type;
};

template<int size>
using uint_t = typename uint<size>::type;

}

template<int size>
uint_t<size> letoh_impl(uint_t<size> a);

template<>
uint_t<1> letoh_impl<1>(uint_t<1> a) {
    return a;
}

template<>
uint_t<2> letoh_impl<2>(uint_t<2> a) {
    return le16toh(a);
}

template<>
uint_t<4> letoh_impl<4>(uint_t<4> a) {
    return le32toh(a);
}

template<>
uint_t<8> letoh_impl<8>(uint_t<8> a) {
    return le64toh(a);
}

template<typename T>
T letoh(T a) {
    return letoh_impl<sizeof(T)>(a);
}

template<int size>
uint_t<size> htole_impl(uint_t<size> a);

template<>
uint_t<1> htole_impl<1>(uint_t<1> a) {
    return a;
}

template<>
uint_t<2> htole_impl<2>(uint_t<2> a) {
    return htole16(a);
}

template<>
uint_t<4> htole_impl<4>(uint_t<4> a) {
    return htole32(a);
}

template<>
uint_t<8> htole_impl<8>(uint_t<8> a) {
    return htole64(a);
}

template<typename T>
T htole(T a) {
    return htole_impl<sizeof(T)>(a);
}

#endif
