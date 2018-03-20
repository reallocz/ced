#include "draw.h"
#include "log.h"

#define TAG "DRAW"

/**** IMPORTANT:
 * The drawing order must be:
 *  1 - statusline
 *  2 - margin
 *  3 - textarea
 * This is because drawing margin clear each line and drawing textarea does NOT!
 */

void draw_bview(WINDOW* nwin, const struct buffer_view* bv, const struct context* context __attribute__((unused)))
{
    log_l(TAG, "Drawing buffer %d (%d lines)...",
            bv->buffer.id, bv->buffer.linecount);

    struct rect area = bv_bounds(bv);
    unsigned int ox = area.x;
    unsigned int oy = area.y;

    unsigned int linesdrawn = 0;
    unsigned int firstline = bv_start(bv);
    for(unsigned int i = 0; i < area.height; ++i) {

        unsigned int linenumber = firstline + i;
        struct line* ln = buf_line(&bv->buffer, linenumber);
        if(ln == NULL) { break; } // No more lines in buffer

        linesdrawn++;
        if(buf_line_hasgap(&bv->buffer, linenumber)) {
            for(unsigned int j = 0; j < ln->len; ++j) {
                if(!buf_ingap(&bv->buffer, j)) {
                    mvwaddch(nwin, area.y, area.x++, ln->data[j]);
                }
            }
        } else {
            for(unsigned int j = 0; j < ln->len; ++j) {
                mvwaddch(nwin, area.y, area.x++, ln->data[j]);
            }
        }
        area.y++;
        area.x = ox;
    }

    log_l(TAG, "%d lines drawn", linesdrawn);

    // Move cursor to the original position
    /*wmove(nwin, oy + bv.cur.line, ox + bv.cur.col);*/
    struct cursor c = bv_relcur(bv);
    wmove(nwin, oy + c.line, ox + c.col);
}


void draw_margin(WINDOW* nwin, struct margin mgn, struct rect area, const struct context* context __attribute__((unused)))
{
    wattron(nwin, A_BOLD);

    int txtpadding = 1;
    // line number to string
    char lnstr[mgn.width + txtpadding];
    for(unsigned int i = 0; i < area.height; ++i) {
        wmove(nwin, i, 0);
        wclrtoeol(nwin);
        unsigned int lnnumber = mgn.start + i;
        if(lnnumber < mgn.linecount) {
            sprintf(lnstr, "%3d", lnnumber + 1);    // 1 indexed
        } else {
            sprintf(lnstr, "%3c", '~');
        }
        waddstr(nwin, lnstr);
    }
    wattroff(nwin, A_BOLD);
}


void draw_statusline(WINDOW* nwin, struct statusline sline, struct rect area, const struct context* context)
{
    // statusline string
    char stsstring[area.width];
    sprintf(stsstring,
            "   %s | %s | CUR %d:%d | GAP: line: %d col: %d size: %d",
            context->modestr, sline.bufname, sline.cur.line, sline.cur.col,
            sline.gap.line, sline.gap.col, sline.gap.size);

    init_pair(1, COLOR_BLACK, COLOR_WHITE);
    wattron(nwin, COLOR_PAIR(1));
    wmove(nwin, area.y, 0);
    wclrtoeol(nwin);
    waddstr(nwin, stsstring);
    wattroff(nwin, COLOR_PAIR(1));
}
