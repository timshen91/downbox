#ifndef __SOCKET_H__
#define __SOCKET_H__

#ifdef _WIN32
#include <WinSock2.h>
#pragma comment(lib, "Ws2_32.lib")
#include <stdint.h>
typedef SSIZE_T ssize_t;
#define constexpr
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#endif
#include <stdio.h>
#include <limits.h>
#include <type_traits>
#include <vector>
#include "endian.h"

#define error() do { throw std::string(__FILE__) + " " + std::to_string(__LINE__); } while (0)

class TCPSocket {
    int sockfd;

    friend class TCPServer;

    void read_impl(void* buff, ssize_t len) {
        while (len > 0) {
            ssize_t count = len;
#ifdef _WIN32
            auto n = ::recv(sockfd, (char*)buff, count, 0);
#else
            auto n = ::read(sockfd, buff, count);
#endif
            if (n < 0) {
                perror("read");
                error();
            }
            if (n == 0) {
                throw nullptr;
            }
            len -= n;
            buff = (void*)((uintptr_t)buff + n);
        }
    }

    void write_impl(const void* buff, ssize_t len) {
        while (len > 0) {
#ifdef _WIN32
            auto n = ::send(sockfd, (char*)buff, len, 0);
#else
            auto n = ::write(sockfd, buff, len);
#endif
            if (n < 0) {
                perror("write");
                error();
            }
            if (n == 0) {
                throw nullptr;
            }
            len -= n;
            buff = (void*)((uintptr_t)buff + n);
        }
    }

public:
    bool init(const char* dest_addr, uint16_t port) {
        if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            perror("socket");
            return false;
        }
        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
#ifdef _WIN32
        if ((addr.sin_addr.S_un.S_addr = inet_addr(dest_addr)) == INADDR_NONE) {
#else
        if (inet_aton(dest_addr, &addr.sin_addr) == 0) {
#endif
            perror("inet_aton");
            return false;
        }
        if (connect(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
            perror("connect");
            return false;
        }
        return true;
    }

    void close() {
#ifdef _WIN32
        closesocket(sockfd);
#else
        ::close(sockfd);
#endif
    }

    template<typename T>
    typename std::enable_if<!std::is_arithmetic<T>::value, void>::type read(T* arr, size_t len) {
        for (size_t i = 0; i < len; i++) {
            *this >> arr[i];
        }
    }

    template<typename T>
    typename std::enable_if<!std::is_arithmetic<T>::value, void>::type write(const T* arr, size_t len) {
        for (size_t i = 0; i < len; i++) {
            *this << arr[i];
        }
    }

    template<typename T>
    typename std::enable_if<std::is_arithmetic<T>::value, void>::type read(T* arr, size_t len) {
        read_impl(arr, len * sizeof(T));
    }

    template<typename T>
    typename std::enable_if<std::is_arithmetic<T>::value, void>::type write(const T* arr, size_t len) {
        write_impl(arr, len * sizeof(T));
    }
};

class TCPServer {
    int sockfd;

public:
    bool init(const char* listen_addr, uint16_t port) {
        if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            perror("socket");
            return false;
        }
        int yes = 1;
#ifdef _WIN32
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char*)&yes, sizeof(yes)) < 0) {
#else
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0) {
#endif
            perror("setsockopt");
            return false;
        }
        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
#ifdef _WIN32
        if ((addr.sin_addr.S_un.S_addr = inet_addr(listen_addr)) == INADDR_NONE) {
#else
        if (inet_aton(listen_addr, &addr.sin_addr) == 0) {
#endif
            perror("inet_aton");
            return false;
        }
        if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
            perror("bind");
            return false;
        }
        if (listen(sockfd, 10) < 0) {
            perror("listen");
            return false;
        }
        return true;
    }

    void close() {
#ifdef _WIN32
        closesocket(sockfd);
#else
        ::close(sockfd);
#endif
    }

    TCPSocket accept() {
        TCPSocket ret;
        if ((ret.sockfd = ::accept(sockfd, nullptr, nullptr)) < 0) {
            error();
        }
        return ret;
    }
};

template<typename T>
typename std::enable_if<std::is_arithmetic<T>::value, TCPSocket&>::type operator>>(TCPSocket& cli, T& obj) {
    cli.read(&obj, 1);
    obj = letoh(obj);
    return cli;
}

template<typename T>
typename std::enable_if<std::is_arithmetic<T>::value, TCPSocket&>::type operator<<(TCPSocket& cli, const T& obj) {
    auto o = htole(obj);
    cli.write(&o, 1);
    return cli;
}

template<typename T>
TCPSocket& operator>>(TCPSocket& cli, std::vector<T>& v) {
    uint32_t len;
    cli >> len;
    v.resize(len);
    cli.read(v.data(), v.size());
    return cli;
}

template<typename T>
TCPSocket& operator<<(TCPSocket& cli, const std::vector<T>& v) {
    cli << (uint32_t)v.size();
    cli.write(v.data(), v.size());
    return cli;
}

#endif
