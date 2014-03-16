#include <signal.h>
#include <thread>
#include <fstream>
#include "socket.h"
#include "protocol.h"
using namespace std;

#define BUF_SIZE 4096
#define WORK_PATH "/tmp"
#define ADDR "0.0.0.0"
#define PORT 9999

//vector<string> split(const string& s, char ch) {
//    size_t last = 0;
//    vector<string> ret;
//    size_t i;
//    for (i = 0; i < s.size(); i++) {
//        if (s[i] == ch) {
//            ret.emplace_back(s.substr(last, i - last));
//            last = i+1;
//        }
//    }
//    ret.emplace_back(s.substr(last, i - last));
//    return move(ret);
//}

bool validate(const string& s) { // TODO
    for (auto c : s) {
        if (c == '\0') {
            return false;
        }
    }
    return true;
}

#define ensure(cond) do { if (!(cond)) goto fail; } while (0)
void work(TCPSocket cli) {
    char header;
    string s;
    while (1) {
        ensure(cli.read(&header));
        switch (header) {
        case CREATE_FILE:
            ensure(cli.read(&s));
            ensure(validate(s));
            {
                ofstream fout(s.data());
                cli.read(&s);
                fout.write(s.data(), s.size());
                fout.close();
            }
            break;
        case CREATE_DIRECTORY:
            break;
        case DELETE:
            ensure(cli.read(&s));
            ensure(validate(s));
            unlink(s.data());
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
    if (chroot(WORK_PATH) < 0 || chdir("/") < 0) {
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
    if (!server.init(ADDR, PORT)) {
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
