#include "window.h"
#include "draw.h"
#include "log.h"
#include <cassert>
#include <clocale>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ncurses.h>

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
    log_l(TAG, "Window created:");
}


void Window::destroy()
{
    delwin(nwin);
    log_l(TAG, "Window destroyed: %d", id);
}


void Window::changeBufferView(BufferView* bv)
{
    assert(bv);
    bview = bv;
}


void Window::update(const Context& context)
{
    // statusline
    sline.bufname = bview->getBuffer().getName();
    sline.cur     = bview->getCursor();

    // Margin
    margin.width     = 3;
    margin.start     = bview->getStart();
    margin.linecount = bview->getBuffer().lineCount();

    // buffer
    // Update bview bounds
    Rect area     = context.bounds;
    Rect bvbounds = RECT(0, margin.width + 1,
                                area.width - margin.width,
                                area.height - STATUSLINE_HEIGHT - CMDLINE_HEIGHT);
    bview->setBounds(bvbounds);
    bview->update();
}


void Window::draw(const Context& context)
{
    Rect area = context.bounds;
    // Stl
    Rect areastl = RECT(area.height - STATUSLINE_HEIGHT - CMDLINE_HEIGHT, 0,
                               area.width, STATUSLINE_HEIGHT);
    draw_statusline(nwin, sline, areastl, context);

    // Margin
    Rect areamgn = RECT(0, 0, margin.width, area.height - STATUSLINE_HEIGHT - CMDLINE_HEIGHT);
    draw_margin(nwin, margin, areamgn, context);

    // Cmdline
    Rect areacmd = RECT(area.height - CMDLINE_HEIGHT, 0,
                               area.width, CMDLINE_HEIGHT);
    draw_cmdline(nwin, cline, areacmd, context);

    // Bufferview
    draw_bview(nwin, *bview, context);

    // Draw cursor
    if (context.mode == Mode::Normal || context.mode == Mode::Insert) {
        // Cursor in BufferView
        Cursor c    = bview->relcur();
        Rect areabv = bview->getBounds();
        wmove(nwin, areabv.y + c.line, areabv.x + c.col);
    } else if (context.mode == Mode::Command) {
        int x = cline.getCursor() + 1;
        //int x = strlen(cmdline.buffer) + 1;    // +1 for ':' prefix
        wmove(nwin, areacmd.y, x);
    } else {
        // ERROR
        wmove(nwin, 0, 0);
    }

    wrefresh(nwin);
}
