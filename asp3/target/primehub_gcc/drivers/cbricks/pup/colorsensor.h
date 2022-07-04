// SPDX-License-Identifier: MIT
/*
 * API for color sensors
 *
 * Copyright (c) 2022 Embedded and Real-Time Systems Laboratory,
 *                    Graduate School of Information Science, Nagoya Univ., JAPAN
 */

/**
 * \file    cbricks/pup/colorsensor.h
 * \brief	  API for color sensors
 * \author	Kajita Shun
 */

/**
 * \addtogroup  PUPDevice
 * @{
 */

/**
 * \~English
 * \defgroup ColorSensor Color Sensor
 * \brief	  APIs for color sensors
 * @{
 *
 * \~Japanese
 * \defgroup ColorSensor カラーセンサ
 * \brief    カラーセンサのAPI．
 * @{
 */


#ifndef _PUP_COLOR_SENSOR_H_
#define _PUP_COLOR_SENSOR_H_

#include <stdint.h>
#include <cbricks/pup_device.h>
#include <pbio/color.h>

/**
 * \~English
 * \brief					Get the PUP device pointer of the color sensor specified with the port ID.
 * \details				If it fails, it outputs an error log and returns NULL.
 * \param		port	PUP port ID to be inquired.
 * \return	pdev	PUP device pointer.
 *
 * \~Japanese
 * \brief				ポートIDで指定されたカラーセンサへのPUPデバイスポインタを取得する。
 * \details			失敗した場合、エラーログを出力し、NULLを返す。
 * \param port	PUPポートID。
 * \return			PUPデバイスポインタ。
 */
pup_device_t *pup_color_sensor_get_device(pbio_port_id_t port);

/**
 * \~English
 * \brief					Get the color of surface or an extra source by a color sensor.
 * \details				By default, it detects red, yellow, green, blue, white, none. You can choose color detected by pup_detectale_colors()
 * \param				pdev	PUP device pointer to be inquired.
 * \param				true	Scan from a surface.
 * \param				false	Scan from a external light sources.
 * \return				Color in hsv style.
 *
 * \~Japanese
 * \brief					カラーセンサで色を測定する。
 * \details				デフォルトでは、赤色、黄色、緑色、青色、白色、無しが検出される。
 * \param					PUPデバイスポインタ
 * \param					trueならば表面の色から、falseならば他の光源の色を検出する。
 * \return				色（hsvによる表現）
 */
pbio_color_hsv_t pup_color_sensor_color(pup_device_t *pdev, bool surface);

/**
 * \~English
 * \brief					Get the color of surface or an extra source by a color sensor.
 * \details				Unlike pup_color_sensor_color(), it gives the hsv detected directly, instead of rounding it to the nearest detectable colors.
 * \param				pdev  PUP device pointer to be inquired.
 * \param				true	Scan from a surface.
 * \param				false	Scan from a external light sources.
 * \return				Color in hsv style.
 *
 * \~Japanese
 * \brief					カラーセンサで色を測定する。
 * \details				pup_color_sensor_color()と異なり、最も近い色に近似されず検出したhsv値を返す。
 * \param					PUPデバイスポインタ
 * \param					trueならば表面の色から、falseならば他の光源の色を検出する。
 * \return				色（hsvによる表現）
 */
pbio_color_hsv_t pup_color_sensor_hsv(pup_device_t *pdev, bool surface);

/**
 * \~English
 * \brief					Get the percentage of how much a surface reflects the light emmited by the sensor.
 * \param				pdev PUP device pointer to be inquired.	
 * \return				Mesured reflection in percentage style.
 *
 * \~Japanese
 * \brief					センサーの発する光を表面がどの程度反射するかを測定する。
 * \param					PUPデバイスポインタ
 * \return				どの程度反射しているか。（％）
 */
int32_t pup_color_sensor_reflection(pup_device_t *pdev);

/**
 * \~English
 * \brief					Get the ambient light intensity.
 * \param				pdev PUP device pointer to be inquired.	
 * \return				Ambient light intensity in percentage style.
 *
 * \~Japanese
 * \brief					周囲の光の強度を測定する。
 * \param					PUPデバイスポインタ
 * \return				周囲の光の強度（％）
 */
int32_t pup_color_sensor_ambient(pup_device_t *pdev);

/**
 * \~English
 * \brief 	     Set the three light brightneses of the color sensor.
 * \param  pdev  PUP device pointer．
 * \param  bv1   Brightness.
 * \retval err   Error number.
 *
 * \~Japanese
 * \brief 	     カラーセンサのライトを設定する。
 * \param  pdev  PUP デバイスポインタ。
 * \param  bv1   輝度。
 * \retval err   エラー番号。
 */
pbio_error_t pup_color_sensor_light_set(pup_device_t *pdev, 
                                             int32_t bv1, int32_t bv2, int32_t bv3);

/**
 * \~English
 * \brief 	     Set the three light brightneses of the color sensor.
 * \details			 Same as pup_color_sensor_light_set(pdev, 100, 100, 100)
 * \param  pdev  PUP device pointer.
 * \retval err   Error number.
 *
 * \~Japanese
 * \brief 	     カラーセンサのライトを点灯する。
 * \details			 pup_color_sensor_light_set(pdev, 100, 100, 100) と同じ。
 * \param  pdev  PUP デバイスポインタ。
 * \retval err   エラー番号。
 */
pbio_error_t pup_color_sensor_light_on(pup_device_t *pdev);

/**
 * \~English
 * \brief 	     Set the three light brightneses of the color sensor.
 * \details			 Same as pup_color_sensor_light_set(pdev, 0, 0, 0)
 * \param  pdev  PUP device pointer.
 * \retval err   Error number.
 *
 * \~Japanese
 * \brief 	     カラーセンサのライトを消灯する。
 * \details			 pup_color_sensor_light_set(pdev, 0, 0, 0) と同じ。
 * \param  pdev  PUP デバイスポインタ。
 * \retval err   エラー番号。
 */
pbio_error_t pup_color_sensor_light_off(pup_device_t *pdev);

/**
 * \~English
 * \brief 	     Configure colors which color sensor should detect.
 * \param  size  Size of the colors array.
 * \param  colors  Array of colors.
 * \retval err   Error number.
 *
 * \~Japanese
 * \brief 	     カラーセンサが検知する色を設定する。
 * \param  			カラーの配列とそのサイズ。 
 * \retval err   色。
 */
pbio_color_hsv_t *pup_color_sensor_detectable_colors(int32_t size, pbio_color_hsv_t *colors);

#endif // _PUP_COLOR_SENSOR_H_

/**
 * @} // End of group
 */

/**
 * @} // End of group
 */
