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
#include "term.h"
#include "ced.h"

static struct {
	const char* tag;
} G;


/** Initialize submodules */
void init_submodules()
{
	G.tag = "MAIN";

	// Logs init first
	log_init();
	log_l(G.tag, "\n--------INIT BEGIN-------");
	/// submodules
	term_init();
	buf_init();
	inp_init();
	line_init();
	menu_init();
	win_init();
	log_l(G.tag, "\n---------INIT END--------\n");
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
	term_exit();
	log_exit();
}


int main()
{
	atexit(onexit);
	init_submodules();
	ced_run();
}


void printmenu(hWindow win)
{
	menu_init();
	hMenu mu = menu_create(QUICK);
	menu_addopt(mu, 1, "Hello");
	menu_addopt(mu, 2, "World!");
	int res = menu_prompt(mu, win);
}
