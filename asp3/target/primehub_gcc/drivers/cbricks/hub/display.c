// SPDX-License-Identifier: MIT
/*
 * API for the hub built-in display.
 *
 * Copyright (c) 2022 Embedded and Real-Time Systems Laboratory,
 *            Graduate School of Information Science, Nagoya Univ., JAPAN
 */

#include <t_syslog.h>
#include <cbricks/hub/display.h>

#include <pbsys/light.h>
#include <pbio/light_matrix.h>

#include <pup_device.h>
#include <cb_error.h>

/*
 * We implement display API by just wrapping functions in external/libpybricks/lib/pbio/src/light/light_matrix.c.
 * However, we don't support animation functions currently.
 */

// Declarations of fonts defined in display_fonts,c
extern const uint8_t pb_digits_5x2[10][5];
extern const uint8_t pb_font_5x5[95][5];

pbio_error_t hub_display_orientation(uint8_t up) {
    pbio_light_matrix_set_orientation(pbsys_hub_light_matrix, up);

    return PBIO_SUCCESS;
}

pbio_error_t hub_display_off(void) {
    check_pbio_error(pbio_light_matrix_clear(pbsys_hub_light_matrix));

    return PBIO_SUCCESS;
}

pbio_error_t hub_display_pixel(uint8_t row, uint8_t column, uint8_t brightness) {
    // Set pixel at the given brightness
    check_pbio_error(pbio_light_matrix_set_pixel(pbsys_hub_light_matrix, row, column, brightness));

    return PBIO_SUCCESS;
}

pbio_error_t hub_display_image(uint8_t* image) {
    // Display the image
    check_pbio_error(pbio_light_matrix_set_image(pbsys_hub_light_matrix, image));

    return PBIO_SUCCESS;
}

pbio_error_t hub_display_number(const int8_t num) {
    uint8_t size = pbio_light_matrix_get_size(pbsys_hub_light_matrix);

    // Currently numbers are only implemented for 5x5 matrices
    if (size != 5) {
        return PBIO_ERROR_NOT_IMPLEMENTED;
    }

    // Get the number
    int8_t number = num;

    // > 99 gets displayed as >
    if (number > 99) {
        check_pbio_error(pbio_light_matrix_set_rows(pbsys_hub_light_matrix, pb_font_5x5['>' - 32]));
        return PBIO_SUCCESS;
    }

    // < -99 gets displayed as <
    if (number < -99) {
        check_pbio_error(pbio_light_matrix_set_rows(pbsys_hub_light_matrix, pb_font_5x5['<' - 32]));
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
    check_pbio_error(pbio_light_matrix_set_rows(pbsys_hub_light_matrix, composite));

    // Display one faint dot in the middle to indicate negative
    if (negative) {
        check_pbio_error(pbio_light_matrix_set_pixel(pbsys_hub_light_matrix, 2, 2, 50));
    }

    return PBIO_SUCCESS;
}

pbio_error_t hub_display_char(const char c) {
    check_pbio_error(pbio_light_matrix_set_rows(pbsys_hub_light_matrix, pb_font_5x5[c - 32]));

    return PBIO_SUCCESS;
}

pbio_error_t hub_display_text(const char* text, uint32_t on, uint32_t off) {
    uint8_t i;
    for(i = 0; text[i] != '\0'; ++i);
    const int text_len = i;

    // Make sure all characters are valid
    for (i = 0; i < text_len; i++) {
        if (text[0] < 32 || text[0] > 126) {
            return PBIO_ERROR_INVALID_ARG;
        }
    }

    // Display all characters one by one
    for (i = 0; i < text_len; i++) {
        check_pbio_error(pbio_light_matrix_set_rows(pbsys_hub_light_matrix, pb_font_5x5[text[i] - 32]));
        dly_tsk(on * 1000);

        // Some off time so we can see multiple of the same characters
        if (off > 0 || i == text_len - 1) {
            check_pbio_error(pbio_light_matrix_clear(pbsys_hub_light_matrix));
            dly_tsk(off * 1000);
        }
    }

    return PBIO_SUCCESS;
}
