////
//// Created by sneeuwpop on 24-12-20.
////

#pragma once

#include <iostream>
#include <functional>
#include <string>
#include <vector>
#include <thread>

static constexpr std::size_t _buffer_size = { 1024 * 256 };
static std::vector<char> buffer( _buffer_size );

class FileWatch
{
    int fd = -1, wd = -1;

public:
    FileWatch() {};
    ~FileWatch();

    void setFileToWatch(const char* fileName);
    bool hasChanged();
};