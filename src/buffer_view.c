#include "buffer_view.h"
#include "log.h"
#include <assert.h>

#define TAG "BVIEW"

struct buffer_view bview_create(struct buffer* buf,
        unsigned int start, unsigned int len)
{
    assert(buf);
    struct buffer_view bv;
    bv.buffer = buf;
    bv.start = start < buf->linecount ? start : buf->linecount;
    bv.len = len < buf->linecount
        ? len : buf->linecount - bv.start;
    bv.cur.line = 0;
    bv.cur.col = 0;
    return bv;
}

void bview_draw(struct buffer_view bv, WINDOW* nwin,
        struct rect area)
{
    log_l(TAG, "Drawing buffer: %d lines", bv.buffer->linecount);
    unsigned int ox = area.x;
    unsigned int oy = area.y;

    for(unsigned int i = 0; i < bv.len; ++i) {
        struct line* ln = buf_line(bv.buffer, bv.start + i);
        log_lc(TAG, "Drawing line: ");
        buf_printline(bv.buffer, i);

        if(bv.buffer->gap.line == i) {
            for(unsigned int j = 0; j < ln->len; ++j) {
                if(!buf_ingap(bv.buffer, j)) {
                    mvwaddch(nwin, area.y, area.x++, ln->data[j]);
                }
            }
        } else {
            for(unsigned int j = 0; j < ln->len; ++j) {
                mvwaddch(nwin, area.y, area.x++, ln->data[j]);
            }
        }
        area.y++;
        area.x = ox;
    }

    // Move cursor to the original position
    wmove(nwin, oy + bv.cur.line, ox + bv.cur.col);
}

