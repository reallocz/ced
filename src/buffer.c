#include "buffer.h"
#include "buffer_internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "log.h"
#include "input_keys.h"

// A maximum of 4 buffers allowed
#define BUFFER_DEFCAP 4
// Gap size
#define BUFFER_GAPSIZE 4


/** Bring struct buffer* alias of id x into scope */
#define SB(alias, hBuffer)\
    struct buffer* alias = _get_buffer(hBuffer)

/** Evals to 1 if i is inside buffer gap */
#define INGAP(buffer, i)\
    ((i >= buffer->gap.pos && i < buffer->gap.pos + buffer->gap.len))


struct buffer {
    /*unique identifier of each buffer for debugging*/
    unsigned int id;
    /* 1 if buffer is in use (not destroyed)*/
    int in_use;
    enum buffer_type type;
    const char* name;		/* Name of buffer(file) */
    char* data;			/* File in memory */
    unsigned int size;		/* Size of buffer */
    unsigned int cur;		/* Cursor position */

    struct {
        unsigned int pos;       /* Beginning of gap */
        unsigned int len;       /* Length of gap */
    } gap;

    struct {
        unsigned int linecount;
    } cache;
};


static struct {
    const char* tag;
    unsigned int bufcap; // Total buffer capacity
    struct buffer buffers[BUFFER_DEFCAP];
} G;


int buf_init()
{
    G.tag = "BUFFER";
    G.bufcap = BUFFER_DEFCAP;

    for(unsigned int i = 0; i < G.bufcap; ++i) {
        G.buffers[i].in_use = 0;
    }
    log_l(G.tag, "Init success");
    return 0;
}


int buf_exit()
{
    // TODO free memory
    return 1;
}

void buf_destroy(hBuffer buf)
{
    SB(b, buf);
    unsigned int id = b->id;
    b->in_use = 0;
    free(b->data);
    log_l(G.tag, "Buffer destroyed (id=%d)", id);
}


hBuffer buf_create(enum buffer_type type)
{
    hBuffer buf = _get_free_handle();
    struct buffer* b = &G.buffers[buf];

    b->id = _generate_id();
    b->in_use = 1;
    b->type = type; // TODO type specific setup
    b->name = "[NEWBUF]";

    b->size = BUFFER_GAPSIZE;
    b->cur = 0;
    b->gap.pos = 0;
    b->gap.len = BUFFER_GAPSIZE;

    b->data = calloc(b->size, sizeof(char));
    assert(b->data);

    b->cache.linecount = 0;
    return buf;
}


void buf_addch(hBuffer buf, char ch)
{
    SB(b, buf);
    assert(b->data);
    _sync_gap(b);
    if(b->gap.len < 1) {
        _gap_add(b);
    }

    b->data[b->gap.pos] = ch;
    b->gap.len--;
    b->gap.pos++;
    b->cur = b->gap.pos;

    if(ch == '\n') {
        _update_cache(b);
    }
    return;
}


void buf_delch(hBuffer buf)
{
    SB(b, buf);
    assert(b->data);
    _sync_gap(b);
    if(b->gap.pos < 1) {
        return;
    }

    b->gap.pos--;
    b->gap.len++;
    b->cur = b->gap.pos;

    char delchar = b->data[b->gap.pos];

    if(delchar == '\n') {
        _update_cache(b);
    }

    if(b->gap.pos + b->gap.len > b->size) {
        log_fatal(G.tag, "ERROR: FIX THIS!");
    }
}


void buf_cur_mvf(hBuffer buf, unsigned int n)
{
    SB(b, buf);
    b->cur++;
    if(b->cur >= b->size) {
        b->cur = b->size - 1;
    }
    return;
}


void buf_cur_mvb(hBuffer buf, unsigned int n)
{
    SB(b, buf);
    b->cur--;
    if(b->cur <= 0) {
        b->cur = 0;
    }
    return;
}


unsigned int buf_cur_get(hBuffer buf)
{
    SB(b, buf);
    return b->cur;
}


unsigned int buf_get_linecount(hBuffer buf)
{
    SB(b, buf);
    return b->cache.linecount;
}


unsigned int buf_get_size(hBuffer buf)
{
    SB(b, buf);
    return b->size;
}


