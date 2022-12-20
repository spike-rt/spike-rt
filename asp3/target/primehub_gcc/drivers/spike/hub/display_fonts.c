// SPDX-License-Identifier: MIT

// Adapted from external/libpybricks/pybricks/common/pb_type_lightmatrix_fonts.c

// Copyright (c) 2016 British Broadcasting Corporation
// Copyright (c) 2018-2020 The Pybricks Authors

#include <t_syslog.h>
#include <t_stddef.h>

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