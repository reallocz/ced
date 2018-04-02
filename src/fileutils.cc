#include "fileutils.h"
#include "log.h"
#include <cassert>
#include <cerrno>
#include <linux/limits.h>
#include <sys/stat.h>
#include <unistd.h>

#define TAG "FUTILS"

namespace FileUtil
{

// Standard empty constructor
Stats::Stats()
{
    type    = Type::Other;
    path    = nullptr;
    abspath = nullptr;
    size    = 0;
    exists  = false;
}


Stats::~Stats()
{
    if (path != nullptr) {
        delete[] path;
}
    if (abspath != nullptr) {
        delete[] abspath;
}
}


void Stats::pprint() const
{
    const char* typestr;
    switch (type) {
    case Type::File:
        typestr = "file";
        break;
    case Type::Dir:
        typestr = "dir";
        break;
    case Type::Other:
        typestr = "other";
        break;
    }
    log_l(TAG, "Stats{path: %s, apath: %s, fsize: %d, type: %s",
          path, abspath, size, typestr);
}


int getStats(const char* path, Stats& stats)
{
    struct stat s {
    };
    int err = 0;
    char abspath[PATH_MAX];
    realpath(path, abspath);
    err = stat(abspath, &s);
    if (err) {
        if (errno == ENOENT) {
            log_l(TAG, "File: %s doesn't exist", path);
        } else {
            log_e(TAG, "Unknown error reading stat()");
        }
        return 1;
    } else {
        stats.path    = path;
        stats.abspath = abspath;
        stats.size    = s.st_size;
        stats.exists  = true;

        // set type specific stuff (man 7 inode)
        if (S_ISREG(s.st_mode)) {    // S_IFREG = regular file
            stats.type = Type::File;
        } else if (S_ISDIR(s.st_mode)) {
            stats.type = Type::Dir;
        } else {
            stats.type = Type::Other;
        }
        stats.pprint();
        return 0;
    }
}


Buffer loadBuffer(const char* path)
{
    // TODO(realloc): Check if the file is indeed a file
    // TODO(realloc): Load empty buffer if doesnt exist

    FILE* f = fopen(path, "r");
    assert(f);
    fseek(f, 0, SEEK_END);
    unsigned int size = ftell(f);
    rewind(f);

    // Count number of lines
    unsigned int linecount = 0;
    char ch;
    while (true) {
        ch = fgetc(f);
        if (ch == '\n') {
            linecount++;
            continue;
        }
        if (ch == EOF) {
            break;
        }
    }
    rewind(f);

    auto* lines = new Line[linecount];
    assert(lines);

    unsigned int count = 0;
    // getline allocates buffer if tmpdata and tmplen == 0
    int linelen   = 0;
    size_t wtfits = 0;
    char* tmpdata = nullptr;
    while (true) {
        linelen = getline(&tmpdata, &wtfits, f);
        if (linelen == -1) {
            delete[] tmpdata; // Recommended by getline
            break;
        }

        unsigned int finallen = linelen;
        char* finaldata = tmpdata;

        // Remove terminating '\0'
        if(tmpdata[linelen] == '\0') {
            finallen = linelen - 1;

            char* data = new char[finallen];
            assert(data);
            for(unsigned int i = 0; i < finallen; ++i) {
                data[i] = tmpdata[i];
            }
            delete[] tmpdata;
            finaldata = data;
        }

        Line ln = Line(finallen, finaldata);
        lines[count++] = ln;
        tmpdata        = nullptr;
        wtfits         = 0;
    }
    fclose(f);
    log_l(TAG, "%d lines read (%d bytes)", linecount, size);
    assert(count == linecount);

    return Buffer(Buffer::Type::Document, path, lines, linecount);
}


int saveBuffer(Buffer& buf, const char* path)
{
    log_l(TAG, "Saving buffer...");
    FILE* f = fopen(path, "w");
    assert(f);
    unsigned int wbytes = 0; // Bytes written
    unsigned int wlines = 0; // Lines return
    for (unsigned int lnum = 0; lnum < buf.lineCount(); ++lnum) {
        unsigned int checkbytes = 0;
        const Line& ln = buf.getLine(lnum);
        for (unsigned int i = 0; i < ln.Len(); ++i) {
            if (!ln.inGap(i)) {
                fputc(ln[i], f);
                wbytes++;
                checkbytes++;
            }
        }
        fputc('\n', f);
        assert(checkbytes == ln.trueLen());
        wlines++;
    }
    log_l(TAG, "Buffer saved (%d lines, %d bytes): %s", wlines, wbytes, path);
    fclose(f);
    return 0;
}
} // namespace FileUtil
