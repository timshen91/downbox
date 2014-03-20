#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include "socket.h"
#include "protocol.h"
using namespace std;

void print_usage(char* argv[]) {
    cout << "Usage : " << argv[0] << " create|mkdir|delete filename\n";
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
    if (cmd == "create") {
        conn.write((char)CREATE_FILE);
        ReqCreateFile req;
        req.path.assign(argv[2]);
        ifstream fin(argv[2]);
        fin.seekg(0, ifstream::end);
        req.content.resize(fin.tellg());
        fin.seekg(0, ifstream::beg);
        fin.read(req.content.data(), req.content.size());
        fin.close();
        conn.write(req);
    } else if (cmd == "mkdir") {
        conn.write((char)CREATE_DIR);
        ReqCreateDir req;
        req.path.assign(argv[2]);
        conn.write(req);
    } else if (cmd == "delete") {
        conn.write((char)DELETE);
        ReqDelete req;
        req.path.assign(argv[2]);
        conn.write(req);
    } else {
        print_usage(argv);
        conn.close();
        return 1;
    }
    conn.close();
    return 0;
}
