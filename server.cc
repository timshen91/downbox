#include <vector>
#include <iostream>
#include "socket.h"
using namespace std;

int main() {
    TCPServer server;
    if (!server.init("0.0.0.0", 9999)) {
        return 1;
    }
    while (1) {
        TCPSocket cli;
        if (!server.accept(&cli)) {
            continue;
        }
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
    server.close();
    return 0;
}
