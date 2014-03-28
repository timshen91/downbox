#ifndef __ENDIAN_H__
#define __ENDIAN_H__

#include <stdint.h>
#include "endian.h""

namespace {

template<int>
struct uint;

#define UINT_GEN(n, T) \
template<>\
struct uint<n> {\
    typedef T type;\
}

UINT_GEN(1, uint8_t);
UINT_GEN(2, uint16_t);
UINT_GEN(4, uint32_t);
UINT_GEN(8, uint64_t);

#undef UINT_GEN

template<int size>
using uint_t = typename uint<size>::type;

template<int size>
uint_t<size> letoh_impl(uint_t<size> a);

template<int size>
uint_t<size> htole_impl(uint_t<size> a);

#define ENDIAN_IMPL(n, f, g) \
template<>\
uint_t<n> f<n>(uint_t<n> a) {\
    return g(a);\
}

ENDIAN_IMPL(1, letoh_impl, );
ENDIAN_IMPL(2, letoh_impl, le16toh);
ENDIAN_IMPL(4, letoh_impl, le32toh);
ENDIAN_IMPL(8, letoh_impl, le64toh);
ENDIAN_IMPL(1, htole_impl, );
ENDIAN_IMPL(2, htole_impl, htole16);
ENDIAN_IMPL(4, htole_impl, htole32);
ENDIAN_IMPL(8, htole_impl, htole64);

#undef ENDIAN_IMPL

}

template<typename T>
T letoh(T a) {
    return letoh_impl<sizeof(T)>(a);
}

template<typename T>
T htole(T a) {
    return htole_impl<sizeof(T)>(a);
}

#endif
