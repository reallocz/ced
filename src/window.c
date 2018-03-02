#include "window.h"
#include <locale.h>
#include <ncurses.h>
#include <stdio.h>
#include <assert.h>
#include "log.h"
#include <stdlib.h>

#define WINDOW_DEFCAP 4

/** Bring struct window* alias of id x into scope */
#define SW(alias, hWindow) \
	struct window* alias = _get_win(hWindow)

struct window {
	int id;
	int flags;

	// Curses
        WINDOW* nwin;
	int x;
	int y;
        int rows;
        int cols;

	// Buffers
	hBuffer buf;	// -1 if no buffer set
};


static struct {
	const char* tag;
	unsigned int wincount;
	struct window windows[WINDOW_DEFCAP];
} G;


static struct window* _get_win(hWindow win)
{
	if(win >= G.wincount && win != 0) {
		log_e(G.tag, "%s: invalid window - %d",
				__func__, win);
		exit(1);
	}
	return &G.windows[win];
}


void win_init()
{
	/// Initialize globals
	G.tag = "WINDOW";
	G.wincount = 0;
	log_l(G.tag, "Init success");
}


void win_exit()
{
}


hWindow win_create(int y, int x, int rows, int cols)
{
	SW(w, G.wincount);

	w->id = G.wincount;
	w->flags = 0;

        w->nwin = NULL;
        w->nwin = newwin(rows, cols, y, x);
        assert(w->nwin);
	int width, height;
	getmaxyx(w->nwin, width, height);
	w->rows = width - 1;	// rows are 0 indexed
	w->cols = height - 1;	// cols are 0 indexed
	w->y = y;
	w->x = x;

	w->buf = -1;

        keypad(w->nwin, TRUE);
	G.wincount++;
        return w->id;
}


void win_destory(hWindow win)
{
	// TODO
}


int win_setbuffer(hWindow win, hBuffer buf)
{
	SW(w, win);
	log_l(G.tag, "%s: hWindow=%d, buf=%d->%d", __func__,
			win, w->buf, buf);
	w->buf = buf;
	return 0;
}


// Accessors

WINDOW* win_getnwin(hWindow win)
{
	SW(w, win);
        return w->nwin;
}


void win_getsize(hWindow win, int* rows, int* cols)
{
	SW(w, win);
	*rows = w->rows;
	*cols = w->cols;
}


void win_get_cursor(hWindow win, int* y, int* x)
{
	SW(w, win);
	int my, mx;
	getyx(w->nwin, my, mx);
	*y = my;
	*x = mx;
}

void win_set_cursor(hWindow win, int y, int x)
{
	SW(w, win);
	wmove(w->nwin, y, x);
}

hBuffer win_getbuffer(hWindow win)
{
	SW(w, win);
	return w->buf;
}


void win_pprint(hWindow win)
{
	SW(w, win);
	log_l(G.tag, "Window {id=%d, y=%d, x=%d, rows=%d, cols=%d}", w->id, w->y, w->x, w->rows, w->cols);
}
