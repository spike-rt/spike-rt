// SPDX-License-Identifier: MIT
/*
 * API for force sensors
 *
 * Copyright (c) 2022 Embedded and Real-Time Systems Laboratory,
 *                    Graduate School of Information Science, Nagoya Univ., JAPAN
 */

/**
 * \file    cbricks/pup/forcesensor.h
 * \brief	  API for force sensors
 * \author	Kajita Shun
 */

/**
 * \addtogroup  PUPDevice
 * @{
 */

/**
 * \~English
 * \defgroup ForceSensor Force Sensor
 * \brief	  APIs for force sensors
 * @{
 *
 * \~Japanese
 * \defgroup ForceSensor フォースセンサ
 * \brief    フォースセンサのAPI．
 * @{
 */


#ifndef _PUP_FORCE_SENSOR_H_
#define _PUP_FORCE_SENSOR_H_

#include <stdint.h>
#include <cbricks/pup_device.h>

/**
 * \~English
 * \brief					Get the PUP device pointer of the force sensor specified with the port ID.
 * \details				If it fails, it outputs an error log and returns NULL.
 * \param		port	PUP port ID to be inquired.
 * \return	pdev	PUP device pointer.
 *
 * \~Japanese
 * \brief				ポートIDで指定されたフォースンサへのPUPデバイスポインタを取得する。
 * \details			失敗した場合、エラーログを出力し、NULLを返す。
 * \param port	PUPポートID。
 * \return			PUPデバイスポインタ。
 */
pup_device_t *pup_force_sensor_get_device(pbio_port_id_t port);

/**
 * \~English
 * \brief					Get the force detected by a force sensor.
 * \param		pdev	PUP devie pointer to be inquired.
 * \return				Force in newton.
 *
 * \~Japanese
 * \brief				フォースセンサで力を測定する。
 * \param port	PUPデバイスポインタ。
 * \return			力（単位：N）。
 */

float pup_force_sensor_force(pup_device_t *pdev);

/**
 * \~English
 * \brief					Get the distance how much force sensor's button has moved.
 * \param		pdev	PUP devie pointer to be inquired.
 * \return				Distance in millimeters.
 *
 * \~Japanese
 * \brief				フォースセンサのボタンの移動距離を取得する。
 * \param port	PUPデバイスポインタ。
 * \return			距離（単位：mm）。
 */
float pup_force_sensor_distance(pup_device_t *pdev);

/**
 * \~English
 * \brief					Check a force sensor's button is pressed.
 * \param		pdev	PUP devie pointer to be inquired.
 * \param		force	Minimum force in newton to be considerd as pressed.
 * \return	true	The button is pressed.
 * \return	false	The button is not pressed.
 *
 * \~Japanese
 * \brief				フォースセンサのボタンが押されているかを検出する。
 * \param		port	PUPデバイスポインタ。
 * \param		force	押されていると判定する最低限の力（単位：N）。
 * \return	true	押されている。
 * \return	false	押されていない。
 */
bool pup_force_sensor_pressed(pup_device_t *pdev, float force);

/**
 * \~English
 * \brief					Check a force sensor's button is touched.
 * \param		pdev	PUP devie pointer to be inquired.
 * \return	true	The button is touched.
 * \return	false	The button is not touched.
 *
 * \~Japanese
 * \brief				フォースセンサのボタンが押されているかを検出する。
 * \param		port	PUPデバイスポインタ。
 * \return	true	押されている。
 * \return	false	押されていない。
 */
bool pup_force_sensor_touched(pup_device_t *pdev);

#endif // _PUP_FORCE_SENSOR_H_

/**
 * @} // End of group
 */

/**
 * @} // End of group
 */
