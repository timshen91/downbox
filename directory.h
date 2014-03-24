#ifndef __DIRECTORY_H__
#define __DIRECTORY_H__

#include <dirent.h>
#include <string.h>

struct Directory {
    DIR* d;

    Directory(const Directory&) = delete;
    Directory(Directory&& rhs) = default;

    // opendir, readdir and closedir are Linux system calls.
    Directory(const char* s) {
        if ((d = opendir(s)) == nullptr) {
            perror("opendir");
            throw "Open directory failed";
        }
    }

    ~Directory() {
        if (closedir(d) < 0) {
            perror("closedir");
        }
    }

    Directory& operator=(const Directory&) = delete;
};

#endif
