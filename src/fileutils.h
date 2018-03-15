#pragma once
#include <stdio.h>
#include <stdlib.h>
#include "buffer.h"


enum file_type {
    F_FILE,
    F_DIR,
    F_UNKNOWN
};

struct file_stats {
    enum file_type type;
    const char* path;
    const char* abspath;
    unsigned int size;
    unsigned int linecount;
    int exists; /* 1 if it does */
};


/** Read's file contents into lines and returns number of lines
 * read */
unsigned int fu_read_file_lines(const char* path,
        struct line** lines);

struct file_stats fu_stats(const char* path);

/* Return 0 if file doesn't exists / error / newfile*/
unsigned int fu_file_size(const char* path);

int fu_write_file(const char* path, const char* data);

/** Return 1 if file exists */
int fu_file_exists(const char* path);

/** Return 1 if dir exists */
int fu_dir_exists(const char* dir);

int fu_mkdir(const char* path);

void fu_abspath(const char* relpath, char* abspath);

/** NULL on error */
char* fu_cwd();

void fu_pprint_stats(const struct file_stats fs);
