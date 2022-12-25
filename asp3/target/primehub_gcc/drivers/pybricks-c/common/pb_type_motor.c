/*
 * SPDX-License-Identifier: MIT
 *
 * Based on https://github.com/pybricks/pybricks-micropython/tree/v3.2.0b3/pybricks/common/pb_type_motor.c
 *
 * Original code Copyright (c) 2018-2022 The Pybricks Authors
 * Modifications for SPIKE-RT Copyright (c) 2022 Embedded and Real-Time Systems Laboratory,
 *                    Graduate School of Information Science, Nagoya Univ., JAPAN
 */

#include <pbio/battery.h>

#include <pbio/dcmotor.h>
#include <pbio/int_math.h>
#include <pbio/servo.h>

#include <pup_device.h>
#include <cb_error.h>

/*
 * Notice: Logger is not used here though original Pybricks uses.
 */

/* Wait for servo maneuver to complete */

static pbio_error_t wait_for_completion(pbio_servo_t *srv) {
    // TODO: Avoid Race Conditions!
    while (!pbio_control_is_done(&srv->control)) {
        // Wait 5 ms
        dly_tsk(5*1000);
    }
    if (!pbio_servo_update_loop_is_running(srv)) {
        return PBIO_ERROR_NO_DEV;
    }
    return PBIO_SUCCESS;
}

// Gets the number of millidegrees of the motor, for each whole degree
// of rotation at the gear train output. For example, if the gear train
// slows the motor down using a 12 teeth and a 36 teeth gear, the result
// should be (36 / 12) * 1000 = 3000.
// TODO:
#if 0
static int32_t get_gear_ratio(mp_obj_t gears_in) {

    // No gears means gear ratio is one.
    if (gears_in == mp_const_none) {
        return 1000;
    }

    // Unpack the main list of multiple gear trains.
    mp_obj_t *trains, *gear_list;
    size_t n_trains, n_gears;
    mp_obj_get_array(gears_in, &n_trains, &trains);

    // Parse gear argument of the form [[12, 20, 36], [20, 40]]
    int32_t first_gear_product = 1;
    int32_t last_gear_product = 1;

    // Check type of gear train given.
    if (mp_obj_is_int(trains[0]) && mp_obj_is_int(trains[n_trains - 1])) {
        // If the first and last element is an integer, assume just one list of
        // gears, e.g. [12, 20, 36]. Take ratio of last and first.
        first_gear_product = mp_obj_get_int(trains[0]);
        last_gear_product = mp_obj_get_int(trains[n_trains - 1]);
    } else {
        // Otherwise, parse gear argument of the form [[12, 20, 36], [20, 40]].
        for (size_t train = 0; train < n_trains; train++) {
            // Unpack the list of gears for this train
            mp_obj_get_array(trains[train], &n_gears, &gear_list);

            first_gear_product *= mp_obj_get_int(gear_list[0]);
            last_gear_product *= mp_obj_get_int(gear_list[n_gears - 1]);
        }
    }

    // Verify the result.
    if (first_gear_product < 1 || last_gear_product < 1) {
        mp_raise_msg(&mp_type_ZeroDivisionError, MP_ERROR_TEXT("Gears must be positive integers."));
    }

    // Return ratio scaled to millidegrees.
    return 1000 * last_gear_product / first_gear_product;
}
#endif

// pybricks._common.Motor.__init__
/*
 * Default:
 *  positive_direction = PBIO_DIRECTION_CLOCKWISE
 *  gears = None
 *  reset_angle = true
 */
pbio_error_t pybricks_c_common_motor_make_new(pbio_servo_t **p_srv, pbio_port_id_t port, pbio_direction_t positive_direction, bool reset_angle) {

    pbio_error_t err;
    pbio_servo_t *srv;

    // Setup motor device and raise error if not connected or ready.
    check_pbio_error(pup_device_setup_motor(port, true));

    // Parse gears argument to get gear ratio.
    int32_t gear_ratio = 1000;
    // TODO:
    // check_pbio_error(get_gear_ratio(&gear_ratio, gears_in));

    // Get pointer to servo and allow tacho to finish syncing
    while ((err = pbio_servo_get_servo(port, p_srv)) == PBIO_ERROR_AGAIN) {
        // Wait 1s
        dly_tsk(1000*1000);
    }
    check_pbio_error(err);

    // Set up servo
    check_pbio_error(pbio_servo_setup(*p_srv, positive_direction, gear_ratio, reset_angle));

    return PBIO_SUCCESS;
}

