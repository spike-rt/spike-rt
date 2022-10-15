// SPDX-License-Identifier: MIT
/*
 * API for motors
 *
 * Copyright (c) 2022 Embedded and Real-Time Systems Laboratory,
 *                    Graduate School of Information Science, Nagoya Univ., JAPAN
 */

/**
 * \file    cbricks/pup/motor.h
 * \brief   API for motors
 * \author  Makoto Shimojima
 */

/**
 * \addtogroup  PUPDevice
 * @{
 */

/**
 * \~English
 * \defgroup Motor Motors
 * \brief    APIs for motors
 * @{
 *
 * \~Japanese
 * \defgroup Motor モータ
 * \brief    モータのAPI．
 * @{
 */

#ifndef _PUP_MOTOR_H_
#define _PUP_MOTOR_H_

#include <pbio/port.h>
#include <pbio/servo.h>

/**
 * \~English
 * \brief    Get the PUP motor device pointer of the motor specified by the port ID.
 * \details  If it fails, it outputs an error log and returns NULL.
 * \param port PUP port ID of the device.
 * \return   PUP motor device pointer.
 *
 * \~Japanese
 * \brief    ポートIDで指定されたモータへのPUPモータデバイスポインタを取得する．
 * \details  失敗した場合，エラーログを出力し，NULLを返す．
 * \param port PUPポートID．
 * \return   PUPモータデバイスポインタ．
 */
typedef pbio_servo_t pup_motor_t;
pup_motor_t *pup_motor_get_device(pbio_port_id_t port);

/**
 * \~English
 * \brief    Setup the PUP motor with the specified direction, possibly resetting the encoder.
 * \details  It could return PBIO_ERROR_AGAIN and (PBIO_ERROR_NO_DEV, too), if it is called "too soon",
 *           in which case you should call this function again sometime later. Errors other than PBIO_ERROR_AGAIN
 *           are logged.
 * \param motor PUP motor device pointer.
 * \param positive_direction Either PUP_DIRECTION_CLOCKWISE or PUP_DIRECTION_COUNTERCLOCKWISE to indicate the desiredi
 *                           positive direction..
 * \param reset_count Indicate if counter should be reset.
 * \return   PBIO_SUCCESS or error number.
 *
 * \~Japanese
 * \brief    モータを初期化する．
 * \details  PBIO_ERROR_AGAINが返ってき場合は時間をおいてもう一度この関数を呼ぶこと．その他のエラーで失敗した場合は
 *           エラーログに出力する．
 * \param motor PUPモータデバイスポインタ．
 * \param positive_direction モータの正回転方向をPUP_DIRECTION_CLOCKWISEまたはPUP_DIRECTION_COUNTERCLOCKWISEで示す．
 * \param reset_count trueかfalseでエンコーダの値をリセットするか示す．
 * \return   PBIO_SUCCESSまたはエラー番号．
 */
typedef enum {
  PUP_DIRECTION_CLOCKWISE        = PBIO_DIRECTION_CLOCKWISE,
  PUP_DIRECTION_COUNTERCLOCKWISE = PBIO_DIRECTION_COUNTERCLOCKWISE,
} pup_direction_t;
pbio_error_t pup_motor_setup(pup_motor_t *motor, pup_direction_t positive_direction, bool reset_count);

/**
 * \~English
 * \brief    Reset the motor encoder.
 * \param motor PUP motor device pointer.
 * \return   PBIO_SUCCESS or error number.
 *
 * \~Japanese
 * \brief    エンコーダをリセットする．
 * \param motor PUPモータデバイスポインタ．
 * \return   PBIO_SUCCESSまたはエラー番号．
 */
pbio_error_t pup_motor_reset_count(pup_motor_t *motor);

/**
 * \~English
 * \brief    Get the encoder value of the motor.
 * \param motor PUP motor device pointer.
 * \return   Encoder value in degree.
 *
 * \~Japanese
 * \brief    エンコーダの値を取得する．
 * \param motor PUPモータデバイスポインタ．
 * \return   エンコーダの値 [°]．
 */
int32_t pup_motor_get_count(pup_motor_t *motor);

