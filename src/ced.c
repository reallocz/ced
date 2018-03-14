#include "ced.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "log.h"
#include "window.h"
#include "input.h"
#include "buffer.h"
#include "term.h"
#include "input_keys.h"

#define TAG "CED"


static struct {
    enum ced_mode mode;
    int quit; // Return to main if 1
    struct window *win;
} G;


void ced_run()
{
    // Initialize globals
    G.quit = 0;

    // Create window and set buffer
    /*struct buffer* buf = buf_create_file(SCRATCH, "doc.txt");*/
    struct buffer* buf = buf_create_empty(SCRATCH);
    G.win = win_create(buf);
    // Start in normal mode
    G.mode = MODE_NORMAL;

    // Main loop
    while(!G.quit) {
        // Check for window resize
        term_update();
        unsigned int width = term_cols();
        unsigned int height = term_rows();

        if(G.mode == MODE_NORMAL) {
            win_draw(G.win, "MODE: NORMAL", 0, 0, height, width);
            inp_poll("NORMAL", G.win, ced_normal_input_cb);
        } else {
            win_draw(G.win, "MODE: INSERT", 0, 0, height, width);
            inp_poll("NORMAL", G.win, ced_insert_input_cb);
        }
    }
}


void ced_insert_input_cb(inpev ev)
{
    struct buffer* buf = G.win->buffer;

    if(ev.type == INP_ALPHA || ev.type == INP_NUM
            || ev.type == INP_SYMBOL || ev.key == k_space
            || ev.key == k_enter)
    {
        /*buf_addch(buf, ev.key);*/
    }

    if(ev.type == INP_SPECIAL)
    {
        switch(ev.key) {
        case k_backspace:
            /*buf_delch(buf);*/
            break;
        case k_esc:
            G.mode = MODE_NORMAL;
            break;
        default:
            break;
        }
    }
}


void ced_normal_input_cb(inpev ev)
{
    struct buffer* buf = G.win->buffer;

    if(ev.key == 'i') {
        // Switch to insert mode
        G.mode = MODE_INSERT;
        return;
    }
    if (ev.key == k_f1) {
        G.quit = 1;
        return;
    }
    if(ev.key == k_f2) {
        /*buf_save_to_disk(buf, "doc.txt"); // TODO prompt for name*/
        return;
    }
    if(ev.key == 'h') {
    }
    if(ev.key == 'l') {
    }
}

