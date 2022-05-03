#ifndef UNITY_CONFIG_H
#define UNITY_CONFIG_H

/*
 * Configuration for Unity.
 * For more information, please see 
 *  - http://www.throwtheswitch.org/build/cross
 *  - https://github.com/ThrowTheSwitch/Unity/blob/master/docs/UnityConfigurationGuide.md
 */

#include "unity.h"

#define UNITY_INT_WIDTH      32
#define UNITY_POINTER_WIDTH  32

#define UNITY_EXCLUDE_SETJMP_H
#define UNITY_EXCLUDE_MATH_H
#define UNITY_EXCLUDE_STDDEF_H
#define UNITY_EXCLUDE_STDINT_H
#define UNITY_EXCLUDE_LIMITS_H
#define UNITY_INCLUDE_PRINT_FORMATTED

#define UNITY_FIXTURE_NO_EXTRAS

void low_putchar(char c);
#define UNITY_OUTPUT_CHAR(c)  low_putchar(c)
#define UNITY_OUTPUT_COLOR

#endif
