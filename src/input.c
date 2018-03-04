#include "input.h"
#include "log.h"

static struct {
	const char* tag;
	hWindow win;
	void (*handler) (inpev); // Callback when inp_poll is called
} G;


int inp_init()
{
	G.tag = "INPUT";
	G.handler = NULL;
	G.win = INVALID_WINDOW;
	log_l(G.tag, "Init success");
	return 0;
}


int inp_exit()
{
	return 0;
}


inpev inp_winpoll(hWindow win)
{
        inpev ev;
	ev.win = win;
        int ch = wgetch(win_nwin(win));
	ev.key = ch;
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


void inp_poll()
{
	if(G.handler == NULL || G.win == INVALID_WINDOW) {
		log_e(G.tag, "%s: No handler(%d)/window(%d) set!", G.handler, G.win);
		return;
	}
	inpev ev = inp_winpoll(G.win);
	G.handler(ev);
}


int inp_set_handler(hWindow win, void (*handler) (inpev))
{
	log_l(G.tag, "Setting input handler: win=%d->%d, handler=%p->%p", G.win, win, G.handler, handler);
	G.handler = handler;
	G.win = win;
	return 0;
}


void inp_unsethandler()
{
	log_l(G.tag, "Unsetting input handler: win=%d, handler=%p", G.win, G.handler);
	G.handler = NULL;
	G.win = INVALID_WINDOW;
}
