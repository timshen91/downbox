#ifndef __PATH_H__
#define __PATH_H__

#include "string.h"

class PathString : private String {
    typedef String BaseT;

    static std::vector<String> split(const String& s, char ch) {
        size_t last = 0;
        std::vector<String> ret;
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

    using BaseT::operator[];
    using BaseT::size;
    using BaseT::data;
    using BaseT::assign;
    using BaseT::operator+=;
    using BaseT::resize;

    void sanitize() {
        // TODO forbid "." and ".."
        if (size() <= 0) {
            throw "Empty string as a path";
        }
        auto ss = split(*this, '/');
        String t;
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

inline
TCPSocket& operator>>(TCPSocket& cli, PathString& s) {
    uint32_t len;
    cli >> len;
    s.resize(len);
    cli.read(&s[0], len);
    s[len] = '\0';
    s.sanitize();
    return cli;
}

inline
TCPSocket& operator<<(TCPSocket& cli, const PathString& s) {
    cli << (uint32_t)s.size();
    cli.write(s.data(), s.size());
    return cli;
}

#endif
