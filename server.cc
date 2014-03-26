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

static void handle_sync(TCPSocket& cli, const string& home) {
    ReqSync req;
    auto path = home + req;
    ifstream fin(path);
    if (!fin) {
        throw "No such file";
    }
    RespSync resp;
    fin.seekg(0, ifstream::end);
    resp.resize(fin.tellg());
    fin.seekg(0, ifstream::beg);
    fin.read(resp.data(), resp.size());
    fin.close();
    cli << resp;
}

static void iterate_dir(const string& path, const function<void (string&&)>& cb) {
    Directory d(path.data());
    struct dirent* ent;
    while ((ent = readdir(d.d)) != nullptr) {
        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) {
            continue;
        }
        cb(path + "/" + ent->d_name);
    }
}

static void handle_list(TCPSocket& cli, const string& home) {
    ReqList req;
    cli >> req;
    string path = home + move(req);
    RespList resp;
    iterate_dir(path, [&](string&& name) {
        struct stat st;
        auto res = stat(name.data(), &st);
        if (res < 0) {
            perror("stat");
            return;
        }
        resp.emplace_back(name, st.st_mtim.tv_sec);
    });
    cli << resp;
}

static void handle_create_file(TCPSocket& cli, const string& home) {
    ReqCreateFile req;
    cli >> req;
    string path = home + move(req.get<0>());
    // ofstream is the C++ way to read a file.
    ofstream fout(path);
    auto res = !fout.fail() && fout.write(req.get<1>().data(), req.get<1>().size());
    fout.close();
    ensure(res);
}

static void handle_mkdir(TCPSocket& cli, const string& home) {
    ReqCreateDir req;
    cli >> req;
    ensure(mkdir((home + move(req)).data(), 0755) >= 0);
}

static void delete_recursive(const string& path) {
    if (unlink(path.data()) >= 0) {
        return;
    } else if (errno != EISDIR) {
        throw "Not a dir nor a file";
    }
    iterate_dir(path, [&](string&& name) {
        delete_recursive(name);
    });
    ensure(rmdir(path.data()) >= 0);
}

static void handle_delete(TCPSocket& cli, const string& home) {
    ReqDelete req;
    cli >> req;
    delete_recursive(home + move(req));
}

TCPServer server;

static void sigint_handler(int signal) {
    if (signal != SIGINT) {
        return;
    }
    server.close();
    exit(0);
}

static void (*cb_table[])(TCPSocket&, const string&) = {
    [SYNC] = &handle_sync,
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
                    const string home = "/root";
                    mkdir(home.data(), 0755);
                    while (1) {
                        uint8_t header;
                        cli >> header;
                        ensure(header < sizeof(cb_table)/(sizeof(*cb_table)));
                        (*cb_table[header])(cli, home);
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
