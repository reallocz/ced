#include "buffer.h"
#include "fileutils.h"
#include "input_keys.h"
#include "log.h"
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#define CURVALID(buf, cur) \
    ((cur).line < (buf)->linecount)

#define TAG "BUFFER"

static unsigned int generate_id()
{
    static unsigned int id = 0;
    return id++;
}

// TODO(realloc): empty buffer
Buffer::Buffer(enum Buffer::Type type)
{
    id         = generate_id();
    this->type = type;    // TODO(realloc): type specific setup
    name       = "[NEWBUF]";

    /** lines */
    linecount = 1;    // Empty buffer has at least one line
    lines     = new Line[linecount];
    assert(lines);
}


Buffer::Buffer(enum Type type, const char* name, Line* lines,
               unsigned int linecount)
    : Buffer(type)
{
    this->name = name;
    this->lines = lines;
    this->linecount = linecount;
}


void Buffer::destroy()
{
    for (unsigned int i = 0; i < linecount; ++i) {
        //free(lines[i].data); // TODO!!!
    }
    free(lines);
    log_l(TAG, "Buffer destroyed (id=%d)", id);
}


/** Returns pointer to line in buffer. NULL if 'num' is out of bounds*/
Line& Buffer::getLine(unsigned int num)
{
    assert(num < linecount);
    return lines[num];
}


const Line& Buffer::getLine(unsigned int num) const
{
    assert(num < linecount);
    return lines[num];
}


bool Buffer::splitLine(const Cursor& cur)
{
    log_l(TAG, "Splitting line at cur: col:%d, line:%d", cur.col, cur.line);
    Line& ln = getLine(cur.line);
    ln.deleteGap();

    // create New line
    unsigned int newlen = ln.Len() - cur.col;
    char* newdata = new char[newlen];
    for(unsigned int i = cur.col, j = 0; i < ln.trueLen(); ++i, ++j)
    {
        newdata[j] = ln[i];
    }

    // Clip old line
    ln.clearToEnd(cur.col);
    // Insert newline into lines
    Line newline = Line(newlen, newdata);
    newline.pprint();

    unsigned int newlinecount = linecount + 1;
    Line* newlinebuffer = new Line[newlinecount];

    for(unsigned int i = 0; i <= cur.line; ++i) {
        newlinebuffer[i] = lines[i];
    }
    newlinebuffer[cur.line + 1] = newline;
    for(unsigned int i = cur.line + 2; i < linecount + 1; ++i) {
        newlinebuffer[i] = lines[i - 1];
    }
    delete[] lines;
    lines = newlinebuffer;
    linecount = newlinecount;
    return true;
}


void Buffer::pprint() const
{
}


void Buffer::pprintLines() const
{
    log_l(TAG, " -- line metadata --");
    for (unsigned int i = 0; i < linecount; ++i) {
        log_l(TAG, "no: %d, len: %d", i, lines[i].Len());
    }
    log_l(TAG, " --/ line metadata --");
}
