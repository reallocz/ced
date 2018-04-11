#include "fileutils.h"
#define _GNU_SOURCE    // Required to use 'getline' function
#include "line.h"
#include "log.h"
#include <assert.h>
#include <errno.h>
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#define TAG "FUTILS"

struct file_stats fu_stats(const char* path)
{
    struct file_stats fs;
    fs.exists = 0;
    fs.path   = path;
    fs.size   = 0;
    fs.type   = F_FILE;

    struct stat s;
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


size_t fu_read_file_lines(const char* path, struct line** lines)
{
    FILE* f = fopen(path, "r");
    assert(f);
    fseek(f, 0, SEEK_END);
    size_t size = ftell(f);
    rewind(f);

    // Count number of lines
    size_t linecount = 0;
    char ch;
    while (1) {
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
    // TODO(realloc): Fix 0 line malloc case and return
    struct line* mlines = malloc(linecount * sizeof(struct line));
    assert(mlines);

    size_t count = 0;
    // getline allocates buffer if tmpdata and tmplen == 0
    int linelen   = 0;
    size_t wtfits = 0;
    char* tmpdata = NULL;
    while (1) {
        linelen = getline(&tmpdata, &wtfits, f);
        if (linelen == -1) {
            free(tmpdata);    // Recommended by getline
            break;
        }
        struct line* ln = &mlines[count++];    // First line
        ln->len         = linelen - 1;         // Ignore trailing \0
        ln->data        = tmpdata;
        /*log_l(TAG, "line: size=%d", ln->len, ln->data);*/
        tmpdata = 0;
        wtfits  = 0;
    }
    fclose(f);
    log_l(TAG, "%d lines read (%d bytes)", linecount, size);
    assert(count == linecount);

    *lines = mlines;
    return linecount;
}


int fu_save_buffer(const struct buffer* buf, const char* path)
{
    log_l(TAG, "Saving buffer...");
    FILE* f = fopen(path, "w");
    assert(f);
    size_t wbytes = 0;
    size_t lcount = 0;
    for (size_t lnum = 0; lnum < buf->linecount; ++lnum) {
        struct line* ln = buf_getline(buf, lnum);
        lcount++;
        for (size_t i = 0; i < ln->len; ++i) {
            if (!INGAP(ln, i)) {
                fputc(ln->data[i], f);
                wbytes++;
            }
        }
        fputc('\n', f);
    }
    log_l(TAG, "Buffer saved (%d lines, %d bytes): %s", lcount,
          wbytes, path);
    fclose(f);
    return 0;
}
