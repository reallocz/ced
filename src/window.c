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
        WINDOW* nwin;
	int x;
	int y;
        int rows;
        int cols;
        int curx;
        int cury;
};

static struct {
	const char* tag;
	// Base window(stdout);
	struct window stdscr;
	int wincount;
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

        /// Initialize ncurses
        setlocale(LC_ALL, ""); // Required
        G.stdscr.nwin = initscr();
	getmaxyx(G.stdscr.nwin, G.stdscr.rows, G.stdscr.cols);
	log_l(G.tag, "STDSCR: rows=%d, cols=%d",
			G.stdscr.rows, G.stdscr.cols);

        /// Set char buffered instead of line buffered input
        cbreak();
        noecho();       // Don't echo input on terminal
        keypad(G.stdscr.nwin, TRUE);
}


void win_exit()
{
        endwin();
}

hWindow win_create(int y, int x, int rows, int cols)
{
	SW(w, G.wincount);

	w->id = G.wincount;
	w->flags = 0;
        w->nwin = NULL;
        w->rows = rows;
        w->cols = cols;
	w->y = y;
	w->x = x;
        w->cury = 0;
        w->curx = 0;

        w->nwin = newwin(w->rows, w->cols, w->y, w->x);
        assert(w->nwin);
        keypad(w->nwin, TRUE);

	G.wincount++;
        return w->id;
}

WINDOW* win_getnwin(hWindow win)
{
	SW(w, win);
        return w->nwin;
}


void win_pprint(hWindow win)
{
	SW(w, win);
	log_l(G.tag, "Window {id=%d, y=%d, x=%d, rows=%d, cols=%d}", w->id, w->y, w->x, w->rows, w->cols);
}
