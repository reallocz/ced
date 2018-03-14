#pragma once
#include "cursor.h"

/** buffer.h
 * A buffer is a collection of lines
 */

enum buffer_type {
    DEFAULT,	// Empty buffer
    DOCUMENT,	// Saved on disk
    SCRATCH		// Temporary buffer
};


/** Line metadata */
struct line {
    unsigned int len;   /** length of line */
    char* data;         /** line data */
};


/** A buffer gap is like a cursor in the document */
struct buffer_gap {
    unsigned int line;
    unsigned int col;
    unsigned int size;
};

struct buffer {
    /*unique identifier of each buffer for debugging*/
    unsigned int id;
    enum buffer_type type;
    const char* name;		/* name of buffer(file) */

    /** gap */
    struct buffer_gap gap;
    /** lines */
    struct line* lines;
    unsigned int linecount;
};

/** Create a new empty buffer of the specified type */
struct buffer* buf_create_empty(enum buffer_type type);
/** buf_destroy destorys the buffer and free's the data*/
void buf_destory(struct buffer* buf);

/** Adding and deleting characters **/
/** Add a char at the cursor position */
void buf_addch(struct buffer* b, char ch, struct cursor cur);
/** Delete a character at current cursor position */
void buf_delch(struct buffer* b, struct cursor cur);

/** Return pointer to the line number */
struct line* buf_line(struct buffer* buf, unsigned int num);
/** Return the number of newlines '\n' in the buffer */
unsigned int buf_line_count(const struct buffer* buf);

/** Save buffer to disk at path*/
int buf_save_to_disk(const struct buffer* buf, const char* path);

/** Count number of occurences of char in buffer between cursors
 * from and including 'from' upto and excluding 'to' */
unsigned int buf_charcount_sec(const struct buffer* b, char ch,
        struct cursor from, struct cursor to);
/** Count number of occurences of char in the whole buffer */
unsigned int buf_charcount(const struct buffer* buf, char ch);
/** Return char at pos. check buf_in_gap().*/
char buf_charat(const struct buffer* b, struct cursor cur);

/** HELPERS */

void buf_pprint(const struct buffer* buf);
void buf_pprint_lines(const struct buffer* buf);
