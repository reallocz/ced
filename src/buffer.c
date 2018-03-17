#include "buffer.h"
#include "buffer_internal.h"
#include "fileutils.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "log.h"
#include "input_keys.h"

#define BUFFER_GAPSIZE 4

/** Evals to 1 if i is inside buffer gap */
#define INGAP(buf, i)\
    ((i >= buf->gap.col && i < buf->gap.col + buf->gap.size))

#define CURVALID(buf, cur)\
    (cur.line < buf->linecount)

#define TAG "BUFFER"

const char* testlines[] = {
    "    First line with 4 gaps at the start that must stay hidden",
    "Second line: @{*(U! 08cv09c8xv c0v89x09cv80xbv8lk123jl1k23",
    "Third - the quick brown fox jumped over the lazy dog\\",
    "Romeo and Juliet :) :( :/ :* ",
};
const int testlinecount = 4;

struct buffer* buf_create_test() {
    struct buffer* buf = malloc(sizeof(struct buffer));
    assert(buf);

    buf->id = generate_id();
    buf->type = TESTBUFFER;
    buf->name = "TEST_BUFFER";

    /** gap */
    buf->gap.line = 0;
    buf->gap.col = 0;
    buf->gap.size = BUFFER_GAPSIZE;

    /** lines */
    buf->linecount = testlinecount;
    buf->lines = calloc(buf->linecount, sizeof(struct line));
    assert(buf->lines);

    for(unsigned int i = 0; i < buf->linecount; ++i) {
        buf->lines[i].data = testlines[i];
        buf->lines[i].len = strlen(testlines[i]);
    }

    return buf;
}

struct buffer* buf_create_empty(enum buffer_type type)
{
    struct buffer* buf = malloc(sizeof(struct buffer));
    assert(buf);

    buf->id = generate_id();
    buf->type = type; // TODO type specific setup
    buf->name = "[NEWBUF]";

    /** gap */
    buf->gap.line = 0;
    buf->gap.col = 0;
    buf->gap.size = BUFFER_GAPSIZE;

    /** lines */
    buf->linecount = 1;    // Empty buffer has at least one line
    buf->lines = calloc(buf->linecount, sizeof(struct line));
    assert(buf->lines);

    // Set gap on the first line
    struct line* fln = buf_line(buf, 0);
    fln->len = BUFFER_GAPSIZE;
    fln->data = calloc(fln->len, sizeof(char));
    return buf;
}

struct buffer* buf_create_file(enum buffer_type type,
        const char* filename)
{
    struct file_stats fstats = fu_stats(filename);
    if(!(fstats.exists == 1 && F_FILE == fstats.type)) {
        if(fstats.type == F_DIR) {
            log_fatal(TAG, "%s: cannot open:%s. (Its a directory)",
                    __func__, filename);
        } else {
            log_fatal(TAG, "%s: couldn't open:%s. exiting.",
                    __func__, filename);
            /*// Return a new empty buffer;*/
            /*struct buffer* buf = buf_create_empty(type);*/
            /*buf->name = filename;*/
            /*return buf;*/
        }
    }

    struct buffer* buf = malloc(sizeof(struct buffer));
    assert(buf);

    buf->id = generate_id();
    buf->type = type;
    buf->name = filename;

    /** gap */
    buf->gap.line = 0;
    buf->gap.col = 5;       // TODO temporary for interject.txt
    buf->gap.size = BUFFER_GAPSIZE;


    /** lines */
    buf->lines = NULL;
    buf->linecount = fu_read_file_lines(fstats.path, &buf->lines);
    assert(buf->lines);

    return buf;
}


void buf_destroy(struct buffer* buf)
{
    unsigned int id = buf->id;
    for(unsigned int i = 0; i < buf->linecount; ++i) {
        struct line* ln = &buf->lines[i];
        free(ln->data);
    }
    free(buf->lines);
    free(buf);
    log_l(TAG, "Buffer destroyed (id=%d)", id);
    buf = NULL;
}



