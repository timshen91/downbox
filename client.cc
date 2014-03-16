#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include "socket.h"
#include "protocol.h"
using namespace std;

int main(int argc, char* argv[]) { 
    if (argc != 3) {
        cout << "Usage : " << argv[0] << " create|mkdir|delete filename\n";
        return 1;
    }
    TCPSocket conn;
    if (!conn.init("127.0.0.1", 9999)) {
        return 1;
    }
    char header;
    string s;
    string cmd(argv[1]); //Q
    if (cmd == "create") {
        header = CREATE_FILE;
        conn.write(header);
        s.assign(argv[2]);
        conn.write(s);
        ifstream fin(argv[2]);
        fin.seekg(0, ifstream::end);
        s.resize(fin.tellg());
        fin.seekg(0, ifstream::beg);
        fin.read(&s[0], s.size());
        fin.close();
        conn.write(s);
    } else if (cmd == "mkdir") {
        header = CREATE_DIRECTORY;
        conn.write(header);
        s.assign(argv[2]);
        conn.write(s);
    } else if (cmd == "delete") {
        header = DELETE;
        conn.write(header);
        s.assign(argv[2]);
        conn.write(s);
    } else {
        cout << "Usage : " << argv[0] << " create|delete filename\n";
        conn.close();
        return 1;
    }
    conn.close();
    return 0;
}
