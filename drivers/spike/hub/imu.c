/*
 * SPDX-License-Identifier: MIT
 *
 * API for the hub built-in IMU.
 *
 * Copyright (c) 2022 Embedded and Real-Time Systems Laboratory,
 *            Graduate School of Information Science, Nagoya Univ., JAPAN
 */

#include <t_syslog.h>
#include <spike/hub/light.h>

#include <pbdrv/imu.h>

/*
 * We implement the IMU API by just wrapping functions in external/libpybricks/lib/pbio/drv/imu/imu_lsm6ds3tr_c_stm32.c.
 */

pbdrv_imu_dev_t *imu_dev;

pbio_error_t hub_imu_init(void) {
  return pbdrv_imu_get_imu(&imu_dev);
}

void hub_imu_get_acceleration(float accel[3]) {
  pbdrv_imu_accel_read(imu_dev, accel);
}

void hub_imu_get_angular_velocity(float angv[3]) {
  pbdrv_imu_gyro_read(imu_dev, angv);
}

float hub_imu_get_temperature(void) {
  return pbdrv_imu_temperature_read(imu_dev);
}
