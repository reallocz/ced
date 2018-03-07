#include "ced.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "log.h"
#include "window.h"
#include "input.h"
#include "buffer.h"
#include "term.h"
#include "input_keys.h"


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
    G.focussed_window = INVALID_WINDOW;

    // Create window and set buffer
    hWindow window = win_create(0, 0, 0, 0);
    hBuffer buffer = buf_create(SCRATCH);
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
        win_draw_buffer(window);
        win_draw_refresh(window);
        inp_poll();
    }
}


void ced_set_mode(enum ced_mode mode)
{
    G.mode = mode;
    const char* modename;
    struct inp_handler handler;
    handler.window = G.focussed_window;

    switch(G.mode) {
        case MODE_NORMAL:
            modename = "MODE_NORMAL";
            handler.name = modename;
            handler.callback = ced_normal_input_cb;
            break;
        case MODE_INSERT:
            modename = "MODE_INSERT";
            handler.name = modename;
            handler.callback = ced_insert_input_cb;
            break;
        default:
            log_fatal(G.tag, "%s: Unknown mode %d",
                    __func__, mode);
    }

    inp_set_handler(handler);
    log_l(G.tag, "Mode set: %s", modename);
}


void ced_set_window_focus(hWindow win)
{
    log_l(G.tag, "Window focus set %d -> %d",
            G.focussed_window, win);
    G.focussed_window = win;
}


void ced_insert_input_cb(inpev ev)
{
    hBuffer buf = win_get_buffer(G.focussed_window);

    if(ev.type == INP_ALPHA || ev.type == INP_NUM
            || ev.type == INP_SYMBOL || ev.key == k_space)
    {
        buf_addch(buf, ev.key);
    }

    if(ev.type == INP_SPECIAL)
    {
        switch(ev.key) {
        case k_enter:
            // TODO
            break;
        case k_backspace:
            buf_delch(buf);
            break;
        case k_esc:
            ced_set_mode(MODE_NORMAL);
            break;
        default:
            break;
        }
    }
}


void ced_normal_input_cb(inpev ev)
{
    if(ev.key == 'i') {
        // Switch to insert mode
        ced_set_mode(MODE_INSERT);
        return;
    } else if (ev.key == k_f1) {
        G.quit = 1;
    }
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
    if(mode == MODE_NORMAL) {
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

