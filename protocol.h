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

typedef std::string ReqList; // Path
typedef std::vector<Tuple<std::string, uint64_t>> RespList; // A list of "File path, then date"

typedef Tuple<std::string, std::vector<char>> ReqCreateFile; // File path, then content
// No response

typedef std::string ReqCreateDir; // Directory path
// No response

typedef std::string ReqDelete; // File/directory path
// No response

#endif
