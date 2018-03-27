#include "buffer.h"
#include "buffer_internal.h"
#include "fileutils.h"
#include "input_keys.h"
#include "log.h"
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#define BUFFER_GAPSIZE 4

#define CURVALID(buf, cur) \
    ((cur).line < (buf)->linecount)

#define TAG "BUFFER"

Buffer::Buffer(enum Buffer::Type type)
{
    id         = generate_id();
    this->type = type;    // TODO(realloc): type specific setup
    name       = "[NEWBUF]";

    /** gap */
    gap.line = 0;
    gap.col  = 0;
    gap.size = BUFFER_GAPSIZE;

    /** lines */
    linecount = 1;    // Empty buffer has at least one line
    lines     = static_cast<Line*>(calloc(linecount, sizeof(Line)));
    assert(lines);

    // Set gap on the first line
    Line* fln = line(0);
    fln->len  = BUFFER_GAPSIZE;
    fln->data = static_cast<char*>(calloc(fln->len, sizeof(char)));
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

    /** gap */
    gap.line = 0;
    gap.col  = 0;
    gap.size = 0;    // Gap starts at 0
    addGap();
    pprintLine(0);
}


void Buffer::destroy()
{
    for (unsigned int i = 0; i < linecount; ++i) {
        free(lines[i].data);
    }
    free(lines);
    log_l(TAG, "Buffer destroyed (id=%d)", id);
}


/** Adding a character to the buffer requires a gap synchronization.
 * It may also require a gap resize(increase). The order of the two
 * operations is not fixed right now but I don't think it really
 * makes a difference */
void Buffer::addCh(char ch, const struct cursor& cur)
{
    moveGap(cur);
    addGapOptional();

    Line* ln          = line(cur.line);
    ln->data[gap.col] = ch;
    // shrink gap
    gap.col++;
    gap.size--;
}

/** Deleting a character in the buffer has the same constraints as
 * the add operation - apart from the buffer resizing */
void Buffer::delCh(const struct cursor& cur)
{
    if (cur.col < 1) {
        return;
    }
    moveGap(cur);
    gap.col--;
    gap.size++;
}

/** Returns pointer to line in buffer. NULL if 'num' is out of bounds*/
Buffer::Line* Buffer::line(unsigned int num)
{
    if (num >= linecount) {
        /*log_e(TAG, "Invalid line request: buf: %d (%d lines), request: %d", buf->id, buf->linecount, num);*/
        return nullptr;
    }
    return &lines[num];
}

const Buffer::Line* Buffer::line(unsigned int num) const
{
    if (num >= linecount) {
        /*log_e(TAG, "Invalid line request: buf: %d (%d lines), request: %d", buf->id, buf->linecount, num);*/
        return nullptr;
    }
    return &lines[num];
}


unsigned int Buffer::lineCount() const
{
    return linecount;
}


bool Buffer::lineHasGap(unsigned int line) const
{
    return gap.line == line;
}


bool Buffer::inGap(unsigned int i) const
{
    return i >= gap.col && i < gap.col + gap.size;
}


bool Buffer::saveToDisk(const char* path)
{
    //return fu_save_buffer(buf, path);
    return false;
}


void Buffer::pprint() const
{
    log_l(TAG, "Buffer{id=%d, gap.pos=%d, gap.size = %d"
               ", lncount=%d}",
          id, gap.col, gap.size, linecount);
}


void Buffer::pprintLines() const
{
    log_l(TAG, " -- line metadata --");
    for (unsigned int i = 0; i < linecount; ++i) {
        log_l(TAG, "no: %d, len: %d", i, lines[i].len);
    }
    log_l(TAG, " --/ line metadata --");
}


