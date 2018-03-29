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

static unsigned int generate_id() {
    static unsigned int id = 0;
    return id++;
}

// TODO empty buffer
Buffer::Buffer(enum Buffer::Type type)
{
    id         = generate_id();
    this->type = type;    // TODO(realloc): type specific setup
    name       = "[NEWBUF]";

    /** lines */
    linecount = 1;    // Empty buffer has at least one line
    lines     = static_cast<Line*>(calloc(linecount, sizeof(Line)));
    assert(lines);
}

Buffer::Buffer(enum Buffer::Type type, const char* filename)
{
    struct file_stats fstats = fu_stats(filename);
    if (!(fstats.exists == 1 && F_FILE == fstats.type)) {
        if (fstats.type == F_DIR) {
            log_fatal(TAG, "%s: cannot open:%s. (Its a directory)",
                      __func__, filename);
        } else {
            log_fatal(TAG, "%s: couldn't open:%s. exiting.",
                      __func__, filename);
            /*// Return a new empty buffer;*/
            /*struct buffer* buf = buf_create_empty(type);*/
            /*buf->name = filename;*/
            /*return buf;*/
        }
    }

    id         = generate_id();
    this->type = type;
    name       = filename;

    /** lines */
    lines     = nullptr;
    linecount = fu_read_file_lines(fstats.path, &lines);
    assert(lines);
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
Line& Buffer::line(unsigned int num)
{
    assert(num < linecount);
    return lines[num];
}

const Line& Buffer::line(unsigned int num) const
{
    assert(num < linecount);
    return lines[num];
}


unsigned int Buffer::lineCount() const
{
    return linecount;
}



bool Buffer::saveToDisk(const char* path)
{
    //return fu_save_buffer(buf, path);
    return false;
}


void Buffer::pprint() const
{
    //log_l(TAG, "Buffer{id=%d, gap.pos=%d, gap.size = %d"
               //", lncount=%d}",
          //id, gap.col, gap.size, linecount);
}


void Buffer::pprintLines() const
{
    log_l(TAG, " -- line metadata --");
    for (unsigned int i = 0; i < linecount; ++i) {
        log_l(TAG, "no: %d, len: %d", i, lines[i].Len());
    }
    log_l(TAG, " --/ line metadata --");
}

