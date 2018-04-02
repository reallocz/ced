#include "input.h"
#include "log.h"
#include <cassert>

#define TAG "INPUT"

inpev inp_poll(Window& win)
{
    inpev ev{};
    int ch  = wgetch(win.Nwin());
    ev.type = inp_classify(ch);
    ev.key  = ch;
    return ev;
}


enum InputType inp_classify(int ch)
{
    if ((ch >= 65 && ch <= 90)         // Uppercase
        || (ch >= 97 && ch <= 122))    // Lowercase
    {
        return InputType::Alpha;
    }

    if (ch >= 48 && ch <= 57) {
        return InputType::Num;
    }

    if ((ch >= 33 && ch <= 47) || (ch >= 58 && ch <= 64) || (ch >= 91 && ch <= 96) || (ch >= 123 && ch <= 126)) {
        return InputType::Symbol;
    }

    if (ch == k_esc || ch == k_enter || ch == k_tab || ch == k_space || ch == k_delete || ch == k_insert || ch == k_backspace) {
        return InputType::Special;
    }

    if (ch == k_up || ch == k_down || ch == k_left || ch == k_right) {
        return InputType::Arrow;
    }

    if (ch >= k_f1 && ch <= k_f12) {
        return InputType::Function;
    }

    log_l(TAG, "Unknown key: %d", ch);
    return InputType::Unknown;
}
