#pragma once
#include "buffer.h"
#include "common.h"
#include "ncurses.h"

/** buffer_view.h
 * renders parts of a buffer */

struct buffer_view {
    unsigned int start; // View starts from
    struct cursor cur;
    struct buffer buffer;
    struct rect bounds;
};

/** Create a new buffer_view */
struct buffer_view bv_create(enum buffer_type type, const char* filename);
/** Update tick function */
void bv_update(struct buffer_view* bv);

/** Set buffer_views bounds */
void bv_bounds_set(struct buffer_view* bv, struct rect bounds);
/** Return a copy of bounds */
struct rect bv_bounds(const struct buffer_view* bv);

/** CURSOR **/
void bv_cset(struct buffer_view* bv, struct cursor cur);
void bv_cmov_fwd(struct buffer_view* bv, unsigned int n);
void bv_cmov_back(struct buffer_view* bv, unsigned int n);
void bv_cmov_lnext(struct buffer_view* bv, unsigned int n);
void bv_cmov_lprev(struct buffer_view* bv, unsigned int n);

/** Move cursor to the start of the line */
void bv_cmov_lstart(struct buffer_view* bv);
/** Move cursor to the end of the line */
void bv_cmov_lend(struct buffer_view* bv);
/** If cursor's outside the line bounds, move it in [0, line.len) */
void bv_cmov_inline(struct buffer_view* bv);

/** Sroll buffer view up by n lines. (LIKE PAGE UP) */
void bv_scrollup(struct buffer_view* bv, unsigned int n);
/** Sroll buffer view down by n lines. (LIKE PAGE DOWN)*/
void bv_scrolldown(struct buffer_view* bv, unsigned int n);

/** Get bounds/extents of the view */
/** Return's the number(0 indexed) of the first line of the buffer_view */
unsigned int bv_start(const struct buffer_view* bv);

/** Return cursor relative to bview.start */
struct cursor bv_relcur(const struct buffer_view* bv);

