#ifndef __SOCKET_H__
#define __SOCKET_H__

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <limits.h>
#include <type_traits>

class TCPSocket {
    int sockfd;

    friend class TCPServer;

public:
    bool init(const char* dest_addr, uint16_t port) {
        if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            perror("socket");
            return false;
        }
        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        if (inet_aton(dest_addr, &addr.sin_addr) == 0) {
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
        ::close(sockfd);
    }

    void read(void* buff, ssize_t len) {
        while (len > 0) {
            ssize_t count = len;
            if (count > SSIZE_MAX) {
                count = SSIZE_MAX;
            }
            auto n = ::read(sockfd, buff, count);
            if (n < 0) {
                perror("read");
                throw nullptr;
            }
            if (n == 0) {
                throw nullptr;
            }
            len -= n;
            buff = (void*)((uintptr_t)buff + count);
        }
    }

    void write(const void* buff, int len) {
        while (len > 0) {
            int n = ::write(sockfd, buff, len);
            if (n < 0) {
                perror("write");
                throw nullptr;
            }
            if (n == 0) {
                throw nullptr;
            }
            len -= n;
            buff = (void*)((uintptr_t)buff + n);
        }
    }
};

class TCPServer {
    int sockfd;
    struct sockaddr_in addr;

public:
    bool init(const char* listen_addr, uint16_t port) {
        if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            perror("socket");
            return false;
        }
        int yes = 1;
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) < 0) {
            perror("setsockopt");
            return false;
        }
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        if (inet_aton(listen_addr, &addr.sin_addr) == 0) {
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
        ::close(sockfd);
    }

    TCPSocket accept() {
        TCPSocket ret;
        if ((ret.sockfd = ::accept(sockfd, nullptr, nullptr)) < 0) {
            throw nullptr;
        }
        return ret;
    }
};

#endif
