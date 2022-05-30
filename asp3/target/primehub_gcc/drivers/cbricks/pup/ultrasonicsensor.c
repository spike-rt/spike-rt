// SPDX-License-Identifier: MIT
/*
 * API for ultrasonic sensors
 *
 * Copyright (c) 2022 Embedded and Real-Time Systems Laboratory,
 *                    Graduate School of Information Science, Nagoya Univ., JAPAN
 */

#include <t_syslog.h>
#include <cbricks/cb_error.h>
#include <cbricks/pup/ultrasonicsensor.h>


pup_device_t *pup_ultrasonic_sensor_get_device(pbio_port_id_t port) {
  // Get iodevices
  return pup_device_get_device(port, PBIO_IODEV_TYPE_ID_SPIKE_ULTRASONIC_SENSOR);
}

int32_t pup_ultrasonic_sensor_distance(pup_device_t *pdev) {
  int32_t distance;
  pbio_error_t err;

  err = pup_device_get_values(pdev, PBIO_IODEV_MODE_PUP_ULTRASONIC_SENSOR__DISTL, &distance);
  if (err != PBIO_SUCCESS) {
    syslog(LOG_ERROR, "pup_ultrasonic_sensor_distance() failed.");
    distance = -err;
  }
  // return (distance < 0 || distance >= 2000) ? 2000 : distance;
  return distance;
}

// Checks for the presence of other ultrasonic sensors by detecting ultrasonic sounds.
bool pup_ultrasonic_sensor_presence(pup_device_t *pdev) {
  int32_t presence;
  pbio_error_t err;

  err = pup_device_get_values(pdev, PBIO_IODEV_MODE_PUP_ULTRASONIC_SENSOR__LISTN, &presence);
  if (err != PBIO_SUCCESS) {
    syslog(LOG_ERROR, "pup_ultrasonic_sensor_presence() failed.");
    presence = false;
  }
  return (bool)presence;
}


pbio_error_t pup_ultrasonic_sensor_light_set(pup_device_t *pdev, 
                                             int32_t bv1, int32_t bv2, int32_t bv3, int32_t bv4) {
  int32_t brightness_values[4] = {bv1, bv2, bv3, bv4};
  return pup_device_set_values(pdev, PBIO_IODEV_MODE_PUP_ULTRASONIC_SENSOR__LIGHT,
                               brightness_values, sizeof(brightness_values)/sizeof(int32_t));
}

pbio_error_t pup_ultrasonic_sensor_light_on(pup_device_t *pdev) {
  return pup_ultrasonic_sensor_light_set(pdev, 100, 100, 100, 100);
}

pbio_error_t pup_ultrasonic_sensor_light_off(pup_device_t *pdev) {
  return pup_ultrasonic_sensor_light_set(pdev, 0, 0, 0, 0);
}
