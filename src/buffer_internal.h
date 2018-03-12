#pragma once
#include "buffer.h"

/** Static method headers */


/** return a new id for a buffer */
static
unsigned int generate_id();

/** Increase buffer gap */
static
void gap_add(struct buffer* b);

static
int update_cache(struct buffer* b);

/** buf_cur_mv* functions just update buffer.cur. they don't move
 * the gap in memory (because that would be expensive)
 * The gap is moved only when addch / delch are called i.e. the buffer
 * is actually modified.
 * This function does exactly that - move's gap to cursor position
 */
static
int gap_sync(struct buffer* b);
