/** buffer.h
 * A window consists of one or more buffers. A buffer
 * consists of a type describing the buffer (document,
 * temporary, dialog's etc.) and a number of 'lines'
 * (line.h)
 * Buffers can be resized and moved around. They can be hidden,
 * saved on disk, or discarded.
 */
#pragma once
#include "line.h"

typedef unsigned int hBuffer;

enum buffer_type {
	DOCUMENT,	// Saved on disk
	SCRATCH		// Temporary buffer
};


/** Initialize buffer submodule */
int buf_init();
int buf_exit();

/** Create a new buffer of the specified type */
hBuffer buf_create(enum buffer_type type);

/** buf_destroy destorys the buffer and discards all its lines */
int buf_destory(hBuffer buf);

