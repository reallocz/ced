#include "window.h"
#include <locale.h>
#include <ncurses.h>
#include <stdio.h>
#include <assert.h>
#include "log.h"
#include <stdlib.h>

#define WINDOW_DEFCAP 4

struct window {
	int id;
	int flags;
        WINDOW* nwin;
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


void win_init()
{
	/// Initialize globals
	G.tag = "WINDOW";
	G.wincount = 0;

        /// Initialize ncurses
        setlocale(LC_ALL, ""); // Required
        G.stdscr.nwin = initscr();

        /// Set char buffered instead of line buffered input
        cbreak();
        noecho();       // Don't echo input on terminal
        keypad(G.stdscr.nwin, TRUE);
}

hWindow win_create()
{
        struct window* w = &G.windows[G.wincount];

	w->id = 0;
	w->flags =0;
        w->nwin = NULL;
        w->rows = 0;
        w->cols = 0;
        w->curx = 0;
        w->cury = 0;

        w->nwin = newwin(0,0,0,0);
        assert(w->nwin);
        keypad(w->nwin, TRUE);

	G.wincount++;
        return w->id;
}

WINDOW* win_getnwin(hWindow w)
{
        return G.windows[w].nwin;
}

void win_exit()
{
        endwin();
}
