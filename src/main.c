#include <stdio.h>
#include <ncurses.h>
#include <locale.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include "log.h"
#include "window.h"
#include "input.h"
#include "line.h"
#include "menu.h"

enum main_mode {
	DEBUG,
	NORMAL
};

static struct {
	const char* tag;
	int quit;	// Break out of mainloop if 1
} G;

/** Initialize submodules */
void init()
{
	G.tag = "MAIN";
	G.quit = 0;

	/// submodules
	// Logs init first
	log_init();
	buf_init();
	inp_init();
	line_init();
	menu_init();
	win_init();
}

/** Cleanup submodules */
void onexit()
{
	inp_exit();
	line_exit();
	menu_exit();
	buf_exit();
	win_exit();
	// Logs exit last
	log_exit();
}

void printmenu(hWindow win)
{
	menu_init();
	hMenu mu = menu_create(QUICK);
	menu_addopt(mu, 1, "Hello");
	menu_addopt(mu, 2, "World!");
	int res = menu_prompt(mu, win);
}


/** input handler callback */
void main_normal_input_handler(inpev ev)
{
	static int x= 0, y = 0;
	WINDOW* nwin = win_getnwin(ev.win);

	if(ev.type == INSERT) {
		mvwaddch(nwin, y, x++, ev.data.ch);
	} else if (ev.type == DELETE) {
		wmove(nwin, 0, --x);
		wdelch(nwin);
	} else if (ev.type == MOVE) {
		x = 0;
		wmove(nwin, ++y, x);
	} else if (ev.type == QUIT) {
		G.quit = 1;
	}
}


// Draw bottom status line
void drawstatus(hWindow win)
{
	WINDOW* w = win_getnwin(win);
	int x, y, rows, cols;
	win_getcur(win, &y, &x);
	win_getsize(win, &rows, &cols);
	wmove(w, rows - 2, 0);
	log_l(G.tag, "moving row:%d->%d, col:%d", y, rows - 2, x);
	log_l(G.tag, "Drawing status row=%d, col=%d", rows, cols);
	whline(w, ACS_HLINE, cols);
	wrefresh(w);
	wmove(w, y, x);
	return;
}


void mainloop(hWindow win)
{
	inpev ev;
	while(!G.quit) {
		inp_poll();
		drawstatus(win);
		wrefresh(win_getnwin(win));
	}
}


int main()
{
	atexit(onexit);

	init();
	// Create window and set buffer
	hWindow w = win_create(0, 0, 0, 0);
	hBuffer b = buf_create(SCRATCH);
	win_setbuffer(w, b);
	// set input handler
	inp_sethandler(w, main_normal_input_handler);

	/*printmenu(w);*/
	mainloop(w);
}
