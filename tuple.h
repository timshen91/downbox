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
    static constexpr typename get_type<i, T>::type& get(T& t) {
        return get_impl<i-1, typename T::second_type>::get(t.second);
    }
};

template<typename T>
struct get_impl<0, T> {
    static constexpr typename get_type<0, T>::type& get(T& t) {
        return t.first;
    }
};

template<typename, typename...> struct read_tuple;
template<typename, typename...> struct write_tuple;

}

template<typename T, typename... Args>
class Tuple {
    typedef T first_type;
    typedef Tuple<Args...> second_type;

    T first;
    Tuple<Args...> second;

    template<size_t, typename> friend struct get_type;
    template<size_t, typename> friend struct get_impl;
    template<typename, typename...> friend struct read_tuple;
    template<typename, typename...> friend struct write_tuple;

public:
    Tuple() {}
    Tuple(const T& f, const Args&... args) : first(f), second(args...) {}
    Tuple(T&& f, Args&&... args) : first(std::move(f)), second(std::move(args)...) {}

    template<size_t i>
    typename get_type<i, Tuple<T, Args...>>::type& get() {
        return get_impl<i, Tuple<T, Args...>>::get(*this);
    }
};

template<typename T>
class Tuple<T> {
    typedef T first_type;

    T first;

    template<size_t, typename> friend struct get_type;
    template<size_t, typename> friend struct get_impl;
    template<typename, typename...> friend struct read_tuple;
    template<typename, typename...> friend struct write_tuple;

public:
    Tuple() {}
    explicit Tuple(const T& f) : first(f) {}
    explicit Tuple(T&& f) : first(std::move(f)) {}

    template<size_t i>
    typename get_type<i, Tuple<T>>::type& get() {
        return get_impl<i, Tuple<T>>::get(*this);
    }
};

namespace {

template<typename T, typename... Args>
struct read_tuple {
    static TCPSocket& read(TCPSocket& cli, Tuple<T, Args...>& t) {
        return cli >> t.first >> t.second;
    }
};

template<typename T>
struct read_tuple<T> {
    static TCPSocket& read(TCPSocket& cli, Tuple<T>& t) {
        return cli >> t.first;
    }
};

template<typename T, typename... Args>
struct write_tuple {
    static TCPSocket& write(TCPSocket& cli, const Tuple<T, Args...>& t) {
        return cli << t.first << t.second;
    }
};

template<typename T>
struct write_tuple<T> {
    static TCPSocket& write(TCPSocket& cli, const Tuple<T>& t) {
        return cli << t.first;
    }
};

}

template<typename T, typename... Args>
TCPSocket& operator>>(TCPSocket& cli, Tuple<T, Args...>& t) {
    return read_tuple<T, Args...>::read(cli, t);
}

template<typename T>
TCPSocket& operator>>(TCPSocket& cli, Tuple<T>& t) {
    return read_tuple<T>::read(cli, t);
}

template<typename T, typename... Args>
TCPSocket& operator<<(TCPSocket& cli, const Tuple<T, Args...>& t) {
    return write_tuple<T, Args...>::write(cli, t);
}

template<typename T>
TCPSocket& operator<<(TCPSocket& cli, const Tuple<T>& t) {
    return write_tuple<T>::write(cli, t);
}

#endif
