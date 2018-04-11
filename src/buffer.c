#include "buffer.h"
#include "buffer_internal.h"
#include "fileutils.h"
#include "input_keys.h"
#include "log.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define TAG "BUFFER"

static size_t gids = 0;

struct buffer* buf_create_empty(enum buffer_type type)
{
    struct buffer* buf = malloc(sizeof(struct buffer));
    assert(buf);

    buf->id   = gids++;
    buf->type = type;    // TODO(realloc): type specific setup
    buf->name = "[NEWBUF]";

    /** lines */
    buf->linecount = 1;    // Empty buffer has at least one line
    buf->lines     = calloc(buf->linecount, sizeof(struct line));
    assert(buf->lines);

    return buf;
}

struct buffer buf_create_file(enum buffer_type type,
                              const char* filename)
{
    struct file_stats fstats = fu_stats(filename);
    if (!(fstats.exists == 1 && F_FILE == fstats.type)) {
        if (fstats.type == F_DIR) {
            log_fatal(TAG, "%s: cannot open:%s. (Its a directory)",
                      __func__, filename);
        } else {
            log_fatal(TAG, "%s: couldn't open:%s. exiting.", __func__,
                      filename);
            /*// Return a new empty buffer;*/
            /*struct buffer* buf = buf_create_empty(type);*/
            /*buf->name = filename;*/
            /*return buf;*/
        }
    }

    struct buffer buf;

    buf.id   = gids++; // TODO Generate
    buf.type = type;
    buf.name = filename;

    /** lines */
    buf.lines     = NULL;
    buf.linecount = fu_read_file_lines(fstats.path, &buf.lines);
    assert(buf.lines);

    /*buf_printline(&buf, 0);*/

    return buf;
}


void buf_destroy(struct buffer* buf)
{
    size_t id = buf->id;
    for (size_t i = 0; i < buf->linecount; ++i) {
        struct line* ln = &buf->lines[i];
        free(ln->data);
    }
    free(buf->lines);
    free(buf);
    log_l(TAG, "Buffer destroyed (id=%d)", id);
    buf = NULL;
}


void buf_addline(struct buffer* b, Line ln, size_t idx)
{
    assert(b);
    assert(idx < b->linecount);
    size_t nlcount = b->linecount + 1;

    Line* nlines = malloc(nlcount * sizeof(Line));
    assert(nlines);
    for(size_t i = 0; i < nlcount; ++i) {
        if(i < idx) {
            nlines[i] = b->lines[i];
        } else if (i == idx) {
            nlines[i] = ln;
        } else {
            nlines[i] = b->lines[i - 1];
        }
    }

    free(b->lines);
    b->lines = nlines;
    b->linecount = nlcount;
}


/** Returns pointer to line in buffer. NULL if 'num' is out of
 * bounds*/
Line* buf_getline(const struct buffer* buf, size_t num)
{
    if (num >= buf->linecount) {
        log_fatal(TAG, "Invalid line request: buf: %d (%d lines),"
         "request: %d", buf->id, buf->linecount, num);
    }
    return &buf->lines[num];
}


size_t buf_line_count(const struct buffer* buf)
{
    return buf->linecount;
}



int buf_save_to_disk(const struct buffer* buf, const char* path)
{
    return fu_save_buffer(buf, path);
}


void buf_pprint(const struct buffer* b)
{
    assert(b);
    log_l(TAG, "Buffer{id=%d , lncount=%d}", b->id, b->linecount);
}


void buf_pprint_lines(const struct buffer* buf)
{
    log_l(TAG, " -- line metadata --");
    for (size_t i = 0; i < buf->linecount; ++i) {
        const struct line* ln = buf_getline(buf, i);
        log_l(TAG, "no: %d, len: %d", i, ln->len);
    }
    log_l(TAG, " --/ line metadata --");
}

/** buffer_internal.h **/


void buf_printline(const struct buffer* buf, size_t i)
{
    assert(i < buf->linecount);
    const struct line* ln = buf_getline(buf, i);
    log_l("LINE", "no: %d, len: %d", i, ln->len);
    for (size_t j = 0; j < ln->len; ++j) {
        if (INGAP(ln, j)) {
            log_lc("-");
        } else {
            log_lc("%c", ln->data[j]);
        }
    }
    log_lc("\n");
}
