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

    for(int i = 0; i < buf->linecount; ++i) {
        buf->lines[i].data = testlines[i];
        buf->lines[i].len = strlen(testlines[i]);
    }

    /*buf->lines[0].data = "    First line with gap(4)";*/
    /*buf->lines[0].len = strlen(buf->lines[0].data);*/

    /*buf->lines[1].data = "This is the second line.s sldf kasdfj";*/
    /*buf->lines[1].len = strlen(buf->lines[1].data);*/

    /*buf->lines[2].data = "the quick brown fox 1375550901, .__12380";*/
    /*buf->lines[2].len = strlen(buf->lines[2].data);*/

    /*buf->lines[3].data = "the quick brown fox 1375550901, .__12380";*/
    /*buf->lines[3].len = strlen(buf->lines[2].data);*/
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
    buf->linecount = 2;    // Empty buffer has at least one line
    buf->lines = calloc(buf->linecount, sizeof(struct line));
    assert(buf->lines);

    // Set gap on the first line
    struct line* fln = buf_line(buf, 0);
    fln->len = BUFFER_GAPSIZE;
    fln->data = calloc(fln->len, sizeof(char));
    buf->lines[1].data = "Yolo";
    buf->lines[1].len = 4;
    return buf;
}

#if 0
struct buffer* buf_create_file(enum buffer_type type,
        const char* filename)
{
    struct file_stats fstats = fu_stats(filename);
    /*log_l(TAG, "%s: loading file: %s", __func__, filename);*/
    if(!(fstats.exists == 1 && F_FILE == fstats.type)) {
        if(fstats.type == F_DIR) {
            log_fatal(TAG, "%s: cannot open:%s. (Its a directory)",
                    __func__, filename);
        } else {
            log_l(TAG, "%s: couldn't open:%s. Creating empty buffer",
                    __func__, filename);
            // Return a new empty buffer;
            struct buffer* buf = buf_create_empty(type);
            buf->name = filename;
            return buf;
        }
    }
    struct buffer* b = malloc(sizeof(struct buffer));
    assert(b);
    b->id = generate_id();
    b->type = type;
    b->name = filename;

    b->size = fstats.size + BUFFER_GAPSIZE;
    b->cur = 0;
    b->gappos = 0;
    b->gaplen = BUFFER_GAPSIZE;

    b->data = calloc(b->size, sizeof(char));
    assert(b->data);

    fu_read_to_buffer(fstats.path, &b->data[b->gappos + b->gaplen]);

    buf_update(b);
    return b;
}
#endif

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

static int gap_resize_if_required(struct buffer* buf)
{
    if(buf->gap.size < 1) {
        unsigned int oldgap = buf->gap.size;
        gap_add(buf);
        log_l(TAG, "Gap resized %d -> %d", oldgap, buf->gap.size);
        return 1;
    }
    return 0;
}

/** Adding a character to the buffer requires a gap synchronization.
 * It may also require a gap resize(increase). The order of the two
 * operations is not fixed right now but I don't think it really
 * makes a difference */
void buf_addch(struct buffer* buf, char ch, struct cursor cur)
{
    gap_move(buf, cur);
    gap_resize_if_required(buf);

    /*if(b->gap.size < 1) {*/
        /*gap_add(b);*/
    /*}*/

    /*b->data[b->gappos] = ch;*/
    /*b->gaplen--;*/
    /*b->gappos++;*/
    /*b->cur = b->gappos;*/

    /*if(ch == '\n') {*/
        /*buf_update(b);*/
    /*}*/
    /*return;*/
}

/** Deleting a character in the buffer has the same constraints as
 * the add operation - apart from the buffer resizing */
void buf_delch(struct buffer* buf, struct cursor cur)
{
    if(cur.col < 1) {
        return;
    }

    gap_move(buf, cur);
    gap_resize_if_required(buf);

    /*b->gap.col--;*/
    /*b->gap.size++;*/

    /*char delchar = b->data[b->gappos];*/

    /*if(delchar == '\n') {*/
        /*buf_update(b);*/
    /*}*/

    /*if(b->gappos + b->gaplen > b->size) {*/
        /*log_fatal(TAG, "%s:ERROR: FIX THIS!", __func__);*/
    /*}*/
}


struct line* buf_line(struct buffer* buf, unsigned int num)
{
    assert(num < buf->linecount);
    return &buf->lines[num];
}


unsigned int buf_line_count(const struct buffer* buf)
{
    return buf->linecount;
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
    struct buffer_gap gap = buf->gap;

    unsigned int newlinelen = ln->len + BUFFER_GAPSIZE;
    unsigned int newgapsize = gap.size + BUFFER_GAPSIZE;

    char* newdata = calloc(newlinelen, sizeof(char));
    assert(newdata);

    /* Copy memory to new data */
    // from start to gap
    memcpy(newdata, ln->data, gap.col * sizeof(char));
    // from end of gap to end of buffer
    memcpy(&newdata[gap.col + newgapsize],
            &ln->data[gap.col + gap.size],
            ln->len - (gap.col + gap.size));
    // free old buffer
    free(ln->data);
    ln->data = newdata;
    ln->len = newlinelen;
    gap.size = newgapsize;
}

// move gap to another line
static void gap_move_to_line(struct buffer* buf, unsigned int line)
{
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
    // Create gap on line
    {
        struct line* ln = buf_line(buf, line);
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
    buf->gap.line = line;
    buf->gap.col = 0;
}

static void gap_move(struct buffer* buf, struct cursor cur)
{
    assert(buf);
    assert(CURVALID(buf, cur));

    // Ensure gap is on the same line as that of the cursor
    if(buf->gap.line != cur.line) {
        gap_move_to_line(buf, cur.line);
    }

    // Cursor and gap lie in the same line
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
        // Cursor already in sync
    }
    return;

}

void buf_printline(const struct buffer* buf, unsigned int i)
{
    assert(i < buf->linecount);
    int hasgap = i == buf->gap.line ? 1 : 0;
    const struct line* ln = buf_line(buf, i);
    log_l("LINE", "no: %d, len: %d, has_gap: %d",
            i, ln->len, hasgap);
}