/** Adding a character to the buffer requires a gap synchronization.
 * It may also require a gap resize(increase). The order of the two
 * operations is not fixed right now but I don't think it really
 * makes a difference */
void buf_addch(struct buffer* buf, char ch, struct cursor cur)
{
    gap_move(buf, cur);
    gap_resize_optional(buf);

    struct line* ln = buf_line(buf, cur.line);
    ln->data[buf->gap.col] = ch;
    // shrink gap
    buf->gap.col++;
    buf->gap.size--;
}

/** Deleting a character in the buffer has the same constraints as
 * the add operation - apart from the buffer resizing */
void buf_delch(struct buffer* buf, struct cursor cur)
{
    if(cur.col < 1) {
        return;
    }
    gap_move(buf, cur);
    buf->gap.col--;
    buf->gap.size++;
}

/** Returns pointer to line in buffer. NULL if 'num' is out of bounds*/
struct line* buf_line(const struct buffer* buf, unsigned int num)
{
    if(num >= buf->linecount) {
        /*log_e(TAG, "Invalid line request: buf: %d (%d lines), request: %d", buf->id, buf->linecount, num);*/
        return NULL;
    }
    return &buf->lines[num];
}


unsigned int buf_line_count(const struct buffer* buf)
{
    return buf->linecount;
}

int buf_line_hasgap(const struct buffer* buf, unsigned int line)
{
    return buf->gap.line == line;
}

unsigned int buf_charcount_sec(const struct buffer* b, char ch,
        struct cursor from, struct cursor to)
{
    return 0;
    /*assert(from < b->size && to <= b->size && from <= to);*/

    /*unsigned int count = 0;*/
    /*for(unsigned int i = from; i < to; ++i) {*/
        /*if((!INGAP(b, i)) && b->data[i] == ch) {*/
            /*count++;*/
        /*}*/
    /*}*/
    /*return count;*/
}


unsigned int buf_charcount(const struct buffer* b, char ch)
{
    /*return buf_charcount_sec(b, ch, (struct cursor){ }, b->size);*/
}


char buf_charat(const struct buffer* b, struct cursor cur)
{
    /*assert(pos < b->size && !INGAP(b, pos));*/
    /*return b->data[pos];*/
}


int buf_save_to_disk(const struct buffer* b, const char* path)
{
    return 0;
    /*FILE* f = fopen(path, "w");*/
    /*assert(f);*/
    /*unsigned int wbytes = 0;*/
    /*for(unsigned int i = 0; i < b->size; ++i) {*/
        /*if(! INGAP(b, i)) {*/
            /*fputc(b->data[i], f);*/
            /*wbytes++;*/
        /*}*/
    /*}*/
    /*log_l(TAG, "Buffer saved (%d bytes): %s", wbytes, path);*/
    /*fclose(f);*/
    /*return 0;*/
}

int buf_ingap(const struct buffer* b, unsigned int i)
{
    return INGAP(b, i);
}


void buf_pprint(const struct buffer* b)
{
    log_l(TAG, "Buffer{id=%d, gap.pos=%d, gap.size = %d"
            ", lncount=%d}",
            b->id, b->gap.col, b->gap.size, b->linecount);
}


void buf_pprint_lines(const struct buffer* buf)
{
    log_l(TAG, " -- line metadata --");
    for(unsigned int i = 0; i < buf->linecount; ++i) {
        const struct line* ln = buf_line(buf, i);
        log_l(TAG, "no: %d, len: %d", i, ln->len);
    }
    log_l(TAG, " --/ line metadata --");
}

/** buffer_internal.h **/

static unsigned int generate_id()
{
    static unsigned int ids = 0;
    return ids++;
}

