#include "buffer_view.h"
#include "log.h"
#include <assert.h>

#define TAG "BVIEW"

struct buffer_view bview_create(struct buffer* buf)
{
    assert(buf);
    struct buffer_view bv;
    bv.cur.line = 0;
    bv.cur.col = 0;

    bv.buffer = buf;
    bv.start = 0;
    bv.len = buf->linecount;

    return bv;
}

