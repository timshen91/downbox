#include <fstream>
#include <iostream>
#include <iterator>
#include "socket.h"
#include "protocol.h"
#include "path.h"
using namespace std;

void print_usage(char* argv[]) {
    cout << "Usage : " << argv[0] << " list|create|mkdir|delete filename\n";
}

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
    if (cmd == "sync") {
        conn << (uint8_t)SYNC;
        ReqSync req = argv[2];
        conn << req;
        RespSync resp;
        conn >> resp;
        ofstream fout(argv[2]);
        if (!fout) {
            goto fail;
        }
        fout.write(resp.data(), resp.size());
        fout.close();
    } else if (cmd == "list") {
        conn << (uint8_t)LIST;
        ReqList req = argv[2];
        conn << req;
        RespList resp;
        conn >> resp;
        for (auto& it : resp) {
            cout << it.get<0>().data() << " " << it.get<1>() << "\n";
        }
    } else if (cmd == "create") {
        conn << (uint8_t)CREATE_FILE;
        ReqCreateFile req;
        req.get<0>().assign(argv[2]);
        ifstream fin(argv[2]);
        if (fin.fail()) {
            goto fail;
        }
        fin.seekg(0, ifstream::end);
        req.get<1>().resize(fin.tellg());
        fin.seekg(0, ifstream::beg);
        fin.read(req.get<1>().data(), req.get<1>().size());
        fin.close();
        conn << req;
    } else if (cmd == "mkdir") {
        conn << (uint8_t)CREATE_DIR;
        ReqCreateDir req;
        req.assign(argv[2]);
        conn << req;
    } else if (cmd == "delete") {
        conn << (uint8_t)DELETE_;
        ReqDelete req;
        req.assign(argv[2]);
        conn << req;
    } else {
        print_usage(argv);
        goto fail;
    }
    conn.close();
    return 0;
fail:
    conn.close();
    return 1;
}
