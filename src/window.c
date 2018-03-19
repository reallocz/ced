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

    // Statusline
    w->sline.bufname = bview.buffer->name;
    w->sline.cur.line = 0;
    w->sline.cur.col = 0;

    // Margin
    w->margin.width = 3;
    w->margin.start = bv_start(&bview);
    w->margin.linecount = w->bview.buffer->linecount;

    log_l(TAG, "Window created: id: %d", w->id);
    win_pprint(w);
    return w;
}


void win_destroy(struct window* win)
{
    delwin(win->nwin);
    log_l(TAG, "Window destroyed: %d", win->id);
}


void win_update(struct window* win, struct rect area, const struct context *context)
{
    // Buffer
    {
        struct rect bvarea = RECT(0, win->margin.width + 1,
                area.width - win->margin.width,
                area.height - STATUSLINE_HEIGHT);
        // Keep cursor on the screen
        struct buffer_view* bv = &win->bview;
        unsigned int firstline = bv_start(bv);
        unsigned int linesleft = buf_line_count(bv->buffer);
        if(bv->cur.line < firstline) {
            struct cursor cur = bv->cur;
            cur.line = firstline;
            bv_cset(bv, cur);
        }

        if(bv->cur.line > firstline + bvarea.height - 1) {
            struct cursor cur = bv->cur;
            cur.line = firstline + bvarea.height - 1;
            bv_cset(bv, cur);
        }
    }

    // statusline
    win->sline.bufname = win->bview.buffer->name;
    win->sline.cur = win->bview.cur;
    win->sline.gap = win->bview.buffer->gap;

    // Margin
    win->margin.width = 3;
    win->margin.start = bv_start(&win->bview);
    win->margin.linecount = win->bview.buffer->linecount;
}


void win_draw(const struct window* win, struct rect area,
        struct context* context)
{
    // Stl
    struct rect areastl = RECT(area.height - STATUSLINE_HEIGHT,0,
            area.width, 1);
    draw_statusline(win->nwin, win->sline, areastl, context);

    // Margin
    struct rect areamgn = RECT(0, 0, win->margin.width, area.height - STATUSLINE_HEIGHT);
    draw_margin(win->nwin, win->margin, areamgn, context);

    // Bufferview
    struct rect areabv = RECT(0, win->margin.width + 1,
            area.width - win->margin.width,
            area.height - STATUSLINE_HEIGHT);
    draw_bview(win->nwin, win->bview, areabv, context);

    wrefresh(win->nwin);
}


void win_pprint(struct window* win)
{
    log_lc(TAG, "Window {id:%d}", win->id);
}