// pybricks._common.Motor.angle
pbio_error_t pybricks_c_common_motor_angle(pbio_servo_t *srv, int32_t *p_angle) {
    int32_t dummy_speed;
    return pbio_servo_get_state_user(srv, p_angle, &dummy_speed);
}

// pybricks._common.Motor.reset_angle
pbio_error_t pybricks_c_common_motor_reset_angle(pbio_servo_t *srv, int32_t reset_angle, bool reset_to_abs) {
    // Set the new angle
    return pbio_servo_reset_angle(srv, reset_angle, reset_to_abs);
}

// pybricks._common.Motor.speed
pbio_error_t pybricks_c_common_motor_speed(pbio_servo_t *srv, int32_t *p_speed) {
    int32_t dummy_angle;
    return pbio_servo_get_state_user(srv, &dummy_angle, p_speed);
}

// pybricks._common.Motor.run
pbio_error_t pybricks_c_common_motor_run(pbio_servo_t *srv, int32_t speed) {
    return pbio_servo_run_forever(srv, speed);
}

// pybricks._common.Motor.hold
pbio_error_t pybricks_c_common_motor_hold(pbio_servo_t *srv) {
    return pbio_servo_stop(srv, PBIO_CONTROL_ON_COMPLETION_HOLD);
}

// pybricks._common.Motor.run_time
/*
 * Default:
 *  time >= 0
 *  then = PBIO_CONTROL_ON_COMPLETION_HOLD
 *  wait = true
 */
pbio_error_t pybricks_c_common_motor_run_time(pbio_servo_t *srv, int32_t speed, int32_t time, pbio_control_on_completion_t then, bool wait) {

    time = pbio_int_math_max(time, 0);

    // Call pbio with parsed user/default arguments
    check_pbio_error(pbio_servo_run_time(srv, speed, time, then));

    if (wait) {
        check_pbio_error(wait_for_completion(srv));
    }

    return PBIO_SUCCESS;
}

// pybricks._common.Motor.run_until_stalled
// Not ported


// pybricks._common.Motor.run_angle
/*
 * Default:
 *  then = PBIO_CONTROL_ON_COMPLETION_HOLD
 *  wait = true
 */
pbio_error_t pybricks_c_common_motor_run_angle(pbio_servo_t *srv, int32_t speed, int32_t rotation_angle, pbio_control_on_completion_t then, bool wait) {

    // Call pbio with parsed user/default arguments
    check_pbio_error(pbio_servo_run_angle(srv, speed, rotation_angle, then));

    if (wait) {
        check_pbio_error(wait_for_completion(srv));
    }

    return PBIO_SUCCESS;
}

// pybricks._common.Motor.run_target
/*
 * Default:
 *  then = PBIO_CONTROL_ON_COMPLETION_HOLD
 *  wait = true
 */
pbio_error_t pybricks_c_common_motor_run_target(pbio_servo_t *srv, int32_t speed, int32_t target_angle, pbio_control_on_completion_t then, bool wait) {

    // Call pbio with parsed user/default arguments
    check_pbio_error(pbio_servo_run_target(srv, speed, target_angle, then));

    if (wait) {
        check_pbio_error(wait_for_completion(srv));
    }

    return PBIO_SUCCESS;
}

// pybricks._common.Motor.track_target
pbio_error_t pybricks_c_common_motor_track_target(pbio_servo_t *srv, int32_t target_angle) {
    check_pbio_error(pbio_servo_track_target(srv, target_angle));
    return PBIO_SUCCESS;
}

// pybricks._common.Motor.stalled
pbio_error_t pybricks_c_common_motor_stalled(pbio_servo_t *srv, bool *p_stalled) {
    uint32_t stall_duration;
    check_pbio_error(pbio_servo_is_stalled(srv, p_stalled, &stall_duration));
    return PBIO_SUCCESS;
}
