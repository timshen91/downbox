#ifndef __SERIALIZATION_H__
#define __SERIALIZATION_H__

#include <string>
#include <vector>
#include "socket.h"
#include "endian.h"

template<typename T>
typename std::enable_if<std::is_arithmetic<T>::value, TCPSocket&>::type operator>>(TCPSocket& cli, T& obj) {
    auto o = letoh(obj);
    cli.read(&o, 1);
    return cli;
}

template<typename T>
typename std::enable_if<std::is_arithmetic<T>::value, TCPSocket&>::type operator<<(TCPSocket& cli, const T& obj) {
    auto o = htole(obj);
    cli.write(&o, 1);
    return cli;
}

inline
TCPSocket& operator>>(TCPSocket& cli, std::string& s) {
    size_t len;
    cli >> len;
    s.resize(len);
    cli.read(&s[0], len);
    s[len] = '\0';
    return cli;
}

inline
TCPSocket& operator<<(TCPSocket& cli, const std::string& s) {
    cli << s.size();
    cli.write(s.data(), s.size());
    return cli;
}

template<typename T>
TCPSocket& operator>>(TCPSocket& cli, std::vector<T>& v) {
    size_t len;
    cli >> len;
    v.resize(len);
    cli.read(v.data(), v.size());
    return cli;
}

template<typename T>
TCPSocket& operator<<(TCPSocket& cli, const std::vector<T>& v) {
    cli << v.size();
    cli.write(v.data(), v.size());
    return cli;
}

#endif