char buf_get_char(hBuffer buf, unsigned int pos)
{
    SB(b, buf);
    assert(pos < b->size);
    return b->data[pos];
}


struct buf_props buf_get_props(hBuffer buf)
{
    SB(b, buf);
    struct buf_props props = {
        .size = b->size,
        .cur = b->cur,
        .linecount = b->cache.linecount,
        .gap.pos = b->gap.pos,
        .gap.len = b->gap.len
    };
    return props;
}


int buf_save_to_disk(hBuffer buf, const char* path)
{
    SB(b, buf);
    FILE* f = fopen(path, "w");
    assert(f);
    unsigned int wbytes = 0;
    for(unsigned int i = 0; i < b->size; ++i) {
        if(! INGAP(b, i)) {
            fputc(b->data[i], f);
            wbytes++;
        }
    }
    log_l(G.tag, "Buffer saved (%d bytes): %s", wbytes, path);
    fclose(f);
    return 0;
}


void buf_pprint(hBuffer buf)
{
    SB(b, buf);
    log_l(G.tag, "Buffer{id=%d, cur=%d, gap.pos=%d, gap.len = %d, size=%d}",
            b->id, b->cur, b->gap.pos, b->gap.len, b->size);
}


void buf_printbuf(const hBuffer buf)
{
    SB(b, buf);
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
unsigned int _generate_id()
{
    static unsigned int ids = 0;
    return ids++;
}

static
struct buffer* _get_buffer(hBuffer buf)
{
    if(buf < 0 || buf >= G.bufcap) {
        log_fatal(G.tag, "Invalid handle: %d", buf);
    }
    struct buffer* b = &G.buffers[buf];
    if(! b->in_use) {
        log_fatal(G.tag, "Bad handle: %d", buf);
    }
    return b;
}


static
hBuffer _get_free_handle()
{
    for(unsigned int i = 0; i < G.bufcap; ++i) {
        if(! G.buffers[i].in_use) {
            return i;
        }
    }
    log_fatal(G.tag, "%s: OUT OF BUFFERS!", __func__);
}


static
unsigned int _calc_charcount(struct buffer* b, char ch)
{
    unsigned int count = 0;
    for(unsigned int i = 0; i < b->size; ++i) {
        if((!INGAP(b, i)) && b->data[i] == ch)
            count++;
    }
    return count;
}


static
int _update_cache(struct buffer* b)
{
    b->cache.linecount = _calc_charcount(b, '\n') + 1;
    return 0;
}


static
void _gap_add(struct buffer* b)
{
    unsigned int newsize = b->size + BUFFER_GAPSIZE;
    unsigned int newgap = b->gap.len + BUFFER_GAPSIZE;

    char* newbuf = calloc(newsize, sizeof(b->data[0]));
    assert(newbuf);
    log_l(G.tag, "Gap added: %d -> %d", b->size, newsize);

    /* Copy memory to new buffer */
    // from start to cursor
    memcpy(newbuf, b->data, b->gap.pos * sizeof(char));
    // from end of gap to end of buffer
    memcpy(&newbuf[b->gap.pos + newgap],
           &b->data[b->gap.pos + b->gap.len],
           b->size - (b->gap.pos + b->gap.len));
    // free old buffer
    free(b->data);
    b->data = newbuf;
    b->size = newsize;
    b->gap.len = newgap;
}

static
int _sync_gap(struct buffer* b)
{
    int diff = b->cur - b->gap.pos;
    unsigned int offset = 0;

    // Cursor is ahead of the gap
    if(diff > 0) {
        offset = diff;
        for(unsigned int i = 0; i < offset; ++i) {
            b->data[b->gap.pos] = b->data[b->gap.pos + b->gap.len];
            b->gap.pos++;
        }
        log_l(G.tag, "%s: diff=%d, offset=%d", __func__, diff, offset);
    }

    // Cursor is behind the gap
    else if(diff < 0) {
        offset = -1 * diff;
        for(unsigned int i = 0; i < offset; ++i) {
            b->data[b->gap.pos + b->gap.len - 1]
                = b->data[b->gap.pos - 1];
            b->gap.pos--;
        }
        log_l(G.tag, "%s: diff=%d offset=%d", __func__, diff, offset);
    }
    else {
        // Cursor already in sync
    }
    return offset;
}
