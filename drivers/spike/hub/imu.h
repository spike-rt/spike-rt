/*
 * SPDX-License-Identifier: MIT
 *
 * API for the hub built-in IMU.
 *
 * Copyright (c) 2022 Embedded and Real-Time Systems Laboratory,
 *            Graduate School of Information Science, Nagoya Univ., JAPAN
 */

/**
 * \file    spike/hub/imu.h
 * \brief   API for the hub built-in IMU.
 * \author  Shu Yoshifumi
 */

/**
 * \addtogroup  Hub Hub
 * @{
 */


/**
 * \~English
 * \defgroup  IMU IMU
 * \brief     API for the hub built-in IMU.
 * @{
 *
 * \~Japanese
 * \defgroup  IMU IMU
 * \brief     ハブ内蔵IMU向けAPI．
 * @{
 */


#ifndef _HUB_IMU_H_
#define _HUB_IMU_H_

#include <stdint.h>
#include <pbio/error.h>

/**
 * \~English
 * \brief         Initialize the IMU driver.
 * \retval PBIO_SUCCESS      Successfully initialized.
 * \retval PBIO_ERROR_FAILED Failed to initialized.
 *
 * \~Japanese
 * \brief       IMUドライバを初期化する．
 * \param PBIO_SUCCESS      初期化が完了．
 * \param PBIO_ERROR_FAILED 初期化に失敗した．
 */
pbio_error_t hub_imu_init(void);

/**
 * \~English
 * \brief       Get the acceleration by the IMU.
 * \param accel   Float array to store the x/y/z axes acceleration[mm/s^2].
 *
 * \~Japanese
 * \brief       IMUから加速度を取得する．
 * \param accel   x/y/z軸の加速度を格納するためのfloat配列[mm/s^2]．
 */
void hub_imu_get_acceleration(float accel[3]);

/**
 * \~English
 * \brief       Get the angular velocity by the IMU.
 * \param angv   Float array to store the x/y/z axes angular velocity [°/s].
 *
 * \~Japanese
 * \brief       IMUから角速度を取得する．
 * \param angv   x/y/z軸の角速度を格納するためのfloat配列[°/s]．
 */
void hub_imu_get_angular_velocity(float angv[3]);

/**
 * \~English
 * \brief       Get the temperature by the IMU.
 * \retval      Temperature [℃]．
 *
 * \~Japanese
 * \brief       IMUから温度を取得する．
 * \retval      温度 [℃]．
 */
float hub_imu_get_temperature(void);

#endif // _HUB_IMU_H_

/**
 * @} // End of group IMU
 */

/**
 * @} // End of group Hub
 */
