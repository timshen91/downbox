#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#include <string>
#include <vector>
#include "tuple.h"

enum Protocol : uint8_t {
    LIST,
    CREATE_FILE,
    CREATE_DIR,
    DELETE_,
};

struct PathString : public std::string {
    PathString() {}
    PathString(const char* s) : std::string(s) {}

    void sanitize() {
        // TODO forbid "." and ".."
    }
};

typedef PathString ReqList; // Path
typedef std::vector<Tuple<PathString, uint64_t>> RespList; // A list of "File path, then date"

typedef Tuple<PathString, std::vector<char>> ReqCreateFile; // File path, then content
// No response

typedef PathString ReqCreateDir; // Directory path
// No response

typedef PathString ReqDelete; // File/directory path
// No response

#endif
