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

	hWindow focussed_window;
} G;

// Draw bottom status line
void drawstatus(hWindow win)
{
	WINDOW* w = win_getnwin(win);
	int x, y, rows, cols;
	win_get_cursor(win, &y, &x);
	win_getsize(win, &rows, &cols);
	win_set_cursor(win, rows - 1, 0);
	whline(w, ACS_HLINE, cols);
	if(G.mode == MODE_NORMAL) {
		mvwaddstr(w, rows, 0, "MODE: NORMAL");
	} else {
		mvwaddstr(w, rows, 0, "MODE: INSERT");
	}
	// Go back to original positions
	win_set_cursor(win, y, x);
	return;
}


void drawnumbers(hWindow win)
{
	WINDOW* w = win_getnwin(win);
	hBuffer buf = win_get_buffer(win);
	unsigned int lcount = buf_get_linecount(buf);
	int curx, cury;
	win_get_cursor(win, &cury, &curx);
	char lnstr[3];
	for(unsigned int i = 0; i < lcount; ++i) {
		win_set_cursor(win, i, 0);
		sprintf(lnstr, "%d", i);
		waddstr(w, lnstr);
	}
	win_set_cursor(win, cury, curx);
}

void ced_run()
{
	// Initialize globals
	G.tag = "CED";
	G.quit = 0;
	G.focussed_window = -1;

	// Create window and set buffer
	hWindow window = win_create(0, 0, 0, 0);
	hBuffer buffer = buf_create(SCRATCH, term_rows());
	buf_pprint(buffer);
	win_set_buffer(window, buffer);
	ced_set_window_focus(window);

	// Start in normal mode
	ced_set_mode(MODE_NORMAL);

	// Main loop
	while(!G.quit) {
		drawstatus(window);
		drawnumbers(window);
		inp_poll();
		wrefresh(win_getnwin(window));
	}
}


void ced_set_mode(enum ced_mode mode)
{
	G.mode = mode;
	const char* modename;

	switch(G.mode) {
	case MODE_NORMAL:
		inp_set_handler(G.focussed_window, ced_normal_input_handler);
		modename = "MODE_NORMAL";
		break;
	case MODE_INSERT:
		inp_set_handler(G.focussed_window, ced_insert_input_handler);
		modename = "MODE_INSERT";
		break;
	case MODE_META:
		inp_set_handler(G.focussed_window, ced_meta_input_handler);
		modename = "MODE_META";
		break;
	default:
		log_fatal(G.tag, "%s: Unknown mode %d",
				__func__, mode);
	}

	log_l(G.tag, "Mode set: %s", modename);
}


void ced_set_window_focus(hWindow win)
{
	log_l(G.tag, "Window focus set %d -> %d",
			G.focussed_window, win);
	G.focussed_window = win;
}


void ced_insert_input_handler(inpev ev)
{
	static int x= 0, y = 0;
	WINDOW* nwin = win_getnwin(ev.win);

	if(ev.type == INSERT) {
		mvwaddch(nwin, y, x++, ev.data.ch);
	} else if (ev.type == DELETE) {
		win_set_cursor(ev.win, 0, --x);
		wdelch(nwin);
	} else if (ev.type == MOVE) {
		x = 0;
		win_set_cursor(ev.win, ++y, x);
	} else if (ev.key == k_esc) {
		ced_set_mode(MODE_NORMAL);
	}
}


void ced_normal_input_handler(inpev ev)
{
	if(ev.key == 'i') {
		// Switch to insert mode
		ced_set_mode(MODE_INSERT);
		return;
	} else if (ev.key == k_f1) {
		G.quit = 1;
	}
}


void ced_meta_input_handler(inpev ev)
{
}
