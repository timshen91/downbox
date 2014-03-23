#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#include <vector>
#include "string.h"
#include "tuple.h"

enum Protocol : uint8_t {
    LOGIN,
    LIST,
    CREATE_FILE,
    CREATE_DIR,
    DELETE_,
};

class PathString;

typedef String ReqLogin;
// No response

typedef PathString ReqList; // Path
typedef std::vector<Tuple<PathString, uint64_t>> RespList; // A list of "File path, then date"

typedef Tuple<PathString, std::vector<char>> ReqCreateFile; // File path, then content
// No response

typedef PathString ReqCreateDir; // Directory path
// No response

typedef PathString ReqDelete; // File/directory path
// No response

#endif
