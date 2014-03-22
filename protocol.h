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
typedef std::vector<Tuple<std::string, time_t>> RespList; // A list of "File path, then date"
typedef Tuple<std::string, std::vector<char>> ReqCreateFile; // File path, then content
typedef std::string ReqCreateDir; // Directory path
typedef std::string ReqDelete; // File/directory path

#endif
