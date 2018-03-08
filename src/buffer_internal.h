#pragma once
#include "buffer.h"

/** Static method headers */


/** return a new id for a buffer */
static
unsigned int _generate_id();

/** return a pointer to the buffer in use of the handle */
static
struct buffer* _get_buffer(hBuffer buf);

/** return a handle to a free buffer (in_use == 0) */
static
hBuffer _get_free_handle();

/** Calculate number of char 'c' in the buffer */
static
unsigned int _calc_charcount(struct buffer* b, char ch);

/** Increase buffer gap */
static
void _gap_add(struct buffer* b);

static
int _update_cache(struct buffer* b);

/** buf_cur_mv* functions just update buffer.cur. they don't move
 * the gap in memory (because that would be expensive)
 * The gap is moved only when addch / delch are called i.e. the buffer
 * is actually modified.
 * This function does exactly that - move's gap to cursor position
 */
static
int _sync_gap(struct buffer* b);
