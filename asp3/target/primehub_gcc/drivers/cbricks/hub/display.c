// SPDX-License-Identifier: MIT
/*
 * API for the hub built-in display.
 *
 * Copyright (c) 2022 Embedded and Real-Time Systems Laboratory,
 *                    Graduate School of Information Science, Nagoya Univ., JAPAN
 */

#include <t_syslog.h>
#include <cbricks/hub/display.h>

#include <pbsys/light.h>
#include <pbio/light_matrix.h>

#include <pup_device.h>
#include <cb_error.h>

// Slim digits 0--9
const uint8_t pb_digits_5x2[10][5] = {
    {   // 0
        0b11, // ⬜⬜
        0b11, // ⬜⬜
        0b11, // ⬜⬜
        0b11, // ⬜⬜
        0b11, // ⬜⬜
    },
    {   // 1
        0b01, // ⬛⬜
        0b01, // ⬛⬜
        0b01, // ⬛⬜
        0b01, // ⬛⬜
        0b01, // ⬛⬜
    },
    {   // 2
        0b11, // ⬜⬜
        0b01, // ⬛⬜
        0b11, // ⬜⬜
        0b10, // ⬜⬛
        0b11, // ⬜⬜
    },
    {   // 3
        0b11, // ⬜⬜
        0b01, // ⬛⬜
        0b11, // ⬜⬜
        0b01, // ⬛⬜
        0b11, // ⬜⬜
    },
    {   // 4
        0b10, // ⬜⬛
        0b11, // ⬜⬜
        0b11, // ⬜⬜
        0b01, // ⬛⬜
        0b01, // ⬛⬜
    },
    {   // 5
        0b11, // ⬜⬜
        0b10, // ⬜⬛
        0b11, // ⬜⬜
        0b01, // ⬛⬜
        0b11, // ⬜⬜
    },
    {   // 6
        0b11, // ⬜⬜
        0b10, // ⬜⬛
        0b11, // ⬜⬜
        0b11, // ⬜⬜
        0b11, // ⬜⬜
    },
    {   // 7
        0b11, // ⬜⬜
        0b01, // ⬛⬜
        0b01, // ⬛⬜
        0b01, // ⬛⬜
        0b01, // ⬛⬜
    },
    {   // 8
        0b11, // ⬜⬜
        0b11, // ⬜⬜
        0b00, // ⬛⬛
        0b11, // ⬜⬜
        0b11, // ⬜⬜
    },
    {   // 9
        0b11, // ⬜⬜
        0b11, // ⬜⬜
        0b11, // ⬜⬜
        0b01, // ⬛⬜
        0b11, // ⬜⬜
    },
};