/**
 * \~English
 * \brief    Get the speed of rotation of the motor.
 * \param motor PUP motor device pointer.
 * \return   Speed in degree/sec.
 *
 * \~Japanese
 * \brief    モータの回転速度を取得する．
 * \param motor PUPモータデバイスポインタ．
 * \return   回転速度 [°/秒]．
 */
int32_t pup_motor_get_speed(pup_motor_t *motor);

/**
 * \~English
 * \brief    Set the speed of rotation of the motor.
 * \param motor PUP motor device pointer.
 * \param speed Speed of rotation in degree/sec.
 * \return   PBIO_SUCCESS or error number.
 *
 * \~Japanese
 * \brief    モータの回転速度を設定する．
 * \param motor PUPモータデバイスポインタ．
 * \param speed モータの回転速度 [°/秒]．
 * \return   PBIO_SUCCESSまたはエラー番号．
 */

pbio_error_t pup_motor_set_speed(pup_motor_t *motor, int speed);

/**
 * \~English
 * \brief    Stop the motor in coast mode.
 * \param motor PUP motor device pointer.
 * \return   PBIO_SUCCESS or error number.
 *
 * \~Japanese
 * \brief    モータを止める．
 * \param motor PUPモータデバイスポインタ．
 * \return   PBIO_SUCCESSまたはエラー番号．
 */
pbio_error_t pup_motor_stop(pup_motor_t *motor);

/**
 * \~English
 * \brief    Stop the motor in brake mode.
 * \param motor PUP motor device pointer.
 * \return   PBIO_SUCCESS or error number.
 *
 * \~Japanese
 * \brief    ブレーキをかけてモータを止める．
 * \param motor PUPモータデバイスポインタ．
 * \return   PBIO_SUCCESSまたはエラー番号．
 */
pbio_error_t pup_motor_brake(pup_motor_t *motor);

/**
 * \~English
 * \brief    Stop the motor and hold it.
 * \param motor PUP motor device pointer.
 * \return   PBIO_SUCCESS or error number.
 *
 * \~Japanese
 * \brief    モータを止めて角度を維持する．
 * \param motor PUPモータデバイスポインタ．
 * \return   PBIO_SUCCESSまたはエラー番号．
 */
pbio_error_t pup_motor_hold(pup_motor_t *motor);

/**
 * \~English
 * \brief    Check if the motor is stalled.
 * \details  Call pup_motor_set_duty_limit before actuating the motor to avoid applying the full motor torque.
 * \param motor PUP motor device pointer.
 * \return   true or false.
 *
 * \~Japanese
 * \brief    モータがストールしているか調べる．
 * \detail   モータを動かす前にpup_motor_set_duty_limitを呼んで最大トルクを下げておくと感度を調整することができる．
 * \param motor PUPモータデバイスポインタ．
 * \return   true or false．
 */
bool pup_motor_is_stalled(pup_motor_t *motor);

/**
 * \~English
 * \brief    Lower the duty limit of the motor.
 * \detail   To restore the limit, call pup_motor_restore_duty_limit with the value returned as its argument.
 * \param motor PUP motor device pointer.
 * \param duty_limit new limit (0-100),
 * \return   max voltage to restore the limit back to the original conditions.
 *
 * \~Japanese
 * \brief    モータのデューティ値を下げる．
 * \detail   元の状態に戻すにはこの関数の戻り値を使ってpup_motor_restore_duty_limitを呼ぶ．．
 * \param motor PUPモータデバイスポインタ．
 * \param duty_limit 新しいデューティ値（0-100）
 * \return   元の状態に戻すための最大電圧．
 */
int32_t pup_motor_set_duty_limit(pup_motor_t *motor, int duty_limit);

/**
 * \~English
 * \brief    Restore the duty limit of the motor.
 * \param motor PUP motor device pointer.
 * \param old_value Voltage returned from pup_motor_set_duty_limit.
 *
 * \~Japanese
 * \brief    モータのデューティ値を元に戻す．．
 * \param motor PUPモータデバイスポインタ．
 * \param old_value pup_motor_set_duty_limitの戻り値．
 */
void pup_motor_restore_duty_limit(pup_motor_t *motor, int old_value);

#endif // _PUP_MOTOR_H_

/**
 * @} // End of group
 */

/**
 * @} // End of group
 */


