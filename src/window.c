#include "window.h"
#include <locale.h>
#include <ncurses.h>
#include <stdio.h>
#include <assert.h>
#include "log.h"
#include <stdlib.h>

#define TAG "WINDOW"
#define STATUSLINE_HEIGHT 2

/** return a new id for a window */
static unsigned int generate_id()
{
    static unsigned int ids = 0;
    return ids++;
}

struct window* win_create(struct buffer* buffer)
{
    assert(buffer);

    struct window* w = malloc(sizeof(struct window));
    assert(w);

    w->id = generate_id();

    // ncurses
    w->nwin = newwin(0, 0, 0, 0);
    assert(w->nwin);
    keypad(w->nwin, TRUE);

    // Buffer view
    w->bview = bview_create(buffer, 0, 10); // TODO

    // Statusline
    w->stl.bufname = "TODO-BUFNAME";
    w->stl.mode = "TODO-MODE";
    w->stl.row = 0;
    w->stl.col = 0;

    // Margin
    w->mgn.width = 3;

    // Cursor
    w->curx = w->cury = 0;
    log_l(TAG, "Window created: id: %d", w->id);
    return w;
}


void win_destroy(struct window* win)
{
    delwin(win->nwin);
    log_l(TAG, "Window destroyed: %d", win->id);
}


static
void draw_stl(const struct window* win, const char* mode,
        unsigned y, unsigned int width)
{
    // statusline string
    char stsstring[width];
    // TODO
    unsigned int row = 0;
    unsigned int lc = buf_line_count(win->bview.buffer);
    unsigned int col = 0;
    unsigned int cur = 0;
    const char* bufname = win->bview.buffer->name;
    sprintf(stsstring,
            "   %s | %s | row:%d/%d, col: %d | cur: %d "
            "| gap - pos: %d, len: %d ",
            mode, bufname, row, lc, col, cur,
            0, 0);

    init_pair(1, COLOR_BLACK, COLOR_WHITE);
    wattron(win->nwin, COLOR_PAIR(1));
    wmove(win->nwin, y, 0);
    wclrtoeol(win->nwin);
    waddstr(win->nwin, stsstring);
    wattroff(win->nwin, COLOR_PAIR(1));
}

static
void draw_margin(const struct window* win, unsigned int height)
{
    wattron(win->nwin, A_BOLD);
    unsigned int linecount = buf_line_count(win->bview.buffer);

    int txtpadding = 1;
    // line number to string
    char lnstr[win->mgn.width + txtpadding];
    for(unsigned int i = 0; i < height; ++i) {
        wmove(win->nwin, i, 0);
        if(i < linecount) {
            sprintf(lnstr, "%3d", i + 1);  // 1 - indexed
            waddstr(win->nwin, lnstr);
        } else {
            sprintf(lnstr, "%3c", '~');
            waddstr(win->nwin, lnstr);
        }
    }
    wattroff(win->nwin, A_BOLD);
}


void win_draw(const struct window* win, const char* mode,
        unsigned int y, unsigned int x,
        unsigned int height, unsigned int width)
{
    unsigned int linecount = buf_line_count(win->bview.buffer);
    // Saved window cursor positions
    unsigned int curx = win->curx;
    unsigned int cury = win->cury;

    draw_stl(win, mode, height - STATUSLINE_HEIGHT, width);
    draw_margin(win, height - STATUSLINE_HEIGHT);

    bview_draw(win->nwin, win->bview, 0, win->mgn.width, height - STATUSLINE_HEIGHT, width - win->mgn.width);

    wrefresh(win->nwin);
    // Move cursor to roiginal pos
    wmove(win->nwin, cury, curx);
}


void win_pprint(struct window* win)
{
    log_lc(TAG, "Window {id:%d, curx:%d, cury: %d", win->id,
            win->curx, win->cury);
}
