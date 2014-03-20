#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#include <string>
#include <vector>
#include "tuple.h"

#pragma pack(push, 1)

enum Protocol : char {
    LIST,
    CREATE_FILE,
    CREATE_DIR,
    DELETE,
};

typedef Tuple<std::string, unsigned long long> ReqList; // File path, then date
typedef Tuple<std::string, std::vector<char>> ReqCreateFile; // File path, then content
typedef std::string ReqCreateDir; // Directory path
typedef std::string ReqDelete; // File/directory path

#pragma pack(pop)

#endif
