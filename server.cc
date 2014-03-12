#include <signal.h>
#include <vector>
#include <iostream>
#include <thread>
#include "socket.h"
using namespace std;

void work(TCPSocket cli) {
    while (1) {
        size_t len;
        if (!cli.read(&len)) {
            break;
        }
        vector<char> v(len);
        if (!(cli.read(v.data(), len) &&
              cli.write(len) &&
              cli.write(v.data(), len))) {
            break;
        }
    }
    cli.close();
}

TCPServer server;

void sigint_handler(int signal) {
    if (signal != SIGINT) {
        return;
    }
    server.close();
    exit(0);
}

int main() {
    {
        struct sigaction action;
        action.sa_handler = sigint_handler;
        if (sigaction(SIGINT, &action, nullptr) < 0) {
            perror("sigaction");
            return 1;
        }
    }
    if (!server.init("0.0.0.0", 9999)) {
        return 1;
    }
    while (1) {
        TCPSocket cli;
        if (server.accept(&cli)) {
            thread(work, cli).detach();
        }
    }
    return 0;
}
