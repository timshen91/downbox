#ifndef __STRING_H__
#define __STRING_H__

#include <string>

struct String : public std::string {
    typedef std::string BaseT;

    String() {}
    String(const std::string& s) : BaseT(s) {}
    String(const char* s) : BaseT(s) {}

    void resize(size_t newsize) {
        if (newsize > 4096) {
            throw "String too long";
        }
        BaseT::resize(newsize);
    }
};

inline
TCPSocket& operator>>(TCPSocket& cli, String& s) {
    uint32_t len;
    cli >> len;
    s.resize(len);
    cli.read(&s[0], len);
    s[len] = '\0';
    return cli;
}

inline
TCPSocket& operator<<(TCPSocket& cli, const String& s) {
    cli << (uint32_t)s.size();
    cli.write(s.data(), s.size());
    return cli;
}

#endif
