#include "buffer_view.h"
#include "log.h"
#include <assert.h>

#define TAG "BVIEW"

struct buffer_view bview_create(struct buffer* buf,
        unsigned int start, unsigned int len)
{
    assert(buf);
    struct buffer_view bv;
    bv.start = start;
    bv.len = len;
    bv.buffer = buf;
    return bv;
}

void bview_draw(WINDOW* nwin, struct buffer_view bv,
        unsigned int y, unsigned int x,
        unsigned int rows, unsigned int cols)
{
    log_l(TAG, "Drawing buffer: %d lines", bv.buffer->linecount);
    x++; // Add padding
    unsigned int ox = x;

    for(unsigned int i = 0; i < bv.buffer->linecount; ++i) {
        struct line* ln = buf_line(bv.buffer, i);
        log_lc(TAG, "Drawing line: ");
        buf_printline(bv.buffer, i);

        if(bv.buffer->gap.line == i) {
            for(unsigned int j = 0; j < ln->len; ++j) {
                if(!buf_ingap(bv.buffer, j)) {
                    mvwaddch(nwin, y, x++, ln->data[j]);
                }
            }
        } else {
            for(unsigned int j = 0; j < ln->len; ++j) {
                mvwaddch(nwin, y, x++, ln->data[j]);
            }
        }
        y++;
        x = ox;
    }
}

