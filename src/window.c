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
    w->bview = bview_create(buffer, 0, 3); // TODO

    // Statusline
    w->stl.bufname = "TODO-BUFNAME";
    w->stl.mode = "TODO-MODE";
    w->stl.row = 0;
    w->stl.col = 0;

    // Margin
    w->mgn.width = 3;

    log_l(TAG, "Window created: id: %d", w->id);
    win_pprint(w);
    return w;
}


void win_destroy(struct window* win)
{
    delwin(win->nwin);
    log_l(TAG, "Window destroyed: %d", win->id);
}


static
void draw_stl(const struct window* win, const char* mode,
        struct rect area)
{
    // statusline string
    char stsstring[area.width];
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
    wmove(win->nwin, area.y, 0);
    wclrtoeol(win->nwin);
    waddstr(win->nwin, stsstring);
    wattroff(win->nwin, COLOR_PAIR(1));
}

static
void draw_margin(const struct window* win, struct rect area)
{
    wattron(win->nwin, A_BOLD);
    unsigned int linecount = buf_line_count(win->bview.buffer);

    int txtpadding = 1;
    // line number to string
    char lnstr[win->mgn.width + txtpadding];
    for(unsigned int i = 0; i < area.height; ++i) {
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


void win_draw(const struct window* win, const char* mode, struct rect area)
{
    // Stl
    struct rect areastl = RECT(area.height - STATUSLINE_HEIGHT,0,
            area.width, 1);
    draw_stl(win, mode, areastl);

    // Margin
    struct rect areamgn = RECT(0, 0, win->mgn.width, area.height - STATUSLINE_HEIGHT);
    draw_margin(win, areamgn);

    // Bufferview
    struct rect areabv = RECT(0, win->mgn.width + 1,
            area.width - win->mgn.width,
            area.height - STATUSLINE_HEIGHT);
    bview_draw(win->bview, win->nwin, areabv);

    wrefresh(win->nwin);
}


void win_pprint(struct window* win)
{
    log_lc(TAG, "Window {id:%d}", win->id);
}
