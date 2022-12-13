// SPDX-License-Identifier: MIT
/*
 * Configurations for tests.
 *
 * Copyright (c) 2022 Embedded and Real-Time Systems Laboratory,
 *                    Graduate School of Information Science, Nagoya Univ., JAPAN
 */

#ifndef TEST_CONFIG_H
#define TEST_CONFIG_H

#include <test.h>

#include <pbio/port.h>

// Configurations for Unity output.
#define UNITY_OUTPUT_VIA_PutLogTarget   (1)
//#define UNITY_OUTPUT_VIA_SERIAL_ADAPTER (1)
//#define UNITY_SERIAL_PORTID   USART_F_PORTID

#define USART_TEST_PORTID     USART_F_PORTID

// Configurations for PUP device test.
#define PBIO_PORT_ID_TEST_PUP_DEVICE        PBIO_PORT_ID_B
#define PBIO_IODEV_TYPE_ID_TEST_PUP_DEVICE  PBIO_IODEV_TYPE_ID_SPIKE_FORCE_SENSOR

#define PBIO_PORT_ID_TEST_MOTOR             PBIO_PORT_ID_A
#define PBIO_PORT_ID_TEST_ULTRASONIC_SENSOR PBIO_PORT_ID_C
#define PBIO_PORT_ID_TEST_COLOR_SENSOR      PBIO_PORT_ID_D
#define PBIO_PORT_ID_TEST_FORCE_SENSOR      PBIO_PORT_ID_B

#endif // TEST_CONFIG_H
