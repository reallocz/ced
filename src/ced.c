#include "ced.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "log.h"
#include "window.h"
#include "input.h"
#include "buffer.h"
#include "line.h"
#include "term.h"
#include "input_keys.h"


static struct {
	const char* tag;
	enum ced_mode mode;
	int quit; // Return to main if 1
} G;

// Draw bottom status line
void drawstatus(hWindow win)
{
	WINDOW* w = win_getnwin(win);
	int x, y, rows, cols;
	win_getcur(win, &y, &x);
	win_getsize(win, &rows, &cols);
	wmove(w, rows - 2, 0);
	whline(w, ACS_HLINE, cols);
	if(G.mode == MODE_NORMAL) {
		mvwaddstr(w, rows -1 , 0, "MODE: NORMAL");
	} else {
		mvwaddstr(w, rows -1 , 0, "MODE: INSERT");
	}
	wrefresh(w);
	// Go back to original positions
	wmove(w, y, x);
	return;
}


void ced_run()
{
	// Initialize globals
	G.tag = "CED";
	G.quit = 0;

	// Create window and set buffer
	hWindow window = win_create(0, 0, 0, 0);
	hBuffer buffer = buf_create(SCRATCH, 4);
	win_setbuffer(window, buffer);

	// Start in normal mode
	ced_set_mode(window, MODE_NORMAL);

	// Main loop
	while(!G.quit) {
		inp_poll();
		drawstatus(window);
		wrefresh(win_getnwin(window));
	}
}


void ced_set_mode(hWindow win, enum ced_mode mode)
{
	G.mode = mode;
	const char* modename;

	switch(G.mode) {
	case MODE_NORMAL:
		inp_set_handler(win, ced_normal_input_handler);
		modename = "MODE_NORMAL";
		break;
	case MODE_INSERT:
		inp_set_handler(win, ced_insert_input_handler);
		modename = "MODE_INSERT";
		break;
	case MODE_META:
		inp_set_handler(win, ced_meta_input_handler);
		modename = "MODE_META";
		break;
	default:
		log_fatal(G.tag, "%s: Unknown mode %d",
				__func__, mode);
	}

	log_l(G.tag, "Mode set: %s", modename);
}


void ced_insert_input_handler(inpev ev)
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
	} else if (ev.key == k_esc) {
		ced_set_mode(ev.win, MODE_NORMAL);
	}
}


void ced_normal_input_handler(inpev ev)
{
	if(ev.key == 'i') {
		// Switch to insert mode
		ced_set_mode(ev.win, MODE_INSERT);
		return;
	} else if (ev.key == k_f1) {
		G.quit = 1;
	}
}


void ced_meta_input_handler(inpev ev)
{
}
