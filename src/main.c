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

	// init submodules
	// Logs init first
	log_init();
	log_l("MAIN", "\n--------INIT BEGIN-------");
	term_init();
	buf_init();
	inp_init();
	line_init();
	menu_init();
	win_init();
	log_l("MAIN", "\n---------INIT END--------\n");

	// Run program
	ced_run();
}

