#pragma once
#include "buffer.h"
#include "common.h"
#include "ncurses.h"

/** buffer_view.h
 * renders parts of a buffer */

struct buffer_view {
    unsigned int start; // View starts from
    unsigned int len;   // Number of lines of the view
    struct cursor cur;
    struct buffer* buffer;
};

/** Create a new buffer_view */
struct buffer_view bview_create(struct buffer* buf,
        unsigned int start, unsigned int len);

/** Draw buffer on window in specified area */
void bview_draw(struct buffer_view bv, WINDOW* nwin,
        struct rect area);

/** Sroll buffer view up by n lines */
void bview_scrollup(struct buffer_view* bv, unsigned int n);
/** Sroll buffer view down by n lines */
void bview_scrolldown(struct buffer_view* bv, unsigned int n);


