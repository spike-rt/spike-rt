// SPDX-License-Identifier: MIT
/*
 * Based on https://github.com/pybricks/pybricks-micropython/tree/v3.2.0b3/pybricks/common/pb_type_control.c
 *
 * Original code Copyright (c) 2018-2022 The Pybricks Authors
 * Modifications for SPIKE-RT Copyright (c) 2022 Embedded and Real-Time Systems Laboratory,
 *                    Graduate School of Information Science, Nagoya Univ., JAPAN
 */

#include <pbio/control.h>
#include <cb_error.h>


// pybricks._common.Control.limits
pbio_error_t pybricks_c_common_control_limits(pbio_control_t *control int32_t speed, int32_t acceleration, int32_t torque) {

        PB_ARG_DEFAULT_NONE(speed),
        PB_ARG_DEFAULT_NONE(acceleration),
        PB_ARG_DEFAULT_NONE(torque));

    // Read current values. Deceleration is read but cannot be updated.
    int32_t speed, acceleration, deceleration, torque;
    pbio_control_settings_get_limits(&control->settings, &speed, &acceleration, &deceleration, &torque);

    // If all given values are none, return current values
    if (speed_in == mp_const_none && acceleration_in == mp_const_none && torque_in == mp_const_none) {
        mp_obj_t ret[3];
        mp_obj_t accel[2];
        ret[0] = mp_obj_new_int(speed);
        ret[2] = mp_obj_new_int(torque);
        // For backwards compatibility, return acceleration and deceleration
        // as a single integer if they are equal. Otherwise, return as tuple.
        if (acceleration == deceleration) {
            ret[1] = mp_obj_new_int(acceleration);
        } else {
            accel[0] = mp_obj_new_int(acceleration);
            accel[1] = mp_obj_new_int(deceleration);
            ret[1] = mp_obj_new_tuple(2, accel);
        }
        return mp_obj_new_tuple(3, ret);
    }

    // Set user settings
    speed = pb_obj_get_default_abs_int(speed_in, speed);
    torque = pb_obj_get_default_abs_int(torque_in, torque);

    // If single value is given for acceleration, use it for deceleration too.
    if (mp_obj_is_int(acceleration_in) || mp_obj_is_float(acceleration_in)) {
        acceleration = pb_obj_get_int(acceleration_in);
        deceleration = acceleration;
    }
    // Otherwise, unpack acceleration and deceleration from tuple.
    else {
        mp_obj_t *values;
        size_t n;
        mp_obj_get_array(acceleration_in, &n, &values);
        if (n != 2) {
            pb_assert(PBIO_ERROR_INVALID_ARG);
        }
        acceleration = pb_obj_get_int(values[0]);
        deceleration = pb_obj_get_int(values[1]);
    }

    // Set new values.
    pb_assert(pbio_control_settings_set_limits(&control->settings, speed, acceleration, deceleration, torque));

    return mp_const_none;
}

