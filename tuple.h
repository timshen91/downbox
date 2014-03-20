#ifndef __TUPLE_H__
#define __TUPLE_H__

template<typename T, typename... Args>
struct Tuple {
    T first;
    Tuple<Args...> second;
};

template<typename T>
struct Tuple<T> {
    T first;
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
