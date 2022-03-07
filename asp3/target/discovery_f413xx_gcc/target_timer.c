/*
 *  TOPPERS/ASP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Advanced Standard Profile Kernel
 * 
 *  Copyright (C) 2016 by Embedded and Real-Time Systems Laboratory
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
 *  $Id: target_timer.c 648 2016-02-20 00:50:56Z ertl-honda $
 */

/*
 *		タイマドライバ（TIM用）
 *		 TIM2をフリーランニング，TIM5を割込み通知用に使用する．
 */

#include "kernel_impl.h"
#include "time_event.h"
#include "target_timer.h"
#include <sil.h>

/*
 *  STM32CubeのTIMのハンドル
 */
TIM_HandleTypeDef Tim2Handle;
TIM_HandleTypeDef Tim5Handle;

/*
 *  タイマの起動処理
 */
void
target_hrt_initialize(intptr_t exinf)
{
	/*
	 * フリーランニングタイマ
	 */
	/* Enable Clock. Use APB1 timer clock */
	__HAL_RCC_TIM2_CLK_ENABLE();

	Tim2Handle.Instance = TIM2;
	Tim2Handle.Init.Period = 0xFFFFFFFF;
	Tim2Handle.Init.Prescaler = (CPU_CLOCK_HZ/1000000);
	Tim2Handle.Init.ClockDivision = 0;
	Tim2Handle.Init.CounterMode = TIM_COUNTERMODE_UP;

	if(HAL_TIM_Base_Init(&Tim2Handle) != HAL_OK) {
		/* Initialization Error */
		Error_Handler();
	}

	if(HAL_TIM_Base_Start(&Tim2Handle) != HAL_OK) {
		/* Initialization Error */
		Error_Handler();
	}

	/*
	 *  割込み通知用タイマ
	 */
	/* Enable Clock. Use APB1 timer clock */
	__HAL_RCC_TIM5_CLK_ENABLE();

	Tim5Handle.Instance = TIM5;
	Tim5Handle.Init.Period = 0xFF;
	Tim5Handle.Init.Prescaler = (CPU_CLOCK_HZ/1000000);
	Tim5Handle.Init.ClockDivision = 0;
	Tim5Handle.Init.CounterMode = TIM_COUNTERMODE_UP;

	/* One-pulse Mode */
	if(HAL_TIM_OnePulse_Init(&Tim5Handle,TIM_OPMODE_SINGLE) != HAL_OK) {
		/* Initialization Error */
		Error_Handler();
	}
	/* clear interrupt */
	__HAL_TIM_CLEAR_FLAG(&Tim5Handle, TIM_SR_UIF);
	/* enable interrupt */
	__HAL_TIM_ENABLE_IT(&Tim5Handle, TIM_IT_UPDATE);
}

/*
 *  タイマの停止処理
 */
void
target_hrt_terminate(intptr_t exinf)
{
	if(HAL_TIM_Base_Stop(&Tim2Handle) != HAL_OK) {
		/* Initialization Error */
		Error_Handler();
	}
	if(HAL_TIM_Base_Stop(&Tim5Handle) != HAL_OK) {
		/* Initialization Error */
		Error_Handler();
	}
}

/*
 *  タイマ割込みハンドラ
 */
void
target_hrt_handler(void)
{
	/* Clear Event */
	__HAL_TIM_CLEAR_FLAG(&Tim5Handle, TIM_SR_UIF);
	/*
	 *  高分解能タイマ割込みを処理する．
	 */
	signal_time();
}
