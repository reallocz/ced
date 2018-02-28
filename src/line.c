#include "line.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define LN_DEFSIZE 8
#define LN_DEFGAP 4
#define LN_DEFCAP 64

struct line {
	unsigned int id;
        unsigned int num;// Line number
        char* buf;// buffer
        unsigned int size;// line size
        unsigned int cur;// Cursor
        unsigned int gap;// Gap buf len
};


static struct {
	int flags;
	const char* tag;
	unsigned int linecount;
	unsigned int linecap;
	struct line* lines;
} G;


int line_init()
{
	G.tag = "LINE";
	G.flags = 0;
	G.linecount = 0;
	G.linecap = LN_DEFCAP;
	G.lines = malloc(G.linecap * sizeof(struct line));
	assert(G.lines);
	return 0;
}

int line_exit()
{
	free(G.lines);
	return 0;
}


/** Return the pointer to the line */
static struct line* _get_line(hLine line)
{
	if(line >= G.linecap) {
		printf("INVALID HLINE!\n");
		return NULL;
	}
	return &G.lines[line];
}


hLine line_create(void)
{
	if(G.linecount >= G.linecap) {
		printf("cnt %d, cap %d\n", G.linecount, G.linecap);
		printf("ERROR: ALLOC MORE LINES!\n");
	}

        struct line* ln = &G.lines[G.linecount];
	ln->id = G.linecount;

        ln->num = 0;
        ln->buf = NULL;
        ln->size = LN_DEFSIZE;
        ln->cur = 0;
        ln->gap = LN_DEFGAP;

        // Allocate memory for buffer
        ln->buf = calloc(ln->size, sizeof(ln->buf[0]));
        assert(ln->buf);

	G.linecount++;
        return ln->id;
}


void line_destroy(hLine ln)
{
	// TODO
}


void line_add_gap(hLine line)
{
	struct line* ln = _get_line(line);
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
	struct line* ln = _get_line(line);
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
	struct line* ln = _get_line(line);
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
	struct line* ln = _get_line(line);
        assert(ln);
        if(ln->cur == 0) {
                return;
        }
        ln->buf[(ln->cur + ln->gap) - 1] = ln->buf[ln->cur - 1];
        ln->cur--;
}



void line_move_cur_right(hLine line)
{
	struct line* ln = _get_line(line);
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
	struct line* ln = _get_line(line);
        assert(ln);
        return ln->cur;
}


void line_pprint(hLine line)
{
	struct line* ln = _get_line(line);
        assert(ln);
        printf("Line{size=%d, cur=%d, gap=%d}\n",
			ln->size, ln->cur, ln->gap);
}


void line_print(hLine line)
{
	struct line* ln = _get_line(line);
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