void Buffer::pprintLine(unsigned int i) const
{
    assert(i < linecount);
    int hasgap = i == gap.line ? 1 : 0;
    Line* ln   = &lines[i];
    log_l("LINE", "no: %d, len: %d, has_gap: %d",
          i, ln->len, hasgap);
    for (unsigned int i = 0; i < ln->len; ++i) {
        if (inGap(i)) {
            log_lc("-");
        } else {
            log_lc("%c", ln->data[i]);
        }
    }
    log_lc("\n");
}

bool Buffer::addGap()
{
    Line* ln = line(gap.line);

    unsigned int newlinelen = ln->len + BUFFER_GAPSIZE;
    unsigned int newgapsize = gap.size + BUFFER_GAPSIZE;

    auto* newdata = static_cast<char*>(calloc(newlinelen, sizeof(char)));
    assert(newdata);

    /* Copy memory to new data */
    // from start to gap
    memcpy(newdata, ln->data, gap.col * sizeof(char));
    // from end of gap to end of buffer
    memcpy(&newdata[gap.col + newgapsize],
           &ln->data[gap.col + gap.size],
           ln->len - (gap.col + gap.size));
    // free old buffer
    free(ln->data);
    ln->data = newdata;
    ln->len  = newlinelen;
    gap.size = newgapsize;

    return true;
}


bool Buffer::addGapOptional()
{
    if (gap.size < 1) {
        unsigned int oldgap = gap.size;
        addGap();
        log_l(TAG, "Gap resized %d -> %d", oldgap, gap.size);
        return true;
    }
    return false;
}


/* NOTE: toline must NOT be same as gap.line! */
bool Buffer::moveGapToLine(unsigned int toline)
{
    assert(gap.line != toline);
    // Delete current gap
    {
        Line* ln                = line(gap.line);
        unsigned int linelength = ln->len - gap.size;
        auto* newdata           = static_cast<char*>(malloc(linelength * sizeof(char)));
        assert(newdata);
        unsigned int count = 0;
        for (unsigned int i = 0; i < ln->len; ++i) {
            if (!inGap(i)) {
                newdata[count] = ln->data[i];
                count++;
            }
        }
        assert(count == linelength);
        free(ln->data);

        // Update line length and data
        ln->len  = linelength;
        ln->data = newdata;
    }
    // Create gap on toline
    {
        Line* ln               = line(toline);
        unsigned int newlength = ln->len + gap.size;
        auto* newdata          = static_cast<char*>(malloc(newlength * sizeof(char)));
        assert(newdata);
        memcpy(&newdata[gap.size],
               ln->data,
               ln->len);
        free(ln->data);

        // Update line length and data
        ln->len  = newlength;
        ln->data = newdata;
    }
    // Update gap location (NOT SIZE!!)
    gap.line = toline;
    gap.col  = 0;
    return true;
}

/** move gap to cursor */
bool Buffer::moveGap(const struct cursor& cur)
{
    assert(cur.line < linecount);

    // Ensure that the gap is on the same line as that of the cursor
    if (gap.line != cur.line) {
        moveGapToLine(cur.line);
    }
    // Now, the cursor and the gap are on the same line

    Line* ln            = line(cur.line);
    unsigned int offset = 0;
    int diff            = cur.col - gap.col;

    if (diff > 0) {
        // Cursor is ahead of the gap
        offset = diff;
        for (unsigned int i = 0; i < offset; ++i) {
            ln->data[gap.col] =
                ln->data[gap.col + gap.size];
            gap.col++;
        }
        log_l(TAG, "%s: diff=%d, offset=%d", __func__, diff, offset);
    } else if (diff < 0) {
        // Cursor is behind the gap
        offset = -1 * diff;
        for (unsigned int i = 0; i < offset; ++i) {
            ln->data[gap.col + gap.size - 1] = ln->data[gap.col - 1];
            gap.col--;
        }
        log_l(TAG, "%s: diff=%d offset=%d", __func__, diff, offset);
    } else {
        // gap is in sync do nothing.
    }
    return true;
}


static unsigned int generate_id()
{
    static unsigned int ids = 0;
    return ids++;
}
