#include "draw.h"
#include "common.h"
#include "context.h"
#include "log.h"

#define TAG "DRAW"

#define EMPTY_MARGIN_CHAR '~'

/**** IMPORTANT:
 * The drawing order must be:
 *  1 - statusline
 *  2 - margin
 *  ... - cmdline, textarea etc
 * This is because drawing margin clear each line and drawing textarea
 * does NOT!
 */

void draw_bview(WINDOW* nwin, const struct buffer_view* bv,
                const struct context* context __attribute__((unused)))
{
    /*log_l(TAG, "Drawing buffer %d (%d lines)...", bv->buffer.id,*/
    /*bv->buffer.linecount);*/

    struct rect area = bv_bounds(bv);
    size_t ox        = area.x;

    size_t linesdrawn = 0;
    size_t firstline  = bv_start(bv);
    size_t linecount = bv->buffer.linecount;
    for (size_t i = 0; i < area.height; ++i) {

        size_t linenumber = firstline + i;
        if(linenumber >= linecount) {
            break;
        }

        struct line* ln   = buf_getline(&bv->buffer, linenumber);

        linesdrawn++;
        for (size_t j = 0; j < ln->len; ++j) {
            if (!INGAP(ln, j)) {
                mvwaddch(nwin, area.y, area.x++, ln->data[j]);
            }
        }
        area.y++;
        area.x = ox;
    }

    /*log_l(TAG, "%d lines drawn", linesdrawn);*/
}


void draw_margin(WINDOW* nwin, struct margin mgn, struct rect area,
                 const struct cursor cur,
                 const struct context* context
                 __attribute__((unused)))
{

    // line number to string
    char lnstr[area.width];    // Arbitary
    for (size_t i = 0; i < area.height; ++i) {
        // Clear line
        wmove(nwin, i, 0);
        wclrtoeol(nwin);

        // 0 indexed line number
        size_t lnnumber   = mgn.start + i;
        int iscurrentline = (cur.line == lnnumber);

        // Format string
        if (lnnumber < mgn.linecount) {
            int finalval;    // 1 indexed if not relative

            if (mgn.relative == 1 && !iscurrentline) {
                int relnum = cur.line - lnnumber;
                finalval   = relnum > 0 ? relnum : -relnum;
            } else {
                finalval = lnnumber + 1;
            }

            sprintf(lnstr, "%3d", finalval);    // 1 indexed
        } else {
            // After eob
            sprintf(lnstr, "%3c", EMPTY_MARGIN_CHAR);
        }

        // Highlight current line number
        if (!iscurrentline) {
            waddstr(nwin, lnstr);
        } else {
            wattron(nwin, A_BOLD);
            waddstr(nwin, lnstr);
            wattroff(nwin, A_BOLD);
        }
    }
}


void draw_statusline(WINDOW* nwin, struct statusline sline,
                     struct rect area, const struct context* context)
{
    // statusline string
    char stsstring[area.width];
    sprintf(stsstring,
            "   %s | %s | CUR %lu:%lu | GAP: col: %lu size: %lu ",
            mode_str[context->mode], sline.bufname, sline.cur.line,
            sline.cur.col, sline.gapcol, sline.gapsize);

    init_pair(1, COLOR_BLACK, COLOR_WHITE);
    wattron(nwin, COLOR_PAIR(1));
    wmove(nwin, area.y, 0);
    wclrtoeol(nwin);
    waddstr(nwin, stsstring);
    wattroff(nwin, COLOR_PAIR(1));
}

void draw_cmdline(WINDOW* nwin, struct cmdline cline,
                  struct rect area, const struct context* context)
{
    wmove(nwin, area.y, 0);
    wclrtoeol(nwin);

    if (context->mode != MODE_COMMAND) {
        return;
    }
    waddch(nwin, ':');
    waddstr(nwin, cline.buffer);
}
