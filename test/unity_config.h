// SPDX-License-Identifier: MIT
/*
 * Configurations for Unity, a test framework.
 * For more information, please see 
 *  - http://www.throwtheswitch.org/build/cross
 *  - https://github.com/ThrowTheSwitch/Unity/blob/master/docs/UnityConfigurationGuide.md
 *
 * Copyright (c) 2022 Embedded and Real-Time Systems Laboratory,
 *                    Graduate School of Information Science, Nagoya Univ., JAPAN
 */

#ifndef UNITY_CONFIG_H
#define UNITY_CONFIG_H

#include <unity.h>

#define UNITY_INT_WIDTH      32
#define UNITY_POINTER_WIDTH  32

#define UNITY_EXCLUDE_SETJMP_H
#define UNITY_EXCLUDE_MATH_H
#define UNITY_EXCLUDE_STDDEF_H
#define UNITY_EXCLUDE_STDINT_H
#define UNITY_EXCLUDE_LIMITS_H
#define UNITY_INCLUDE_PRINT_FORMATTED

#define UNITY_FIXTURE_NO_EXTRAS

#include <stdbool.h>
#include <stdint.h>

extern void tPutLogTarget_ePutLog_putChar(char c);
#define UNITY_OUTPUT_CHAR(c)  tPutLogTarget_ePutLog_putChar(c)
#define UNITY_PRINT_EOL() \
    do { UNITY_OUTPUT_CHAR('\r'); UNITY_OUTPUT_CHAR('\n'); } while (0)
#define UNITY_OUTPUT_COLOR

#endif