// ASCII Characters 32--126
// Adapted from micropython/ports/nrf/boards/microbit/modules/microbitfont.h
const uint8_t pb_font_5x5[95][5] = {
    {   // space
        0b00000, // ⬛⬛⬛⬛⬛
        0b00000, // ⬛⬛⬛⬛⬛
        0b00000, // ⬛⬛⬛⬛⬛
        0b00000, // ⬛⬛⬛⬛⬛
        0b00000, // ⬛⬛⬛⬛⬛
    },
    {   // !
        0b01000, // ⬛⬜⬛⬛⬛
        0b01000, // ⬛⬜⬛⬛⬛
        0b01000, // ⬛⬜⬛⬛⬛
        0b00000, // ⬛⬛⬛⬛⬛
        0b01000, // ⬛⬜⬛⬛⬛
    },
    {   // "
        0b01010, // ⬛⬜⬛⬜⬛
        0b01010, // ⬛⬜⬛⬜⬛
        0b00000, // ⬛⬛⬛⬛⬛
        0b00000, // ⬛⬛⬛⬛⬛
        0b00000, // ⬛⬛⬛⬛⬛
    },
    {   // #
        0b01010, // ⬛⬜⬛⬜⬛
        0b11111, // ⬜⬜⬜⬜⬜
        0b01010, // ⬛⬜⬛⬜⬛
        0b11111, // ⬜⬜⬜⬜⬜
        0b01010, // ⬛⬜⬛⬜⬛
    },
    {   // $
        0b01110, // ⬛⬜⬜⬜⬛
        0b11001, // ⬜⬜⬛⬛⬜
        0b01110, // ⬛⬜⬜⬜⬛
        0b10011, // ⬜⬛⬛⬜⬜
        0b01110, // ⬛⬜⬜⬜⬛
    },
    {   // %
        0b11001, // ⬜⬜⬛⬛⬜
        0b10010, // ⬜⬛⬛⬜⬛
        0b00100, // ⬛⬛⬜⬛⬛
        0b01001, // ⬛⬜⬛⬛⬜
        0b10011, // ⬜⬛⬛⬜⬜
    },
    {   // &
        0b01100, // ⬛⬜⬜⬛⬛
        0b10010, // ⬜⬛⬛⬜⬛
        0b01100, // ⬛⬜⬜⬛⬛
        0b10010, // ⬜⬛⬛⬜⬛
        0b01101, // ⬛⬜⬜⬛⬜
    },
    {   // '
        0b01000, // ⬛⬜⬛⬛⬛
        0b01000, // ⬛⬜⬛⬛⬛
        0b00000, // ⬛⬛⬛⬛⬛
        0b00000, // ⬛⬛⬛⬛⬛
        0b00000, // ⬛⬛⬛⬛⬛
    },
    {   // (
        0b00100, // ⬛⬛⬜⬛⬛
        0b01000, // ⬛⬜⬛⬛⬛
        0b01000, // ⬛⬜⬛⬛⬛
        0b01000, // ⬛⬜⬛⬛⬛
        0b00100, // ⬛⬛⬜⬛⬛
    },
    {   // )
        0b01000, // ⬛⬜⬛⬛⬛
        0b00100, // ⬛⬛⬜⬛⬛
        0b00100, // ⬛⬛⬜⬛⬛
        0b00100, // ⬛⬛⬜⬛⬛
        0b01000, // ⬛⬜⬛⬛⬛
    },
    {   // *
        0b00000, // ⬛⬛⬛⬛⬛
        0b01010, // ⬛⬜⬛⬜⬛
        0b00100, // ⬛⬛⬜⬛⬛
        0b01010, // ⬛⬜⬛⬜⬛
        0b00000, // ⬛⬛⬛⬛⬛
    },
    {   // +
        0b00000, // ⬛⬛⬛⬛⬛
        0b00100, // ⬛⬛⬜⬛⬛
        0b01110, // ⬛⬜⬜⬜⬛
        0b00100, // ⬛⬛⬜⬛⬛
        0b00000, // ⬛⬛⬛⬛⬛
    },
    {   // ,
        0b00000, // ⬛⬛⬛⬛⬛
        0b00000, // ⬛⬛⬛⬛⬛
        0b00000, // ⬛⬛⬛⬛⬛
        0b00100, // ⬛⬛⬜⬛⬛
        0b01000, // ⬛⬜⬛⬛⬛
    },
    {   // -
        0b00000, // ⬛⬛⬛⬛⬛
        0b00000, // ⬛⬛⬛⬛⬛
        0b01110, // ⬛⬜⬜⬜⬛
        0b00000, // ⬛⬛⬛⬛⬛
        0b00000, // ⬛⬛⬛⬛⬛
    },
    {   // .
        0b00000, // ⬛⬛⬛⬛⬛
        0b00000, // ⬛⬛⬛⬛⬛
        0b00000, // ⬛⬛⬛⬛⬛
        0b01000, // ⬛⬜⬛⬛⬛
        0b00000, // ⬛⬛⬛⬛⬛
    },
    {   // /
        0b00001, // ⬛⬛⬛⬛⬜
        0b00010, // ⬛⬛⬛⬜⬛
        0b00100, // ⬛⬛⬜⬛⬛
        0b01000, // ⬛⬜⬛⬛⬛
        0b10000, // ⬜⬛⬛⬛⬛
    },
    {   // 0
        0b01100, // ⬛⬜⬜⬛⬛
        0b10010, // ⬜⬛⬛⬜⬛
        0b10010, // ⬜⬛⬛⬜⬛
        0b10010, // ⬜⬛⬛⬜⬛
        0b01100, // ⬛⬜⬜⬛⬛
    },
    {   // 1
        0b00100, // ⬛⬛⬜⬛⬛
        0b01100, // ⬛⬜⬜⬛⬛
        0b00100, // ⬛⬛⬜⬛⬛
        0b00100, // ⬛⬛⬜⬛⬛
        0b01110, // ⬛⬜⬜⬜⬛
    },
    {   // 2
        0b11100, // ⬜⬜⬜⬛⬛
        0b00010, // ⬛⬛⬛⬜⬛
        0b01100, // ⬛⬜⬜⬛⬛
        0b10000, // ⬜⬛⬛⬛⬛
        0b11110, // ⬜⬜⬜⬜⬛
    },
    {   // 3
        0b11110, // ⬜⬜⬜⬜⬛
        0b00010, // ⬛⬛⬛⬜⬛
        0b00100, // ⬛⬛⬜⬛⬛
        0b10010, // ⬜⬛⬛⬜⬛
        0b01100, // ⬛⬜⬜⬛⬛
    },
    {   // 4
        0b00110, // ⬛⬛⬜⬜⬛
        0b01010, // ⬛⬜⬛⬜⬛
        0b10010, // ⬜⬛⬛⬜⬛
        0b11111, // ⬜⬜⬜⬜⬜
        0b00010, // ⬛⬛⬛⬜⬛
    },
    {   // 5
        0b11111, // ⬜⬜⬜⬜⬜
        0b10000, // ⬜⬛⬛⬛⬛
        0b11110, // ⬜⬜⬜⬜⬛
        0b00001, // ⬛⬛⬛⬛⬜
        0b11110, // ⬜⬜⬜⬜⬛
    },
    {   // 6
        0b00010, // ⬛⬛⬛⬜⬛
        0b00100, // ⬛⬛⬜⬛⬛
        0b01110, // ⬛⬜⬜⬜⬛
        0b10001, // ⬜⬛⬛⬛⬜
        0b01110, // ⬛⬜⬜⬜⬛
    },
    {   // 7
        0b11111, // ⬜⬜⬜⬜⬜
        0b00010, // ⬛⬛⬛⬜⬛
        0b00100, // ⬛⬛⬜⬛⬛
        0b01000, // ⬛⬜⬛⬛⬛
        0b10000, // ⬜⬛⬛⬛⬛
    },
    {   // 8
        0b01110, // ⬛⬜⬜⬜⬛
        0b10001, // ⬜⬛⬛⬛⬜
        0b01110, // ⬛⬜⬜⬜⬛
        0b10001, // ⬜⬛⬛⬛⬜
        0b01110, // ⬛⬜⬜⬜⬛
    },
    {   // 9
        0b01110, // ⬛⬜⬜⬜⬛
        0b10001, // ⬜⬛⬛⬛⬜
        0b01110, // ⬛⬜⬜⬜⬛
        0b00100, // ⬛⬛⬜⬛⬛
        0b01000, // ⬛⬜⬛⬛⬛
    },
    {   // :
        0b00000, // ⬛⬛⬛⬛⬛
        0b01000, // ⬛⬜⬛⬛⬛
        0b00000, // ⬛⬛⬛⬛⬛
        0b01000, // ⬛⬜⬛⬛⬛
        0b00000, // ⬛⬛⬛⬛⬛
    },
    {   // ;
        0b00000, // ⬛⬛⬛⬛⬛
        0b00100, // ⬛⬛⬜⬛⬛
        0b00000, // ⬛⬛⬛⬛⬛
        0b00100, // ⬛⬛⬜⬛⬛
        0b01000, // ⬛⬜⬛⬛⬛
    },
    {   // <
        0b00010, // ⬛⬛⬛⬜⬛
        0b00100, // ⬛⬛⬜⬛⬛
        0b01000, // ⬛⬜⬛⬛⬛
        0b00100, // ⬛⬛⬜⬛⬛
        0b00010, // ⬛⬛⬛⬜⬛
    },
    {   // =
        0b00000, // ⬛⬛⬛⬛⬛
        0b01110, // ⬛⬜⬜⬜⬛
        0b00000, // ⬛⬛⬛⬛⬛
        0b01110, // ⬛⬜⬜⬜⬛
        0b00000, // ⬛⬛⬛⬛⬛
    },
    {   // >
        0b01000, // ⬛⬜⬛⬛⬛
        0b00100, // ⬛⬛⬜⬛⬛
        0b00010, // ⬛⬛⬛⬜⬛
        0b00100, // ⬛⬛⬜⬛⬛
        0b01000, // ⬛⬜⬛⬛⬛
    },
    {   // ?
        0b01110, // ⬛⬜⬜⬜⬛
        0b10001, // ⬜⬛⬛⬛⬜
        0b00110, // ⬛⬛⬜⬜⬛
        0b00000, // ⬛⬛⬛⬛⬛
        0b00100, // ⬛⬛⬜⬛⬛
    },
    {   // @
        0b01110, // ⬛⬜⬜⬜⬛
        0b10001, // ⬜⬛⬛⬛⬜
        0b10101, // ⬜⬛⬜⬛⬜
        0b10011, // ⬜⬛⬛⬜⬜
        0b01100, // ⬛⬜⬜⬛⬛
    },
    {   // A
        0b01100, // ⬛⬜⬜⬛⬛
        0b10010, // ⬜⬛⬛⬜⬛
        0b11110, // ⬜⬜⬜⬜⬛
        0b10010, // ⬜⬛⬛⬜⬛
        0b10010, // ⬜⬛⬛⬜⬛
    },
    {   // B
        0b11100, // ⬜⬜⬜⬛⬛
        0b10010, // ⬜⬛⬛⬜⬛
        0b11100, // ⬜⬜⬜⬛⬛
        0b10010, // ⬜⬛⬛⬜⬛
        0b11100, // ⬜⬜⬜⬛⬛
    },
    {   // C
        0b01110, // ⬛⬜⬜⬜⬛
        0b10000, // ⬜⬛⬛⬛⬛
        0b10000, // ⬜⬛⬛⬛⬛
        0b10000, // ⬜⬛⬛⬛⬛
        0b01110, // ⬛⬜⬜⬜⬛
    },
    {   // D
        0b11100, // ⬜⬜⬜⬛⬛
        0b10010, // ⬜⬛⬛⬜⬛
        0b10010, // ⬜⬛⬛⬜⬛
        0b10010, // ⬜⬛⬛⬜⬛
        0b11100, // ⬜⬜⬜⬛⬛
    },
    {   // E
        0b11110, // ⬜⬜⬜⬜⬛
        0b10000, // ⬜⬛⬛⬛⬛
        0b11100, // ⬜⬜⬜⬛⬛
        0b10000, // ⬜⬛⬛⬛⬛
        0b11110, // ⬜⬜⬜⬜⬛
    },
    {   // F
        0b11110, // ⬜⬜⬜⬜⬛
        0b10000, // ⬜⬛⬛⬛⬛
        0b11100, // ⬜⬜⬜⬛⬛
        0b10000, // ⬜⬛⬛⬛⬛
        0b10000, // ⬜⬛⬛⬛⬛
    },
    {   // G
        0b01110, // ⬛⬜⬜⬜⬛
        0b10000, // ⬜⬛⬛⬛⬛
        0b10011, // ⬜⬛⬛⬜⬜
        0b10001, // ⬜⬛⬛⬛⬜
        0b01110, // ⬛⬜⬜⬜⬛
    },
    {   // H
        0b10010, // ⬜⬛⬛⬜⬛
        0b10010, // ⬜⬛⬛⬜⬛
        0b11110, // ⬜⬜⬜⬜⬛
        0b10010, // ⬜⬛⬛⬜⬛
        0b10010, // ⬜⬛⬛⬜⬛
    },
    {   // I
        0b11100, // ⬜⬜⬜⬛⬛
        0b01000, // ⬛⬜⬛⬛⬛
        0b01000, // ⬛⬜⬛⬛⬛
        0b01000, // ⬛⬜⬛⬛⬛
        0b11100, // ⬜⬜⬜⬛⬛
    },
    {   // J
        0b11111, // ⬜⬜⬜⬜⬜
        0b00010, // ⬛⬛⬛⬜⬛
        0b00010, // ⬛⬛⬛⬜⬛
        0b10010, // ⬜⬛⬛⬜⬛
        0b01100, // ⬛⬜⬜⬛⬛
    },
    {   // K
        0b10010, // ⬜⬛⬛⬜⬛
        0b10100, // ⬜⬛⬜⬛⬛
        0b11000, // ⬜⬜⬛⬛⬛
        0b10100, // ⬜⬛⬜⬛⬛
        0b10010, // ⬜⬛⬛⬜⬛
    },
    {   // L
        0b10000, // ⬜⬛⬛⬛⬛
        0b10000, // ⬜⬛⬛⬛⬛
        0b10000, // ⬜⬛⬛⬛⬛
        0b10000, // ⬜⬛⬛⬛⬛
        0b11110, // ⬜⬜⬜⬜⬛
    },
    {   // M
        0b10001, // ⬜⬛⬛⬛⬜
        0b11011, // ⬜⬜⬛⬜⬜
        0b10101, // ⬜⬛⬜⬛⬜
        0b10001, // ⬜⬛⬛⬛⬜
        0b10001, // ⬜⬛⬛⬛⬜
    },
    {   // N
        0b10001, // ⬜⬛⬛⬛⬜
        0b11001, // ⬜⬜⬛⬛⬜
        0b10101, // ⬜⬛⬜⬛⬜
        0b10011, // ⬜⬛⬛⬜⬜
        0b10001, // ⬜⬛⬛⬛⬜
    },
    {   // O
        0b01100, // ⬛⬜⬜⬛⬛
        0b10010, // ⬜⬛⬛⬜⬛
        0b10010, // ⬜⬛⬛⬜⬛
        0b10010, // ⬜⬛⬛⬜⬛
        0b01100, // ⬛⬜⬜⬛⬛
    },
    {   // P
        0b11100, // ⬜⬜⬜⬛⬛
        0b10010, // ⬜⬛⬛⬜⬛
        0b11100, // ⬜⬜⬜⬛⬛
        0b10000, // ⬜⬛⬛⬛⬛
        0b10000, // ⬜⬛⬛⬛⬛
    },
    {   // Q
        0b01100, // ⬛⬜⬜⬛⬛
        0b10010, // ⬜⬛⬛⬜⬛
        0b10010, // ⬜⬛⬛⬜⬛
        0b01100, // ⬛⬜⬜⬛⬛
        0b00110, // ⬛⬛⬜⬜⬛
    },
    {   // R
        0b11100, // ⬜⬜⬜⬛⬛
        0b10010, // ⬜⬛⬛⬜⬛
        0b11100, // ⬜⬜⬜⬛⬛
        0b10010, // ⬜⬛⬛⬜⬛
        0b10001, // ⬜⬛⬛⬛⬜
    },
    {   // S
        0b01110, // ⬛⬜⬜⬜⬛
        0b10000, // ⬜⬛⬛⬛⬛
        0b01100, // ⬛⬜⬜⬛⬛
        0b00010, // ⬛⬛⬛⬜⬛
        0b11100, // ⬜⬜⬜⬛⬛
    },
    {   // T
        0b11111, // ⬜⬜⬜⬜⬜
        0b00100, // ⬛⬛⬜⬛⬛
        0b00100, // ⬛⬛⬜⬛⬛
        0b00100, // ⬛⬛⬜⬛⬛
        0b00100, // ⬛⬛⬜⬛⬛
    },
    {   // U
        0b10010, // ⬜⬛⬛⬜⬛
        0b10010, // ⬜⬛⬛⬜⬛
        0b10010, // ⬜⬛⬛⬜⬛
        0b10010, // ⬜⬛⬛⬜⬛
        0b01100, // ⬛⬜⬜⬛⬛
    },
    {   // V
        0b10001, // ⬜⬛⬛⬛⬜
        0b10001, // ⬜⬛⬛⬛⬜
        0b10001, // ⬜⬛⬛⬛⬜
        0b01010, // ⬛⬜⬛⬜⬛
        0b00100, // ⬛⬛⬜⬛⬛
    },
    {   // W
        0b10001, // ⬜⬛⬛⬛⬜
        0b10001, // ⬜⬛⬛⬛⬜
        0b10101, // ⬜⬛⬜⬛⬜
        0b11011, // ⬜⬜⬛⬜⬜
        0b10001, // ⬜⬛⬛⬛⬜
    },
    {   // X
        0b10010, // ⬜⬛⬛⬜⬛
        0b10010, // ⬜⬛⬛⬜⬛
        0b01100, // ⬛⬜⬜⬛⬛
        0b10010, // ⬜⬛⬛⬜⬛
        0b10010, // ⬜⬛⬛⬜⬛
    },
    {   // Y
        0b10001, // ⬜⬛⬛⬛⬜
        0b01010, // ⬛⬜⬛⬜⬛
        0b00100, // ⬛⬛⬜⬛⬛
        0b00100, // ⬛⬛⬜⬛⬛
        0b00100, // ⬛⬛⬜⬛⬛
    },
    {   // Z
        0b11110, // ⬜⬜⬜⬜⬛
        0b00100, // ⬛⬛⬜⬛⬛
        0b01000, // ⬛⬜⬛⬛⬛
        0b10000, // ⬜⬛⬛⬛⬛
        0b11110, // ⬜⬜⬜⬜⬛
    },
    {   // [
        0b01110, // ⬛⬜⬜⬜⬛
        0b01000, // ⬛⬜⬛⬛⬛
        0b01000, // ⬛⬜⬛⬛⬛
        0b01000, // ⬛⬜⬛⬛⬛
        0b01110, // ⬛⬜⬜⬜⬛
    },
    {   // backslash
        0b10000, // ⬜⬛⬛⬛⬛
        0b01000, // ⬛⬜⬛⬛⬛
        0b00100, // ⬛⬛⬜⬛⬛
        0b00010, // ⬛⬛⬛⬜⬛
        0b00001, // ⬛⬛⬛⬛⬜
    },
    {   // ]
        0b01110, // ⬛⬜⬜⬜⬛
        0b00010, // ⬛⬛⬛⬜⬛
        0b00010, // ⬛⬛⬛⬜⬛
        0b00010, // ⬛⬛⬛⬜⬛
        0b01110, // ⬛⬜⬜⬜⬛
    },
    {   // ^
        0b00100, // ⬛⬛⬜⬛⬛
        0b01010, // ⬛⬜⬛⬜⬛
        0b00000, // ⬛⬛⬛⬛⬛
        0b00000, // ⬛⬛⬛⬛⬛
        0b00000, // ⬛⬛⬛⬛⬛
    },
    {   // _
        0b00000, // ⬛⬛⬛⬛⬛
        0b00000, // ⬛⬛⬛⬛⬛
        0b00000, // ⬛⬛⬛⬛⬛
        0b00000, // ⬛⬛⬛⬛⬛
        0b11111, // ⬜⬜⬜⬜⬜
    },
    {   // `
        0b01000, // ⬛⬜⬛⬛⬛
        0b00100, // ⬛⬛⬜⬛⬛
        0b00000, // ⬛⬛⬛⬛⬛
        0b00000, // ⬛⬛⬛⬛⬛
        0b00000, // ⬛⬛⬛⬛⬛
    },
    {   // a
        0b00000, // ⬛⬛⬛⬛⬛
        0b01110, // ⬛⬜⬜⬜⬛
        0b10010, // ⬜⬛⬛⬜⬛
        0b10010, // ⬜⬛⬛⬜⬛
        0b01111, // ⬛⬜⬜⬜⬜
    },
    {   // b
        0b10000, // ⬜⬛⬛⬛⬛
        0b10000, // ⬜⬛⬛⬛⬛
        0b11100, // ⬜⬜⬜⬛⬛
        0b10010, // ⬜⬛⬛⬜⬛
        0b11100, // ⬜⬜⬜⬛⬛
    },
    {   // c
        0b00000, // ⬛⬛⬛⬛⬛
        0b01110, // ⬛⬜⬜⬜⬛
        0b10000, // ⬜⬛⬛⬛⬛
        0b10000, // ⬜⬛⬛⬛⬛
        0b01110, // ⬛⬜⬜⬜⬛
    },
    {   // d
        0b00010, // ⬛⬛⬛⬜⬛
        0b00010, // ⬛⬛⬛⬜⬛
        0b01110, // ⬛⬜⬜⬜⬛
        0b10010, // ⬜⬛⬛⬜⬛
        0b01110, // ⬛⬜⬜⬜⬛
    },
    {   // e
        0b01100, // ⬛⬜⬜⬛⬛
        0b10010, // ⬜⬛⬛⬜⬛
        0b11100, // ⬜⬜⬜⬛⬛
        0b10000, // ⬜⬛⬛⬛⬛
        0b01110, // ⬛⬜⬜⬜⬛
    },
    {   // f
        0b00110, // ⬛⬛⬜⬜⬛
        0b01000, // ⬛⬜⬛⬛⬛
        0b11100, // ⬜⬜⬜⬛⬛
        0b01000, // ⬛⬜⬛⬛⬛
        0b01000, // ⬛⬜⬛⬛⬛
    },
    {   // g
        0b01110, // ⬛⬜⬜⬜⬛
        0b10010, // ⬜⬛⬛⬜⬛
        0b01110, // ⬛⬜⬜⬜⬛
        0b00010, // ⬛⬛⬛⬜⬛
        0b01100, // ⬛⬜⬜⬛⬛
    },
    {   // h
        0b10000, // ⬜⬛⬛⬛⬛
        0b10000, // ⬜⬛⬛⬛⬛
        0b11100, // ⬜⬜⬜⬛⬛
        0b10010, // ⬜⬛⬛⬜⬛
        0b10010, // ⬜⬛⬛⬜⬛
    },
    {   // i
        0b01000, // ⬛⬜⬛⬛⬛
        0b00000, // ⬛⬛⬛⬛⬛
        0b01000, // ⬛⬜⬛⬛⬛
        0b01000, // ⬛⬜⬛⬛⬛
        0b01000, // ⬛⬜⬛⬛⬛
    },
    {   // j
        0b00010, // ⬛⬛⬛⬜⬛
        0b00000, // ⬛⬛⬛⬛⬛
        0b00010, // ⬛⬛⬛⬜⬛
        0b00010, // ⬛⬛⬛⬜⬛
        0b01100, // ⬛⬜⬜⬛⬛
    },
    {   // k
        0b10000, // ⬜⬛⬛⬛⬛
        0b10100, // ⬜⬛⬜⬛⬛
        0b11000, // ⬜⬜⬛⬛⬛
        0b10100, // ⬜⬛⬜⬛⬛
        0b10010, // ⬜⬛⬛⬜⬛
    },
    {   // l
        0b01000, // ⬛⬜⬛⬛⬛
        0b01000, // ⬛⬜⬛⬛⬛
        0b01000, // ⬛⬜⬛⬛⬛
        0b01000, // ⬛⬜⬛⬛⬛
        0b00110, // ⬛⬛⬜⬜⬛
    },
    {   // m
        0b00000, // ⬛⬛⬛⬛⬛
        0b11011, // ⬜⬜⬛⬜⬜
        0b10101, // ⬜⬛⬜⬛⬜
        0b10001, // ⬜⬛⬛⬛⬜
        0b10001, // ⬜⬛⬛⬛⬜
    },
    {   // n
        0b00000, // ⬛⬛⬛⬛⬛
        0b11100, // ⬜⬜⬜⬛⬛
        0b10010, // ⬜⬛⬛⬜⬛
        0b10010, // ⬜⬛⬛⬜⬛
        0b10010, // ⬜⬛⬛⬜⬛
    },
    {   // o
        0b00000, // ⬛⬛⬛⬛⬛
        0b01100, // ⬛⬜⬜⬛⬛
        0b10010, // ⬜⬛⬛⬜⬛
        0b10010, // ⬜⬛⬛⬜⬛
        0b01100, // ⬛⬜⬜⬛⬛
    },
    {   // p
        0b00000, // ⬛⬛⬛⬛⬛
        0b11100, // ⬜⬜⬜⬛⬛
        0b10010, // ⬜⬛⬛⬜⬛
        0b11100, // ⬜⬜⬜⬛⬛
        0b10000, // ⬜⬛⬛⬛⬛
    },
    {   // q
        0b00000, // ⬛⬛⬛⬛⬛
        0b01110, // ⬛⬜⬜⬜⬛
        0b10010, // ⬜⬛⬛⬜⬛
        0b01110, // ⬛⬜⬜⬜⬛
        0b00010, // ⬛⬛⬛⬜⬛
    },
    {   // r
        0b00000, // ⬛⬛⬛⬛⬛
        0b01110, // ⬛⬜⬜⬜⬛
        0b10000, // ⬜⬛⬛⬛⬛
        0b10000, // ⬜⬛⬛⬛⬛
        0b10000, // ⬜⬛⬛⬛⬛
    },
    {   // s
        0b00000, // ⬛⬛⬛⬛⬛
        0b00110, // ⬛⬛⬜⬜⬛
        0b01000, // ⬛⬜⬛⬛⬛
        0b00100, // ⬛⬛⬜⬛⬛
        0b11000, // ⬜⬜⬛⬛⬛
    },
    {   // t
        0b01000, // ⬛⬜⬛⬛⬛
        0b01000, // ⬛⬜⬛⬛⬛
        0b01110, // ⬛⬜⬜⬜⬛
        0b01000, // ⬛⬜⬛⬛⬛
        0b00111, // ⬛⬛⬜⬜⬜
    },
    {   // u
        0b00000, // ⬛⬛⬛⬛⬛
        0b10010, // ⬜⬛⬛⬜⬛
        0b10010, // ⬜⬛⬛⬜⬛
        0b10010, // ⬜⬛⬛⬜⬛
        0b01111, // ⬛⬜⬜⬜⬜
    },
    {   // v
        0b00000, // ⬛⬛⬛⬛⬛
        0b10001, // ⬜⬛⬛⬛⬜
        0b10001, // ⬜⬛⬛⬛⬜
        0b01010, // ⬛⬜⬛⬜⬛
        0b00100, // ⬛⬛⬜⬛⬛
    },
    {   // w
        0b00000, // ⬛⬛⬛⬛⬛
        0b10001, // ⬜⬛⬛⬛⬜
        0b10001, // ⬜⬛⬛⬛⬜
        0b10101, // ⬜⬛⬜⬛⬜
        0b11011, // ⬜⬜⬛⬜⬜
    },
    {   // x
        0b00000, // ⬛⬛⬛⬛⬛
        0b10010, // ⬜⬛⬛⬜⬛
        0b01100, // ⬛⬜⬜⬛⬛
        0b01100, // ⬛⬜⬜⬛⬛
        0b10010, // ⬜⬛⬛⬜⬛
    },
    {   // y
        0b00000, // ⬛⬛⬛⬛⬛
        0b10001, // ⬜⬛⬛⬛⬜
        0b01010, // ⬛⬜⬛⬜⬛
        0b00100, // ⬛⬛⬜⬛⬛
        0b11000, // ⬜⬜⬛⬛⬛
    },
    {   // z
        0b00000, // ⬛⬛⬛⬛⬛
        0b11110, // ⬜⬜⬜⬜⬛
        0b00100, // ⬛⬛⬜⬛⬛
        0b01000, // ⬛⬜⬛⬛⬛
        0b11110, // ⬜⬜⬜⬜⬛
    },
    {   // {
        0b00110, // ⬛⬛⬜⬜⬛
        0b00100, // ⬛⬛⬜⬛⬛
        0b01100, // ⬛⬜⬜⬛⬛
        0b00100, // ⬛⬛⬜⬛⬛
        0b00110, // ⬛⬛⬜⬜⬛
    },
    {   // |
        0b01000, // ⬛⬜⬛⬛⬛
        0b01000, // ⬛⬜⬛⬛⬛
        0b01000, // ⬛⬜⬛⬛⬛
        0b01000, // ⬛⬜⬛⬛⬛
        0b01000, // ⬛⬜⬛⬛⬛
    },
    {   // }
        0b11000, // ⬜⬜⬛⬛⬛
        0b01000, // ⬛⬜⬛⬛⬛
        0b01100, // ⬛⬜⬜⬛⬛
        0b01000, // ⬛⬜⬛⬛⬛
        0b11000, // ⬜⬜⬛⬛⬛
    },
    {   // ~
        0b00000, // ⬛⬛⬛⬛⬛
        0b00000, // ⬛⬛⬛⬛⬛
        0b01100, // ⬛⬜⬜⬛⬛
        0b00011, // ⬛⬛⬛⬜⬜
        0b00000, // ⬛⬛⬛⬛⬛
    },
};

