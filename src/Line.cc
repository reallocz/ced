#include "Line.h"
#include <cassert>

#define GAPSIZE 4
#define TAG "LINE"

static unsigned int uid = 0;

Line::Line()
{
    id     = uid++;
    gapcol = 0;
    gaplen = 0;
    len    = 0;
    data   = nullptr;
}


Line::Line(unsigned int len, char* data)
    : Line()
{
    this->len  = len;
    this->data = data;
}


const char Line::operator[](std::size_t index) const
{
    assert(index < len);
    return data[index];
}

char& Line::operator[](std::size_t index)
{
    assert(index < len);
    return data[index];
}


bool Line::inGap(unsigned int i) const
{
    return (i >= gapcol) && (i < gapcol + gaplen);
}


void Line::addCh(char ch, const struct cursor& cur)
{
    moveGap(cur);
    addGapOptional();
    data[gapcol] = ch;
    gapcol++;
    gaplen--;
}


void Line::delCh(const struct cursor& cur)
{
    if (cur.col < 1) {
        return;
    }
    moveGap(cur);
    gapcol--;
    gaplen++;
}


bool Line::addGap()
{
    unsigned int newlinelen = Len() + GAPSIZE;
    unsigned int newgaplen  = gaplen + GAPSIZE;
    char* newdata           = new char[newlinelen];
    assert(newdata);

    /* Copy memory to new data */
    // from start to gap
    //memcpy(newdata, data, gapcol * sizeof(char));
    for (unsigned int i = 0; i < gapcol; ++i) {
        newdata[i] = data[i];
    }

    // from end of gap to end of buffer
    unsigned int aftergap    = gapcol + gaplen;
    unsigned int aftergaplen = Len() - (gapcol + gaplen);
    for (unsigned int i = 0; i < aftergaplen; ++i) {
        newdata[gapcol + newgaplen + i] = data[aftergap + i];
    }
    //memcpy(&newdata[gapcol + newgaplen],
    //&data[gapcol + gaplen],
    //Len() - (gapcol + gaplen));

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
        unsigned int oldgap = gaplen;
        addGap();
        log_l(TAG, "Gap resized %d -> %d", oldgap, gaplen);
        return true;
    }
    return false;
}


bool Line::moveGap(const struct cursor& cur)
{
    if (cur.col == gapcol) {
        return false;
    }

    unsigned int offset = 0;
    int diff            = cur.col - gapcol;

    if (diff > 0) {
        // Cursor is ahead of the gap
        offset = diff;
        for (unsigned int i = 0; i < offset; ++i) {
            data[gapcol] = data[gapcol + gaplen];
            gapcol++;
        }
        log_l(TAG, "%s: diff=%d, offset=%d", __func__, diff, offset);
    } else if (diff < 0) {
        // Cursor is behind the gap
        offset = -1 * diff;
        for (unsigned int i = 0; i < offset; ++i) {
            data[gapcol + gaplen - 1] = data[gapcol - 1];
            gapcol--;
        }
        log_l(TAG, "%s: diff=%d offset=%d", __func__, diff, offset);
    } else {
        // gap is in sync do nothing.
    }
    return true;
}
