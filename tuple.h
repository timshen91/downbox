#ifndef __TUPLE_H__
#define __TUPLE_H__

#include "socket.h"

namespace {

template<size_t i, typename T>
struct get_type {
    typedef typename get_type<i-1, typename T::second_type>::type type;
};

template<typename T>
struct get_type<0, T> {
    typedef typename T::first_type type;
};

template<size_t i, typename T>
struct get_impl {
    constexpr typename get_type<i, T>::type& operator()(T& t) {
        return get_impl<i-1, typename T::second_type>()(t.second);
    }
};

template<typename T>
struct get_impl<0, T> {
    constexpr typename get_type<0, T>::type& operator()(T& t) {
        return t.first;
    }
};

}

template<typename T, typename... Args>
class Tuple {
    typedef T first_type;
    typedef Tuple<Args...> second_type;

    T first;
    Tuple<Args...> second;

    template<size_t, typename> friend struct get_type;
    template<size_t, typename> friend struct get_impl;
    template<typename Tp, typename... Ap>
    friend bool read(TCPSocket* cli, Tuple<Tp, Ap...>* t);
    template<typename Tp, typename... Ap>
    friend bool write(TCPSocket* cli, const Tuple<Tp, Ap...>& t);

public:
    template<size_t i>
    typename get_type<i, Tuple<T, Args...>>::type& get() {
        return get_impl<i, Tuple<T, Args...>>()(*this);
    }
};

template<typename T>
class Tuple<T> {
    typedef T first_type;

    T first;

    template<size_t, typename> friend struct get_type;
    template<size_t, typename> friend struct get_impl;
    template<typename Tp>
    friend bool read(TCPSocket* cli, Tuple<Tp>* t);
    template<typename Tp>
    friend bool write(TCPSocket* cli, const Tuple<Tp>& t);

public:
    template<size_t i>
    typename get_type<i, Tuple<T>>::type& get() {
        return get_impl<i, Tuple<T>>()(*this);
    }
};

template<typename T, typename... Args>
bool read(TCPSocket* cli, Tuple<T, Args...>* t) {
    return read(cli, &t->first) && read(cli, &t->second);
}

template<typename T>
bool read(TCPSocket* cli, Tuple<T>* t) {
    return read(cli, &t->first);
}

template<typename T, typename... Args>
bool write(TCPSocket* cli, const Tuple<T, Args...>& t) {
    return write(cli, t.first) && write(cli, t.second);
}

template<typename T>
bool write(TCPSocket* cli, const Tuple<T>& t) {
    return write(cli, t.first);
}

#endif
