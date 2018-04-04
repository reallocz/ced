#include "buffer.h"
#include "fileutils.h"
#include "input_keys.h"
#include "log.h"
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#define CURVALID(buf, cur) ((cur).line < (buf)->linecount)

#define TAG "BUFFER"

static size_t generate_id()
{
    static size_t id = 0;
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
               size_t linecount)
    : Buffer(type)
{
    this->name      = name;
    this->lines     = lines;
    this->linecount = linecount;
}


void Buffer::destroy()
{
    for (size_t i = 0; i < linecount; ++i) {
        // free(lines[i].data); // TODO!!!
    }
    free(lines);
    log_l(TAG, "Buffer destroyed (id=%d)", id);
}


/** Returns pointer to line in buffer. NULL if 'num' is out of
 * bounds*/
Line& Buffer::getLine(size_t num)
{
    assert(num < linecount);
    return lines[num];
}


const Line& Buffer::getLine(size_t num) const
{
    assert(num < linecount);
    return lines[num];
}


void Buffer::addLineAt(size_t idx, Line& ln)
{
    assert(idx <= lineCount());
    size_t nlcount = linecount + 1;
    auto* nlines = new Line[nlcount];
    assert(nlines);

    for(unsigned int i = 0; i < nlcount; ++i) {
        if(i < idx) {
            nlines[i] = lines[i];
        } else if (i == idx) {
            nlines[i] = ln;
        } else {
            nlines[i] = lines[i - 1];
        }
    }

    delete[] lines;
    lines = nlines;
    linecount = nlcount;
}


void Buffer::pprint() const {}


void Buffer::pprintLines() const
{
    log_l(TAG, " -- line metadata --");
    for (size_t i = 0; i < linecount; ++i) {
        log_l(TAG, "no: %d, len: %d", i, lines[i].length());
    }
    log_l(TAG, " --/ line metadata --");
}
