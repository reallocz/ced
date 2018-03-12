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


void win_draw(const struct window* win, const char* mode,
        unsigned int y, unsigned int x,
        unsigned int height, unsigned int width)
{
    int txtpadding = 1;
    unsigned int linecount = buf_get_linecount(win->buffer);
    // Saved window cursor positions
    unsigned int curx = win->curx;
    unsigned int cury = win->cury;

    // Statusline
    {
        // statusline string
        char stsstring[width];
        unsigned int row = 0;
        unsigned int col = win->buffer->cur;
        const char* bufname = win->buffer->name;
        sprintf(stsstring, "%s | %s | row:%d - col: %d | line: %d",
                mode, bufname, row, col, row + 1);

        init_pair(1, COLOR_BLACK, COLOR_WHITE);
        wattron(win->nwin, COLOR_PAIR(1));
        wmove(win->nwin, height - STATUSLINE_HEIGHT, 0);
        wclrtoeol(win->nwin);
        waddstr(win->nwin, stsstring);
        wattroff(win->nwin, COLOR_PAIR(1));

        // Go back to original positions
        wmove(win->nwin, cury, curx);
    }


    // Margin
    {
        wattron(win->nwin, A_BOLD);

        // line number to string
        char lnstr[win->mgn.width + txtpadding];
        for(unsigned int i = 0; i < height - STATUSLINE_HEIGHT; ++i) {
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
        wmove(win->nwin, cury, curx);
    }

    // Text area
    {
        int leftedge = win->mgn.width + txtpadding;
        struct buffer* buf = win->buffer;

        unsigned int col = 0;
        unsigned int row = 0;

        // Clear first row
        wmove(win->nwin, 0, leftedge);
        wclrtoeol(win->nwin);

        // Draw lines
        for(unsigned int i = 0; i < buf->size; ++i) {
            if(!buf_ingap(buf, i)) {
                char c = buf->data[i];

                if(c == '\n') {
                    // Clear next row
                    row++;
                    col = 0; // Reset col
                    wmove(win->nwin, row, leftedge + col);
                    wclrtoeol(win->nwin);
                } else if (c != 0) {
                    waddch(win->nwin, c);
                    /*mvwaddch(win->nwin, row, leftedge + col++, c);*/
                } else {
                    log_fatal(TAG, "%s: ERROR: THIS SHOULDN'T HAPPEN", __func__);
                }
            }
        }

        // Sync window cursor and buffer cursor
        unsigned int curline = buf_cur_line(win->buffer);
        wmove(win->nwin, curline, win->mgn.width + txtpadding + buf_cur_lineoffset(buf));
        /*buf_printbuf(buf);  //Debugging gapbuffer*/
    }
}


void win_pprint(struct window* win)
{
    log_lc(TAG, "Window {id:%d, curx:%d, cury: %d", win->id,
            win->curx, win->cury);
}
