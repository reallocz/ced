#pragma once
#include "common.h"

/** buffer.h
 * A buffer is a collection of lines
 */

enum buffer_type {
    DEFAULT,     // Empty buffer
    DOCUMENT,    // Saved on disk
    SCRATCH,     // Temporary buffer
    TESTBUFFER,
};


/** Line metadata */
struct line {
    size_t len; /** length of line */
    char* data;       /** line data */
};


/** A buffer gap is like a cursor in the document */
struct buffer_gap {
    size_t line;
    size_t col;
    size_t size;
};

struct buffer {
    /*unique identifier of each buffer for debugging*/
    size_t id;
    enum buffer_type type;
    const char* name; /* name of buffer(file) */

    /** gap */
    struct buffer_gap gap;
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

/** Adding and deleting characters **/
/** Add a char at the cursor position */
void buf_addch(struct buffer* b, char ch, struct cursor cur);
/** Delete a character at current cursor position */
void buf_delch(struct buffer* b, struct cursor cur);
/** INTERNALAdd a new line at the cursor. Copy the portion after the
 * cursor on the newline and place it at the beginning of the
 * newline. NOTE: the '\n' (newline) character must be passed to
 * the addch function. */
void buf_addline(struct buffer* b, struct cursor cur);

/** Return pointer to the line number */
struct line* buf_line(const struct buffer* buf, size_t num);
/** Return the number of newlines '\n' in the buffer */
size_t buf_line_count(const struct buffer* buf);
/** Return 1 if line has gap */
int buf_line_hasgap(const struct buffer* buf, size_t line);

/** Save buffer to disk at path*/
int buf_save_to_disk(const struct buffer* buf, const char* path);

/** Count number of occurences of char in buffer between cursors
 * from and including 'from' upto and excluding 'to' */
size_t buf_charcount_sec(const struct buffer* b, char ch,
                               struct cursor from, struct cursor to);
/** Count number of occurences of char in the whole buffer */
size_t buf_charcount(const struct buffer* buf, char ch);
/** Return char at pos. check buf_in_gap().*/
char buf_charat(const struct buffer* b, struct cursor cur);

int buf_ingap(const struct buffer* b, size_t i);

/** HELPERS */

void buf_pprint(const struct buffer* buf);
void buf_printline(const struct buffer* buf, size_t i);
void buf_pprint_lines(const struct buffer* buf);
