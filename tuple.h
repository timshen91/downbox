#ifndef __TUPLE_H__
#define __TUPLE_H__

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

template<typename, typename...> struct rw_tuple;

}

template<typename... Args>
class Tuple {
};

template<typename T, typename... Args>
class Tuple<T, Args...> {
    typedef T first_type;
    typedef Tuple<Args...> second_type;

    T first;
    Tuple<Args...> second;

    template<size_t, typename> friend struct get_type;
    template<size_t, typename> friend struct get_impl;
    template<typename, typename...> friend struct rw_tuple;

public:
    Tuple() {}
    Tuple(const T& f, const Args&... args) : first(f), second(args...) {}
    Tuple(T&& f, Args&&... args) : first(std::move(f)), second(std::move(args)...) {}

    template<size_t i>
    typename get_type<i, Tuple<T, Args...>>::type& get() {
        return get_impl<i, Tuple<T, Args...>>::get(*this);
    }
};

namespace {

template<typename StreamT, typename... Args>
struct rw_tuple {
    static StreamT& read(StreamT& cli, Tuple<Args...>& t) {
        return cli;
    }

    static StreamT& write(StreamT& cli, const Tuple<Args...>& t) {
        return cli;
    }
};

template<typename StreamT, typename T, typename... Args>
struct rw_tuple<StreamT, T, Args...> {
    static StreamT& read(StreamT& cli, Tuple<T, Args...>& t) {
        return cli >> t.first >> t.second;
    }

    static StreamT& write(StreamT& cli, const Tuple<T, Args...>& t) {
        return cli << t.first << t.second;
    }
};

}

template<typename IStreamT, typename... Args>
IStreamT& operator>>(IStreamT& cli, Tuple<Args...>& t) {
    return rw_tuple<IStreamT, Args...>::read(cli, t);
}

template<typename OStreamT, typename... Args>
OStreamT& operator<<(OStreamT& cli, const Tuple<Args...>& t) {
    return rw_tuple<OStreamT, Args...>::write(cli, t);
}

#endif