// pybricks._common.LightMatrix.orientation
pbio_error_t hub_display_orientation(uint8_t up) {
//STATIC mp_obj_t common_Lightmatrix_orientation(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
//    PB_PARSE_ARGS_METHOD(n_args, pos_args, kw_args,
//        common_Lightmatrix_obj_t, self,
//        PB_ARG_REQUIRED(up));

    pbio_light_matrix_set_orientation(pbsys_hub_light_matrix, up);

    //return mp_const_none;
    return PBIO_SUCCESS;
}

// pybricks._common.LightMatrix.off
pbio_error_t hub_display_off(void) {
//STATIC mp_obj_t common_Lightmatrix_off(mp_obj_t self_in) {
//    common_Lightmatrix_obj_t *self = MP_OBJ_TO_PTR(self_in);

    check_pbio_error(pbio_light_matrix_clear(pbsys_hub_light_matrix));

//    return mp_const_none;
    return PBIO_SUCCESS;
}

// pybricks._common.LightMatrix.pixel
pbio_error_t hub_display_pixel(uint8_t row, uint8_t column, uint8_t brightness) {
//STATIC mp_obj_t common_Lightmatrix_pixel(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
//    PB_PARSE_ARGS_METHOD(n_args, pos_args, kw_args,
//        common_Lightmatrix_obj_t, self,
//       PB_ARG_REQUIRED(row),
//        PB_ARG_REQUIRED(column),
//        PB_ARG_DEFAULT_INT(brightness, 100));

    // Set pixel at the given brightness
    //pb_assert(pbio_light_matrix_set_pixel(self->light_matrix, pb_obj_get_int(row_in), pb_obj_get_int(column_in), pb_obj_get_pct(brightness_in)));
    check_pbio_error(pbio_light_matrix_set_pixel(pbsys_hub_light_matrix, row, column, brightness));

    //return mp_const_none;
    return PBIO_SUCCESS;
}

