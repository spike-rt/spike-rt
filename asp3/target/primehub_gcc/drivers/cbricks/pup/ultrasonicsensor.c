// SPDX-License-Identifier: MIT
// Copyright (c) 2018-2020 The Pybricks Authors

#include <cbricks/pup_device.h>
#include <stdint.h>

// pybricks.pupdevices.UltrasonicSensor.__init__
pup_device_t* UltrasonicSensor_init(pbio_port_id_t port) {
    pup_device_t *pbdev;

    // Get iodevices
    pbdev = pup_device_get_device(port, PBIO_IODEV_TYPE_ID_SPIKE_ULTRASONIC_SENSOR);

    return pbdev;
}

// pybricks.pupdevices.UltrasonicSensor.distance
int32_t UltrasonicSensor_distance(pup_device_t *pbdev) {
    int32_t distance;
    pup_device_get_values(pbdev, PBIO_IODEV_MODE_PUP_ULTRASONIC_SENSOR__DISTL, &distance);
    return (distance < 0 || distance >= 2000) ? 2000 : distance;
}

// pybricks.pupdevices.UltrasonicSensor.presence
// Checks for the presence of other ultrasonic sensors by detecting ultrasonic sounds.
bool UltrasonicSensor_presence(pup_device_t *pbdev) {
    int32_t presence;
    pup_device_get_values(pbdev, PBIO_IODEV_MODE_PUP_ULTRASONIC_SENSOR__LISTN, &presence);
    return (bool)presence;
}

void UltrasonicSensor_light_on(pup_device_t *pbdev) {
    int32_t brightness_values[4] = {100, 100, 100, 100};
    pup_device_set_values(pbdev, PBIO_IODEV_MODE_PUP_ULTRASONIC_SENSOR__LIGHT,
                         brightness_values, sizeof(brightness_values)/sizeof(int32_t));
}

void UltrasonicSensor_light_off(pup_device_t *pbdev) {
    int32_t brightness_values[4] = {0};
    pup_device_set_values(pbdev, PBIO_IODEV_MODE_PUP_ULTRASONIC_SENSOR__LIGHT,
                         brightness_values, sizeof(brightness_values)/sizeof(int32_t));
}
