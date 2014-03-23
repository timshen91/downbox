#include <sys/stat.h>
#include <dirent.h>
#include <signal.h>
#include <string.h>
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

vector<string> split(const string& s, char ch) {
    size_t last = 0;
    vector<string> ret;
    size_t i;
    for (i = 0; i < s.size(); i++) {
        if (s[i] == ch) {
            ret.emplace_back(s.substr(last, i - last));
            last = i+1;
        }
    }
    ret.emplace_back(s.substr(last, i - last));
    return move(ret);
}

#define ensure(cond) do { if (!(cond)) throw std::string(__FILE__) + " " + to_string(__LINE__); } while (0)

static void handle_list(TCPSocket& cli) {
    ReqList req;
    cli >> req;
    // opendir, readdir and closedir are Linux system calls.
    DIR* dir;
    ensure((dir = opendir(req.data())) != nullptr);
    RespList resp;
    struct dirent* ent;
    while ((ent = readdir(dir)) != nullptr) {
        const char* name = ent->d_name;
        struct stat st;
        if (stat((req + "/" + name).data(), &st) < 0) {
            perror("stat");
            continue;
        }
        resp.emplace_back(name, st.st_mtim.tv_sec);
    }
    closedir(dir);
    cli << resp;
}

static void handle_create_file(TCPSocket& cli) {
    ReqCreateFile req;
    cli >> req;
    // ofstream is the C++ way to read a file.
    ofstream fout(req.get<0>().data());
    auto res = !fout.fail() && fout.write(req.get<1>().data(), req.get<1>().size());
    fout.close();
    ensure(res);
}

static void handle_mkdir(TCPSocket& cli) {
    ReqCreateDir req;
    cli >> req;
    ensure(mkdir(req.data(), 0755) >= 0);
}

static bool delete_recursive(const string& path) {
    if (unlink(path.data()) >= 0) {
        return true;
    } else if (errno != EISDIR) {
        return false;
    }
    DIR* d;
    if ((d = opendir(path.data())) == nullptr) {
        return false;
    }
    struct dirent* ent;
    while ((ent = readdir(d)) != nullptr) {
        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) {
            continue;
        }
        if (!delete_recursive(path + "/" + ent->d_name)) {
            closedir(d);
            return false;
        }
    }
    closedir(d);
    rmdir(path.data());
    return true;
}

static void handle_delete(TCPSocket& cli) {
    ReqDelete req;
    cli >> req;
    ensure(delete_recursive(req));
}

TCPServer server;

static void sigint_handler(int signal) {
    if (signal != SIGINT) {
        return;
    }
    server.close();
    exit(0);
}

static void (*cb_table[])(TCPSocket&) = {
    [LIST] = &handle_list,
    [CREATE_FILE] = &handle_create_file,
    [CREATE_DIR] = &handle_mkdir,
    [DELETE_] = &handle_delete,
};

int main() {
    // chroot to make a filesystem jail. It'll be a little troubler when supporting multiple users.
    if (chroot(WORK_PATH) < 0 || chdir("/") < 0) {
        return 1;
    }
    // Register the action on the signal SIGINT (typically triggered when the user hits Ctrl+c in the terminal).
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
    // Server main loop:
    // 0) listen on the port;
    // 1) accept a new client;
    // 2) create a new thread;
    while (1) {
        try {
            TCPSocket cli = server.accept();
            // C++11 lambda.
            thread([](TCPSocket cli) {
                try {
                    // The thread get request at a time, dispatch it to the correspond handler (cb_table, means callback table) by the header.
                    while (1) {
                        uint8_t header;
                        cli >> header;
                        ensure(header < sizeof(cb_table)/(sizeof*cb_table));
                        (*cb_table[header])(cli);
                    }
                } catch (const std::string& e) {
                }
                cli.close();
            }, cli).detach();
        } catch (const std::string& e) {
        }
    }
    return 0;
}
