#pragma once
#include <stdio.h>
#include <stdlib.h>

/** Read file to buffer */
char* fu_read_file(const char* path);

int fu_write_file(const char* path, const char* data);

/** Return 1 if file exists */
int fu_file_exists(const char* path);

/** Return 1 if dir exists */
int fu_dir_exists(const char* dir);

int fu_mkdir(const char* path);

void fu_abspath(const char* relpath, char* abspath);

/** NULL on error */
char* fu_cwd();
