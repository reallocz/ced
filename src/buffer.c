#include "buffer.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "log.h"

// A maximum of 4 buffers allowed
#define BUFFER_DEFCAP 4
#define BUFFER_DEFSIZE 8
#define BUFFER_DEFGAP 4


/** Bring struct buffer* alias of id x into scope */
#define SB(alias, hBuffer)\
    struct buffer* alias = _get_buffer(hBuffer)

/** Evals to 1 if pos is inside buffer gap */
#define INGAP(buffer, pos)\
    (pos >= buffer->cur) && (pos < buffer->cur + buffer->gap)

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
    unsigned int gap;		/* Size of the gap */
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


/** return a new id for a buffer */
    static
unsigned int _generate_id()
{
    static unsigned int ids = 0;
    return ids++;
}

/** return a pointer to the buffer in use of the handle */
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


/** return a handle to a free buffer (in_use == 0) */
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


void buf_destroy(hBuffer buf)
{
    SB(b, buf);
    unsigned int id = b->id;
    b->in_use = 0;
    free(b->data);
}


hBuffer buf_create(enum buffer_type type)
{
    hBuffer buf = _get_free_handle();
    struct buffer* b = &G.buffers[buf];

    b->id = _generate_id();
    b->in_use = 1;
    b->type = type; // TODO type specific setup
    b->name = "[NEWBUF]";
    b->size = BUFFER_DEFSIZE;
    b->data = calloc(b->size, sizeof(char));
    b->gap = BUFFER_DEFGAP;
    b->cur = 0;
    return buf;
}


void buf_add_gap(hBuffer buf)
{
    SB(b, buf);
    unsigned int newsize = b->size + BUFFER_DEFGAP;
    unsigned int newgap = b->gap + BUFFER_DEFGAP;
    char* newbuf = calloc(newsize, sizeof(b->data[0]));
    assert(newbuf);
    /*log_l(G.tag, "Gap added: %d -> %d", b->size, newsize);*/

    /* Copy memory to new buffer */
    // from start to cursor
    memcpy(newbuf, b->data, b->cur * sizeof(char));
    // from end of gap to end of buffer
    memcpy(&newbuf[b->cur + newgap],
           &b->data[b->cur + b->gap],
           b->size - (b->cur + b->gap));
    // free old buffer
    free(b->data);
    b->data = newbuf;
    b->size = newsize;
    b->gap = newgap;
}


void buf_addch(hBuffer buf, char ch)
{
    SB(b, buf);
    assert(b->data);
    if(b->gap < 1) {
        buf_add_gap(buf);
    }
    b->data[b->cur] = ch;
    b->cur++;
    b->gap--;
    return;
}


void buf_delch(hBuffer buf)
{
    SB(b, buf);
    assert(b && b->data);
    if(b->cur < 1) {
        return;
    }
    b->cur--;
    b->gap++;
    if(b->cur + b->gap > b->size) {
        log_fatal(G.tag, "ERROR: FIX THIS!");
    }
}


void buf_move_cur_forward(hBuffer buf, unsigned int n)
{
    SB(b, buf);
    assert(b);
    if(b->cur + b->gap == b->size) {
        log_l(G.tag, "EOB!");
        return;
    }
    b->data[b->cur] = b->data[b->cur + b->gap];
    b->cur++;
}

void buf_move_cur_back(hBuffer buf, unsigned int n)
{
    SB(b, buf);
    assert(b);
    if(b->cur == 0) {
        return;
    }
    b->data[(b->cur + b->gap) - 1] = b->data[b->cur - 1];
    b->cur--;
}


unsigned int buf_get_cursor(hBuffer buf)
{
    SB(b, buf);
    assert(b);
    return b->cur;
}


unsigned int buf_get_linecount(hBuffer buf)
{
    SB(b, buf);
    assert(b);
    unsigned int count = 0;
    for(unsigned int i = 0; i < b->size; ++i) {
        if(i >= b->cur && i < b->gap) {
            continue;
        }
        if(b->data[i] == '\n')
            count++;
    }
    return count;
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
        .gap = b->gap,
        .cur = b->cur,
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
    log_l(G.tag, "Buffer{id=%d, cur=%d, gap=%d, size=%d}",
            b->id, b->cur, b->gap, b->size);
}


void buf_printbuf(const hBuffer buf)
{
    SB(b, buf);
    for(unsigned int i = 0; i < b->size; ++i) {
        if(i >= b->cur && i < b->cur + b->gap) {
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
