#include <sys/stat.h>
#include <signal.h>
#include <thread>
#include <fstream>
#include "socket.h"
#include "protocol.h"
#include "serialization.h"
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

#define ensure(cond) do { if (!(cond)) return false; } while (0)
static bool handle_list(TCPSocket* cli) {
    return false;
}

static bool handle_create_file(TCPSocket* cli) {
    ReqCreateFile req;
    ensure(read(cli, &req));
    ofstream fout(req.get<0>().data());
    ensure(!fout.fail());
    ensure(fout.write(req.get<1>().data(), req.get<1>().size()));
    fout.close();
    return true;
}

static bool handle_mkdir(TCPSocket* cli) {
    ReqCreateDir req;
    ensure(read(cli, &req));
    ensure(mkdir(req.data(), 0755) >= 0);
    return true;
}

static bool handle_delete(TCPSocket* cli) {
    ReqDelete req;
    ensure(read(cli, &req));
    struct stat st;
    ensure(stat(req.data(), &st) >= 0);
    if (S_ISDIR(st.st_mode)) {
        ensure(rmdir(req.data()) >= 0);
    } else if (S_ISREG(st.st_mode)) {
        ensure(unlink(req.data()) >= 0);
    } else {
        return false;
    }
    return true;
}
#undef ensure

TCPServer server;

static void sigint_handler(int signal) {
    if (signal != SIGINT) {
        return;
    }
    server.close();
    exit(0);
}

static bool (*cb_table[])(TCPSocket*) = {
    [LIST] = &handle_list,
    [CREATE_FILE] = &handle_create_file,
    [CREATE_DIR] = &handle_mkdir,
    [DELETE] = &handle_delete,
};

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
            thread([=]() mutable {
                unsigned char header;
                while (read(&cli, &header) &&
                       header < PROTOCOL_COUNT &&
                       (*cb_table[header])(&cli));
                cli.close();
            }).detach();
        }
    }
    return 0;
}
