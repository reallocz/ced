#include "window.h"
#include "draw.h"
#include "log.h"
#include <cassert>
#include <clocale>
#include <ncurses.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#define TAG "WINDOW"
#define STATUSLINE_HEIGHT 1
#define CMDLINE_HEIGHT 1

/** return a new id for a window */
static unsigned int generate_id()
{
    static unsigned int ids = 0;
    return ids++;
}


Window::Window()
{
    id = generate_id();

    // ncurses
    nwin = newwin(0, 0, 0, 0);
    assert(nwin);
    keypad(nwin, TRUE);
    bview = nullptr;
    log_l(TAG, "Window created:");
}



void Window::destroy()
{
    delwin(nwin);
    log_l(TAG, "Window destroyed: %d", id);
}



void Window::changeBufferView(struct buffer_view* bv)
{
    assert(bv);
    bview = bv;
}


void Window::update(const Context& context)
{
    // statusline
    sline.bufname = bview->buffer.name;
    sline.cur     = bview->cur;
    sline.gap     = bview->buffer.gap;

    // Margin
    margin.width     = 3;
    margin.start     = bv_start(bview);
    margin.linecount = bview->buffer.linecount;

    // buffer
    // Update bview bounds
    struct rect area     = context.bounds;
    struct rect bvbounds = RECT(0, margin.width + 1,
                                area.width - margin.width,
                                area.height - STATUSLINE_HEIGHT - CMDLINE_HEIGHT);
    bv_bounds_set(bview, bvbounds);
    bv_update(bview);
}


void Window::draw(const Context& context)
{
    struct rect area = context.bounds;
    // Stl
    struct rect areastl = RECT(area.height - STATUSLINE_HEIGHT - CMDLINE_HEIGHT, 0,
                               area.width, STATUSLINE_HEIGHT);
    draw_statusline(nwin, sline, areastl, context);

    // Margin
    struct rect areamgn = RECT(0, 0, margin.width, area.height - STATUSLINE_HEIGHT - CMDLINE_HEIGHT);
    draw_margin(nwin, margin, areamgn, context);

    // Cmdline
    struct rect areacmd = RECT(area.height - CMDLINE_HEIGHT, 0,
                               area.width, CMDLINE_HEIGHT);
    draw_cmdline(nwin, cmdline, areacmd, context);

    // Bufferview
    draw_bview(nwin, bview, context);

    // Draw cursor
    if (context.mode == Mode::Normal || context.mode == Mode::Insert) {
        // Cursor in buffer_view
        struct cursor c    = bv_relcur(bview);
        struct rect areabv = bv_bounds(bview);
        wmove(nwin, areabv.y + c.line, areabv.x + c.col);
    } else if (context.mode == Mode::Command) {
        int x = strlen(cmdline.buffer) + 1;    // +1 for ':' prefix
        wmove(nwin, areacmd.y, x);
    } else {
        // ERROR
        wmove(nwin, 0, 0);
    }

    wrefresh(nwin);
}

