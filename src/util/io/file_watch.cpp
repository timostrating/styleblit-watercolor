////
//// Created by sneeuwpop on 24-12-20.
////
//// based on:
////    https://github.com/ThomasMonkman/filewatch/blob/master/FileWatch.hpp
////   and
////    https://github.com/dibidabidab/gu
////

#include "file_watch.h"
#include "../debug/nice_error.h"

#ifdef linux
#include <sys/inotify.h>
#include <unistd.h>

void FileWatch::setFileToWatch(const char* fileName) {
//    fd = inotify_init1(IN_NONBLOCK);
//    if ( fd == -1 )
//        throw nice_error(std::string("Cannot initialize inotify ").append(fileName));
//
//    wd = inotify_add_watch(fd, fileName, IN_CLOSE_WRITE);
//    if ( wd == -1 )
//        throw nice_error("Cannot watch file at path: " + (std::string)fileName);
//
//    std::cout << "+ FileWatch " << fd << " " << fileName << " " << wd << std::endl;
}

bool FileWatch::hasChanged()
{
    if (fd == -1)
        return false;

    const ssize_t length = read(fd, static_cast<void*>(buffer.data()), buffer.size());

    return (length > 0);
}

FileWatch::~FileWatch() {
    inotify_rm_watch(fd, wd);
    close(fd);
    std::cout << "- FileWatch " << fd << std::endl;
}

#else
FileWatch::FileWatch(const char* fileName) { nice_error("FileWatch is only supported on Linux"); }
bool FileWatch::hasChanged() { nice_error("FileWatch is only supported on Linux"); }
FileWatch::~FileWatch() { nice_error("FileWatch is only supported on Linux"); }
#endif