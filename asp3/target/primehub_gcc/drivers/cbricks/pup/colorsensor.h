// SPDX-License-Identifier: MIT
/*
 * API for color sensors
 *
 * Copyright (c) 2022 Embedded and Real-Time Systems Laboratory,
 *                    Graduate School of Information Science, Nagoya Univ., JAPAN
 */

/**
 * \file    cbricks/pup/colorsensor.h
 * \brief	  API for color sensors
 * \author	Kajita Shun
 */

/**
 * \~English
 * \defgroup PUPDevice ColorSensor
 * \brief	  APIs for color sensors
 * @{
 *
 * \~Japanese
 * \defgroup PUPデバイス カラーセンサ
 * \brief    カラーセンサのAPI．
 * @{
 */


#ifndef _PUP_COLOR_SENSOR_H_
#define _PUP_COLOR_SENSOR_H_

#include <stdint.h>
#include <cbricks/pup_device.h>
#include <pbio/color.h>

/**
 * \~English
 *
 * \~Japanese
 */
pup_device_t *pup_color_sensor_get_device(pbio_port_id_t port);

/**
 * \~English
 *
 * \~Japanese
 */
pbio_color_hsv_t pup_color_sensor_color(pup_device_t *pdev, bool surface);

pbio_color_hsv_t pup_color_sensor_hsv(pup_device_t *pdev, bool surface);

/**
 * \~English
 */
int32_t pup_color_sensor_reflection(pup_device_t *pdev);

int32_t pup_color_sensor_ambient(pup_device_t *pdev);

/**
 */
pbio_error_t pup_color_sensor_light_set(pup_device_t *pdev, 
                                             int32_t bv1, int32_t bv2, int32_t bv3);

/**
 */
pbio_error_t pup_color_sensor_light_on(pup_device_t *pdev);

/**
 */
pbio_error_t pup_color_sensor_light_off(pup_device_t *pdev);

/**
 * @} // End of group
 */

#endif // _PUP_COLOR_SENSOR_H_
