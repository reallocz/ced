#include "buffer_view.h"
#include "log.h"
#include <assert.h>

#define TAG "BVIEW"

struct buffer_view bv_create(struct buffer* buf)
{
    assert(buf);
    struct buffer_view bv;
    bv.cur.line = 0;
    bv.cur.col = 0;

    bv.buffer = buf;
    bv.start = 0;

    return bv;
}


void bv_cmov_fwd(struct buffer_view* bv, unsigned int n)
{
    assert(bv);
    const struct buffer* buf = bv->buffer;
    const struct line* ln = buf_line(buf, bv->cur.line);

    unsigned int newcol = bv->cur.col + n;
    unsigned int maxcol = buf_line_hasgap(buf, bv->cur.line)
        ? (ln->len - buf->gap.size) : ln->len;

    if(newcol > maxcol) {
        bv_cmov_lend(bv);
    } else {
        bv->cur.col = newcol;
    }
}


void bv_cmov_back(struct buffer_view* bv, unsigned int n)
{
    assert(bv);
    if(n > bv->cur.col) {
        bv->cur.col = 0;
    } else {
        bv->cur.col -= n;
    }
}


void bv_cmov_lnext(struct buffer_view* bv, unsigned int n)
{
    assert(bv);
    // Move to next line
    if((bv->cur.line + n) < bv->buffer->linecount) {
        bv->cur.line += n;
    } else {
        bv->cur.line = bv->buffer->linecount - 1;
    }

    // Check next line's bounds and move accordingly
    struct line* ln = buf_line(bv->buffer, bv->cur.line);
    if(bv->cur.col >= ln->len) {
        bv_cmov_lend(bv);
    }
}


void bv_cmov_lprev(struct buffer_view* bv, unsigned int n)
{
    assert(bv);
    // Move to prev line
    if(n > bv->cur.line) {
        bv_cmov_lstart(bv);
    } else {
        bv->cur.line -= n;
    }

    // Check prev line's bounds and move accordingly
    struct line* ln = buf_line(bv->buffer, bv->cur.line);
    if(bv->cur.col >= ln->len) {
        bv_cmov_lend(bv);
    }
}


void bv_cmov_lstart(struct buffer_view* bv)
{
    assert(bv);
    bv->cur.col = 0;
}


void bv_cmov_lend(struct buffer_view* bv)
{
    assert(bv);
    struct line* ln = buf_line(bv->buffer, bv->cur.line);
    if(buf_line_hasgap(bv->buffer, bv->cur.line)) {
        bv->cur.col = ln->len - bv->buffer->gap.size;
    } else {
        bv->cur.col = ln->len;
    }
}


void bv_scrollup(struct buffer_view* bv, unsigned int n)
{
    log_l(TAG, "SCROLLING UP!");
    if(n > bv->start) {
        bv->start = 0;
    } else {
        bv->start -= n;
    }
}


void bv_scrolldown(struct buffer_view* bv, unsigned int n)
{
    log_l(TAG, "SCROLLING DOWN!");
    unsigned int newstart = bv->start + n;
    if(!(newstart < bv->buffer->linecount)) {
        // last line
        newstart = bv->buffer->linecount - 1;
    }
    bv->start = newstart;
}


unsigned int bv_start(struct buffer_view* bv)
{
    return bv->start;
}

