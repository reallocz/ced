#include "input.h"
#include "log.h"
#include <assert.h>
#include "defs.h"

static struct {
    const char* tag;
    struct inp_handler handler;
} G;


/** Return an empty handler */
static
struct inp_handler _empty_handler()
{
    struct inp_handler handler = {
        .name = "DEFAULT",
        .window = INVALID_ID,
        .callback = NULL,
    };
    return handler;
}

/** Return 1 if set handler is valid. else return 0 */
static
int _handler_valid()
{
    return G.handler.window != INVALID_ID
        && G.handler.callback != NULL;
}

int inp_init()
{
    G.tag = "INPUT";
    G.handler = _empty_handler();
    log_l(G.tag, "Init success");
    return 0;
}


int inp_exit()
{
    return 0;
}


int inp_set_handler(struct inp_handler handler)
{
    assert(handler.callback && handler.window != INVALID_ID);

    /*log_l(G.tag, "Setting handler: %s->%s, handler: %p->%p\*/
            /*window: %d->%d",*/
            /*G.handler.name, handler.name,*/
            /*G.handler.callback, handler.callback,*/
            /*G.handler.window, handler.window);*/
    G.handler = handler;
    return 0;
}


void inp_unsethandler()
{
    log_l(G.tag, "Unsetting handler: %s, handler: %p, window: %d",
            G.handler.name, G.handler.callback, G.handler.window);
    G.handler = _empty_handler();
}


void inp_poll()
{
    if(! _handler_valid())
    {
        log_e(G.tag, "%s: No input handler set!");
        return;
    }

    inpev ev;
    int ch = wgetch(win_nwin(G.handler.window));
    ev.type = inp_classify(ch);
    ev.key = ch;

    G.handler.callback(ev);
}


enum inp_type inp_classify(int ch)
{
    if((ch >= 65 && ch <= 90)   // Uppercase
            || (ch >= 97 && ch <= 122)) // Lowercase
    {
        return INP_ALPHA;
    }

    if(ch >= 48 && ch <= 57)
    {
        return INP_NUM;
    }

    if((ch >= 33 && ch <= 47) || (ch >= 58 && ch <= 64)
            ||(ch >= 91 && ch <= 96) || (ch >= 123 && ch <= 126))
    {
        return INP_SYMBOL;
    }

    if(ch == k_esc || ch == k_enter || ch == k_tab || ch == k_space
            || ch == k_delete || ch == k_insert || ch == k_backspace)
    {
        return INP_SPECIAL;
    }

    if(ch == k_up || ch == k_down || ch == k_left || ch == k_right)
    {
        return INP_ARROW;
    }

    if(ch >= k_f1 && ch <= k_f12)
    {
        return INP_FUNCTION;
    }

    log_l(G.tag, "Unknown key: %d", ch);
    return INP_UNKNOWN;
}
