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
struct buffer_view bview_create(struct buffer* buf);

/** CURSOR **/
void bview_curmove_f(struct buffer_view* bv, unsigned int n);
void bview_curmove_b(struct buffer_view* bv, unsigned int n);
void bview_curmove_nextline(struct buffer_view* bv, unsigned int n);
void bview_curmove_prevline(struct buffer_view* bv, unsigned int n);

/** Sroll buffer view up by n lines */
void bview_scrollup(struct buffer_view* bv, unsigned int n);
/** Sroll buffer view down by n lines */
void bview_scrolldown(struct buffer_view* bv, unsigned int n);


