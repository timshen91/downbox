#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>
#include "socket.h"
#include "protocol.h"
using namespace std;

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cout << "Usage : " << argv[0] << " create|delete filename\n";
        return 1;
    }
    string cmd(argv[1]);
    if (cmd != "create" && cmd != "delete") {
        return 1;
    }
    string s;
    TCPSocket conn;
    if (!conn.init("127.0.0.1", 9999)) {
        return 1;
    }
    char header;
    vector<char> v;
    if (cmd == "create") {
        header = CREATE_FILE;
        conn.write(header);
        string s(argv[2]);
        v.assign(s.begin(), s.end());
        conn.writebytes(v);
        v.clear();
        ifstream fin(argv[2]);
        fin.seekg(0, ifstream::end);
        v.resize(fin.tellg());
        fin.seekg(0, ifstream::beg);
        fin.read(v.data(), v.size());
        fin.close();
        conn.writebytes(v);
    } else {
        header = DELETE;
        conn.write(header);
        string s(argv[2]);
        v.assign(s.begin(), s.end());
        conn.writebytes(v);
    }
    conn.close();
    return 0;
}
