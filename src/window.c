#include "window.h"
#include <locale.h>
#include <ncurses.h>
#include <stdio.h>
#include <assert.h>
#include "log.h"
#include <stdlib.h>


// A maximum of 4 windows allowed
#define WINDOW_DEFCAP 4
#define WINDOW_STATUSHEIGHT 2

/** Bring struct window* alias of id x into scope */
#define SW(alias, hWindow) \
    struct window* alias = _get_window(hWindow)

struct window {
    /*unique identifier of each window for debugging*/
    unsigned int id;

    /*1 if the window is in use (not destroyed)*/
    int in_use;

    /** Handle to ncurses WINDOW* */
    WINDOW* nwin;

    /**
     * Handle to a buffer.
     * Each window can have upto one attached buffer
     * which is manipulated using the win_buf_*
     * functions.
     * INVALID_BUFFER if no buffer set
     */
    hBuffer buffer;

    /** contains window properties */
    struct win_props props;
};


static struct {
    const char* tag;
    unsigned int windowcap;
    struct window windows[WINDOW_DEFCAP];
} G;


void win_init()
{
    /// Initialize globals
    G.tag = "WINDOW";
    G.windowcap = WINDOW_DEFCAP;
    for(unsigned int i = 0; i < G.windowcap; ++i) {
        G.windows[i].in_use = 0;
        G.windows[i].buffer = INVALID_BUFFER;
    }
    log_l(G.tag, "Init success");
}


void win_exit()
{
}


/** return a new id for a window */
    static
unsigned int _generate_id()
{
    static unsigned int ids = 0;
    return ids++;
}


/** return a pointer to the window of the handle */
    static
struct window* _get_window(hWindow win)
{
    if(win < 0 || win >= G.windowcap) {
        log_fatal(G.tag, "Invalid handle: %d", win);
    }
    struct window* w = &G.windows[win];
    if(!w->in_use) {
        log_fatal(G.tag, "Bad handle: %d", win);
    }
    return w;
}

/** return a handle to a free window (in_use == 0)*/
    static
hWindow _get_free_handle()
{
    for(unsigned int i = 0; i < G.windowcap; ++i) {
        if(! G.windows[i].in_use) {
            return i;
        }
    }
    log_fatal(G.tag, "%s: OUT OF WINDOWS!", __func__);
}


hWindow win_create(int y, int x, int rows, int cols)
{
    hWindow win = _get_free_handle();
    struct window* w = &G.windows[win];
    w->id = _generate_id();
    w->in_use = 1;
    w->buffer = INVALID_BUFFER;
    w->nwin = newwin(rows, cols, y, x);
    assert(w->nwin);
    win_update(win);
    keypad(w->nwin, TRUE);
    return win;
}


void win_destroy(hWindow win)
{
    SW(w, win);
    // Mark window as not in use
    w->in_use = 0;
    delwin(w->nwin);
    log_l(G.tag, "Window destroyed: %d", win);
}



void win_update(hWindow win)
{
    SW(w, win);

    struct win_props props;

    // Dimensions
    {
        getyx(w->nwin, props.wy, props.wx);

        int width, height;
        getmaxyx(w->nwin, width, height);
        // 0 indexed
        props.wrows = width;
        props.wcols = height;
    }

    // statusline
    {
        props.sy = props.wrows - 1 - WINDOW_STATUSHEIGHT;
        props.sx = 0;
        props.swidth = props.wcols;
        props.sheight = WINDOW_STATUSHEIGHT;
    }

    // margin
    {
        props.my = 0;
        props.mx = 0;

        unsigned int lcount = 100;
        if(w->buffer != INVALID_BUFFER) {
            /*lcount = buf_get_linecount(w->buffer);*/
            lcount = 8; // TODO Fix this
        }
        props.mwidth = lcount > 999 ? 4 : 3;
        props.mheight = props.wrows - 1 - WINDOW_STATUSHEIGHT;
    }

    // textarea
    {
        props.ty = 0;
        props.tx = props.mwidth;
        props.twidth = props.wcols - props.mwidth;
        props.theight = props.mheight;
    }

    w->props = props;

    // TODO Set window title based on the buffer

}



WINDOW* win_nwin(hWindow win)
{
    SW(w, win);
    return w->nwin;
}


int win_set_buffer(hWindow win, hBuffer buf)
{
    SW(w, win);
    log_l(G.tag, "(win:%d) buffer set: %d -> %d", win,
            w->buffer, buf);
    w->buffer = buf;
    win_update(win);
    return 0;
}


hBuffer win_get_buffer(hWindow win)
{
    SW(w, win);
    return w->buffer;
}


struct win_props win_get_props(hWindow win)
{
    SW(w, win);
    return w->props;
}


void win_set_cursor(hWindow win, int y, int x)
{
    SW(w, win);
    wmove(w->nwin, y, x);
}


void win_get_cursor(hWindow win, int* y, int* x)
{
    SW(w, win);
    int my, mx;
    getyx(w->nwin, my, mx);
    *y = my;
    *x = mx;
}



void win_draw_refresh(hWindow win)
{
    SW(w, win);
    wrefresh(w->nwin);
}


void win_draw_buffer(hWindow win)
{
    SW(w, win);
    hBuffer buf = w->buffer;
    assert(buf != INVALID_BUFFER);

    int y = w->props.ty;
    int x = w->props.tx;
    struct buf_props bprops = buf_get_props(buf);

    // Clear current line
    wmove(w->nwin, y, x);
    wclrtoeol(w->nwin);

    // Draw line
    for(unsigned int i = 0; i < bprops.size; ++i) {
        if(i >= bprops.gap.pos
                && i < bprops.gap.pos + bprops.gap.len)
        {
            // In gap
            continue;
        } else {
            char c = buf_get_char(buf, i);
            if(c != 0)
                mvwaddch(w->nwin, y, x++, buf_get_char(buf, i));
        }
    }

    // Sync window cursor and buffer cursor
    wmove(w->nwin, y, w->props.tx + bprops.cur);
    /*buf_printbuf(buf);*/ // Debugging gapbuffer
}


void win_pprint(hWindow win)
{
    SW(w, win);
    log_l(G.tag, "Window {");
    log_lc("id: %d, wy: %d, wx: %d, wrows: %d, wcols: %d\n",
            w->id, w->props.wy, w->props.wx, w->props.wrows, w->props.wcols);
    log_lc("sy: %d, sx: %d, swidth: %d, sheight: %d\n",
            w->props.sy, w->props.sx, w->props.swidth, w->props.sheight);
    log_lc("my: %d, mx: %d, mwidth: %d, mheight: %d\n",
            w->props.my, w->props.mx, w->props.mwidth, w->props.mheight);
    log_lc("ty: %d, tx: %d, twidth: %d, theight: %d\n}\n",
            w->props.ty, w->props.tx, w->props.twidth, w->props.theight);
}
