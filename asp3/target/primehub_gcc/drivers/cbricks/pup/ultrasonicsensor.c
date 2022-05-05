// SPDX-License-Identifier: MIT
// Copyright (c) 2022 Shu Yoshifumi <envzhu@gmail.com>

#include <cbricks/pup/ultrasonicsensor.h>

// pybricks.pupdevices.UltrasonicSensor.__init__
pup_device_t* UltrasonicSensor_init(pbio_port_id_t port) {
    pup_device_t *pdev;

    // Get iodevices
    pdev = pup_device_get_device(port, PBIO_IODEV_TYPE_ID_SPIKE_ULTRASONIC_SENSOR);

    return pdev;
}

// pybricks.pupdevices.UltrasonicSensor.distance
int32_t UltrasonicSensor_distance(pup_device_t *pdev) {
    int32_t distance;
    pup_device_get_values(pdev, PBIO_IODEV_MODE_PUP_ULTRASONIC_SENSOR__DISTL, &distance);
    return (distance < 0 || distance >= 2000) ? 2000 : distance;
}

// pybricks.pupdevices.UltrasonicSensor.presence
// Checks for the presence of other ultrasonic sensors by detecting ultrasonic sounds.
bool UltrasonicSensor_presence(pup_device_t *pdev) {
    int32_t presence;
    pup_device_get_values(pdev, PBIO_IODEV_MODE_PUP_ULTRASONIC_SENSOR__LISTN, &presence);
    return (bool)presence;
}

void UltrasonicSensor_light_on(pup_device_t *pdev) {
    int32_t brightness_values[4] = {100, 100, 100, 100};
    pup_device_set_values(pdev, PBIO_IODEV_MODE_PUP_ULTRASONIC_SENSOR__LIGHT,
                         brightness_values, sizeof(brightness_values)/sizeof(int32_t));
}

void UltrasonicSensor_light_off(pup_device_t *pdev) {
    int32_t brightness_values[4] = {0};
    pup_device_set_values(pdev, PBIO_IODEV_MODE_PUP_ULTRASONIC_SENSOR__LIGHT,
                         brightness_values, sizeof(brightness_values)/sizeof(int32_t));
}
