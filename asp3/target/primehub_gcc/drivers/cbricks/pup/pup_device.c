// SPDX-License-Identifier: MIT
/*
 * Common codes for controlling PUP devices.
 * Based on https://github.com/pybricks/pybricks-micropython/blob/master/pybricks/util_pb/pb_device_stm32.c
 *
 * Copyright (c) 2018-2021 The Pybricks Authors
 * Modifications for SPIKE-RT Copyright (c) 2022 Embedded and Real-Time Systems Laboratory,
 *                                               Graduate School of Information Science, Nagoya Univ., JAPAN
 */

#include <kernel.h>
#include <t_syslog.h>

#include <pbdrv/ioport.h>
#include <pbdrv/motor.h>
#include <pbio/color.h>
#include <pbio/iodev.h>

#include <pup_device.h>
#include <cb_error.h>

struct _pup_device_t {
    pbio_iodev_t iodev;
};

static pbio_error_t wait(pbio_error_t (*end)(pbio_iodev_t *), void (*cancel)(pbio_iodev_t *), pbio_iodev_t *iodev) {
    pbio_error_t err;

    while ((err = end(iodev)) == PBIO_ERROR_AGAIN) {
        // Wait 1 ms
        dly_tsk(1000);
    }
    
    if (err != PBIO_SUCCESS) {
        cancel(iodev);
        while (end(iodev) == PBIO_ERROR_AGAIN) {
            // Wait 1 ms
            dly_tsk(1000);
        }
    }

    return err;
}


// Get the required mode switch time delay for a given sensor type and/or mode
static uint32_t get_mode_switch_delay(pbio_iodev_type_id_t id, uint8_t mode) {
    switch (id) {
        case PBIO_IODEV_TYPE_ID_COLOR_DIST_SENSOR:
            return 30;
        case PBIO_IODEV_TYPE_ID_SPIKE_COLOR_SENSOR:
            return 30;
        case PBIO_IODEV_TYPE_ID_SPIKE_ULTRASONIC_SENSOR:
            return 50;
        // Default delay for other sensors and modes:
        default:
            return 0;
    }
}

static pbio_error_t set_mode(pbio_iodev_t *iodev, uint8_t new_mode) {
    pbio_error_t err;

    if (iodev->mode == new_mode) {
        return;
    }
    
    while ((err = pbio_iodev_set_mode_begin(iodev, new_mode)) == PBIO_ERROR_AGAIN) {
        // Wait 1 ms
        dly_tsk(1000);
    }
    check_pbio_error(err);

    wait(pbio_iodev_set_mode_end, pbio_iodev_set_mode_cancel, iodev);

    // Give some time for the mode to take effect and discard stale data
    uint32_t delay = get_mode_switch_delay(iodev->info->type_id, new_mode);
    if (delay > 0) {
        // Wait delay ms
        dly_tsk(delay*1000);
    }

    return PBIO_SUCCESS;
}

pup_device_t *pup_device_get_device(pbio_port_id_t port, pbio_iodev_type_id_t valid_id) {
    // Get the iodevice
    pbio_iodev_t *iodev;
    pbio_error_t err;

    // Set up device
    while ((err = pbdrv_ioport_get_iodev(port, &iodev)) == PBIO_ERROR_AGAIN) {
        // Wait 50 ms
        dly_tsk(50*1000);
    }
    check_pbio_error_r(err, NULL);

    // Verify the ID or always allow generic LUMP device
    if (iodev->info->type_id != valid_id && valid_id != PBIO_IODEV_TYPE_ID_LUMP_UART) {
        check_pbio_error_r(PBIO_ERROR_NO_DEV, NULL);
    }

    iodev->port = port;
    return (pup_device_t *)iodev;
}

pbio_error_t pup_device_get_values(pup_device_t *pdev, uint8_t mode, int32_t *values) {

    pbio_iodev_t *iodev = &pdev->iodev;

    uint8_t *data;
    uint8_t len;
    pbio_iodev_data_type_t type;

    check_pbio_error(set_mode(iodev, mode));

    check_pbio_error(pbio_iodev_get_data(iodev, &data));
    check_pbio_error(pbio_iodev_get_data_format(iodev, iodev->mode, &len, &type));

    if (len == 0) {
        return PBIO_ERROR_IO;
    }

    for (uint8_t i = 0; i < len; i++) {
        switch (type & PBIO_IODEV_DATA_TYPE_MASK) {
            case PBIO_IODEV_DATA_TYPE_INT8:
                values[i] = *((int8_t *)(data + i * 1));
                break;
            case PBIO_IODEV_DATA_TYPE_INT16:
                values[i] = *((int16_t *)(data + i * 2));
                break;
            case PBIO_IODEV_DATA_TYPE_INT32:
                values[i] = *((int32_t *)(data + i * 4));
                break;
            case PBIO_IODEV_DATA_TYPE_FLOAT:
                *(float *)(values + i) = *((float *)(data + i * 4));
                break;
            default:
                return PBIO_ERROR_IO;
        }
    }
    return PBIO_SUCCESS;
}

pbio_error_t pup_device_set_values(pup_device_t *pdev, uint8_t mode, int32_t *values, uint8_t num_values) {

    pbio_iodev_t *iodev = &pdev->iodev;

    uint8_t data[PBIO_IODEV_MAX_DATA_SIZE];
    uint8_t len;
    pbio_iodev_data_type_t type;
    pbio_error_t err;

    check_pbio_error(set_mode(iodev, mode));

    check_pbio_error(pbio_iodev_get_data_format(iodev, iodev->mode, &len, &type));

    if (len != num_values) {
        return PBIO_ERROR_INVALID_ARG;
    }

    for (uint8_t i = 0; i < len; i++) {
        switch (type & PBIO_IODEV_DATA_TYPE_MASK) {
            case PBIO_IODEV_DATA_TYPE_INT8:
                *(int8_t *)(data + i) = values[i];
                break;
            case PBIO_IODEV_DATA_TYPE_INT16:
                *(int16_t *)(data + i * 2) = values[i];
                break;
            case PBIO_IODEV_DATA_TYPE_INT32:
                *(int32_t *)(data + i * 4) = values[i];
                break;
            case PBIO_IODEV_DATA_TYPE_FLOAT:
                *(float *)(data + i * 4) = values[i];
                break;
            default:
                return PBIO_ERROR_IO;
        }
    }

    while ((err = pbio_iodev_set_data_begin(iodev, iodev->mode, data)) == PBIO_ERROR_AGAIN) {
        // Wait 1 ms
        dly_tsk(1000);
    }
    check_pbio_error(err);

    wait(pbio_iodev_set_data_end, pbio_iodev_set_data_cancel, iodev);

    // Give some time for the set values to take effect
    uint32_t delay = get_mode_switch_delay(iodev->info->type_id, mode);
    if (delay > 0) {
        // Wait delay/10 ms
        dly_tsk(delay/10*1000);
    }

    return PBIO_SUCCESS;
}
