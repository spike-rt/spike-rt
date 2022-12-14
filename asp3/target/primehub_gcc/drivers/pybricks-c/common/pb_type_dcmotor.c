// SPDX-License-Identifier: MIT
/*
 * Based on https://github.com/pybricks/pybricks-micropython/tree/v3.2.0b3/pybricks/common/pb_type_dcmotor.c
 *
 * Original code Copyright (c) 2018-2022 The Pybricks Authors
 * Modifications for SPIKE-RT Copyright (c) 2022 Embedded and Real-Time Systems Laboratory,
 *                    Graduate School of Information Science, Nagoya Univ., JAPAN
 */

#include <pbio/battery.h>
#include <pbio/dcmotor.h>
#include <pup_device.h>
#include <cb_error.h>

// pybricks._common.DCMotor.__init__
/*
 * Default:
 *  positive_direction = PBIO_DIRECTION_CLOCKWISE
 */
pbio_error_t pybricks_c_common_dcmotor_make_new(pbio_dcmotor_t **p_dcmotor, pbio_port_id_t port, pbio_direction_t positive_direction) {

    pb_device_setup_motor(port, false);

    // Get and initialize DC Motor
    check_pbio_error(pbio_dcmotor_get_dcmotor(port, p_dcmotor));
    check_pbio_error(pbio_dcmotor_setup(*p_dcmotor, positive_direction));

    return PBIO_SUCCESS;
}

// pybricks._common.DCMotor.__repr__
// pybricks._common.Motor.__repr__
// Not ported.


// pybricks._common.DCMotor.dc
// pybricks._common.Motor.dc
pbio_error_t pybricks_c_common_dcmotor_duty(pbio_dcmotor_t *dcmotor, int32_t duty) {
    // pbio has only voltage setters now, but the .dc() method will continue to
    // exist for backwards compatibility. So, we convert duty cycle to voltages.
    int32_t voltage = pbio_battery_get_voltage_from_duty_pct(duty);

    check_pbio_error(pbio_dcmotor_user_command(dcmotor, false, voltage));

    return PBIO_SUCCESS;
}

// pybricks._common.DCMotor.stop
// pybricks._common.Motor.stop
pbio_error_t pybricks_c_common_dcmotor_stop(pbio_dcmotor_t *dcmotor) {

    check_pbio_error(pbio_dcmotor_user_command(dcmotor, true, 0));

    return PBIO_SUCCESS;
}

// pybricks._common.DCMotor.brake
// pybricks._common.Motor.brake
pbio_error_t pybricks_c_common_dcmotor_brake(pbio_dcmotor_t *dcmotor) {

    check_pbio_error(pbio_dcmotor_user_command(dcmotor, false, 0));

    return PBIO_SUCCESS;
}

// pybricks._common.DCMotor.dc
// pybricks._common.Motor.dc
pbio_error_t pybricks_c_common_dcmotor_dc_settings(pbio_dcmotor_t *dcmotor, int32_t max_voltage_in, int32_t *p_max_voltage_now) {
    // Parse all arguments except the first one (self)
    // If no arguments given, return existing values
    if (max_voltage_in < 0) {
        pbio_dcmotor_get_settings(dcmotor, p_max_voltage_now);
    }

    // Set the new limit
    check_pbio_error(pbio_dcmotor_set_settings(dcmotor, max_voltage_in));

    return PBIO_SUCCESS;
}
