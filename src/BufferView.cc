#include "BufferView.h"
#include "log.h"
#include <cassert>

#define TAG "BVIEW"

BufferView::BufferView() {}

BufferView::BufferView(enum buffer_type type, const char* filename)
{
    assert(filename);
    cur.line = 0;
    cur.col  = 0;

    buffer = buf_create_file(type, filename);
    start  = 0;
    bounds = (struct rect) RECT(0, 0, 0, 0);
}

void BufferView::update()
{
    // Keep cursor on the screen
    struct rect bvarea     = Bounds();
    unsigned int firstline = Start();

    if (cur.line < firstline) {
        struct cursor newcur = Cursor();
        newcur.line          = firstline;
        setCursor(newcur);
    }

    if (cur.line > firstline + bvarea.height - 1) {
        struct cursor newcur = Cursor();
        newcur.line          = firstline + bvarea.height - 1;
        setCursor(newcur);
    }
}


void BufferView::setBounds(struct rect newbounds)
{
    log_l(TAG, "bounds set: {%d, %d, %d, %d} -> {%d, %d, %d, %d}",
          bounds.y, bounds.x, bounds.width, bounds.height,
          newbounds.y, newbounds.x, newbounds.width, newbounds.height);
    bounds = newbounds;
}

/** Cursor **/

void BufferView::setCursor(struct cursor newcur)
{
    log_l(TAG, "Setting cursor: line: %d -> %d, col: %d -> %d",
          cur.line, newcur.line, cur.col, newcur.col);
    cur = newcur;
    cmovInline();
}


void BufferView::cmovFwd(unsigned int n)
{
    const struct line* ln    = buf_line(&buffer, cur.line);

    unsigned int newcol = cur.col + n;
    unsigned int maxcol = buf_line_hasgap(&buffer, cur.line)
                              ? (ln->len - buffer.gap.size)
                              : ln->len;

    if (newcol > maxcol) {
        cmovLend();
    } else {
        cur.col = newcol;
    }
}


void BufferView::cmovBack(unsigned int n)
{
    if (n > cur.col) {
        cur.col = 0;
    } else {
        cur.col -= n;
    }
}


void BufferView::cmovLnext(unsigned int n)
{
    // Move to next line
    if ((cur.line + n) < buffer.linecount) {
        cur.line += n;
    } else {
        cur.line = buffer.linecount - 1;
    }
    cmovInline();
}


void BufferView::cmovLprev(unsigned int n)
{
    // Move to prev line
    if (n > cur.line) {
        cmovLstart();
    } else {
        cur.line -= n;
    }
    cmovInline();
}


void BufferView::cmovLstart()
{
    cur.col = 0;
}


void BufferView::cmovLend()
{
    struct line* ln = buf_line(&buffer, cur.line);
    if (buf_line_hasgap(&buffer, cur.line)) {
        cur.col = ln->len - buffer.gap.size;
    } else {
        cur.col = ln->len;
    }
}


void BufferView::cmovInline()
{
    struct line* ln = buf_line(&buffer, cur.line);
    if (cur.col >= ln->len) {
        cmovLend();
    }
}


void BufferView::scrollUp(unsigned int n)
{
    log_l(TAG, "SCROLLING UP!");
    if (n > Start()) {
        start = 0;
    } else {
        start -= n;
    }
}


void BufferView::scrollDown(unsigned int n)
{
    log_l(TAG, "SCROLLING DOWN!");
    unsigned int newstart = Start() + n;
    if (!(newstart < buffer.linecount)) {
        // last line
        newstart = buffer.linecount - 1;
    }
    start = newstart;
}


struct cursor BufferView::relcur() const
{
    struct cursor c = {
        .line = cur.line - start,
        .col  = cur.col};
    return c;
}
