#include <sys/stat.h>
#include <signal.h>
#include <thread>
#include <fstream>
#include <iostream>
#include "socket.h"
#include "protocol.h"
#include "path.h"
#include "directory.h"
using namespace std;

#define BUF_SIZE 4096
#define WORK_PATH "/tmp"
#define ADDR "0.0.0.0"
#define PORT 9999

#define ensure(cond) do { if (!(cond)) throw std::string(__FILE__) + " " + to_string(__LINE__); } while (0)

//static std::string handle_login(TCPSocket& cli) {
//    ReqLogin req;
//    cli >> req;
//    if (req != "root") {
//        throw "Invalid username";
//    }
//    return req;
//}

static void handle_list(TCPSocket& cli) {
    ReqList req;
    cli >> req;
    RespList resp;
    Directory dir(req.data());
    req += '/';
    const char* name;
    while ((name = dir.next()) != nullptr) {
        struct stat st;
        auto old_size = req.size();
        req += name;
        auto res = stat(req.data(), &st);
        req.resize(old_size);
        if (res < 0) {
            perror("stat");
            continue;
        }
        resp.emplace_back(name, st.st_mtim.tv_sec);
    }
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

static bool delete_recursive(PathString& path) {
    if (unlink(path.data()) >= 0) {
        return true;
    } else if (errno != EISDIR) {
        return false;
    }
    try {
        Directory dir(path.data());
        path += '/';
        const char* name;
        while ((name = dir.next()) != nullptr) {
            auto old_size = path.size();
            path += name;
            auto res = delete_recursive(path);
            path.resize(old_size);
            if (!res) {
                return false;
            }
        }
    } catch (...) {
        return false;
    }
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
                    //auto username = handle_login(cli);
                    // TODO
                    while (1) {
                        uint8_t header;
                        cli >> header;
                        ensure(header < sizeof(cb_table)/(sizeof*cb_table));
                        (*cb_table[header])(cli);
                    }
                } catch (nullptr_t) {
                } catch (const std::string& e) {
                    std::cerr << e << "\n";
                } catch (const char* e) {
                    std::cerr << e << "\n";
                }
                cli.close();
            }, cli).detach();
        } catch (const std::string& e) {
        }
    }
    return 0;
}
