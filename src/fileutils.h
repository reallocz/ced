#pragma once
#include "buffer.h"
#include <stdio.h>
#include <stdlib.h>

namespace FileUtil
{

enum class Type { File, Dir, Other };


struct Stats {
    enum Type type;
    const char* path;
    const char* abspath;
    size_t size;
    bool exists;

    Stats();
    ~Stats();
    void pprint() const;
};

int getStats(const char* path, Stats& stats);

Buffer loadBuffer(const char* path);

int saveBuffer(Buffer& buf, const char* path);

}    // namespace FileUtil
