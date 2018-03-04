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
#include "defaults.h"


static struct {
	const char* tag;
	enum ced_mode mode;
	int quit; // Return to main if 1

	hWindow focussed_window;
} G;



void ced_run()
{
	// Initialize globals
	G.tag = "CED";
	G.quit = 0;
	G.focussed_window = WINDOW_DEFID;

	// Create window and set buffer
	hWindow window = win_create(0, 0, 0, 0);
	hBuffer buffer = buf_create(SCRATCH, term_rows());
	buf_pprint(buffer);
	win_set_buffer(window, buffer);
	ced_set_window_focus(window);

	struct win_props props = win_get_props(window);
	// Start in normal mode
	ced_set_mode(MODE_NORMAL);
	// Set cursor to text area
	win_set_cursor(window, 0, props.mwidth);

	// Main loop
	while(!G.quit) {
		ced_draw_statusline(G.focussed_window, G.mode);
		ced_draw_numberline(G.focussed_window, G.mode);
		inp_poll();
		wrefresh(win_nwin(window));
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
	static int x = 0, y = 0;
	WINDOW* nwin = win_nwin(ev.win);
	struct win_props props = win_get_props(ev.win);
	if(ev.type == INSERT) {
		mvwaddch(nwin, y, props.mwidth + x++, ev.data.ch);
	} else if (ev.type == DELETE) {
		win_set_cursor(ev.win, 0, props.mwidth + --x);
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


void ced_draw_statusline(hWindow win, enum ced_mode mode)
{
	WINDOW* w = win_nwin(win);
	// Save cursor position
	int curx, cury;
	win_get_cursor(win, &cury, &curx);

	struct win_props props = win_get_props(win);
	win_set_cursor(win, props.sy, props.sx);
	whline(w, ACS_HLINE, props.swidth);
	if(G.mode == MODE_NORMAL) {
		mvwaddstr(w, props.sy + 1, 0, "MODE: NORMAL");
	} else {
		mvwaddstr(w, props.sy + 1, 0, "MODE: INSERT");
	}
	// Go back to original positions
	win_set_cursor(win, cury, curx);
	return;
}


void ced_draw_numberline(hWindow win, enum ced_mode mode)
{
	WINDOW* w = win_nwin(win);

	struct win_props props = win_get_props(win);

	int curx, cury;
	win_get_cursor(win, &cury, &curx);

	wattron(w, A_BOLD);
	char lnstr[props.mwidth]; // line number to string
	for(unsigned int i = 0; i < props.mheight; ++i) {
		win_set_cursor(win, i, 0);
		if(i < 10) {
			sprintf(lnstr, " %d", i);
		} else if(i < 100) {
			sprintf(lnstr, "%d", i);
		} else {
			sprintf(lnstr, "%d", i);
		}
		waddstr(w, lnstr);
	}
	wattroff(w, A_BOLD);
	win_set_cursor(win, cury, curx);
}
