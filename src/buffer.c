#include "buffer.h"
#include "buffer_internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "log.h"
#include "input_keys.h"

#define BUFFER_GAPSIZE 4

/** Evals to 1 if i is inside buffer gap */
#define INGAP(buffer, i)\
    ((i >= buffer->gappos && i < buffer->gappos + buffer->gaplen))

#define TAG "BUFFER"


struct buffer* buf_create(enum buffer_type type)
{
    struct buffer* b = malloc(sizeof(struct buffer));
    assert(b);

    b->id = generate_id();
    b->type = type; // TODO type specific setup
    b->name = "[NEWBUF]";

    b->size = BUFFER_GAPSIZE;
    b->cur = 0;
    b->gappos = 0;
    b->gaplen = BUFFER_GAPSIZE;

    b->data = calloc(b->size, sizeof(char));
    assert(b->data);

    update_cache(b);
    return b;
}


void buf_destroy(struct buffer* b)
{
    unsigned int id = b->id;
    free(b->data);
    free(b);
    log_l(TAG, "Buffer destroyed (id=%d)", id);
    b = NULL;
}


void buf_addch(struct buffer* b, char ch)
{
    gap_sync(b);
    if(b->gaplen < 1) {
        gap_add(b);
    }

    b->data[b->gappos] = ch;
    b->gaplen--;
    b->gappos++;
    b->cur = b->gappos;

    if(ch == '\n') {
        update_cache(b);
    }
    return;
}


void buf_delch(struct buffer* b)
{
    gap_sync(b);
    if(b->gappos < 1) {
        return;
    }

    b->gappos--;
    b->gaplen++;
    b->cur = b->gappos;

    char delchar = b->data[b->gappos];

    if(delchar == '\n') {
        update_cache(b);
    }

    if(b->gappos + b->gaplen > b->size) {
        log_fatal(TAG, "ERROR: FIX THIS!");
    }
}


void buf_cur_mvf(struct buffer* b, unsigned int n)
{
    unsigned int upperbound = b->size - b->gaplen;
    unsigned int newcur = b->cur + n;
    // min(newcur, upperbound)
    b->cur = newcur < upperbound ? newcur : upperbound;
}


void buf_cur_mvb(struct buffer* b, unsigned int n)
{
    // avoid unsigned int underflow
    if(n >= b->cur) {
        b->cur = 0;
    } else {
        b->cur -= n;
    }
}


unsigned int buf_cur_mveol(struct buffer* buf)
{
    for(unsigned int i = buf->cur; i < buf->size; ++i) {
        if(!INGAP(buf, i) && buf->data[i] == '\n') {
            unsigned int offset = i - buf->cur;
            buf->cur = i;
            return offset;
        }
    }
    // EOB
    unsigned int eob = buf->size - 1;
    unsigned int offset = eob - buf->cur;
    buf->cur = eob;
    return offset;
}


unsigned int buf_get_linecount(struct buffer* b)
{
    return b->cache.linecount;
}


char buf_get_char(struct buffer* b, unsigned int pos)
{
    assert(pos < b->size);
    return b->data[pos];
}


int buf_save_to_disk(struct buffer* b, const char* path)
{
    FILE* f = fopen(path, "w");
    assert(f);
    unsigned int wbytes = 0;
    for(unsigned int i = 0; i < b->size; ++i) {
        if(! INGAP(b, i)) {
            fputc(b->data[i], f);
            wbytes++;
        }
    }
    log_l(TAG, "Buffer saved (%d bytes): %s", wbytes, path);
    fclose(f);
    return 0;
}


unsigned int buf_charcount(struct buffer* b, char ch)
{
    unsigned int count = 0;
    for(unsigned int i = 0; i < b->size; ++i) {
        if((!INGAP(b, i)) && b->data[i] == ch)
            count++;
    }
    return count;
}


int buf_ingap(struct buffer* buf, unsigned int pos)
{
    return INGAP(buf, pos);
}


void buf_pprint(struct buffer* b)
{
    log_l(TAG, "Buffer{id=%d, cur=%d, gappos=%d, gaplen = %d, size=%d}",
            b->id, b->cur, b->gappos, b->gaplen, b->size);
}


void buf_printbuf(const struct buffer* b)
{
    for(unsigned int i = 0; i < b->size; ++i) {
        if(INGAP(b, i)) {
            log_lc("-");
            continue;
        }
        if(b->data[i] == 0) {
            log_lc("_");
        } else {
            log_lc("%c", b->data[i]);
        }
    }
    log_lc("\n");
}


/** buffer_internal.h **/

static
unsigned int generate_id()
{
    static unsigned int ids = 0;
    return ids++;
}


static
int update_cache(struct buffer* b)
{
    // Linecount
    unsigned int nlcount = buf_charcount(b, '\n');
    b->cache.linecount = nlcount + 1;
    return 0;
}


static
void gap_add(struct buffer* b)
{
    unsigned int newsize = b->size + BUFFER_GAPSIZE;
    unsigned int newgap = b->gaplen + BUFFER_GAPSIZE;

    char* newbuf = calloc(newsize, sizeof(b->data[0]));
    assert(newbuf);
    log_l(TAG, "Gap added: %d -> %d", b->size, newsize);

    /* Copy memory to new buffer */
    // from start to cursor
    memcpy(newbuf, b->data, b->gappos * sizeof(char));
    // from end of gap to end of buffer
    memcpy(&newbuf[b->gappos + newgap],
           &b->data[b->gappos + b->gaplen],
           b->size - (b->gappos + b->gaplen));
    // free old buffer
    free(b->data);
    b->data = newbuf;
    b->size = newsize;
    b->gaplen = newgap;
}


static
int gap_sync(struct buffer* b)
{
    int diff = b->cur - b->gappos;
    unsigned int offset = 0;

    if(diff > 0) {
        // Cursor is ahead of the gap
        offset = diff;
        for(unsigned int i = 0; i < offset; ++i) {
            b->data[b->gappos] = b->data[b->gappos + b->gaplen];
            b->gappos++;
        }
        log_l(TAG, "%s: diff=%d, offset=%d", __func__, diff, offset);
    }
    else if(diff < 0) {
        // Cursor is behind the gap
        offset = -1 * diff;
        for(unsigned int i = 0; i < offset; ++i) {
            b->data[b->gappos + b->gaplen - 1]
                = b->data[b->gappos - 1];
            b->gappos--;
        }
        log_l(TAG, "%s: diff=%d offset=%d", __func__, diff, offset);
    }
    else {
        // Cursor already in sync
    }

    return offset;
}
