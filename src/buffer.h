#pragma once
#include "line.h"


/** buffer.h
 * A buffer consists of a type describing the buffer
 * (document temporary, dialog's etc.) and a number
 * of 'lines' (line.h)
 * They can be saved to disk, or discarded.
 */


typedef int hBuffer;

enum buffer_type {
	DEFAULT,	// Empty buffer
	DOCUMENT,	// Saved on disk
	SCRATCH		// Temporary buffer
};

/** Initialize buffer submodule */
int buf_init();
int buf_exit();

/** Create a new empty buffer of the specified type */
hBuffer buf_create(enum buffer_type type, unsigned int linecount);

/** Create a new buffer from an existing document */
hBuffer buf_createfrom_file(const char* filename);

/** buf_destroy destorys the buffer and discards all its lines */
void buf_destory(hBuffer buf);

void buf_pprint(hBuffer buf);
