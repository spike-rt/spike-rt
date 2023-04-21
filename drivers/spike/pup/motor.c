// SPDX-License-Identifier: MIT
/*
 * API for motors
 *
 * Copyright (c) 2022 Embedded and Real-Time Systems Laboratory,
 *                    Graduate School of Information Science, Nagoya Univ., JAPAN
 */

#include <kernel.h>
#include <t_syslog.h>
#include <spike/cb_error.h>
#include <spike/pup_device.h>
#include <spike/pup/motor.h>
#include <pbio/servo.h>
#include <pbio/control.h>
#include <pbio/battery.h>
#include <pbio/dcmotor.h>

pbio_error_t pybricks_c_common_motor_angle(pbio_servo_t *srv, int32_t *p_angle);
pbio_error_t pybricks_c_common_motor_speed(pbio_servo_t *srv, int32_t *p_speed);
pbio_error_t pybricks_c_common_motor_run(pbio_servo_t *srv, int32_t speed);
pbio_error_t pybricks_c_common_motor_hold(pbio_servo_t *srv);
pbio_error_t pybricks_c_common_motor_stalled(pbio_servo_t *srv, bool *p_stalled);
pbio_error_t pybricks_c_common_dcmotor_duty(pbio_dcmotor_t *dcmotor, int32_t duty);
pbio_error_t pybricks_c_common_dcmotor_stop(pbio_dcmotor_t *dcmotor);
pbio_error_t pybricks_c_common_dcmotor_brake(pbio_dcmotor_t *dcmotor);
pbio_error_t pybricks_c_common_dcmotor_dc_settings(pbio_dcmotor_t *dcmotor, int32_t max_voltage_in, int32_t *p_max_voltage_now);

#include <stdio.h>
static void errlog(char *name, pbio_port_id_t port, pbio_error_t err) {
  syslog(LOG_ERROR, "%s failed for port %c: %s", name, port, pbio_error_str(err));
}

pup_motor_t *pup_motor_get_device(pbio_port_id_t port) {
  pup_motor_t *motor = NULL;
  pbio_error_t err = pup_device_setup_motor(port, true);
  if (err != PBIO_SUCCESS) {
    errlog("pup_motor_get_device()", port, err);
    return NULL;
  }

  for(int i = 0; i < 10; i++)
  {
    err = pbio_servo_get_servo(port, &motor);
    if ((err == PBIO_SUCCESS) || (err != PBIO_ERROR_AGAIN)) {
      break;
    }
    dly_tsk(1000*1000);
  }
  if (err != PBIO_SUCCESS) {
    errlog("pup_motor_get_device()", port, err);
    return NULL;
  }
  return motor;
}

pbio_error_t pup_motor_setup(pup_motor_t *motor, pup_direction_t positive_direction, bool reset_count) {
  int32_t gear_ratio = 1000; // 1 count == 1 degree.
  pbio_error_t err = pbio_servo_setup(motor, positive_direction, gear_ratio, reset_count);
  if (err != PBIO_SUCCESS) {
    errlog("pup_motor_setup()", motor->dcmotor->port, err);
  }
  return err;
}

pbio_error_t pup_motor_reset_count(pup_motor_t *motor) {
  pbio_error_t err = pbio_servo_reset_angle(motor, 0, false);
  if (err != PBIO_SUCCESS) {
    errlog("pup_motor_reset_count()", motor->dcmotor->port, err);
  } 
  return err;
}

int32_t pup_motor_get_count(pup_motor_t *motor) {
  int32_t count = 0;
  pbio_error_t err = pybricks_c_common_motor_angle(motor, &count);
  if (err != PBIO_SUCCESS) {
    errlog("pup_motor_get_count()", motor->dcmotor->port, err);
  } 
  return count;
}

int32_t pup_motor_get_speed(pup_motor_t *motor) {
  int32_t speed = 0;
  pbio_error_t err = pybricks_c_common_motor_speed(motor, &speed);
  if (err != PBIO_SUCCESS) {
    errlog("pup_motor_get_speed()", motor->dcmotor->port, err);
  } 
  return speed;
}

pbio_error_t pup_motor_stop(pup_motor_t *motor) {
  pbio_error_t err = pybricks_c_common_dcmotor_stop(motor->dcmotor);
  if (err != PBIO_SUCCESS) {
    errlog("pup_motor_stop()", motor->dcmotor->port, err);
  } 
  return err;
}

pbio_error_t pup_motor_brake(pup_motor_t *motor) {
  pbio_error_t err = pybricks_c_common_dcmotor_brake(motor->dcmotor);
  if (err != PBIO_SUCCESS) {
    errlog("pup_motor_brake()", motor->dcmotor->port, err);
  } 
  return err;
}

pbio_error_t pup_motor_hold(pup_motor_t *motor) {
  pbio_error_t err = pybricks_c_common_motor_hold(motor);
  if (err != PBIO_SUCCESS) {
    errlog("pup_motor_hold()", motor->dcmotor->port, err);
  } 
  return err;
}

int32_t pup_motor_get_power(pup_motor_t *motor) {
  int32_t voltage    = motor->dcmotor->voltage_now; // retrieve the cached value
  int32_t duty_cycle = pbio_battery_get_duty_from_voltage(voltage);
  duty_cycle = (duty_cycle + 5) / 10; // 0 .. 100
  if (motor->dcmotor->direction == PUP_DIRECTION_COUNTERCLOCKWISE) {
    duty_cycle = -duty_cycle;         // -100 .. 0
  }
  return duty_cycle;
}

pbio_error_t pup_motor_set_power(pup_motor_t *motor, int power) {
  pbio_error_t err = pybricks_c_common_dcmotor_duty(motor->dcmotor, power);
  if (err != PBIO_SUCCESS) {
    errlog("pup_motor_set_power()", motor->dcmotor->port, err);
  } 
  return err;
}

pbio_error_t pup_motor_set_speed(pup_motor_t *motor, int speed) {
  pbio_error_t err = pybricks_c_common_motor_run(motor, speed);
  if (err != PBIO_SUCCESS) {
    errlog("pup_motor_set_speed()", motor->dcmotor->port, err);
  } 
  return err;
}

bool pup_motor_is_stalled(pup_motor_t *motor) {
  bool stalled;
  pybricks_c_common_motor_stalled(motor, &stalled);
  return stalled;
}

int32_t pup_motor_set_duty_limit(pup_motor_t *motor, int limit) {
  int32_t old_value = 0;
  int32_t new_value = pbio_battery_get_voltage_from_duty_pct(limit);
  pbio_dcmotor_get_settings(motor->dcmotor, &old_value);
  pbio_dcmotor_set_settings(motor->dcmotor, new_value);
  return old_value;
}

void pup_motor_restore_duty_limit(pup_motor_t *motor, int old_value) {
  pbio_dcmotor_set_settings(motor->dcmotor, old_value);
}
