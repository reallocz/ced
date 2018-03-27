#include "fileutils.h"
#include "log.h"
#include <cassert>
#include <cerrno>
#include <linux/limits.h>
#include <sys/stat.h>
#include <unistd.h>

#define TAG "FUTILS"

struct file_stats fu_stats(const char* path)
{
    struct file_stats fs {
    };
    fs.exists = 0;
    fs.path   = path;
    fs.size   = 0;
    fs.type   = F_FILE;

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
        return fs;
    } else {
        fs.path    = path;
        fs.abspath = abspath;
        fs.size    = s.st_size;
        fs.exists  = 1;

        // set type specific stuff (man 7 inode)
        if (S_ISREG(s.st_mode)) {    // S_IFREG = regular file
            fs.type = F_FILE;
        } else if (S_ISDIR(s.st_mode)) {
            fs.type = F_DIR;
        } else {
            fs.type = F_UNKNOWN;
        }
        log_l(TAG, "%s: path: %s, apath: %s, fsize: %d, type: %s",
              __func__, fs.path, fs.abspath, fs.size,
              fs.type == F_FILE ? "file" : "dir");
        return fs;
    }
}


unsigned int fu_read_file_lines(const char* path,
                                Buffer::Line** lines)
{
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

    auto* mlines = static_cast<Buffer::Line*>(malloc(linecount * sizeof(Buffer::Line)));
    assert(mlines);

    unsigned int count = 0;
    // getline allocates buffer if tmpdata and tmplen == 0
    int linelen   = 0;
    size_t wtfits = 0;
    char* tmpdata = nullptr;
    while (true) {
        linelen = getline(&tmpdata, &wtfits, f);
        if (linelen == -1) {
            break;
        }
        Buffer::Line* ln = &mlines[count++];    // First line
        ln->len          = linelen;
        ln->data         = tmpdata;
        /*log_l(TAG, "line: size=%d", ln->len, ln->data);*/
        tmpdata = nullptr;
        wtfits  = 0;
    }
    fclose(f);
    log_l(TAG, "%d lines read (%d bytes)", linecount, size);
    assert(count == linecount);

    *lines = mlines;
    return linecount;
}


void fu_abspath(const char* relpath, char* abspath)
{
    realpath(relpath, abspath);
}

int fu_file_exists(const char* path)
{
    struct stat s {
    };
    int err = 0;
    char abspath[PATH_MAX];
    realpath(path, abspath);
    err = stat(abspath, &s);

    if (err) {
        /* Either it doesn't exist or we lack the permissions.
         * Either way we failed */
        return 0;
    }
    // man 7 inode
    if ((s.st_mode & S_IFMT) == S_IFREG) {    // S_IFREG = regular file
        return 1;
    } else {
        return 0;
    }
}

int fu_dir_exists(const char* path)
{
    struct stat s {
    };
    int err = 0;
    err     = stat(path, &s);

    char abspath[PATH_MAX];
    realpath(path, abspath);
    if (err) {
        // Either it doesn't exist or we lack the permissions. Either way
        // we failed
        return 0;
    }
    // man 7 inode
    if ((s.st_mode & S_IFMT) == S_IFDIR) {    // S_IFDIR = directory
        return 1;
    } else {
        return 0;
    }
}

char* fu_cwd()
{
    return getcwd(nullptr, PATH_MAX);
}

int fu_save_buffer(Buffer* buf, const char* path)
{
    log_l(TAG, "Saving buffer...");
    FILE* f = fopen(path, "w");
    assert(f);
    unsigned int wbytes = 0;
    unsigned int lcount = 0;
    for (unsigned int lnum = 0; lnum < buf->lineCount(); ++lnum) {
        Buffer::Line* ln = buf->line(lnum);
        lcount++;
        for (unsigned int i = 0; i < ln->len; ++i) {
            if (buf->lineHasGap(lnum)) {
                // Line with the gap
                if (!buf->inGap(i)) {
                    fputc(ln->data[i], f);
                    wbytes++;
                }
            } else {
                // Lines without the gap
                fputc(ln->data[i], f);
                wbytes++;
            }
        }
    }
    log_l(TAG, "Buffer saved (%d lines, %d bytes): %s", lcount, wbytes, path);
    fclose(f);
    return 0;
}
