#include "window.h"
#include <locale.h>
#include <ncurses.h>
#include <stdio.h>
#include <assert.h>
#include "log.h"
#include <stdlib.h>

#define TAG "WINDOW"

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
    w->buffer = buffer;

    // ncurses
    w->nwin = newwin(0, 0, 0, 0);
    assert(w->nwin);
    keypad(w->nwin, TRUE);

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


void win_draw(struct window* win, const char* mode,
        unsigned int y, unsigned int x,
        unsigned int height, unsigned int width)
{
    int txtpadding = 1;

    // Statusline
    {
        // statusline string
        char stsstring[width];
        unsigned int row, col;
        row = 0;
        col = win->buffer->cur;
        const char* bufname = win->buffer->name;
        sprintf(stsstring, "%s | %s | row:%d - col: %d",
                mode, bufname, row, col);

        // Save cursor position
        int curx = win->curx;
        int cury = win->cury;
        int stsheight = 2;


        init_pair(1, COLOR_BLACK, COLOR_WHITE);
        wattron(win->nwin, COLOR_PAIR(1));
        wmove(win->nwin, height - stsheight, 0);
        wclrtoeol(win->nwin);
        waddstr(win->nwin, stsstring);
        wattroff(win->nwin, COLOR_PAIR(1));
        // Go back to original positions
        wmove(win->nwin, cury, curx);
    }


    // Margin
    {
        // Save cursor position
        int curx = win->curx;
        int cury = win->cury;
        unsigned int linecount = buf_get_linecount(win->buffer);

        wattron(win->nwin, A_BOLD);

        // line number to string
        char lnstr[win->mgn.width + txtpadding];
        for(unsigned int i = 0; i < height - 2; ++i) {
            wmove(win->nwin, i, 0);
            if(i <= linecount) {
                sprintf(lnstr, "%3d", i + 1);
                waddstr(win->nwin, lnstr);
            } else {
                sprintf(lnstr, "%3c", '~');
                waddstr(win->nwin, lnstr);
            }
        }
        wattroff(win->nwin, A_BOLD);
        wmove(win->nwin, cury, curx);
    }

    // Text area
    {
        int y = 0;
        int x = win->mgn.width + txtpadding;
        struct buffer* buf = win->buffer;

        // Clear current line
        wmove(win->nwin, y, x);
        wclrtoeol(win->nwin);

        // Draw line
        for(unsigned int i = 0; i < buf->size; ++i) {
            if(!buf_ingap(buf, i)) {
                char c = buf_get_char(buf, i);
                if(c != 0)
                    mvwaddch(win->nwin, y, x++, buf_get_char(buf, i));
                else
                    log_e(TAG, "%s: ERROR: THIS SHOULDN'T HAPPEN",
                            __func__);
            }
        }

        // Sync window cursor and buffer cursor
        wmove(win->nwin, y, win->mgn.width + txtpadding + buf->cur);
        /*buf_printbuf(buf);  //Debugging gapbuffer*/
    }
}


void win_pprint(struct window* win)
{
    log_lc(TAG, "Window {id:%d, curx:%d, cury: %d", win->id,
            win->curx, win->cury);
}
