#include "fileutils.h"
#include <sys/stat.h>
#include <limits.h>
#include <errno.h>
#include <assert.h>
#include <unistd.h>
#include "log.h"

#define TAG "FUTILS"

struct file_stats fu_stats(const char* path)
{
    struct file_stats fs;
    fs.exists = 0;
    fs.path = path;
    fs.size = 0;
    fs.type = F_FILE;

    struct stat s;
    int err = 0;
    char abspath[PATH_MAX];
    realpath(path, abspath);
    err = stat(abspath, &s);
    if(err) {
        if(errno == ENOENT) {
            log_l(TAG, "File: %s doesn't exist", path);
        } else {
            log_e(TAG, "Unknown error reading stat()");
        }
        return fs;
    } else {
        // man 7 inode
        if(S_ISREG(s.st_mode)) { // S_IFREG = regular file
            fs.type = F_FILE;
        } else if(S_ISDIR(s.st_mode)) {
            fs.type = F_DIR;
        }
        fs.path = path;
        fs.abspath = abspath;
        fs.size = s.st_size;
        fs.exists = 1;
        log_l(TAG, "%s: path: %s, apath: %s, fsize: %d, type: %s",
                __func__, fs.path, fs.abspath, fs.size,
                fs.type == F_FILE ? "file" : "dir");
        return fs;
    }
}


int fu_read_to_buffer(const char* path, char* buffer)
{
    FILE* f = fopen(path, "r");
    assert(f);
    long bytes = 0;
    fseek(f, 0, SEEK_END);
    bytes = ftell(f);
    rewind(f);
    fread(buffer, sizeof(char), bytes, f);
    fclose(f);
    return 0;
}


int fu_write_file(const char* path, const char* data)
{
    // TODO
    return 0;
}

void fu_abspath(const char* relpath, char* abspath)
{
    realpath(relpath, abspath);
}

int fu_file_exists(const char* path)
{
    struct stat s;
    int err = 0;
    char abspath[PATH_MAX];
    realpath(path, abspath);
    err = stat(abspath, &s);

    if(err) {
        /* Either it doesn't exist or we lack the permissions.
         * Either way we failed */
        return 0;
    }
    // man 7 inode
    if((s.st_mode & S_IFMT) == S_IFREG) { // S_IFREG = regular file
        return 1;
    } else {
        return 0;
    }
}

int fu_dir_exists(const char* path)
{
    struct stat s;
    int err = 0;
    err = stat(path, &s);

    char abspath[PATH_MAX];
    realpath(path, abspath);
    if(err) {
        // Either it doesn't exist or we lack the permissions. Either way
        // we failed
        return 0;
    }
    // man 7 inode
    if((s.st_mode & S_IFMT) == S_IFDIR) { // S_IFDIR = directory
        return 1;
    } else {
        return 0;
    }
}

char* fu_cwd()
{
    return getcwd(NULL, PATH_MAX);
}
