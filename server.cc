#include <sys/stat.h>
#include <signal.h>
#include <thread>
#include <fstream>
#include <iostream>
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

#define ensure(cond) do { if (!(cond)) goto fail; } while (0)
void work(TCPSocket cli) {
    char header;
    string s;
    while (1) {
        ensure(cli.read(&header));
        switch (header) {
        case CREATE_FILE:
            ensure(cli.read(&s));
            {
                ofstream fout(s.data());
                ensure(!fout.fail());
                ensure(cli.read(&s));
                ensure(fout.write(s.data(), s.size()));
                fout.close();
            }
            break;
        case CREATE_DIRECTORY:
            ensure(cli.read(&s));
            ensure(mkdir(s.data(), 0755) >= 0);
            break;
        case DELETE:
            ensure(cli.read(&s));
            {
                struct stat st;
                ensure(stat(s.data(), &st) >= 0);
                if (S_ISDIR(st.st_mode)) {
                    ensure(rmdir(s.data()) >= 0);
                } else if (S_ISREG(st.st_mode)) {
                    ensure(unlink(s.data()) >= 0);
                } else {
                    goto fail;
                }
            }
            break;
        default: goto fail;
        }
    }
fail:
    cli.close();
}
#undef ensure

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
