#include "buffer.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "log.h"

// A maximum of 4 buffers allowed
#define BUFFER_DEFCAP 4

struct buffer {
	int id;
	int flags;

	// Type of buffer
	enum buffer_type type;
	int width, height;

	int linecount;
	hLine* lines;
};

static struct {
	const char* tag;
	int bufcount;
	struct buffer buffers[BUFFER_DEFCAP];
} G;


int buf_init()
{
	G.tag = "BUFFER";
	G.bufcount = 0;
	return 0;
}

int buf_exit()
{
	// TODO
	return 1;
}


hBuffer buf_create(enum buffer_type type)
{
	return 0;
}

