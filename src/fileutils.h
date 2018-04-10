#pragma once
#include "buffer.h"


enum file_type { F_FILE, F_DIR, F_UNKNOWN };

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

int fu_write_file(const char* path, const char* data);


void fu_pprint_stats(const struct file_stats fs);

/** Save buffer to path */
int fu_save_buffer(const struct buffer* buf, const char* path);
