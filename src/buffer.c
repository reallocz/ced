#include "buffer.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "log.h"

// A maximum of 4 buffers allowed
#define BUFFER_DEFCAP 4
// Empty buffer line count
#define BUFFER_DEFLINECOUNT 4

/** Bring struct buffer* alias of id x into scope */
#define SB(alias, hBuffer) \
	struct buffer* alias = _get_buffer(hBuffer)

struct buffer {
	int id;		// -1 for unused buffer
	int flags;
	enum buffer_type type;
	char name[64];
	unsigned int linecount;
	hLine* lines;
};


static struct {
	const char* tag;
	unsigned int bufcount;
	unsigned int bufcap;
	struct buffer buffers[BUFFER_DEFCAP];
} G;


int buf_init()
{
	G.tag = "BUFFER";
	G.bufcount = 0;
	G.bufcap = BUFFER_DEFCAP;
	return 0;
}


int buf_exit()
{
	// TODO
	return 1;
}

static struct buffer* _get_buffer(hBuffer buf)
{
	if(buf >= G.bufcap) {
		log_e(G.tag, "%s: invalid buffer: %d. (bufcap=%d)", __func__, buf, G.bufcap);
		exit(1);
	}
	return &G.buffers[buf];
}


hBuffer buf_create(enum buffer_type type)
{
	SB(b, G.bufcount);
	b->id = G.bufcount;
	b->flags = 0;
	b->type = type;
	b->linecount = BUFFER_DEFLINECOUNT;

	b->lines = malloc(sizeof(hLine) * b->linecount);
	for(unsigned int i = 0; i < b->linecount; ++i) {
		b->lines[i] = line_create();
	}
	log_l(G.tag, "buffer created");
	buf_pprint(b->id);

	G.bufcount++;
	return b->id;
}

void buf_destroy(hBuffer buf)
{
	SB(b, buf);
	b->id = -1;
	// TODO free lines
	return 0;
}


void buf_pprint(hBuffer buf)
{
	SB(b, buf);
	log_l(G.tag, "Buffer{id=%d, name=%s, lncount=%d",
			b->id, b->name, b->linecount);
}
