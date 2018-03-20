#include "window.h"
#include <locale.h>
#include <ncurses.h>
#include <stdio.h>
#include <assert.h>
#include "log.h"
#include "draw.h"
#include <stdlib.h>

#define TAG "WINDOW"
#define STATUSLINE_HEIGHT 2

/** return a new id for a window */
static unsigned int generate_id()
{
    static unsigned int ids = 0;
    return ids++;
}

struct window* win_create(struct buffer_view bview)
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


void win_update(struct window* win, struct context *context)
{
    // statusline
    win->sline.bufname = win->bview.buffer.name;
    win->sline.cur = win->bview.cur;
    win->sline.gap = win->bview.buffer.gap;

    // Margin
    win->margin.width = 3;
    win->margin.start = bv_start(&win->bview);
    win->margin.linecount = win->bview.buffer.linecount;

    // buffer
    if(ISFLAGSET(context->flags, Farea_update)) {
        // Update bview bounds
        struct rect area = context->bounds;
        struct rect bvbounds = RECT(0, win->margin.width + 1,
                area.width - win->margin.width,
                area.height - STATUSLINE_HEIGHT);
        bv_bounds_set(&win->bview, bvbounds);
        UNSETFLAG(context->flags, Farea_update);
    }
    bv_update(&win->bview);

}


void win_draw(const struct window* win, const struct context* context)
{
    struct rect area = context->bounds;
    // Stl
    struct rect areastl = RECT(area.height - STATUSLINE_HEIGHT,0,
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
