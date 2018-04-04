#include "bufferview.h"
#include "fileutils.h"
#include "log.h"
#include <cassert>

#define TAG "BVIEW"

BufferView::BufferView()
{
    start  = 0;
    cur    = {0, 0};
    bounds = RECT(0, 0, 0, 0);
}

BufferView::BufferView(enum Buffer::Type  /*type*/, const char* filename)
    : BufferView()
{
    assert(filename);
    // TODO(realloc): error handling
    buffer = FileUtil::loadBuffer(filename);
}

void BufferView::update()
{
    // Keep cursor on the screen
    Rect bvarea      = getBounds();
    size_t firstline = getStart();

    if (cur.line < firstline) {
        Cursor newcur = Cursor();
        newcur.line   = firstline;
        setCursor(newcur);
    }

    if (cur.line > firstline + bvarea.height - 1) {
        Cursor newcur = Cursor();
        newcur.line   = firstline + bvarea.height - 1;
        setCursor(newcur);
    }
}


void BufferView::setBounds(Rect newbounds)
{
    // log_l(TAG, "bounds set: {%d, %d, %d, %d} -> {%d, %d, %d, %d}",
    // bounds.y, bounds.x, bounds.width, bounds.height,
    // newbounds.y, newbounds.x, newbounds.width, newbounds.height);
    bounds = newbounds;
}

/** Cursor **/

void BufferView::setCursor(Cursor newcur)
{
    log_l(TAG, "Setting cursor: line: %d -> %d, col: %d -> %d",
          cur.line, newcur.line, cur.col, newcur.col);
    cur = newcur;
    cmovInline();
}


void BufferView::cmovFwd(size_t n)
{
    const Line& ln = buffer.getLine(cur.line);

    size_t newcol = cur.col + n;
    size_t maxcol = ln.trueLength();

    if (newcol > maxcol) {
        cmovLend();
    } else {
        cur.col = newcol;
    }
}


void BufferView::cmovBack(size_t n)
{
    if (n > cur.col) {
        cur.col = 0;
    } else {
        cur.col -= n;
    }
}


void BufferView::cmovLnext(size_t n)
{
    // Move to next line
    if ((cur.line + n) < buffer.lineCount()) {
        cur.line += n;
    } else {
        cur.line = buffer.lineCount() - 1;
    }
    cmovInline();
}


void BufferView::cmovLprev(size_t n)
{
    // Move to prev line
    if (n > cur.line) {
        cur.line = 0;
    } else if (cur.line - n < getStart()) {
        // TODO(realloc): remove this condition when scroll on curmove's added
        cur.line = getStart();
    } else {
        cur.line -= n;
    }
    cmovInline();
}


void BufferView::cmovLstart() { cur.col = 0; }


void BufferView::cmovLend()
{
    const Line& ln = buffer.getLine(cur.line);
    cur.col        = ln.trueLength();
}


void BufferView::cmovInline()
{
    const Line& ln = buffer.getLine(cur.line);
    if (cur.col >= ln.trueLength()) {
        cmovLend();
    }
}


void BufferView::scrollUp(size_t n)
{
    log_l(TAG, "SCROLLING UP!");
    if (n > getStart()) {
        start = 0;
    } else {
        start -= n;
    }
}


void BufferView::scrollDown(size_t n)
{
    log_l(TAG, "SCROLLING DOWN!");
    size_t newstart = getStart() + n;
    if (!(newstart < buffer.lineCount())) {
        // last line
        newstart = buffer.lineCount() - 1;
    }
    start = newstart;
}


Cursor BufferView::relcur() const
{
    Cursor c = {.line = cur.line - start, .col = cur.col};
    return c;
}
