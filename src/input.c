#include "input.h"
#include "log.h"

static struct {
	const char* tag;
} G;

int inp_init()
{
	G.tag = "INPUT";
	return 0;
}

int inp_exit()
{
	return 0;
}

inpev inp_poll(hWindow win)
{
        inpev ev;
        int ch = wgetch(win_getnwin(win));
        if((ch >= 33 && ch <= 126) || ch == k_space) {
                ev.type = INSERT;
                ev.data.ch = (char)ch;
        } else if (ch == k_esc || ch == k_f1) {
                ev.type = QUIT;
                ev.data.i = 0;
        } else if (ch == k_backspace) {
                ev.type = DELETE;
                ev.data.i = -1;
        } else if ( ch == k_enter ) {
                ev.type = MOVE;
                ev.data.i = 1;
        } else {
                ev.type = NOP;
                ev.data.i = 0;
        }
        return ev;
}
