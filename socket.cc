#include "socket.h"
#include <string>
#include <vector>

template<typename T>
bool TCPSocket::read(T* obj) {
    static_assert(std::is_arithmetic<T>::value, "Unimplemented unserialization");
    return read_impl(obj, sizeof(T));
}

template<typename T>
bool TCPSocket::write(const T& obj) {
    static_assert(std::is_arithmetic<T>::value, "Unimplemented serialization");
    return write_impl(&obj, sizeof(T));
}

template bool TCPSocket::read<char>(char* obj);
template bool TCPSocket::write<char>(const char& obj);

template<>
bool TCPSocket::read<std::string>(std::string* s) {
    size_t len;
    if (!read(&len)) {
        return false;
    }
    s->resize(len);
    if (!read_impl(&s->front(), len)) {
        return false;
    }
    (*s)[len] = '\0';
    return true;
}

template<>
bool TCPSocket::write<std::string>(const std::string& s) {
    if (!write(s.size())) {
        return false;
    }
    if (!write_impl(s.data(), s.size())) {
        return false;
    }
    return true;
}

template<>
bool TCPSocket::read<std::vector<char>>(std::vector<char>* s) {
    size_t len;
    if (!read(&len)) {
        return false;
    }
    s->resize(len);
    if (!read_impl(s->data(), len)) {
        return false;
    }
    return true;
}

template<>
bool TCPSocket::write<std::vector<char>>(const std::vector<char>& s) {
    return write(s.size()) && write_impl(s.data(), s.size());
}
