/*
 * SPDX-License-Identifier: MIT
 *
 * Header for Pybricks Task.
 *
 * Copyright (c) 2022 Embedded and Real-Time Systems Laboratory,
 *            Graduate School of Information Science, Nagoya Univ., JAPAN
 */

#include <kernel.h>

/*
 * Macro definitions
 */
#ifndef TOPPERS_USE_QEMU
#define PYBRICKS_TASK_ATR TA_ACT
#else
#define PYBRICKS_TASK_ATR TA_NULL
#endif

#define PYBRICKS_PRIORITY	4		/* タスクの優先度 */

#define	PYBRICKS_STACK_SIZE		0x2000		/* タスクのスタックサイズ 8KB */

#define INHNO_DMA1_STREAM3  (DMA1_Stream3_IRQn + 16)	/* 割込みハンドラ番号 */
#define INTNO_DMA1_STREAM3  (DMA1_Stream3_IRQn + 16)	/* 割込み番号 */
#define INTPRI_DMA1_STREAM3 (TMIN_INTPRI + 5)	/* 割込み優先度 */
#define INTATR_DMA1_STREAM3 TA_NULL				/* 割込み属性 */

#define INHNO_DMA1_STREAM4  (DMA1_Stream4_IRQn + 16)	/* 割込みハンドラ番号 */
#define INTNO_DMA1_STREAM4  (DMA1_Stream4_IRQn + 16)	/* 割込み番号 */
#define INTPRI_DMA1_STREAM4 (TMIN_INTPRI + 5)	/* 割込み優先度 */
#define INTATR_DMA1_STREAM4 TA_NULL				/* 割込み属性 */

#define INHNO_DMA1_STREAM5	(DMA1_Stream5_IRQn + 16)	/* 割込みハンドラ番号 */
#define INTNO_DMA1_STREAM5	(DMA1_Stream5_IRQn + 16)	/* 割込み番号 */
#define INTPRI_DMA1_STREAM5	(TMIN_INTPRI + 4)	/* 割込み優先度 */
#define INTATR_DMA1_STREAM5	TA_NULL				/* 割込み属性 */

#define INHNO_DMA1_STREAM6	(DMA1_Stream6_IRQn + 16)	/* 割込みハンドラ番号 */
#define INTNO_DMA1_STREAM6	(DMA1_Stream6_IRQn + 16)	/* 割込み番号 */
#define INTPRI_DMA1_STREAM6	(TMIN_INTPRI + 1)	/* 割込み優先度 */
#define INTATR_DMA1_STREAM6	TA_NULL				/* 割込み属性 */

#define INHNO_DMA1_STREAM7	(DMA1_Stream7_IRQn + 16)	/* 割込みハンドラ番号 */
#define INTNO_DMA1_STREAM7	(DMA1_Stream7_IRQn + 16)	/* 割込み番号 */
#define INTPRI_DMA1_STREAM7	(TMIN_INTPRI + 1)	/* 割込み優先度 */
#define INTATR_DMA1_STREAM7	TA_NULL				/* 割込み属性 */

#define INHNO_DMA2_STREAM0	(DMA2_Stream0_IRQn + 16)	/* 割込みハンドラ番号 */
#define INTNO_DMA2_STREAM0	(DMA2_Stream0_IRQn + 16)	/* 割込み番号 */
#define INTPRI_DMA2_STREAM0	(TMIN_INTPRI + 7)	/* 割込み優先度 */
#define INTATR_DMA2_STREAM0	TA_NULL				/* 割込み属性 */

#define INHNO_DMA2_STREAM2	(DMA2_Stream2_IRQn + 16)	/* 割込みハンドラ番号 */
#define INTNO_DMA2_STREAM2	(DMA2_Stream2_IRQn + 16)	/* 割込み番号 */
#define INTPRI_DMA2_STREAM2	(TMIN_INTPRI + 7)	/* 割込み優先度 */
#define INTATR_DMA2_STREAM2	TA_NULL				/* 割込み属性 */

#define INHNO_DMA2_STREAM3	(DMA2_Stream3_IRQn + 16)	/* 割込みハンドラ番号 */
#define INTNO_DMA2_STREAM3	(DMA2_Stream3_IRQn + 16)	/* 割込み番号 */
#define INTPRI_DMA2_STREAM3	(TMIN_INTPRI + 7)	/* 割込み優先度 */
#define INTATR_DMA2_STREAM3	TA_NULL				/* 割込み属性 */

#define INHNO_USART2		(USART2_IRQn + 16)	/* 割込みハンドラ番号 */
#define INTNO_USART2		(USART2_IRQn + 16)	/* 割込み番号 */
#define INTPRI_USART2	(TMIN_INTPRI + 1)	/* 割込み優先度 */
#define INTATR_USART2	TA_NULL				/* 割込み属性 */

