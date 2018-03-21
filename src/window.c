#include "window.h"
#include <assert.h>
#include <locale.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include "draw.h"
#include "log.h"

#define TAG "WINDOW"
#define STATUSLINE_HEIGHT 2

/** return a new id for a window */
static unsigned int generate_id()
{
    static unsigned int ids = 0;
    return ids++;
}

struct window* win_create(struct buffer_view* bview)
{
    struct window* w = malloc(sizeof(struct window));
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

void win_update(struct window* win, struct context* context)
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
    struct rect area     = context->bounds;
    struct rect bvbounds = RECT(0, win->margin.width + 1,
                                area.width - win->margin.width,
                                area.height - STATUSLINE_HEIGHT);
    bv_bounds_set(bv, bvbounds);
    bv_update(bv);
}


void win_draw(const struct window* win, const struct context* context)
{
    struct rect area = context->bounds;
    // Stl
    struct rect areastl = RECT(area.height - STATUSLINE_HEIGHT, 0,
                               area.width, 1);
    draw_statusline(win->nwin, win->sline, areastl, context);

    // Margin
    struct rect areamgn = RECT(0, 0, win->margin.width, area.height - STATUSLINE_HEIGHT);
    draw_margin(win->nwin, win->margin, areamgn, context);

    // Bufferview
    draw_bview(win->nwin, win->bview, context);

    wrefresh(win->nwin);
}


void win_pprint(struct window* win)
{
    log_l(TAG, "Window {id:%d}", win->id);
}
