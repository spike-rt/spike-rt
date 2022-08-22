/*
 *  TOPPERS Software
 *      Toyohashi Open Platform for Embedded Real-Time Systems
 * 
 *  Copyright (C) 2006-2016 by Embedded and Real-Time Systems Laboratory
 *              Graduate School of Information Science, Nagoya Univ., JAPAN
 * 
 *  上記著作権者は，以下の(1)〜(4)の条件を満たす場合に限り，本ソフトウェ
 *  ア（本ソフトウェアを改変したものを含む．以下同じ）を使用・複製・改
 *  変・再配布（以下，利用と呼ぶ）することを無償で許諾する．
 *  (1) 本ソフトウェアをソースコードの形で利用する場合には，上記の著作
 *      権表示，この利用条件および下記の無保証規定が，そのままの形でソー
 *      スコード中に含まれていること．
 *  (2) 本ソフトウェアを，ライブラリ形式など，他のソフトウェア開発に使
 *      用できる形で再配布する場合には，再配布に伴うドキュメント（利用
 *      者マニュアルなど）に，上記の著作権表示，この利用条件および下記
 *      の無保証規定を掲載すること．
 *  (3) 本ソフトウェアを，機器に組み込むなど，他のソフトウェア開発に使
 *      用できない形で再配布する場合には，次のいずれかの条件を満たすこ
 *      と．
 *    (a) 再配布に伴うドキュメント（利用者マニュアルなど）に，上記の著
 *        作権表示，この利用条件および下記の無保証規定を掲載すること．
 *    (b) 再配布の形態を，別に定める方法によって，TOPPERSプロジェクトに
 *        報告すること．
 *  (4) 本ソフトウェアの利用により直接的または間接的に生じるいかなる損
 *      害からも，上記著作権者およびTOPPERSプロジェクトを免責すること．
 *      また，本ソフトウェアのユーザまたはエンドユーザからのいかなる理
 *      由に基づく請求からも，上記著作権者およびTOPPERSプロジェクトを
 *      免責すること．
 * 
 *  本ソフトウェアは，無保証で提供されているものである．上記著作権者お
 *  よびTOPPERSプロジェクトは，本ソフトウェアに関して，特定の使用目的
 *  に対する適合性も含めて，いかなる保証も行わない．また，本ソフトウェ
 *  アの利用により直接的または間接的に生じたいかなる損害に関しても，そ
 *  の責任を負わない．
 * 
 *  $Id: nucleo_f401re.h 648 2016-02-20 00:50:56Z ertl-honda $
 */

/*
 *		NUCLEO F401RE サポートモジュール
 */

#ifndef TOPPERS_PRIMEHUB_H
#define TOPPERS_PRIMEHUB_H

/*
 *  コアのクロック周波数（96MHz）
 */
#define CPU_CLOCK_HZ	96000000

/*
 *  割込み数
 */
#define TMAX_INTNO (101 + 16)

/*
 *  微少時間待ちのための定義（本来はSILのターゲット依存部）
 */
#define SIL_DLY_TIM1    162
#define SIL_DLY_TIM2    100

#ifndef TOPPERS_MACRO_ONLY
/*
 *  tecsgen実行時にstm32f4xx_nucleo.hを読み込むとtecsgenがエラーとなるため
 *  tecsgen実行時に必要な定義はこのファイルで行う
 */
#ifndef TECSGEN
#include "stm32f4xx_nucleo.h"
#else /* !TECSGEN */
#define UART9_BASE  0x40011800U
#define UART9_IRQn  88
#endif /* TECSGEN */
#endif /* TOPPERS_MACRO_ONLY */

/*
 *  USART関連の定義
 */
#define USART_INTNO (UART9_IRQn + 16)
#define USART_NAME  UART9
#define USART_BASE  UART9_BASE 

/*
 *  ボーレート
 */
#define BPS_SETTING  (115200)

#ifndef TOPPERS_MACRO_ONLY
#ifndef TECSGEN
/*
 *  UsartのクロックとIOの初期化
 */
Inline void
usart_low_init(void) {
	GPIO_InitTypeDef  GPIO_InitStruct;

	/* Enable Clock */
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_UART9_CLK_ENABLE();
  
	/* UART TX GPIO pin configuration  */
	GPIO_InitStruct.Pin       = GPIO_PIN_15;
	GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull      = GPIO_PULLUP;
	GPIO_InitStruct.Speed     = GPIO_SPEED_FAST;
	GPIO_InitStruct.Alternate = GPIO_AF11_UART9;

	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
    
	/* UART RX GPIO pin configuration  */
	GPIO_InitStruct.Pin = GPIO_PIN_14;
	GPIO_InitStruct.Alternate = GPIO_AF11_UART9;
    
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
}
#endif /* TECSGEN */
#endif /* TOPPERS_MACRO_ONLY */

#endif /* TOPPERS_PRIMEHUB_H */
