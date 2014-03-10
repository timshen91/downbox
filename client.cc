#include "socket.h"
#include <iostream>
#include <string>
#include <vector>
using namespace std;

int main() {
    string s;
    TCPSocket conn;
    if (!conn.init("127.0.0.1", 9999)) {
	return 1;
    }
    while (getline(cin, s)) {
	size_t len;
	if (!(conn.write(s.size()) &&
	      conn.write(s.data(), s.size()) &&
	      conn.read(&len))) {
	    break;
	}
	vector<char> v(len);
	if (!(conn.read(v.data(), len))) {
	    break;
	}
	if (v.data()) {
	    cout << v.data();
	}
	cout << "\n";
    }
    conn.close();
    return 0;
}
