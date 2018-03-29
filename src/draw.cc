#include "draw.h"
#include "common.h"
#include "log.h"

#define TAG "DRAW"

/**** IMPORTANT:
 * The drawing order must be:
 *  1 - statusline
 *  2 - margin
 *  ... - cmdline, textarea etc
 * This is because drawing margin clear each line and drawing textarea does NOT!
 */

void draw_bview(WINDOW* nwin, const BufferView& bv, const Context& context __attribute__((unused)))
{
    const Buffer& buffer = bv.getBuffer();

    log_l(TAG, "Drawing buffer %d (%d lines)...",
          buffer.Id(), buffer.lineCount());

    Rect area       = bv.getBounds();
    unsigned int ox = area.x;
    unsigned int oy = area.y;

    unsigned int linesdrawn = 0;
    unsigned int firstline  = bv.getStart();
    for (unsigned int i = 0; i < area.height; ++i) {
        unsigned int linenumber = firstline + i;

        if (linenumber >= buffer.lineCount()) {
            break;
        }

        const Line& ln = buffer.line(linenumber);


        linesdrawn++;
        for (unsigned int j = 0; j < ln.Len(); ++j) {
            if (!ln.inGap(j)) {
                mvwaddch(nwin, area.y, area.x++, ln[j]);
            }
        }
        area.y++;
        area.x = ox;
    }

    log_l(TAG, "%d lines drawn", linesdrawn);
}


void draw_margin(WINDOW* nwin, const Margin& mgn, Rect area, const Context& context __attribute__((unused)))
{
    wattron(nwin, A_BOLD);

    int txtpadding = 1;
    // line number to string
    char lnstr[mgn.width + txtpadding];
    for (unsigned int i = 0; i < area.height; ++i) {
        wmove(nwin, i, 0);
        wclrtoeol(nwin);
        unsigned int lnnumber = mgn.start + i;
        if (lnnumber < mgn.linecount) {
            sprintf(lnstr, "%3d", lnnumber + 1);    // 1 indexed
        } else {
            sprintf(lnstr, "%3c", '~');
        }
        waddstr(nwin, lnstr);
    }
    wattroff(nwin, A_BOLD);
}


void draw_statusline(WINDOW* nwin, const StatusLine& sline, Rect area, const Context& context)
{
    // statusline string
    char stsstring[area.width];
    sprintf(stsstring,
            "   %s | %s | CUR %d:%d | ",
            context.modestr, sline.bufname, sline.cur.line,
            sline.cur.col);

    init_pair(1, COLOR_BLACK, COLOR_WHITE);
    wattron(nwin, COLOR_PAIR(1));
    wmove(nwin, area.y, 0);
    wclrtoeol(nwin);
    waddstr(nwin, stsstring);
    wattroff(nwin, COLOR_PAIR(1));
}

void draw_cmdline(WINDOW* nwin, const CmdLine& cline,
                  Rect area, const Context& context)
{
    wmove(nwin, area.y, 0);
    wclrtoeol(nwin);

    if (context.mode != Mode::Command) {
        return;
    }
    waddch(nwin, ':');
    waddstr(nwin, cline.buffer);
}
