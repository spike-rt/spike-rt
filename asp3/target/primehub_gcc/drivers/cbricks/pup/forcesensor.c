// SPDX-License-Identifier: MIT
/*
 * API for force sensors
 *
 *  Based on https://github.com/pybricks/pybricks-micropython/blob/master/pybricks/pupdevices/pb_type_pupdevices_forcesensor.c
 *
 * Copyright (c) 2018-2020 The Pybricks Authors
 * Modifications for TOPPERS/APS3 Kernel Copyright (c) 2022 Embedded and Real-Time Systems Laboratory,
 *                    Graduate School of Information Science, Nagoya Univ., JAPAN
 */

#include <t_syslog.h>

#include <cbricks/cb_error.h>
#include <cbricks/pup/forcesensor.h>

#include <pbio/math.h>
#include <fixmath.h>

typedef struct {
	int32_t raw_released;
	int32_t raw_offset;
	int32_t raw_start;
	int32_t raw_end;
} cb_force_sensor_obj_t;

cb_force_sensor_obj_t cb_force_sensor_obj;

static int32_t pup_force_sensor__raw(pup_device_t *pdev) {
	int32_t raw;
	pbio_error_t err;
	err = pup_device_get_values(pdev, PBIO_IODEV_MODE_PUP_FORCE_SENSOR__FRAW, &raw);
	if (err != PBIO_SUCCESS) {
		syslog(LOG_ERROR, "pup_force_sensor__raw() failed.");
		raw = -err;
	}
	return raw;
}

static int32_t pup_force_sensor__force(pup_device_t *pdev) {
	int32_t raw = pup_force_sensor__raw(pdev);

	int32_t force = (10000 * (raw - cb_force_sensor_obj.raw_released - cb_force_sensor_obj.raw_offset)) / (cb_force_sensor_obj.raw_end - cb_force_sensor_obj.raw_released);

	return force < 0 ? 0 : force;
}

static int32_t pup_force_sensor__distance(pup_device_t *pdev) {
	int32_t raw = pup_force_sensor__raw(pdev);

	return (6670 * (raw - cb_force_sensor_obj.raw_released)) / (cb_force_sensor_obj.raw_end - cb_force_sensor_obj.raw_released);
}

pup_device_t *pup_force_sensor_get_device(pbio_port_id_t port) {
	
  // Get iodevices
  pup_device_t *pdev =  pup_device_get_device(port, PBIO_IODEV_TYPE_ID_SPIKE_FORCE_SENSOR);

	int32_t calib[8];
	pup_device_get_values(pdev, PBIO_IODEV_MODE_PUP_FORCE_SENSOR__CALIB, calib);
	cb_force_sensor_obj.raw_offset = calib[1];
	cb_force_sensor_obj.raw_released = calib[2];
	cb_force_sensor_obj.raw_end = calib[6];

	if(cb_force_sensor_obj.raw_released >= cb_force_sensor_obj.raw_end) {
		return NULL;
	}

	pup_force_sensor__raw(pdev);

	return pdev;
}

float pup_force_sensor_force(pup_device_t *pdev) {

	return pup_force_sensor__force(pdev) / 1000;
}

float pup_force_sensor_distance(pup_device_t *pdev) {
	return pup_force_sensor__distance(pdev) / 1000;
}

bool pup_force_sensor_pressed(pup_device_t *pdev, float force) {
	int32_t f_arg = pbio_math_mul_i32_fix16(1000, fix16_from_float(force));

	return pup_force_sensor__force(pdev) >= f_arg;
}

bool pup_force_sensor_touched(pup_device_t *pdev) {
	return pup_force_sensor__raw(pdev) > cb_force_sensor_obj.raw_released + 4;
}
