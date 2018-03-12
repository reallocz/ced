#pragma once

/** buffer.h
 * A buffer is just contents of a file and some metadata.
 * It's implemented as a gap-buffer.
 */

enum buffer_type {
    DEFAULT,	// Empty buffer
    DOCUMENT,	// Saved on disk
    SCRATCH		// Temporary buffer
};

struct buffer {
    /*unique identifier of each buffer for debugging*/
    unsigned int id;
    enum buffer_type type;

    const char* name;		/* name of buffer(file) */
    char* data;			/* file in memory */
    unsigned int size;		/* size of buffer */
    unsigned int cur;		/* cursor position */
    unsigned int gappos;
    unsigned int gaplen;

    struct {
        unsigned int linecount;     /* number of '\n' + 1 */
        unsigned int curline;       /* line the cursor is on */
        unsigned int curlineindex;  /* beginning of curline*/
    } cache;
};

/** Create a new empty buffer of the specified type */
struct buffer* buf_create(enum buffer_type type);
/** buf_destroy destorys the buffer and free's the data*/
void buf_destory(struct buffer* buf);

/** Adding and deleting characters **/
/** Add a char at the current cursor position */
void buf_addch(struct buffer* buf, char ch);
/** Delete a character at current cursor position */
void buf_delch(struct buffer* buf);

/** Cursor **/
/** Move cursor forward (towards EOB) by 'n' chars */
void buf_cur_mvf(struct buffer* buf, unsigned int n);
/** Move cursor backward (beginning of buffer) by 'n' chars */
void buf_cur_mvb(struct buffer* buf, unsigned int n);
/** Move cursor to EOL/EOB. Return offset */
unsigned int buf_cur_mveol(struct buffer* buf);
/** Move cursor to the beginning of buffer. Return offset */
unsigned int buf_cur_mvbeg(struct buffer* buf);
/** Return the line on which the cursor is on */
unsigned int buf_cur_line(const struct buffer* buf);
/** Returns the offset of cursor from the last newline */
unsigned int buf_cur_lineoffset(const struct buffer* buf);

/** Return the number of newlines '\n' in the buffer */
unsigned int buf_get_linecount(const struct buffer* buf);
/** Return char at pos. check buf_in_gap().*/
char buf_get_char(const struct buffer* buf, unsigned int pos);
/** Save buffer to disk at path*/
int buf_save_to_disk(const struct buffer* buf, const char* path);

unsigned int buf_charcount(const struct buffer* buf, char ch);

/** HELPERS */
/** Return 1 if the position is inside the gap */
int buf_ingap(const struct buffer* buf, unsigned int pos);

void buf_pprint(const struct buffer* buf);
/** Print contents of buffer */
void buf_printbuf(const struct buffer* buf);