#if 0
STATIC void common_Lightmatrix__renew(common_Lightmatrix_obj_t *self, uint8_t frames) {    
    void common_Lightmatrix__renew(uint8_t frames) {
    // Matrix with/height
    size_t size = pbio_light_matrix_get_size(pbsys_hub_light_matrix);

    // Renew buffer for new number of frames
    pbsys_hub_light_matrix->data = m_renew(uint8_t, self->data, size * size * self->frames, size * size * frames);

    // Save new number of frames
    self->frames = frames;
}

static void common_Lightmatrix_image__extract(mp_obj_t image_in, size_t size, uint8_t *data) {
    #if MICROPY_PY_BUILTINS_FLOAT
    // If image is a matrix, copy data from there
    if (mp_obj_is_type(image_in, &pb_type_Matrix)) {
        for (size_t r = 0; r < size; r++) {
            for (size_t c = 0; c < size; c++) {
                float scalar = pb_type_Matrix_get_scalar(image_in, r, c);
                scalar = scalar > 100 ? 100 : (scalar < 0 ? 0: scalar);
                data[r * size + c] = (uint8_t)scalar;
            }
        }
        return;
    }
    #endif // MICROPY_PY_BUILTINS_FLOAT

    // Unpack the main list of rows and get the requested sizes
    mp_obj_t *row_objs, *scalar_objs;
    size_t m;
    mp_obj_get_array(image_in, &m, &row_objs);
    if (m != size) {
        pb_assert(PBIO_ERROR_INVALID_ARG);
    }

    // Iterate through each of the rows to get the scalars
    for (size_t r = 0; r < size; r++) {
        size_t n;
        mp_obj_get_array(row_objs[r], &n, &scalar_objs);
        if (n != size) {
            pb_assert(PBIO_ERROR_INVALID_ARG);
        }
        // Unpack the scalars
        for (size_t c = 0; c < size; c++) {
            data[r * size + c] = pb_obj_get_pct(scalar_objs[c]);
        }
    }
}
#endif

// pybricks._common.LightMatrix.image
pbio_error_t hub_display_image(uint8_t* image) {
//STATIC mp_obj_t common_Lightmatrix_image(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
//    PB_PARSE_ARGS_METHOD(n_args, pos_args, kw_args,
//        common_Lightmatrix_obj_t, self,
//        PB_ARG_REQUIRED(image));

    // Allocate and extract image data
//    size_t size = pbio_light_matrix_get_size(self->light_matrix);
//    size_t size = pbio_light_matrix_get_size(pbsys_hub_light_matrix);
//    common_Lightmatrix__renew(self, 1);
//    uint8_t *data = common_Lightmatrix_image__extract(image, size);

    // Display the image
    //pb_assert(pbio_light_matrix_set_image(self->light_matrix, self->data));
    check_pbio_error(pbio_light_matrix_set_image(pbsys_hub_light_matrix, image));
    
//    return mp_const_none;
    return PBIO_SUCCESS;
}

/*
 * We implement light API by just wrapping functions in external/libpybricks/lib/pbio/src/light/color_light.c.
 * However, we don't support animation functions currently.
 */
pbio_error_t hub_display_number(const int8_t num) {

//    PB_PARSE_ARGS_METHOD(n_args, pos_args, kw_args,
//        common_Lightmatrix_obj_t, self,
//        PB_ARG_REQUIRED(number));

//    uint8_t size = pbio_light_matrix_get_size(self->light_matrix);
    uint8_t size = pbio_light_matrix_get_size(pbsys_hub_light_matrix);

    // Currently numbers are only implemented for 5x5 matrices
    if (size != 5) {
//        pb_assert(PBIO_ERROR_NOT_IMPLEMENTED);
        return PBIO_ERROR_NOT_IMPLEMENTED;
    }

    // Get the number
//    mp_int_t number = pb_obj_get_int(number_in);
    int8_t number = num;

    // > 99 gets displayed as >
    if (number > 99) {
//        pb_assert(pbio_light_matrix_set_rows(self->light_matrix, pb_font_5x5['>' - 32]));
//        check_pbio_error(pbio_light_matrix_set_rows(self->light_matrix, pb_font_5x5['>' - 32]));
        check_pbio_error(pbio_light_matrix_set_rows(pbsys_hub_light_matrix, pb_font_5x5['>' - 32]));
//        return mp_const_none;
          return PBIO_SUCCESS;
    }

    // < -99 gets displayed as <
    if (number < -99) {
//        pb_assert(pbio_light_matrix_set_rows(self->light_matrix, pb_font_5x5['<' - 32]));
//        check_pbio_errror(pbio_light_matrix_set_rows(self->light_matrix, pb_font_5x5['<' - 32]));
        check_pbio_error(pbio_light_matrix_set_rows(pbsys_hub_light_matrix, pb_font_5x5['<' - 32]));
//        return mp_const_none;
        return PBIO_SUCCESS;
    }

    // Remember sign but make value positive
    bool negative = number < 0;
    if (negative) {
        number = -number;
    }

    // Compose number as two digits
    uint8_t composite[5];
    uint8_t i;
    for (i = 0; i < 5; i++) {
        composite[i] = pb_digits_5x2[number / 10][i] << 3 | pb_digits_5x2[number % 10][i];
    }

    // Display the result
//    pb_assert(pbio_light_matrix_set_rows(self->light_matrix, composite));
//    check_pbio_error(pbio_light_matrix_set_rows(self->light_matrix, composite));
    check_pbio_error(pbio_light_matrix_set_rows(pbsys_hub_light_matrix, composite));

    // Display one faint dot in the middle to indicate negative
    if (negative) {
        //pb_assert(pbio_light_matrix_set_pixel(self->light_matrix, 2, 2, 50));
//        check_pbio_error(pbio_light_matrix_set_pixel(self->light_matrix, 2, 2, 50));
        check_pbio_error(pbio_light_matrix_set_pixel(pbsys_hub_light_matrix, 2, 2, 50));
    }

//    return mp_const_none;
    return PBIO_SUCCESS;
}


// pybricks._common.LightMatrix.char
pbio_error_t hub_display_char(const char c) {
//STATIC mp_obj_t common_Lightmatrix_char(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
//    PB_PARSE_ARGS_METHOD(n_args, pos_args, kw_args,
//        common_Lightmatrix_obj_t, self,
//        PB_ARG_REQUIRED(char));

    // Argument must be a qstring or string
//    if (!mp_obj_is_qstr(char_in)) {
//        pb_assert_type(char_in, &mp_type_str);
//    }

    // Assert that the input is a single character
//    GET_STR_DATA_LEN(char_in, text, text_len);
//    if (text_len != 1 || text[0] < 32 || text[0] > 126) {
//        pb_assert(PBIO_ERROR_INVALID_ARG);
//    }

    // Pick corresponding image and display it
//    pb_assert(pbio_light_matrix_set_rows(self->light_matrix, pb_font_5x5[text[0] - 32]));
    check_pbio_error(pbio_light_matrix_set_rows(pbsys_hub_light_matrix, pb_font_5x5[c - 32]));

//    return mp_const_none;
    return PBIO_SUCCESS;
}

// pybricks._common.LightMatrix.text
pbio_error_t hub_display_text(const char* text, uint32_t on, uint32_t off) {
//STATIC mp_obj_t common_Lightmatrix_text(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
//    PB_PARSE_ARGS_METHOD(n_args, pos_args, kw_args,
//        common_Lightmatrix_obj_t, self,
//        PB_ARG_REQUIRED(text),
//        PB_ARG_DEFAULT_INT(on, 500),
//        PB_ARG_DEFAULT_INT(off, 50));

    // Assert that the input is a single text
  //  GET_STR_DATA_LEN(text_in, text, text_len);
    uint8_t i;
    for(i = 0; text[i] != '\0'; ++i);
    const int text_len = i;

    // Make sure all characters are valid
    for (i = 0; i < text_len; i++) {
        if (text[0] < 32 || text[0] > 126) {
//            pb_assert(PBIO_ERROR_INVALID_ARG);
            return PBIO_ERROR_INVALID_ARG;
        }
    }

//    mp_int_t on = pb_obj_get_int(on_in);
//    mp_int_t off = pb_obj_get_int(off_in);

    // Display all characters one by one
    for (i = 0; i < text_len; i++) {
        //pb_assert(pbio_light_matrix_set_rows(self->light_matrix, pb_font_5x5[text[i] - 32]));
        check_pbio_error(pbio_light_matrix_set_rows(pbsys_hub_light_matrix, pb_font_5x5[text[i] - 32]));
        //mp_hal_delay_ms(on);
        dly_tsk(on * 1000);

        // Some off time so we can see multiple of the same characters
        if (off > 0 || i == text_len - 1) {
            //pb_assert(pbio_light_matrix_clear(self->light_matrix));
            check_pbio_error(pbio_light_matrix_clear(pbsys_hub_light_matrix));
            //mp_hal_delay_ms(off);
            dly_tsk(off * 1000);
        }
    }

    //return mp_const_none;
    return PBIO_SUCCESS;
}
/*
pbio_error_t hub_light_on_color(pbio_color_t color) {
  return pbio_color_light_on(pbsys_status_light, color);
}

pbio_error_t hub_light_off(void) {
  return pbio_color_light_off(pbsys_status_light);
}
*/
