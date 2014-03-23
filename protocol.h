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

class PathString : private std::string {
    typedef std::string BaseT;

    static std::vector<std::string> split(const std::string& s, char ch) {
        size_t last = 0;
        std::vector<std::string> ret;
        size_t i;
        for (i = 0; i < s.size(); i++) {
            if (s[i] == '\0') {
                throw "\\0 in a string";
            }
            if (s[i] == ch) {
                ret.emplace_back(s.substr(last, i - last));
                last = i+1;
            }
        }
        ret.emplace_back(s.substr(last, i - last));
        return move(ret);
    }

public:
    PathString() {}
    PathString(const char* s) : BaseT(s) {}

    void resize(size_t newsize) {
        if (newsize > 4096) {
            throw "Path name too long";
        }
        BaseT::resize(newsize);
    }

    using BaseT::operator[];
    using BaseT::size;
    using BaseT::data;
    using BaseT::assign;
    using BaseT::operator+=;

    void sanitize() {
        // TODO forbid "." and ".."
        if (size() <= 0) {
            throw "Empty string as a path";
        }
        auto ss = split(*this, '/');
        std::string t;
        for (auto& s : ss) {
            if (s.size() == 0) {
                continue;
            }
            if (s == "." || s == "..") {
                throw ". or .. as a dirctory";
            }
            t += '/';
            t += s;
        }
        this->assign(t);
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
