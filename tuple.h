#ifndef __TUPLE_H__
#define __TUPLE_H__

namespace {

template<size_t i, typename T, typename... Args>
struct get_type {
    typedef typename get_type<i-1, Args...>::type type;
};

template<typename T, typename... Args>
struct get_type<0, T, Args...> {
    typedef T type;
};

}

template<typename... Args>
class Tuple {};

template<typename T, typename... Args>
class Tuple<T, Args...> {
    T first;
    Tuple<Args...> second;

    typedef T first_type;
    typedef Tuple<Args...> second_type;

public:
    Tuple() {}
    Tuple(const T& f, const Args&... args) : first(f), second(args...) {}
    Tuple(T&& f, Args&&... args) : first(std::move(f)), second(std::move(args)...) {}

    first_type& get_first() {
        return first;
    }

    const first_type& get_first() const {
        return first;
    }

    second_type& get_rest() {
        return second;
    }

    const second_type& get_rest() const {
        return second;
    }

    template<size_t i>
    typename get_type<i, T, Args...>::type& get();

    template<size_t i>
    const typename get_type<i, T, Args...>::type& get() const;
};

namespace {

template<size_t i, typename T, typename... Args>
struct get_impl {
    static constexpr typename get_type<i, T, Args...>::type& get(Tuple<T, Args...>& t) {
        return get_impl<i-1, Args...>::get(t.get_rest());
    }

    static constexpr const typename get_type<i, T, Args...>::type& get(const Tuple<T, Args...>& t) {
        return get_impl<i-1, Args...>::get(t.get_rest());
    }
};

template<typename T, typename... Args>
struct get_impl<0, T, Args...> {
    static constexpr typename get_type<0, T, Args...>::type& get(Tuple<T, Args...>& t) {
        return t.get_first();
    }

    static constexpr const typename get_type<0, T, Args...>::type& get(const Tuple<T, Args...>& t) {
        return t.get_first();
    }
};

template<typename StreamT, typename... Args>
struct rw_tuple {
    static constexpr StreamT& read(StreamT& cli, Tuple<Args...>& t) {
        return cli;
    }

    static constexpr StreamT& write(StreamT& cli, const Tuple<Args...>& t) {
        return cli;
    }
};

template<typename StreamT, typename T, typename... Args>
struct rw_tuple<StreamT, T, Args...> {
    static constexpr StreamT& read(StreamT& cli, Tuple<T, Args...>& t) {
        return cli >> t.get_first() >> t.get_rest();
    }

    static constexpr StreamT& write(StreamT& cli, const Tuple<T, Args...>& t) {
        return cli << t.get_first() << t.get_rest();
    }
};

}

template<typename T, typename... Args>
template<size_t i>
typename get_type<i, T, Args...>::type& Tuple<T, Args...>::get() {
    return get_impl<i, T, Args...>::get(*this);
}

template<typename T, typename... Args>
template<size_t i>
const typename get_type<i, T, Args...>::type& Tuple<T, Args...>::get() const {
    return get_impl<i, T, Args...>::get(*this);
}

template<typename IStreamT, typename... Args>
constexpr IStreamT& operator>>(IStreamT& cli, Tuple<Args...>& t) {
    return rw_tuple<IStreamT, Args...>::read(cli, t);
}

template<typename OStreamT, typename... Args>
constexpr OStreamT& operator<<(OStreamT& cli, const Tuple<Args...>& t) {
    return rw_tuple<OStreamT, Args...>::write(cli, t);
}

#endif
