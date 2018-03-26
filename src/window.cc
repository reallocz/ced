#include "window.h"
#include "draw.h"
#include "log.h"
#include <assert.h>
#include <locale.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAG "WINDOW"
#define STATUSLINE_HEIGHT 1
#define CMDLINE_HEIGHT 1

/** return a new id for a window */
static unsigned int generate_id()
{
    static unsigned int ids = 0;
    return ids++;
}

struct window* win_create(struct buffer_view* bview)
{
    struct window* w = (struct window*) malloc(sizeof(struct window));
    assert(w);

    w->id = generate_id();

    // ncurses
    w->nwin = newwin(0, 0, 0, 0);
    assert(w->nwin);
    keypad(w->nwin, TRUE);

    // Buffer view
    w->bview = bview;

    log_l(TAG, "Window created:");
    win_pprint(w);
    return w;
}


void win_destroy(struct window* win)
{
    delwin(win->nwin);
    log_l(TAG, "Window destroyed: %d", win->id);
}


void win_setbview(struct window* win, struct buffer_view* bv)
{
    assert(win && bv);
    win->bview = bv;
}


void win_update(struct window* win, const Context& context)
{
    struct buffer_view* bv = win->bview;
    // statusline
    win->sline.bufname = bv->buffer.name;
    win->sline.cur     = bv->cur;
    win->sline.gap     = bv->buffer.gap;

    // Margin
    win->margin.width     = 3;
    win->margin.start     = bv_start(bv);
    win->margin.linecount = bv->buffer.linecount;

    // buffer
    // Update bview bounds
    struct rect area     = context.bounds;
    struct rect bvbounds = RECT(0, win->margin.width + 1,
                                area.width - win->margin.width,
                                area.height - STATUSLINE_HEIGHT - CMDLINE_HEIGHT);
    bv_bounds_set(bv, bvbounds);
    bv_update(bv);
}


void win_draw(const struct window* win, const Context& context)
{
    struct rect area = context.bounds;
    // Stl
    struct rect areastl = RECT(area.height - STATUSLINE_HEIGHT - CMDLINE_HEIGHT, 0,
                               area.width, STATUSLINE_HEIGHT);
    draw_statusline(win->nwin, win->sline, areastl, context);

    // Margin
    struct rect areamgn = RECT(0, 0, win->margin.width, area.height - STATUSLINE_HEIGHT - CMDLINE_HEIGHT);
    draw_margin(win->nwin, win->margin, areamgn, context);

    // Cmdline
    struct rect areacmd = RECT(area.height - CMDLINE_HEIGHT, 0,
                               area.width, CMDLINE_HEIGHT);
    draw_cmdline(win->nwin, win->cmdline, areacmd, context);

    // Bufferview
    draw_bview(win->nwin, win->bview, context);

    // Draw cursor
    if (context.mode == Mode::Normal || context.mode == Mode::Insert) {
        // Cursor in buffer_view
        struct cursor c    = bv_relcur(win->bview);
        struct rect areabv = bv_bounds(win->bview);
        wmove(win->nwin, areabv.y + c.line, areabv.x + c.col);
    } else if (context.mode == Mode::Command) {
        int x = strlen(win->cmdline.buffer) + 1;    // +1 for ':' prefix
        wmove(win->nwin, areacmd.y, x);
    } else {
        // ERROR
        wmove(win->nwin, 0, 0);
    }

    wrefresh(win->nwin);
}


void win_pprint(struct window* win)
{
    log_l(TAG, "Window {id:%d}", win->id);
}
