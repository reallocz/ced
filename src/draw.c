#include "draw.h"
#include "log.h"

#define TAG "DRAW"

void draw_bview(WINDOW* nwin, struct buffer_view bv, struct rect area)
{
    log_l(TAG, "Drawing buffer: %d lines", bv.buffer->linecount);
    unsigned int ox = area.x;
    unsigned int oy = area.y;

    for(unsigned int i = 0; i < bv.len; ++i) {
        struct line* ln = buf_line(bv.buffer, bv.start + i);
        log_lc(TAG, "Drawing line: ");
        buf_printline(bv.buffer, i);

        if(bv.buffer->gap.line == i) {
            for(unsigned int j = 0; j < ln->len; ++j) {
                if(!buf_ingap(bv.buffer, j)) {
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

    // Move cursor to the original position
    wmove(nwin, oy + bv.cur.line, ox + bv.cur.col);
}


void draw_margin(WINDOW* nwin, struct margin mgn, struct rect area)
{
    wattron(nwin, A_BOLD);

    int txtpadding = 1;
    // line number to string
    char lnstr[mgn.width + txtpadding];
    for(unsigned int i = 0; i < area.height; ++i) {
        wmove(nwin, i, 0);
        if(i < mgn.end) {
            sprintf(lnstr, "%3d", i + 1);  // 1 - indexed
        } else {
            sprintf(lnstr, "%3c", '~');
        }
        waddstr(nwin, lnstr);
    }
    wattroff(nwin, A_BOLD);
}


void draw_statusline(WINDOW* nwin, struct statusline sline, struct rect area)
{
    // statusline string
    char stsstring[area.width];
    sprintf(stsstring,
            "   %s | %s | CUR %d:%d | GAP: line: %d col: %d size: %d",
            sline.mode, sline.bufname, sline.cur.line, sline.cur.col,
            sline.gap.line, sline.gap.col, sline.gap.size);

    init_pair(1, COLOR_BLACK, COLOR_WHITE);
    wattron(nwin, COLOR_PAIR(1));
    wmove(nwin, area.y, 0);
    wclrtoeol(nwin);
    waddstr(nwin, stsstring);
    wattroff(nwin, COLOR_PAIR(1));
}
