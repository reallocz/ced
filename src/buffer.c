#include "buffer.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "log.h"

// A maximum of 4 buffers allowed
#define BUFFER_DEFCAP 4

/** Bring struct buffer* alias of id x into scope */
#define SB(alias, hBuffer) \
	struct buffer* alias = _get_buffer(hBuffer)

struct buffer {
	/*unique identifier of each buffer for debugging*/
	unsigned int id;
	/* 1 if buffer is in use (not destroyed)*/
	int in_use;

	enum buffer_type type;
	const char* name;
	unsigned int linecount;
	hLine* lines;
};


static struct {
	const char* tag;
	unsigned int bufcap; // Total buffer capacity
	struct buffer buffers[BUFFER_DEFCAP];
} G;


int buf_init()
{
	G.tag = "BUFFER";
	G.bufcap = BUFFER_DEFCAP;

	for(unsigned int i = 0; i < G.bufcap; ++i) {
		G.buffers[i].in_use = 0;
	}
	log_l(G.tag, "Init success");
	return 0;
}


int buf_exit()
{
	// TODO
	return 1;
}


/** return a new id for a buffer */
static
unsigned int _generate_id()
{
	static unsigned int ids = 0;
	return ids++;
}

/** return a pointer to the buffer in use of the handle */
static
struct buffer* _get_buffer(hBuffer buf)
{
	if(buf < 0 || buf >= G.bufcap) {
		log_fatal(G.tag, "Invalid handle: %d", buf);
	}
	struct buffer* b = &G.buffers[buf];
	if(! b->in_use) {
		log_fatal(G.tag, "Bad handle: %d", buf);
	}
	return b;
}


/** return a handle to a free buffer (in_use == 0)*/
static
hBuffer _get_free_handle()
{
	for(unsigned int i = 0; i < G.bufcap; ++i) {
		if(! G.buffers[i].in_use) {
			return i;
		}
	}
	log_fatal(G.tag, "%s: OUT OF BUFFERS!", __func__);
}


void buf_destroy(hBuffer buf)
{
	SB(b, buf);
	unsigned int id = b->id;
	b->in_use = 0;
	for(unsigned int i = 0; i < b->linecount; ++i) {
		line_destroy(b->lines[i]);
	}
	free(b->lines);
	log_l(G.tag, "Buffer %d destroyed", id);
}



hBuffer buf_create(enum buffer_type type, unsigned int linecount)
{
	assert(linecount);

	hBuffer buf = _get_free_handle();
	struct buffer* b = &G.buffers[buf];

	b->id = _generate_id();
	b->in_use = 1;
	b->type = type; // TODO type specific setup
	b->linecount = linecount;

	b->lines = malloc(sizeof(hLine) * b->linecount);
	for(unsigned int i = 0; i < b->linecount; ++i) {
		b->lines[i] = line_create();
	}
	return buf;
}


unsigned int buf_get_linecount(hBuffer buf)
{
	SB(b, buf);
	return b->linecount;
}

void buf_pprint(hBuffer buf)
{
	SB(b, buf);
	log_l(G.tag, "Buffer{id=%d, name=%s, lncount=%d",
			b->id, b->name, b->linecount);
}
