#pragma once
#include "buffer.h"

/** Static method headers */

/** return a new id for a buffer */
static unsigned int generate_id(void);

/** Increase buffer gap */
static void gap_add(struct buffer* buf);

/** Resize buffer gap if needed */
static int gap_resize_optional(struct buffer* buf);

/** move gap to cursor */
static void gap_move(struct buffer* buf, struct cursor cur);
