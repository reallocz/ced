#include <stdio.h>
#include <ncurses.h>
#include <locale.h>
#include <assert.h>
#include <unistd.h>
#include "log.h"
#include "window.h"
#include "input.h"
#include "line.h"
#include "menu.h"

/** Initialize submodules */
void init()
{
	// Logs init first
	log_init();
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


/** Process each keystroke */
void process_input(hWindow win, inpev ev)
{

	static int x= 0, y = 0;
	WINDOW* nwin = win_getnwin(win);

	if(ev.type == INSERT) {
		mvwaddch(nwin, y, x++, ev.data.ch);
	} else if (ev.type == DELETE) {
		wmove(nwin, 0, --x);
		wdelch(nwin);
	} else if (ev.type == MOVE) {
		x = 0;
		wmove(nwin, ++y, x);
	}
}


void mainloop(hWindow win)
{
	inpev ev;
	while(1) {
		ev = inp_poll(win);
		if(ev.type == QUIT)
			break;
		process_input(win, ev);
		wrefresh(win_getnwin(win));
	}
}


int main()
{
	init();
	hWindow w = win_create(0, 0, 0, 0);
	printmenu(w);
	mainloop(w);
	onexit();
}
