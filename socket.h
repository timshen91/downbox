#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
//#include <sys/epoll.h>
#include <unistd.h>
#include <stdio.h>
#include <limits.h>
//#include <vector>

class TCPSocket {
    int sockfd;
    struct sockaddr_in addr;

    friend class TCPServer;

public:
    bool init(const char* dest_addr, uint16_t port) {
        if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            perror("socket");
            return false;
        }
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

    bool read(void* buff, ssize_t len) {
        while (len > 0) {
            ssize_t count = len;
            if (count > SSIZE_MAX) {
                count = SSIZE_MAX;
            }
            auto n = ::read(sockfd, buff, count);
            if (n < 0) {
                perror("read");
                return false;
            }
            if (n == 0) {
                return false;
            }
            len -= n;
            buff = (void*)((uintptr_t)buff + count);
        }
        return true;
    }

    template<typename T>
      bool read(T* obj) {
          return read(obj, sizeof(T));
      }

    bool write(const void* buff, int len) {
        while (len > 0) {
            int n = ::write(sockfd, buff, len);
            if (n < 0) {
                perror("write");
                return false;
            }
            if (n == 0) {
                return false;
            }
            len -= n;
            buff = (void*)((uintptr_t)buff + n);
        }
        return true;
    }

    template<typename T>
      bool write(const T& obj) {
          return write(&obj, sizeof(T));
      }
};

// invariant: events.size() == clients.size()
class TCPServer {
    int sockfd;
//    int epollfd;
    struct sockaddr_in addr;
//    std::vector<struct epoll_event> events;
//    std::vector<TCPSocket> clients; // FIXME reuse
//    std::vector<uint32_t> free_list; // reuse clients
//
//    struct epoll_event ev; // tmp

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
//        if ((epollfd = epoll_create1(0)) < 0) {
//            perror("epoll_create");
//            return false;
//        }
//        ev.events = EPOLLIN;
//        ev.data.u32 = 0;
//        if (epoll_ctl(epollfd, EPOLL_CTL_ADD, sockfd, &ev) < 0) {
//            perror("epoll_ctl");
//            return false;
//        }
//        clients.resize(1);
//        events.resize(1);
        return true;
    }

    void close() {
        ::close(sockfd);
    }

    bool accept(TCPSocket* cli) {
        socklen_t len = sizeof(cli->addr);
        return (cli->sockfd = ::accept(sockfd, (struct sockaddr*)&cli->addr, &len)) >= 0;
    }

/*
    void epoll(void (*callback)(const TCPSocket&)) {
        int n = epoll_wait(epollfd, events.data(), events.size(), -1);
        if (n < 0) {
            return;
        }
        for (auto& it : events) {
            if (it.data.u32 == 0) {
                TCPSocket cli;
                socklen_t len = sizeof(cli.addr);
                if ((cli.sockfd = ::accept(sockfd, (struct sockaddr*)&cli.addr, &len)) < 0) {
                    perror("accept");
                    continue;
                }
                ev.events = EPOLLIN | EPOLLOUT;
#define EPCTL \
                    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, cli.sockfd, &ev) < 0) {\
                        perror("epoll_ctl");\
                        continue;\
                    }
                if (free_list.size() > 0) {
                    ev.data.u32 = free_list.back();
                    EPCTL;
                    free_list.pop_back();
                } else {
                    ev.data.u32 = clients.size();
                    EPCTL;
                    clients.emplace_back();
                }
#undef EPCTL
                clients[ev.data.u32] = cli;
                events.resize(clients.size());
            } else {
                callback(clients[it.data.u32], it.events);
            }
        }
    }
*/
};
