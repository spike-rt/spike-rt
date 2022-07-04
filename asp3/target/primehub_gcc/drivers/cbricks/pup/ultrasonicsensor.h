// SPDX-License-Identifier: MIT
/*
 * API for ultrasonic sensors
 *
 * Copyright (c) 2022 Embedded and Real-Time Systems Laboratory,
 *                    Graduate School of Information Science, Nagoya Univ., JAPAN
 */

/**
 * \file    cbricks/pup/ultrasonicsensor.h
 * \brief   API for ultrasonic sensors
 * \author  Shu Yoshifumi
 */

/**
 * \addtogroup  PUPDevice
 * @{
 */

/**
 * \~English
 * \defgroup  UltrasonicSensor Ultrasonic Sensor
 * \brief     APIs for ultrasonic sensors
 * @{
 *
 * \~Japanese
 * \defgroup  UltrasonicSensor 超音波センサ
 * \brief     超音波センサのAPI．
 * @{
 */


#ifndef _PUP_ULTRASONIC_SENSOR_H_
#define _PUP_ULTRASONIC_SENSOR_H_

#include <stdint.h>
#include <cbricks/pup_device.h>

/**
 * \~English
 * \brief         Get the PUP device pointer of the ultrasonic sensor specified with the port ID.
 * \details       If it fails, it outputs an error log and returns NULL.
 * \param   port  PUP port ID to be inquired.
 * \return  pdev  PUP device pointer.
 * \return  NULL  PUP device pointer.
 *
 * \~Japanese
 * \brief 	    ポート ID で指定された超音波センサへの PUP デバイスポインタを取得する．
 * \details     失敗した場合，エラーログを出力し，NULL を返す．
 * \param port  PUP ポート ID．
 * \return      PUP デバイスポインタ．
 */
pup_device_t *pup_ultrasonic_sensor_get_device(pbio_port_id_t port);

/**
 * \~English
 * \brief 	    Get the distance by a ultrasonic sensor.
 * \details     Always returns 0 (error log is output) when an invalid sensor number is specified.
 * \param pdev  PUP Device pointer to be inquired
 * \return      Distance in centimeters.
 *
 * \~Japanese
 * \brief 	    超音波センサで距離を測定する．
 * \details     失敗した場合，エラーログを出力し，負の値を返す．
 * \param pdev  PUP デバイスポインタ．
 * \return      距離（単位：mm ）
 */
int32_t pup_ultrasonic_sensor_distance(pup_device_t *pdev);

/**
 * \~English
 * \brief 	     Check a ultrasonic signal by a ultrasonic sensor.
 * \details      When an invalid sensor support number is specified, always returns false (error log is output).
 * \param port   Sensor port to be inquired
 * \retval true  A signal has been received
 * \retval false No signal has been received
 *
 * \~Japanese
 * \brief 	     超音波センサで超音波信号を検出する．
 * \details      失敗した場合，\a false を返す．
 * \param  pdev  PUP デバイスポインタ．
 * \retval true  超音波を検出した．
 * \retval false 超音波を検出しなかった．
 */
bool pup_ultrasonic_sensor_presence(pup_device_t *pdev);

/**
 * \~English
 * \brief 	     Set the four light brightneses of the ultrasonic sensor.
 * \param  pdev  PUP デバイスポインタ．
 * \param  bv1   Brightness.
 * \retval err   Error number.
 *
 * \~Japanese
 * \brief 	     超音波センサで超音波信号を検出する．
 * \details      失敗した場合，\a false を返す．
 * \param  pdev  PUP デバイスポインタ．
 * \param  bv1   輝度.
 * \retval err   エラー番号．
 */
pbio_error_t pup_ultrasonic_sensor_light_set(pup_device_t *pdev, 
                                             int32_t bv1, int32_t bv2, int32_t bv3, int32_t bv4);

/**
 * \~English
 * \brief 	     Set the four light brightneses of the ultrasonic sensor.
 * \param  pdev  PUP デバイスポインタ．
 * \retval err   Error number.
 *
 * \~Japanese
 * \brief 	    超音波センサのライトを点灯させる．
 * \details     pup_ultrasonic_sensor_light_set(pdev, 100, 100, 100, 100) と同じ．
 * \param  pdev PUP デバイスポインタ．
 * \retval err  エラー番号．
 */
pbio_error_t pup_ultrasonic_sensor_light_on(pup_device_t *pdev);

/**
 * \~English
 * \brief 	     Set the four light brightneses of the ultrasonic sensor.
 * \param  pdev  PUP デバイスポインタ．
 * \retval err   Error number.
 *
 * \~Japanese
 * \brief 	    超音波センサのライトを消灯させる．
 * \details     pup_ultrasonic_sensor_light_set(pdev, 0, 0, 0, 0) と同じ．
 * \param  pdev PUP デバイスポインタ．
 * \retval err  エラー番号．
 */
pbio_error_t pup_ultrasonic_sensor_light_off(pup_device_t *pdev);

#endif // _PUP_ULTRASONIC_SENSOR_H_

/**
 * @} // End of group
 */

/**
 * @} // End of group
 */

