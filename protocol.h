#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#include <vector>
#include "tuple.h"
#include "path.h"

enum Protocol : uint8_t {
    SYNC,
    LIST,
    CREATE_FILE,
    CREATE_DIR,
    DELETE_,
    MOVE,
};

typedef PathString ReqSync; // Path
typedef std::vector<char> RespSync; // Content

typedef std::string ReqLogin; // Username so far.
// No response

typedef PathString ReqList; // Path
typedef std::vector<Tuple<std::string, uint64_t>> RespList; // A list of "File path, then date"

typedef Tuple<PathString, std::vector<char>> ReqCreateFile; // File path, then content
// No response

typedef PathString ReqCreateDir; // Directory path
// No response

typedef PathString ReqDelete; // File/directory path
// No response

typedef Tuple<PathString, PathString> ReqMove; // source path; destination path
// No response

#endif
