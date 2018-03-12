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
    // Statusline
    {
        // Save cursor position
        int curx = win->curx;
        int cury = win->cury;

        // Draw bottom horizontal line
        wmove(win->nwin, height - 2, 0);
        whline(win->nwin, ACS_HLINE, width);

        mvwaddstr(win->nwin, height - 1, 0, mode);

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
        char lnstr[win->mgn.width]; // line number to string
        for(unsigned int i = 0; i < height - 2; ++i) {
            wmove(win->nwin, i, 0);
            if(i <= linecount) {
                sprintf(lnstr, "%d", i + 1);
                waddstr(win->nwin, lnstr);
            } else {
                waddstr(win->nwin, " ~");
            }
        }
        wattroff(win->nwin, A_BOLD);
        wmove(win->nwin, cury, curx);
    }

    // Text area
    {
        int y = 0;
        int x = win->mgn.width;
        struct buffer* buf = win->buffer;

        // Clear current line
        wmove(win->nwin, y, x);
        wclrtoeol(win->nwin);

        // Draw line
        for(unsigned int i = 0; i < buf->size; ++i) {
            if(!buf_ingap(buf, i))
            {
                char c = buf_get_char(buf, i);
                if(c != 0)
                    mvwaddch(win->nwin, y, x++, buf_get_char(buf, i));
                else
                    log_e(TAG, "%s: ERROR: THIS SHOULDN'T HAPPEN",
                            __func__);
            }
        }

        // Sync window cursor and buffer cursor
        wmove(win->nwin, y, win->mgn.width + buf->cur);
        /*buf_printbuf(buf);  //Debugging gapbuffer*/
    }
}


void win_update(struct window* win)
{

    // Dimensions
    /*getyx(w->nwin, props->y, props->x);*/
    /*getmaxyx(w->nwin, props->cols, props->rows);*/

    /*// statusline*/
    /*[>props->sy = props->wrows - 1 - WINDOW_STATUSHEIGHT;<]*/
    /*[>props->sx = 0;<]*/
    /*[>props->swidth = props.wcols;<]*/
    /*[>props->sheight = WINDOW_STATUSHEIGHT;<]*/

    /*// margin*/
    /*props->marx = 0;*/
    /*props->mary = 0;*/

    /*props->marw = 4;    // TODO calculate from buffer linecount*/
    /*props->marh = props->rows - 1 - WINDOW_STATUSHEIGHT;*/

    /*// textarea*/
    /*props->texy = 0;*/
    /*props->texx = props->marw;*/
    /*props->texw = props.wcols - props.mwidth;*/
    /*props->texh = props->marh;*/

    // TODO Set window title based on the buffer
}



void win_draw_buffer(struct window* win)
{
        /*hBuffer buf = w->buffer;*/
    /*assert(buf != INVALID_ID);*/

    /*int y = w->props.texy;*/
    /*int x = w->props.texx;*/
    /*struct buf_props bprops = buf_get_props(buf);*/

    /*// Clear current line*/
    /*wmove(w->nwin, y, x);*/
    /*wclrtoeol(w->nwin);*/

    /*// Draw line*/
    /*for(unsigned int i = 0; i < bprops.size; ++i) {*/
        /*if(i >= bprops.gap.pos*/
                /*&& i < bprops.gap.pos + bprops.gap.len)*/
        /*{*/
            /*// In gap*/
            /*continue;*/
        /*} else {*/
            /*char c = buf_get_char(buf, i);*/
            /*if(c != 0)*/
                /*mvwaddch(w->nwin, y, x++, buf_get_char(buf, i));*/
        /*}*/
    /*}*/

    /*// Sync window cursor and buffer cursor*/
    /*wmove(w->nwin, y, w->props.tx + bprops.cur);*/
    /*[>buf_printbuf(buf);<] // Debugging gapbuffer*/
}


void win_pprint(struct window* win)
{
    log_lc(TAG, "Window {id:%d, curx:%d, cury: %d", win->id,
            win->curx, win->cury);
}
