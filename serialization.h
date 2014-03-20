#ifndef __SERIALIZATION_H__
#define __SERIALIZATION_H__

#include <string>
#include <vector>
#include "socket.h"

template<typename T>
typename std::enable_if<std::is_arithmetic<T>::value, bool>::type read(TCPSocket* cli, T* obj) {
    return cli->read(obj, sizeof(T));
}

template<typename T>
typename std::enable_if<std::is_arithmetic<T>::value, bool>::type write(TCPSocket* cli, const T& obj) {
    return cli->write(&obj, sizeof(T));
}

bool read(TCPSocket* cli, std::string* s) {
    size_t len;
    if (!read(cli, &len)) {
        return false;
    }
    s->resize(len);
    if (!cli->read(&s->front(), len)) {
        return false;
    }
    (*s)[len] = '\0';
    return true;
}

bool write(TCPSocket* cli, const std::string& s) {
    return write(cli, s.size()) && cli->write(s.data(), s.size());
}

bool read(TCPSocket* cli, std::vector<char>* s) {
    size_t len;
    if (!read(cli, &len)) {
        return false;
    }
    s->resize(len);
    if (!cli->read(s->data(), len)) {
        return false;
    }
    return true;
}

bool write(TCPSocket* cli, const std::vector<char>& s) {
    return write(cli, s.size()) && cli->write(s.data(), s.size());
}

#endif