static void gap_add(struct buffer* buf)
{
    struct line* ln = buf_line(buf, buf->gap.line);
    struct buffer_gap* gap = &buf->gap;

    unsigned int newlinelen = ln->len + BUFFER_GAPSIZE;
    unsigned int newgapsize = gap->size + BUFFER_GAPSIZE;

    char* newdata = calloc(newlinelen, sizeof(char));
    assert(newdata);

    /* Copy memory to new data */
    // from start to gap
    memcpy(newdata, ln->data, gap->col * sizeof(char));
    // from end of gap to end of buffer
    memcpy(&newdata[gap->col + newgapsize],
            &ln->data[gap->col + gap->size],
            ln->len - (gap->col + gap->size));
    // free old buffer
    free(ln->data);
    ln->data = newdata;
    ln->len = newlinelen;
    gap->size = newgapsize;
}

/* NOTE: toline must NOT be same as gap.line! */
static void gap_move_to_line(struct buffer* buf, unsigned int toline)
{
    assert(buf->gap.line != toline);
    // Delete current gap
    {
        struct line* ln = buf_line(buf, buf->gap.line);
        unsigned int linelength = ln->len - buf->gap.size;
        char* newdata = malloc(linelength * sizeof(char));
        assert(newdata);
        unsigned int count = 0;
        for(unsigned int i = 0; i < ln->len; ++i) {
            if(!INGAP(buf, i)) {
                newdata[count] = ln->data[i];
                count++;
            }
        }
        assert(count == linelength);
        free(ln->data);

        // Update line length and data
        ln->len = linelength;
        ln->data = newdata;
    }
    // Create gap on toline
    {
        struct line* ln = buf_line(buf, toline);
        unsigned int newlength = ln->len + buf->gap.size;
        char* newdata = malloc(newlength * sizeof(char));
        assert(newdata);
        memcpy(&newdata[buf->gap.size],
                ln->data,
                ln->len);
        free(ln->data);

        // Update line length and data
        ln->len = newlength;
        ln->data = newdata;

    }
    // Update gap location (NOT SIZE!!)
    buf->gap.line = toline;
    buf->gap.col = 0;
}

/** move gap to cursor */
static void gap_move(struct buffer* buf, struct cursor cur)
{
    assert(buf);
    assert(CURVALID(buf, cur));

    // Ensure that the gap is on the same line as that of the cursor
    if(buf->gap.line != cur.line) {
        gap_move_to_line(buf, cur.line);
    }
    // Now, the cursor and the gap are on the same line

    struct line* ln = buf_line(buf, cur.line);
    unsigned int offset = 0;
    int diff = cur.col - buf->gap.col;

    if(diff > 0) {
        // Cursor is ahead of the gap
        offset = diff;
        for(unsigned int i = 0; i < offset; ++i) {
            ln->data[buf->gap.col] =
                ln->data[buf->gap.col + buf->gap.size];
            buf->gap.col++;
        }
        log_l(TAG, "%s: diff=%d, offset=%d", __func__, diff, offset);
    }
    else if(diff < 0) {
        // Cursor is behind the gap
        offset = -1 * diff;
        for(unsigned int i = 0; i < offset; ++i) {
            ln->data[buf->gap.col + buf->gap.size - 1]
                = ln->data[buf->gap.col - 1];
            buf->gap.col--;
        }
        log_l(TAG, "%s: diff=%d offset=%d", __func__, diff, offset);
    }
    else {
        // gap is in sync do nothing.
    }
    return;

}


static int gap_resize_optional(struct buffer* buf)
{
    if(buf->gap.size < 1) {
        unsigned int oldgap = buf->gap.size;
        gap_add(buf);
        log_l(TAG, "Gap resized %d -> %d", oldgap, buf->gap.size);
        return 1;
    }
    return 0;
}


void buf_printline(const struct buffer* buf, unsigned int i)
{
    assert(i < buf->linecount);
    int hasgap = i == buf->gap.line ? 1 : 0;
    const struct line* ln = buf_line(buf, i);
    log_l("LINE", "no: %d, len: %d, has_gap: %d",
            i, ln->len, hasgap);
}
