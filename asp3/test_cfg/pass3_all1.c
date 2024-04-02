/*
 *		全静的APIのパス3でのエラー検出のテスト(1)
 * 
 *  $Id: pass3_all1.c 1441 2020-05-25 04:35:47Z ertl-hiro $
 */

#include "cfg_common.h"

/*
 *  スタック領域，カーネルの管理領域の定義
 */
STK_T	stack_TASK3[COUNT_STK_T(STACK_SIZE)];
MPF_T	mpf_MPF1[20 * COUNT_MPF_T(200)];
STK_T	istack[COUNT_STK_T(10 * STACK_SIZE)];

/*
 *  通知用の変数の定義
 */
volatile intptr_t	nvar1;
volatile intptr_t	nvar2;

/*
 *  関数の定義
 */
void	task1(EXINF exinf) { }
void	task3(EXINF exinf) { }
void	task4(EXINF exinf) { }
void	cyclic1_handler(EXINF exinf) { }
void	alarm1_handler(EXINF exinf) { }
void	isr1(EXINF exinf) { }
void	isr2(EXINF exinf) { }
void	isr3(EXINF exinf) { }
void	int1_handler(void) { }
void	cpuexc1_handler(void *p_excinf) { }
void	cpuexc2_handler(void *p_excinf) { }
void	initialize1_routine(EXINF exinf) { }
void	terminate1_routine(EXINF exinf) { }
