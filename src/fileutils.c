#include "fileutils.h"
#include <sys/stat.h>
#include <limits.h>
#include <errno.h>
#include <assert.h>
#include <unistd.h>

char* fu_read_file(const char* path)
{
    // TODO
    return NULL;
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
    err = stat(path, &s);

    char abspath[PATH_MAX];
    realpath(path, abspath);
    if(err) {
        // Either it doesn't exist or we lack the permissions. Either way
        // we failed
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
