// SPDX-License-Identifier: MIT
/*
 * Common codes for controlling PUP devices.
 * Based on https://github.com/pybricks/pybricks-micropython/blob/master/pybricks/util_pb/pb_device.h
 *
 * Original codes Copyright (c) 2018-2021 The Pybricks Authors
 * Modifications for TOPPERS/APS3 Kernel Copyright (c) 2022 Embedded and Real-Time Systems Laboratory,
 *                                                          Graduate School of Information Science, Nagoya Univ., JAPAN
 */


#ifndef __PUP_DEVICE_H__
#define __PUP_DEVICE_H__

#include <stdint.h>

#include <pbio/color.h>
#include <pbio/error.h>
#include <pbio/iodev.h>

typedef struct _pup_device_t pup_device_t;

pup_device_t *pup_device_get_device(pbio_port_id_t port, pbio_iodev_type_id_t valid_id);
pbio_error_t pup_device_get_values(pup_device_t *pdev, uint8_t mode, int32_t *values);
pbio_error_t pup_device_set_values(pup_device_t *pdev, uint8_t mode, int32_t *values, uint8_t num_values);

pbio_error_t pup_device_setup_motor(pbio_port_id_t port, bool is_servo);

#endif // _PBDEVICE_H_
