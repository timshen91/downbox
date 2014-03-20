#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include "socket.h"
#include "protocol.h"
#include "serialization.h"
using namespace std;

void print_usage(char* argv[]) {
    cout << "Usage : " << argv[0] << " create|mkdir|delete filename\n";
}

#define ensure(cond) do { if (!(cond)) goto fail; } while (0)
int main(int argc, char* argv[]) { 
    if (argc != 3) {
        print_usage(argv);
        return 1;
    }
    TCPSocket conn;
    if (!conn.init("127.0.0.1", 9999)) {
        return 1;
    }
    string cmd(argv[1]);
    if (cmd == "create") {
        ensure(write(&conn, (char)CREATE_FILE));
        ReqCreateFile req;
        req.first.assign(argv[2]);
        ifstream fin(argv[2]);
        fin.seekg(0, ifstream::end);
        req.second.first.resize(fin.tellg());
        fin.seekg(0, ifstream::beg);
        fin.read(req.second.first.data(), req.second.first.size());
        fin.close();
        ensure(write(&conn, req));
    } else if (cmd == "mkdir") {
        ensure(write(&conn, (char)CREATE_DIR));
        ReqCreateDir req;
        req.assign(argv[2]);
        ensure(write(&conn, req));
    } else if (cmd == "delete") {
        ensure(write(&conn, (char)DELETE));
        ReqDelete req;
        req.assign(argv[2]);
        ensure(write(&conn, req));
    } else {
        print_usage(argv);
        goto fail;
        return 1;
    }
    conn.close();
    return 0;
fail:
    conn.close();
    return 1;
}
