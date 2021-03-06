#include "buffer_view.h"
#include "log.h"
#include <assert.h>

#define TAG "BVIEW"

struct buffer_view bv_create(enum buffer_type type,
                             const char* filename)
{
    assert(filename);
    struct buffer_view bv;
    bv.cur.line = 0;
    bv.cur.col  = 0;

    bv.buffer = buf_create_file(type, filename);

    bv.start  = 0;
    bv.bounds = (struct rect) RECT(0, 0, 0, 0);

    return bv;
}


void bv_update(struct buffer_view* bv)
{
    // Keep cursor on the screen
    struct rect bvarea     = bv_bounds(bv);
    size_t firstline = bv_start(bv);

    if (bv->cur.line < firstline) {
        struct cursor cur = bv->cur;
        cur.line          = firstline;
        bv_cset(bv, cur);
    }

    if (bv->cur.line > firstline + bvarea.height - 1) {
        struct cursor cur = bv->cur;
        cur.line          = firstline + bvarea.height - 1;
        bv_cset(bv, cur);
    }
}


Line* bv_curline(struct buffer_view* bv)
{
    return &(bv->buffer.lines[bv->cur.line]);
}


void bv_bounds_set(struct buffer_view* bv, struct rect bounds)
{
    assert(bv);
    /*log_l(TAG, "bounds set: {%d, %d, %d, %d} -> {%d, %d, %d, %d}",*/
          /*bv->bounds.y, bv->bounds.x, bv->bounds.width,*/
          /*bv->bounds.height, bounds.y, bounds.x, bounds.width,*/
          /*bounds.height);*/
    bv->bounds = bounds;
}


struct rect bv_bounds(const struct buffer_view* bv)
{
    assert(bv);
    return bv->bounds;
}


/** Cursor **/

void bv_cset(struct buffer_view* bv, struct cursor cur)
{
    assert(bv);
    log_l(TAG, "Setting cursor: line: %d -> %d, col: %d -> %d",
          bv->cur.line, cur.line, bv->cur.col, cur.col);
    bv->cur = cur;
    bv_cmov_inline(bv);
}


void bv_cmov_fwd(struct buffer_view* bv, size_t n)
{
    assert(bv);
    const struct buffer* buf = &bv->buffer;
    const struct line* ln    = buf_getline(buf, bv->cur.line);

    size_t newcol = bv->cur.col + n;
    size_t maxcol = ln_truelen(ln);

    if (newcol > maxcol) {
        bv_cmov_lend(bv);
    } else {
        bv->cur.col = newcol;
    }
}


void bv_cmov_back(struct buffer_view* bv, size_t n)
{
    assert(bv);
    if (n > bv->cur.col) {
        bv->cur.col = 0;
    } else {
        bv->cur.col -= n;
    }
}


void bv_cmov_lnext(struct buffer_view* bv, size_t n)
{
    assert(bv);
    // Move to next line
    if ((bv->cur.line + n) < bv->buffer.linecount) {
        bv->cur.line += n;
    } else {
        bv->cur.line = bv->buffer.linecount - 1;
    }
    bv_cmov_inline(bv);
}


void bv_cmov_lprev(struct buffer_view* bv, size_t n)
{
    assert(bv);
    // Move to prev line
    if (n > bv->cur.line) {
        bv->cur.line = 0;
    } else {
        bv->cur.line -= n;
    }
    // TODO Scroll buffer if out of bounds
    bv_cmov_inline(bv);
}


void bv_cmov_lstart(struct buffer_view* bv)
{
    assert(bv);
    bv->cur.col = 0;
}


void bv_cmov_lend(struct buffer_view* bv)
{
    assert(bv);
    struct line* ln = buf_getline(&bv->buffer, bv->cur.line);
    bv->cur.col = ln_truelen(ln);
}


void bv_cmov_inline(struct buffer_view* bv)
{
    assert(bv);
    struct line* ln = buf_getline(&bv->buffer, bv->cur.line);
    if (bv->cur.col >= ln->len) {
        bv_cmov_lend(bv);
    }
}


void bv_scrollup(struct buffer_view* bv, size_t n)
{
    log_l(TAG, "SCROLLING UP!");
    if (n > bv->start) {
        bv->start = 0;
    } else {
        bv->start -= n;
    }
}


void bv_scrolldown(struct buffer_view* bv, size_t n)
{
    log_l(TAG, "SCROLLING DOWN!");
    size_t newstart = bv->start + n;
    if (!(newstart < bv->buffer.linecount)) {
        // last line
        newstart = bv->buffer.linecount - 1;
    }
    bv->start = newstart;
}


size_t bv_start(const struct buffer_view* bv)
{
    return bv->start;
}


struct cursor bv_relcur(const struct buffer_view* bv)
{
    struct cursor c = {.line = bv->cur.line - bv->start,
                       .col  = bv->cur.col};
    return c;
}
