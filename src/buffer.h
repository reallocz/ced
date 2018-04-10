#pragma once
#include "common.h"
#include "line.h"

/** buffer.h
 * A buffer is a collection of lines
 */

enum buffer_type {
    DEFAULT,     // Empty buffer
    DOCUMENT,    // Saved on disk
    SCRATCH,     // Temporary buffer
    TESTBUFFER,
};

struct buffer {
    /*unique identifier of each buffer for debugging*/
    size_t id;
    enum buffer_type type;
    const char* name; /* name of buffer(file) */

    /** lines */
    struct line* lines;
    size_t linecount;
};

/** Create a new empty buffer of the specified type */
struct buffer* buf_create_empty(enum buffer_type type);
struct buffer buf_create_file(enum buffer_type type,
                              const char* filename);
/** buf_destroy destorys the buffer and free's the data*/
void buf_destroy(struct buffer* buf);

/** INTERNALAdd a new line at the cursor. Copy the portion after the
 * cursor on the newline and place it at the beginning of the
 * newline. NOTE: the '\n' (newline) character must be passed to
 * the addch function. */
void buf_addline(struct buffer* b, struct cursor cur);

/** Return pointer to the line number */
Line* buf_getline(const struct buffer* buf, size_t num);
/** Return the number of newlines '\n' in the buffer */
size_t buf_line_count(const struct buffer* buf);

/** Save buffer to disk at path*/
int buf_save_to_disk(const struct buffer* buf, const char* path);

/** HELPERS */

void buf_pprint(const struct buffer* buf);
void buf_printline(const struct buffer* buf, size_t i);
void buf_pprint_lines(const struct buffer* buf);
