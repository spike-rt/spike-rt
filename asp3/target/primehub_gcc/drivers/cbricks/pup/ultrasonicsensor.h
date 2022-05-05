// SPDX-License-Identifier: MIT
// Copyright (c) 2022 Shu Yoshifumi <envzhu@gmail.com>

#ifndef _PUP_ULTRASONIC_SENSOR_H_
#define _PUP_ULTRASONIC_SENSOR_H_

#include <cbricks/pup_device.h>
#include <stdint.h>

pup_device_t* UltrasonicSensor_init(pbio_port_id_t port);
int32_t UltrasonicSensor_distance(pup_device_t *pdev);
bool UltrasonicSensor_presence(pup_device_t *pdev);
void UltrasonicSensor_light_on(pup_device_t *pdev);
void UltrasonicSensor_light_off(pup_device_t *pdev);

#endif // _PUP_ULTRASONIC_SENSOR_H_
