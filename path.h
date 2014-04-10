#ifndef __PATH_H__
#define __PATH_H__

class PathString : public std::string {
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

template<typename IStreamT>
IStreamT& operator>>(IStreamT& cli, PathString& s) {
    cli >> static_cast<std::string&>(s);
    s.sanitize();
    return cli;
}

template<typename OStreamT>
OStreamT& operator<<(OStreamT& cli, const PathString& s) {
    return cli << static_cast<const std::string&>(s);
}

#endif
