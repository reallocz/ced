#pragma once

#define INVALID_BUFFER -1

/** buffer.h
 * A buffer is just contents of a file and some metadata.
 * It's implemented as a gap-buffer.
 */


/**
 * Buffer handle
 * Implemented in similar fashion as hWindow (window.h)
 */
typedef int hBuffer;

enum buffer_type {
    DEFAULT,	// Empty buffer
    DOCUMENT,	// Saved on disk
    SCRATCH		// Temporary buffer
};

struct buf_props {
    unsigned int size;
    unsigned int cur;
    unsigned int linecount;
    struct {
        unsigned int pos;
        unsigned int len;
    } gap;
};

/** Initialize buffer submodule */
int buf_init();
int buf_exit();

/** Create a new empty buffer of the specified type */
hBuffer buf_create(enum buffer_type type);
/** buf_destroy destorys the buffer and free's the data*/
void buf_destory(hBuffer buf);

/** Adding and deleting characters **/
/** Add a char at the current cursor position */
void buf_addch(hBuffer buf, char ch);
/** Delete a character at current cursor position */
void buf_delch(hBuffer buf);

/** Cursor **/
/** Returns the cursor position from the start of buffer (0) */
unsigned int buf_cur_get(hBuffer buf);
/** Move cursor forward (towards EOB) by 'n' chars */
void buf_cur_mvf(hBuffer buf, unsigned int n);
/** Move cursor backward (beginning of buffer) by 'n' chars */
void buf_cur_mvb(hBuffer buf, unsigned int n);
/** Move cursor to EOL/EOB */
unsigned int buf_cur_mveol(hBuffer buf);


/** Return buffer properties */
struct buf_props buf_get_props(hBuffer buf);
/** Return the number of newlines '\n' in the buffer */
unsigned int buf_get_linecount(hBuffer buf);
/** Return size of the buffer data */
unsigned int buf_get_size(hBuffer buf);
/** Return char at pos. check buf_in_gap().*/
char buf_get_char(hBuffer buf, unsigned int pos);
/** Save buffer to disk at path*/
int buf_save_to_disk(hBuffer buf, const char* path);


void buf_pprint(hBuffer buf);
/** Print contents of buffer */
void buf_printbuf(const hBuffer buf);
