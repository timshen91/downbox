#ifndef __DIR_H__
#define __DIR_H__

#include <dirent.h>

class Directory {
    DIR* d;
    int fd;

public:
    Directory(const Directory&) = delete;
    Directory(Directory&& rhs) = default;

    // opendir, readdir and closedir are Linux system calls.
    Directory(const char* s) {
        if ((d = opendir(s)) == nullptr) {
            perror("opendir");
            throw "Open directory failed";
        }
        if ((fd = dirfd(d)) < 0) {
            perror("dirfd");
            throw "Get dir fd failed";
        }
    }

    ~Directory() {
        if (closedir(d) < 0) {
            perror("closedir");
        }
    }

    Directory& operator=(const Directory&) = delete;

    int get_fd() {
        return fd;
    }

    const char* next() {
        struct dirent* ent;
        if ((ent = readdir(d)) == nullptr) {
            return nullptr;
        }
        return ent->d_name;
    }
};

#endif
