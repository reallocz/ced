#pragma once

/** Key wrapper */
enum inp_key {
        /* Control + letters */
        k_ctrl_a = 1,
        k_ctrl_b = 2,
        k_ctrl_c = 3,
        k_ctrl_d = 4,
        k_ctrl_e = 5,
        k_ctrl_f = 6,
        k_ctrl_g = 7,
        k_ctrl_h = 8,   // Backspace
        k_ctrl_i = 9,   // TAB
        k_ctrl_j = 10,  // Linefeed / Enter
        k_ctrl_k = 11,
        k_ctrl_l = 12,
        k_ctrl_m = 13,  // Carriage return
        k_ctrl_n = 14,
        k_ctrl_o = 15,
        k_ctrl_p = 16,
        k_ctrl_q = 17,
        k_ctrl_r = 18,
        k_ctrl_s = 19,
        k_ctrl_t = 20,
        k_ctrl_u = 21,
        k_ctrl_v = 22,
        k_ctrl_w = 23,
        k_ctrl_x = 24,
        k_ctrl_y = 25,
        k_ctrl_z = 26,

        // Override
        k_tab = 9,
        k_enter = 10,
        k_esc = 27,
        k_space = 32,
        k_del = 330,
        k_insert = 331,
        k_backspace = KEY_BACKSPACE, //263?

        // Arrow
        k_up = KEY_UP,
        k_down = KEY_DOWN,
        k_left = KEY_LEFT,
        k_right = KEY_RIGHT,

        k_f1 = KEY_F(1),
        k_f2 = KEY_F(2),
        k_f3 = KEY_F(3),
        k_f4 = KEY_F(4),
        k_f5 = KEY_F(5),
        k_f6 = KEY_F(6),
        k_f7 = KEY_F(7),
        k_f8 = KEY_F(8),
        k_f9 = KEY_F(9),
        k_f10 = KEY_F(10),
        k_f11 = KEY_F(11),
        k_f12 = KEY_F(12),
};

