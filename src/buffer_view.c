#include "buffer_view.h"
#include "log.h"
#include <assert.h>

#define TAG "BVIEW"

struct buffer_view bview_create(struct buffer* buf)
{
    assert(buf);
    struct buffer_view bv;
    bv.cur.line = 0;
    bv.cur.col = 0;

    bv.buffer = buf;
    bv.start = 0;
    bv.len = buf->linecount;

    return bv;
}

void bview_curmove_f(struct buffer_view* bv, unsigned int n)
{
    const struct buffer* buf = bv->buffer;
    const struct line* ln = buf_line(buf, bv->cur.line);

    if(!buf_line_hasgap(buf, bv->cur.line)) {
        // no gap
        unsigned int newcol = bv->cur.col + n;
        if(newcol > ln->len)
            newcol = ln->len;
        bv->cur.col = newcol;
    } else {
        // gap
        unsigned int newcol = bv->cur.col + n;
        if(newcol > ln->len - buf->gap.size) {
            bv->cur.col = ln->len - buf->gap.size;
        } else {
            bv->cur.col = newcol;
        }
    }

}

void bview_curmove_b(struct buffer_view* bv, unsigned int n)
{
    if(n > bv->cur.col) {
        bv->cur.col = 0;
    } else {
        bv->cur.col -= n;
    }
}

void bview_curmove_nextline(struct buffer_view* bv, unsigned int n)
{
    unsigned int newline = bv->cur.line + n;
    if(!(newline < bv->buffer->linecount)) {
        newline = bv->buffer->linecount - 1;
    }
    bv->cur.line = newline;
}


void bview_curmove_prevline(struct buffer_view* bv, unsigned int n)
{
    if(n > bv->cur.line) {
        bv->cur.line = 0;
    } else {
        bv->cur.line -= n;
    }
}
