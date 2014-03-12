#include <signal.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <thread>
#include "socket.h"
#include "protocol.h"
using namespace std;

const char work_path[] = "/tmp/";

bool validate(const vector<char> v) {
    for (auto c : v) {
        if (c == '/' || c == '\0') {
            return false;
        }
    }
    return true; // TODO
}

#define ensure(cond) do { if (!(cond)) goto fail; } while (0)
void work(TCPSocket cli) {
    char header;
    vector<char> v;
    while (1) {
        ensure(cli.read(&header));
        switch (header) {
        case CREATE_FILE:
          ensure(cli.readbytes(&v));
          ensure(validate(v));
          {
              v.push_back('\0');
              ofstream fout(v.data());
              cli.readbytes(&v);
              fout.write(v.data(), v.size());
              fout.close();
          }
          break;
        case DELETE:
          ensure(cli.readbytes(&v));
          ensure(validate(v));
          v.push_back('\0');
          unlink((char*)v.data());
          break;
        default: goto fail;
        }
    }
fail:
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
    if (chdir(work_path) < 0) {
        return 1;
    }
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
    TCPSocket cli;
    while (1) {
        if (server.accept(&cli)) {
            thread(work, cli).detach();
        }
    }
    return 0;
}
