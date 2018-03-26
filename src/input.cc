#include "input.h"
#include "log.h"
#include <assert.h>

#define TAG "INPUT"

inpev inp_poll(struct window* win)
{
    inpev ev;
    int ch  = wgetch(win->nwin);
    ev.type = inp_classify(ch);
    ev.key  = ch;
    return ev;
}


enum inp_type inp_classify(int ch)
{
    if ((ch >= 65 && ch <= 90)         // Uppercase
        || (ch >= 97 && ch <= 122))    // Lowercase
    {
        return INP_ALPHA;
    }

    if (ch >= 48 && ch <= 57) {
        return INP_NUM;
    }

    if ((ch >= 33 && ch <= 47) || (ch >= 58 && ch <= 64) || (ch >= 91 && ch <= 96) || (ch >= 123 && ch <= 126)) {
        return INP_SYMBOL;
    }

    if (ch == k_esc || ch == k_enter || ch == k_tab || ch == k_space || ch == k_delete || ch == k_insert || ch == k_backspace) {
        return INP_SPECIAL;
    }

    if (ch == k_up || ch == k_down || ch == k_left || ch == k_right) {
        return INP_ARROW;
    }

    if (ch >= k_f1 && ch <= k_f12) {
        return INP_FUNCTION;
    }

    log_l(TAG, "Unknown key: %d", ch);
    return INP_UNKNOWN;
}
