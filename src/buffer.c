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
	int id;		// -1 for unused buffer
	int flags;
	enum buffer_type type;
	const char* name;
	unsigned int linecount;
	hLine* lines;
};


static struct {
	const char* tag;

	unsigned int ids; // Generate id's by incrementing
	unsigned int bufcap; // Total buffer capacity
	struct buffer buffers[BUFFER_DEFCAP];
} G;


int buf_init()
{
	G.tag = "BUFFER";
	G.bufcap = BUFFER_DEFCAP;
	G.ids = 0;
	for(unsigned int i = 0; i < G.bufcap; ++i) {
		G.buffers[i].id = -1;
	}
	log_l(G.tag, "Init success");
	return 0;
}


int buf_exit()
{
	// TODO
	return 1;
}


static
struct buffer* _get_buffer(hBuffer buf)
{
	for(unsigned int i = 0; i < G.bufcap; ++i) {
		if(G.buffers[i].id == buf) {
			return &G.buffers[i];
		}
	}

	// Buffer not found
	log_e(G.tag, "%s: buffer not found: %d.", __func__,
			buf, G.bufcap);
	log_ec("Available buffers: ");
	for(unsigned int i = 0; i < G.bufcap; ++i) {
		if(G.buffers[i].id != -1) {
			log_ec("%d, ", G.buffers[i].id);
		}
	}
	log_ec("\n");
	exit(1);
}


/** Generate a new id for buffer */
static
unsigned int _generate_id()
{
	return G.ids++;
}


/** Returns index of a free buffer */
static
struct buffer*  _get_free_buffer()
{
	for(unsigned int i = 0; i < G.bufcap; ++i) {
		if(G.buffers[i].id == -1) {
			log_l(G.tag, "%s: found free buffer at: %d", __func__, i);
			return &G.buffers[i];
		}
	}
	log_e(G.tag, "%s: OUT OF BUFFERS!", __func__);
	exit(1);
}



void buf_destroy(hBuffer buf)
{
	SB(b, buf);
	unsigned int id = b->id;
	b->id = -1;
	for(unsigned int i = 0; i < b->linecount; ++i) {
		line_destroy(b->lines[i]);
	}
	free(b->lines);
	log_l(G.tag, "Buffer %d destroyed", id);
}



hBuffer buf_create(enum buffer_type type, unsigned int linecount)
{
	assert(linecount);

	struct buffer* b = _get_free_buffer();
	assert(b->id == -1);

	b->id = _generate_id();
	b->flags = 0;
	b->type = type; // TODO type specific setup
	b->linecount = linecount;

	b->lines = malloc(sizeof(hLine) * b->linecount);
	for(unsigned int i = 0; i < b->linecount; ++i) {
		b->lines[i] = line_create();
	}

	log_l(G.tag, "buffer created");
	buf_pprint(b->id);

	return b->id;
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