#define INHNO_UART4		(UART4_IRQn + 16)	/* 割込みハンドラ番号 */
#define INTNO_UART4		(UART4_IRQn + 16)	/* 割込み番号 */
#define INTPRI_UART4	(TMIN_INTPRI + 0)	/* 割込み優先度 */
#define INTATR_UART4	TA_NULL				/* 割込み属性 */

#define INHNO_UART5		(UART5_IRQn + 16)	/* 割込みハンドラ番号 */
#define INTNO_UART5		(UART5_IRQn + 16)	/* 割込み番号 */
#define INTPRI_UART5	(TMIN_INTPRI + 0)	/* 割込み優先度 */
#define INTATR_UART5	TA_NULL				/* 割込み属性 */

#define INHNO_UART7		(UART7_IRQn + 16)	/* 割込みハンドラ番号 */
#define INTNO_UART7		(UART7_IRQn + 16)	/* 割込み番号 */
#define INTPRI_UART7	(TMIN_INTPRI + 0)	/* 割込み優先度 */
#define INTATR_UART7	TA_NULL				/* 割込み属性 */

#define INHNO_UART8		(UART8_IRQn + 16)	/* 割込みハンドラ番号 */
#define INTNO_UART8		(UART8_IRQn + 16)	/* 割込み番号 */
#define INTPRI_UART8	(TMIN_INTPRI + 0)	/* 割込み優先度 */
#define INTATR_UART8	TA_NULL				/* 割込み属性 */

#define INHNO_UART9		(UART9_IRQn + 16)	/* 割込みハンドラ番号 */
#define INTNO_UART9		(UART9_IRQn + 16)	/* 割込み番号 */
#define INTPRI_UART9	(TMIN_INTPRI + 0)	/* 割込み優先度 */
#define INTATR_UART9	TA_NULL				/* 割込み属性 */

#define INHNO_UART10	(UART10_IRQn + 16)	/* 割込みハンドラ番号 */
#define INTNO_UART10	(UART10_IRQn + 16)	/* 割込み番号 */
#define INTPRI_UART10	(TMIN_INTPRI + 0)	/* 割込み優先度 */
#define INTATR_UART10	TA_NULL				/* 割込み属性 */

#define INHNO_SPI1	(SPI1_IRQn + 16)	/* 割込みハンドラ番号 */
#define INTNO_SPI1	(SPI1_IRQn + 16)	/* 割込み番号 */
#define INTPRI_SPI1	(TMIN_INTPRI + 7)	/* 割込み優先度 */
#define INTATR_SPI1	TA_NULL				/* 割込み属性 */

#define INHNO_SPI2	(SPI2_IRQn + 16)	/* 割込みハンドラ番号 */
#define INTNO_SPI2	(SPI2_IRQn + 16)	/* 割込み番号 */
#define INTPRI_SPI2	(TMIN_INTPRI + 6)	/* 割込み優先度 */
#define INTATR_SPI2	TA_NULL				/* 割込み属性 */

#define INHNO_OTG_FS	(OTG_FS_IRQn + 16)	/* 割込みハンドラ番号 */
#define INTNO_OTG_FS	(OTG_FS_IRQn + 16)	/* 割込み番号 */
#define INTPRI_OTG_FS	(TMIN_INTPRI + 6)	/* 割込み優先度 */
#define INTATR_OTG_FS	TA_NULL				/* 割込み属性 */

#define INHNO_I2C2_ER	(I2C2_ER_IRQn + 16)	/* 割込みハンドラ番号 */
#define INTNO_I2C2_ER	(I2C2_ER_IRQn + 16)	/* 割込み番号 */
#define INTPRI_I2C2_ER	(TMIN_INTPRI + 3)	/* 割込み優先度 */
#define INTATR_I2C2_ER	TA_NULL				/* 割込み属性 */

#define INHNO_I2C2_EV	(I2C2_EV_IRQn + 16)	/* 割込みハンドラ番号 */
#define INTNO_I2C2_EV	(I2C2_EV_IRQn + 16)	/* 割込み番号 */
#define INTPRI_I2C2_EV	(TMIN_INTPRI + 3)	/* 割込み優先度 */
#define INTATR_I2C2_EV	TA_NULL				/* 割込み属性 */

#define INHNO_EXTI9_5	(EXTI9_5_IRQn + 16)	/* 割込みハンドラ番号 */
#define INTNO_EXTI9_5	(EXTI9_5_IRQn + 16)	/* 割込み番号 */
#define INTPRI_EXTI9_5	(TMIN_INTPRI + 6)	/* 割込み優先度 */
#define INTATR_EXTI9_5	TA_NULL				/* 割込み属性 */


/*
 * Prototype declarations
 */
#ifndef TOPPERS_MACRO_ONLY

extern void	pb_main_task(intptr_t exinf);
extern void pbdrv_timer_handler(void);

#endif /* TOPPERS_MACRO_ONLY */
