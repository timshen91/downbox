#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#include <string>
#include <vector>

#pragma pack(push, 1)

enum Protocol : char {
    LIST,
    CREATE_FILE,
    CREATE_DIR,
    DELETE,
};

struct ReqCreateFile {
    std::string path;
    std::vector<char> content;
};

template<>
bool TCPSocket::read<ReqCreateFile>(ReqCreateFile* req) {
    return read(&req->path) && read(&req->content);
}

template<>
bool TCPSocket::write<ReqCreateFile>(const ReqCreateFile& req) {
    return write(req.path) && write(req.content);
}

struct ReqCreateDir {
    std::string path;
};

template<>
bool TCPSocket::read<ReqCreateDir>(ReqCreateDir* req) {
    return read(&req->path);
}

template<>
bool TCPSocket::write<ReqCreateDir>(const ReqCreateDir& req) {
    return write(req.path);
}

#pragma pack(pop)

#endif
