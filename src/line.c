#include "line.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "log.h"

#define LN_DEFSIZE 8
#define LN_DEFGAP 4
#define LN_DEFCAP 64

/** Bring struct line* alias of id x into scope */
#define SL(alias, hLine) \
	struct line* alias = _get_line(hLine)

struct line {
	/*unique identifier of each line for debugging*/
	unsigned int id;
	/* 1 if line is in use (not destroyed)*/
	int in_use;

        unsigned int num;	// Line number
        char* buf;		// buffer
        unsigned int size;	// line size
        unsigned int cur;	// Cursor
        unsigned int gap;	// Gap size
};


static struct {
	const char* tag;
	unsigned int linecap;
	struct line* lines;
} G;


int line_init()
{
	G.tag = "LINE";
	G.linecap = LN_DEFCAP;

	G.lines = malloc(G.linecap * sizeof(struct line));
	assert(G.lines);

	for(unsigned int i = 0; i < G.linecap; ++i) {
		G.lines[i].in_use = 0;
	}

	log_l(G.tag, "Init success");
	return 0;
}


int line_exit()
{
	for(unsigned int i = 0; i < G.linecap; ++i) {
		free(G.lines[i].buf);
	}
	free(G.lines);
	return 0;
}

/** return a new id for a line */
static
unsigned int _generate_id()
{
	static unsigned int ids = 0;
	return ids++;
}


/** Return the pointer to the line in use*/
static
struct line* _get_line(hLine line)
{
	if(line < 0 || line >= G.linecap) {
		log_fatal(G.tag, "Invalid handle: %d", line);
	}
	struct line* ln = &G.lines[line];
	if(!ln->in_use) {
		log_fatal(G.tag, "Bad handle: %d", line);
	}
	return ln;
}



/** return a handle to a free line (in_use == 0)*/
static
hLine _get_free_handle()
{
	for(unsigned int i = 0; i < G.linecap; ++i) {
		if(! G.lines[i].in_use) {
			return i;
		}
	}
	log_fatal(G.tag, "%s: OUT OF LINES!", __func__);
}


hLine line_create(void)
{
	hLine line = _get_free_handle();
        struct line* ln = &G.lines[line];
	ln->id = _generate_id();
	ln->in_use = 1;
        ln->num = 0;
        ln->buf = NULL;
        ln->size = LN_DEFSIZE;
        ln->cur = 0;
        ln->gap = LN_DEFGAP;

        // Allocate memory for buffer
        ln->buf = calloc(ln->size, sizeof(ln->buf[0]));
        assert(ln->buf);

        return line;
}


void line_destroy(hLine line)
{
	SL(ln, line);
	ln->in_use = 0;
	free(ln->buf);
}


void line_add_gap(hLine line)
{
	SL(ln, line);

	unsigned int newsize = ln->size + LN_DEFGAP;
	unsigned int newgap = ln->gap + LN_DEFGAP;
	char* newbuf = calloc(newsize, sizeof(ln->buf[0]));
	assert(newbuf);

	printf("%s: size(%d->%d), gap(%d->%d)\n", __func__,
			ln->size, newsize, ln->gap, newgap);

	// Left slice
	memcpy(newbuf, ln->buf, ln->cur * sizeof(char));
	// Right slice
	memcpy(&newbuf[ln->cur + newgap],
			&ln->buf[ln->cur + ln->gap],
			ln->size - (ln->cur + ln->gap));

	// Free old buffer
	free(ln->buf);
	ln->buf = newbuf;
	ln->size = newsize;
	ln->gap = newgap;
}


void line_addch(hLine line, char c)
{
	SL(ln, line);
        assert(ln && ln->buf);
        if(ln->gap < 1) {
		line_add_gap(line);
        }
        ln->buf[ln->cur] = c;
        ln->cur++;
        ln->gap--;
        return;
}


void line_delch(hLine line)
{
	SL(ln, line);
        assert(ln && ln->buf);
        if(ln->cur < 1) {
                return;
        }
        ln->cur--;
        ln->gap++;
        if(ln->cur + ln->gap > ln->size) {
		printf("ERROR: FIX THIS!\n");
                ln->gap--;
        }
}


void line_move_cur_left(hLine line)
{
	SL(ln, line);
        assert(ln);
        if(ln->cur == 0) {
                return;
        }
        ln->buf[(ln->cur + ln->gap) - 1] = ln->buf[ln->cur - 1];
        ln->cur--;
}



void line_move_cur_right(hLine line)
{
	SL(ln, line);
        assert(ln);
        if(ln->cur + ln->gap  == ln->size) {
		printf("EOB!\n");
                return;
        }
        ln->buf[ln->cur] = ln->buf[ln->cur + ln->gap];
        ln->cur++;
}


int line_get_cursor(hLine line)
{
	SL(ln, line);
        assert(ln);
        return ln->cur;
}


void line_pprint(hLine line)
{
	SL(ln, line);
        assert(ln);
        printf("Line{size=%d, cur=%d, gap=%d}\n",
			ln->size, ln->cur, ln->gap);
}


void line_print(hLine line)
{
	SL(ln, line);
        assert(ln);
        for(unsigned int i = 0; i < ln->size; ++i) {
                if(i >= ln->cur && i < ln->cur + ln->gap) {
                        printf("-");
                } else {
                        if(ln->buf[i])
                                printf("%c", ln->buf[i]);
                        else
                                printf(".");
                }
        }
        printf("\n");
}
