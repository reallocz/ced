#include "line.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "log.h"
#include "defaults.h"

#define LN_DEFSIZE 8
#define LN_DEFGAP 4
#define LN_DEFCAP 64

/** Bring struct line* alias of id x into scope */
#define SL(alias, hLine) \
	struct line* alias = _get_line(hLine)

struct line {
	unsigned int id;
        unsigned int num;	// Line number
        char* buf;		// buffer
        unsigned int size;	// line size
        unsigned int cur;	// Cursor
        unsigned int gap;	// Gap size
};


static struct {
	int flags;
	unsigned int ids; // Generate id's by incrementing
	const char* tag;
	unsigned int linecap;
	struct line* lines;
} G;


int line_init()
{
	G.tag = "LINE";
	G.flags = 0;
	G.linecap = LN_DEFCAP;
	G.ids = 0;
	G.lines = malloc(G.linecap * sizeof(struct line));
	assert(G.lines);

	for(unsigned int i = 0; i < G.linecap; ++i) {
		G.lines[i].id = LINE_DEFID;
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


/** Return the pointer to the line */
static struct line* _get_line(hLine line)
{
	for(unsigned int i = 0; i < G.linecap; ++i) {
		if(G.lines[i].id == line) {
			return &G.lines[i];
		}
	}
	log_e(G.tag, "%s: line not found: %d", __func__,
			line);
	exit(1);
}


/** Generate a new id for line */
static
unsigned int _generate_id()
{
	return ++G.ids;
}


static
struct line* _get_free_line()
{
	for(unsigned int i = 0; i < G.linecap; ++i) {
		if(G.lines[i].id == LINE_DEFID) {
			/*log_l(G.tag, "%s: found free line at: %d", __func__, i);*/
			return &G.lines[i];
		}
	}
	log_e(G.tag, "%s: OUT OF LINES!", __func__);
	exit(1);
}


hLine line_create(void)
{
        struct line* ln = _get_free_line();

	ln->id = _generate_id();
        ln->num = 0;
        ln->buf = NULL;
        ln->size = LN_DEFSIZE;
        ln->cur = 0;
        ln->gap = LN_DEFGAP;

        // Allocate memory for buffer
        ln->buf = calloc(ln->size, sizeof(ln->buf[0]));
        assert(ln->buf);

        return ln->id;
}


void line_destroy(hLine line)
{
	SL(ln, line);
	ln->id = LINE_DEFID;
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
