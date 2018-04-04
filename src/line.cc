#include "line.h"
#include <cassert>

#define GAPSIZE 4
#define TAG "LINE"

static size_t uid = 0;

Line::Line()
{
    id     = uid++;
    gapcol = 0;
    gaplen = 0;
    len    = 0;
    data   = nullptr;
}


Line::Line(size_t len, char* data)
    : Line()
{
    this->len  = len;
    this->data = data;
}


bool Line::inGap(size_t i) const
{
    return (i >= gapcol) && (i < gapcol + gaplen);
}


const char Line::operator[](std::size_t index) const
{
    assert(index < len);
    return data[index];
}


Line Line::split(size_t from)
{
    moveGap(from);

    size_t newlen = trueLength() - from;
    auto* newdata = new char[newlen];
    assert(newdata);
    log_l(TAG, "NEWLEN: %d", newlen);

    unsigned int count = 0;
    for(size_t i = from; i < length(); ++i) {
        if(!inGap(i)) {
            newdata[count++] = data[i];
        }
    }
    assert(count == newlen); // count increments after write

    // Create new line
    Line newln = Line(newlen, newdata);

    // Expand current gap
    gaplen = length() - from;

    return newln;
}


void Line::addCh(char ch, const Cursor& cur)
{
    moveGap(cur.col);
    addGapOptional();
    data[gapcol] = ch;
    gapcol++;
    gaplen--;
}


void Line::delCh(const Cursor& cur)
{
    if (cur.col < 1) {
        return;
    }
    moveGap(cur.col);
    gapcol--;
    gaplen++;
}


void Line::clear()
{
    gapcol = 0;
    gaplen = len - 1;    // Last char is a '\n'
}


bool Line::addGap()
{
    size_t newlinelen = length() + GAPSIZE;
    size_t newgaplen  = gaplen + GAPSIZE;
    auto* newdata     = new char[newlinelen];
    assert(newdata);

    for (size_t i = 0; i < gapcol; ++i) {
        newdata[i] = data[i];
    }

    // from end of gap to end of buffer
    size_t aftergap    = gapcol + gaplen;
    size_t aftergaplen = length() - (gapcol + gaplen);
    for (size_t i = 0; i < aftergaplen; ++i) {
        newdata[gapcol + newgaplen + i] = data[aftergap + i];
    }

    // free old buffer
    delete[] data;

    data   = newdata;
    len    = newlinelen;
    gaplen = newgaplen;

    return true;
}


bool Line::addGapOptional()
{
    if (gaplen < 1) {
        size_t oldgap = gaplen;
        addGap();
        log_l(TAG, "Gap resized %d -> %d", oldgap, gaplen);
        return true;
    }
    return false;
}


bool Line::moveGap(const size_t col)
{
    if (col == gapcol) {
        return false;
    }

    size_t offset = 0;
    int diff      = col - gapcol;

    if (diff > 0) {
        // Cursor is ahead of the gap
        offset = diff;
        for (size_t i = 0; i < offset; ++i) {
            data[gapcol] = data[gapcol + gaplen];
            gapcol++;
        }
        log_l(TAG, "%s: diff=%d, offset=%d", __func__, diff, offset);
    } else if (diff < 0) {
        // Cursor is behind the gap
        offset = -1 * diff;
        for (size_t i = 0; i < offset; ++i) {
            data[gapcol + gaplen - 1] = data[gapcol - 1];
            gapcol--;
        }
        log_l(TAG, "%s: diff=%d offset=%d", __func__, diff, offset);
    } else {
        // gap is in sync do nothing.
    }
    return true;
}

void Line::pprint() const
{
    log_l(TAG, "Line{len: %d, gapcol: %d, gaplen: %d, data: ", len,
          gapcol, gaplen);
    for (size_t i = 0; i < len; ++i) {
        if(!inGap(i)) {
            log_lc("%c", data[i]);
        }
    }
    log_lc("}\n");
}
