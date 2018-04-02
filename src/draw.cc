#include "draw.h"
#include "common.h"
#include "log.h"

#define TAG "DRAW"

/**** IMPORTANT:
 * The drawing order must be:
 *  1 - statusline
 *  2 - margin
 *  ... - cmdline, textarea etc
 * This is because drawing margin clear each line and drawing textarea
 * does NOT!
 */

void draw_bview(WINDOW* nwin, const BufferView& bv,
                const Context& context __attribute__((unused)))
{
    const Buffer& buffer = bv.getBuffer();

    // log_l(TAG, "Drawing buffer %d (%d lines)...",
    // buffer.Id(), buffer.lineCount());

    Rect area = bv.getBounds();
    size_t ox = area.x;
    size_t oy = area.y;

    size_t linesdrawn = 0;
    size_t firstline  = bv.getStart();
    for (size_t i = 0; i < area.height; ++i) {
        size_t linenumber = firstline + i;

        if (linenumber >= buffer.lineCount()) {
            break;
        }

        const Line& ln = buffer.getLine(linenumber);


        linesdrawn++;
        for (size_t j = 0; j < ln.Len(); ++j) {
            if (!ln.inGap(j)) {
                mvwaddch(nwin, area.y, area.x++, ln[j]);
            }
        }
        area.y++;
        area.x = ox;
    }

    // log_l(TAG, "%d lines drawn", linesdrawn);
}


void draw_margin(WINDOW* nwin, const Margin& mgn, Rect area,
                 const Cursor& cursor,
                 const Context& context __attribute__((unused)))
{

    int txtpadding = 1;
    char lnstr[mgn.width + txtpadding];    // line number to string

    for (size_t i = 0; i < area.height; ++i) {
        // Clear line
        wmove(nwin, i, 0);
        wclrtoeol(nwin);

        size_t lnnumber = mgn.start + i;

        if (lnnumber < mgn.linecount) {
            int finalValue;    // 1 indexed calculated ln number

            if (mgn.relative && cursor.line != lnnumber) {
                // Relative number
                int relnum = cursor.line - lnnumber;
                finalValue = std::abs(relnum);
            } else {
                // Regular number
                finalValue = lnnumber + 1;
            }

            sprintf(lnstr, "%3d", finalValue);
        } else {
            // Beyond End of buffer
            sprintf(lnstr, "%3c", '~');
        }

        // Highlight current line number
        if (cursor.line == lnnumber) {
            wattron(nwin, A_BOLD);
            waddstr(nwin, lnstr);
            wattroff(nwin, A_BOLD);
        } else {
            waddstr(nwin, lnstr);
        }
    }
}


void draw_statusline(WINDOW* nwin, const StatusLine& sline, Rect area,
                     const Context& context)
{
    // statusline string
    char stsstring[area.width];
    sprintf(stsstring,
            "   %s | %s | CUR %d:%d | Gap: col: %d, len: %d |",
            context.modestr, sline.bufname,
            static_cast<int>(sline.cur.line),
            static_cast<int>(sline.cur.col),
            static_cast<int>(sline.gapcol),
            static_cast<int>(sline.gaplen));

    init_pair(1, COLOR_BLACK, COLOR_WHITE);
    wattron(nwin, COLOR_PAIR(1));
    wmove(nwin, area.y, 0);
    wclrtoeol(nwin);
    waddstr(nwin, stsstring);
    wattroff(nwin, COLOR_PAIR(1));
}

void draw_cmdline(WINDOW* nwin, const CommandLine& cline, Rect area,
                  const Context& context)
{
    wmove(nwin, area.y, 0);
    wclrtoeol(nwin);

    if (context.mode != Mode::Command) {
        return;
    }
    waddch(nwin, ':');
    waddstr(nwin, cline.data());
}
