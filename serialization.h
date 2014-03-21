#ifndef __SERIALIZATION_H__
#define __SERIALIZATION_H__

#include <string>
#include <vector>
#include "socket.h"

template<typename T>
typename std::enable_if<std::is_arithmetic<T>::value, TCPSocket&>::type operator>>(TCPSocket& cli, T& obj) {
    cli.read(&obj, sizeof(T));
    return cli;
}

template<typename T>
typename std::enable_if<std::is_arithmetic<T>::value, TCPSocket&>::type operator<<(TCPSocket& cli, const T& obj) {
    cli.write(&obj, sizeof(T));
    return cli;
}

inline
TCPSocket& operator>>(TCPSocket& cli, std::string& s) {
    size_t len;
    cli >> len;
    s.resize(len);
    cli.read(&s.front(), len);
    s[len] = '\0';
    return cli;
}

inline
TCPSocket& operator<<(TCPSocket& cli, const std::string& s) {
    cli << s.size();
    cli.write(s.data(), s.size());
    return cli;
}

inline
TCPSocket& operator>>(TCPSocket& cli, std::vector<char>& s) {
    size_t len;
    cli >> len;
    s.resize(len);
    cli.read(s.data(), len);
    return cli;
}

inline
TCPSocket& operator<<(TCPSocket& cli, const std::vector<char>& s) {
    cli << s.size();
    cli.write(s.data(), s.size());
    return cli;
}

#endif
