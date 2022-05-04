// SPDX-License-Identifier: MIT
// Copyright (c) 2018-2021 The Pybricks Authors

#ifndef _PBDEVICE_H_
#define _PBDEVICE_H_

#include <stdint.h>

#include <pbio/color.h>
#include <pbio/error.h>
#include <pbio/iodev.h>

typedef struct _pup_device_t pup_device_t;

pup_device_t *pup_device_get_device(pbio_port_id_t port, pbio_iodev_type_id_t valid_id);

void pup_device_get_values(pup_device_t *pbdev, uint8_t mode, int32_t *values);

void pup_device_set_values(pup_device_t *pbdev, uint8_t mode, int32_t *values, uint8_t num_values);

void pup_device_set_power_supply(pup_device_t *pbdev, int32_t duty);

pbio_iodev_type_id_t pup_device_get_id(pup_device_t *pbdev);

uint8_t pup_device_get_mode(pup_device_t *pbdev);

uint8_t pup_device_get_num_values(pup_device_t *pbdev);

int8_t pup_device_get_mode_id_from_str(pup_device_t *pbdev, const char *mode_str);

// LEGO POWERED UP Color and Distance Sensor
enum {
    PBIO_IODEV_MODE_PUP_COLOR_DISTANCE_SENSOR__COLOR = 0,  // read 1x int8_t
    PBIO_IODEV_MODE_PUP_COLOR_DISTANCE_SENSOR__PROX  = 1,  // read 1x int8_t
    PBIO_IODEV_MODE_PUP_COLOR_DISTANCE_SENSOR__COUNT = 2,  // read 1x int32_t
    PBIO_IODEV_MODE_PUP_COLOR_DISTANCE_SENSOR__REFLT = 3,  // read 1x int8_t
    PBIO_IODEV_MODE_PUP_COLOR_DISTANCE_SENSOR__AMBI  = 4,  // read 1x int8_t
    PBIO_IODEV_MODE_PUP_COLOR_DISTANCE_SENSOR__COL_O = 5,  // writ 1x int8_t
    PBIO_IODEV_MODE_PUP_COLOR_DISTANCE_SENSOR__RGB_I = 6,  // read 3x int16_t
    PBIO_IODEV_MODE_PUP_COLOR_DISTANCE_SENSOR__IR_TX = 7,  // writ 1x int16_t
    PBIO_IODEV_MODE_PUP_COLOR_DISTANCE_SENSOR__SPEC1 = 8,  // rrwr 4x int8_t
    PBIO_IODEV_MODE_PUP_COLOR_DISTANCE_SENSOR__DEBUG = 9,  // ?? 2x int16_t
    PBIO_IODEV_MODE_PUP_COLOR_DISTANCE_SENSOR__CALIB = 10, // ?? 8x int16_t
};

// LEGO SPIKE Color Sensor
enum {
    PBIO_IODEV_MODE_PUP_COLOR_SENSOR__COLOR = 0,  // read 1x int8_t
    PBIO_IODEV_MODE_PUP_COLOR_SENSOR__REFLT = 1,  // read 1x int8_t
    PBIO_IODEV_MODE_PUP_COLOR_SENSOR__AMBI  = 2,  // read 1x int8_t
    PBIO_IODEV_MODE_PUP_COLOR_SENSOR__LIGHT = 3,  // writ 3x int8_t
    PBIO_IODEV_MODE_PUP_COLOR_SENSOR__RREFL = 4,  // read 2x int16_t
    PBIO_IODEV_MODE_PUP_COLOR_SENSOR__RGB_I = 5,  // read 4x int16_t
    PBIO_IODEV_MODE_PUP_COLOR_SENSOR__HSV   = 6,  // read 3x int16_t
    PBIO_IODEV_MODE_PUP_COLOR_SENSOR__SHSV  = 7,  // read 4x int16_t
    PBIO_IODEV_MODE_PUP_COLOR_SENSOR__DEBUG = 8,  // ??   2x int16_t
    PBIO_IODEV_MODE_PUP_COLOR_SENSOR__CALIB = 9,  // ??   7x int16_t
};

// LEGO SPIKE Ultrasonic Sensor
enum {
    PBIO_IODEV_MODE_PUP_ULTRASONIC_SENSOR__DISTL = 0,  // read 1x int16_t
    PBIO_IODEV_MODE_PUP_ULTRASONIC_SENSOR__DISTS = 1,  // read 1x int16_t
    PBIO_IODEV_MODE_PUP_ULTRASONIC_SENSOR__SINGL = 2,  // read 1x int16_t
    PBIO_IODEV_MODE_PUP_ULTRASONIC_SENSOR__LISTN = 3,  // read 1x int8_t
    PBIO_IODEV_MODE_PUP_ULTRASONIC_SENSOR__TRAW  = 4,  // read 1x int32_t
    PBIO_IODEV_MODE_PUP_ULTRASONIC_SENSOR__LIGHT = 5,  // writ 4x int8_t
    PBIO_IODEV_MODE_PUP_ULTRASONIC_SENSOR__PING  = 6,  // ??   1x int8_t
    PBIO_IODEV_MODE_PUP_ULTRASONIC_SENSOR__ADRAW = 7,  // read 1x int16_t
    PBIO_IODEV_MODE_PUP_ULTRASONIC_SENSOR__CALIB = 8,  // ??   7x int16_t
};

// LEGO SPIKE Force Sensor
enum {
    PBIO_IODEV_MODE_PUP_FORCE_SENSOR__FRAW  = 4,  // read 1x int16_t
    PBIO_IODEV_MODE_PUP_FORCE_SENSOR__CALIB = 6,  // ??   8x int16_t
};

// LEGO Technic Color Light Matrix
enum {
    PBIO_IODEV_MODE_PUP_COLOR_LIGHT_MATRIX__LEV_O = 0,
    PBIO_IODEV_MODE_PUP_COLOR_LIGHT_MATRIX__COL_O = 1,
    PBIO_IODEV_MODE_PUP_COLOR_LIGHT_MATRIX__PIX_O = 2,
    PBIO_IODEV_MODE_PUP_COLOR_LIGHT_MATRIX__TRANS = 3,
};

#endif // _PBDEVICE_H_