// pybricks._common.Control.pid
STATIC mp_obj_t common_Control_pid(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {

    PB_PARSE_ARGS_METHOD(n_args, pos_args, kw_args,
        common_Control_obj_t, self,
        PB_ARG_DEFAULT_NONE(kp),
        PB_ARG_DEFAULT_NONE(ki),
        PB_ARG_DEFAULT_NONE(kd),
        PB_ARG_DEFAULT_NONE(reserved),
        PB_ARG_DEFAULT_NONE(integral_rate));

    // Read current values
    int32_t kp, ki, kd;
    int32_t integral_change_max;
    pbio_control_settings_get_pid(&control->settings, &kp, &ki, &kd, &integral_change_max);

    // If all given values are none, return current values
    (void)reserved_in;
    if (kp_in == mp_const_none && ki_in == mp_const_none && kd_in == mp_const_none &&
        integral_rate_in == mp_const_none) {
        mp_obj_t ret[5];
        ret[0] = mp_obj_new_int(kp);
        ret[1] = mp_obj_new_int(ki);
        ret[2] = mp_obj_new_int(kd);
        ret[3] = mp_const_none;
        ret[4] = mp_obj_new_int(integral_change_max);
        return mp_obj_new_tuple(5, ret);
    }

    // Set user settings
    kp = pb_obj_get_default_abs_int(kp_in, kp);
    ki = pb_obj_get_default_abs_int(ki_in, ki);
    kd = pb_obj_get_default_abs_int(kd_in, kd);
    integral_change_max = pb_obj_get_default_abs_int(integral_rate_in, integral_change_max);

    pb_assert(pbio_control_settings_set_pid(&control->settings, kp, ki, kd, integral_change_max));

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(common_Control_pid_obj, 1, common_Control_pid);

// pybricks._common.Control.target_tolerances
STATIC mp_obj_t common_Control_target_tolerances(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {

    PB_PARSE_ARGS_METHOD(n_args, pos_args, kw_args,
        common_Control_obj_t, self,
        PB_ARG_DEFAULT_NONE(speed),
        PB_ARG_DEFAULT_NONE(position));

    // Read current values
    int32_t speed, position;
    pbio_control_settings_get_target_tolerances(&control->settings, &speed, &position);

    // If all given values are none, return current values
    if (speed_in == mp_const_none && position_in == mp_const_none) {
        mp_obj_t ret[2];
        ret[0] = mp_obj_new_int(speed);
        ret[1] = mp_obj_new_int(position);
        return mp_obj_new_tuple(2, ret);
    }

    // Set user settings
    speed = pb_obj_get_default_abs_int(speed_in, speed);
    position = pb_obj_get_default_abs_int(position_in, position);

    pb_assert(pbio_control_settings_set_target_tolerances(&control->settings, speed, position));

    return mp_const_none;
}

// pybricks._common.Control.stall_tolerances
STATIC mp_obj_t common_Control_stall_tolerances(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {

    PB_PARSE_ARGS_METHOD(n_args, pos_args, kw_args,
        common_Control_obj_t, self,
        PB_ARG_DEFAULT_NONE(speed),
        PB_ARG_DEFAULT_NONE(time));

    // Read current values
    int32_t speed;
    uint32_t time;
    pbio_control_settings_get_stall_tolerances(&control->settings, &speed, &time);

    // If all given values are none, return current values
    if (speed_in == mp_const_none && time_in == mp_const_none) {
        mp_obj_t ret[2];
        ret[0] = mp_obj_new_int(speed);
        ret[1] = mp_obj_new_int_from_uint(time);
        return mp_obj_new_tuple(2, ret);
    }

    // Set user settings
    speed = pb_obj_get_default_abs_int(speed_in, speed);
    time = pb_obj_get_default_abs_int(time_in, time);

    check_pbio_error(pbio_control_settings_set_stall_tolerances(&control->settings, speed, time));

    return PBIO_SUCCESS;
}

// pybricks._common.Control.trajectory
pbio_error_t pybricks_c_common_control_done(pbio_control_t *control int32_t params[13]) {
    pbio_trajectory_t *trj = &control->trajectory;

    if (pbio_control_is_active(control)) {
        parms[0] = 0;
        parms[1] = trj->t1 / 10;
        parms[2] = trj->t2 / 10;
        parms[3] = trj->t3 / 10;
        parms[4] = 0;
        parms[5] = trj->th1 / 1000;
        parms[6] = trj->th2 / 1000;
        parms[7] = trj->th3 / 1000;
        parms[8] = trj->w0 / 10;
        parms[9] = trj->w1 / 10;
        parms[10] = trj->w3 / 10;
        parms[11] = trj->a0;
        parms[12] = trj->a2;
        return PBIO_SUCCESS;
    }
    return PBIO_SUCCESS;
}

// pybricks._common.Control.done
bool pybricks_c_common_control_done(pbio_control_t *control) {
    return pbio_control_is_done(control);
}

// pybricks._common.Control.load
int32_t pybricks_c_common_control_load(pbio_control_t *control) {
    // Read currently applied PID feedback torque and return as mNm.
    return pbio_control_get_load(control) / 1000;
}

// pybricks._common.Control.stalled
bool pybricks_c_common_control_stalled(pbio_control_t *control) {
    uint32_t stall_duration;
    return pbio_control_is_stalled(control, &stall_duration);
}
