/* kernel_cfg.c */
#include "kernel/kernel_int.h"
#include "kernel_cfg.h"

#if !(TKERNEL_PRID == 0x0007U && (TKERNEL_PRVER & 0xf000U) == 0x3000U)
#error The kernel does not match this configuration file.
#endif

/*
 *  Include Directives
 */

#include "target_timer.h"
#ifdef TOPPERS_SUPPORT_OVRHDR
#endif
#include "cfg_common.h"

/*
 *  Task Management Functions
 */

const ID _kernel_tmax_tskid = (TMIN_TSKID + TNUM_TSKID - 1);

static STK_T _kernel_stack_TASK1[COUNT_STK_T(STACK_SIZE)];
const TINIB _kernel_tinib_table[TNUM_TSKID] = {
	{ (TA_ACT), (EXINF)(1), (TASK)(task1), INT_PRIORITY(MID_PRIORITY), ROUND_STK_T(STACK_SIZE), _kernel_stack_TASK1 },
	{ (TA_NULL), (EXINF)(2), (TASK)(task2), INT_PRIORITY(HIGH_PRIORITY), STACK_SIZE, (void *)(stack_TASK2) }
};

TCB _kernel_tcb_table[TNUM_TSKID];

const ID _kernel_torder_table[TNUM_TSKID] = { 
	TASK1, TASK2
};

/*
 *  Semaphore Functions
 */

const ID _kernel_tmax_semid = (TMIN_SEMID + TNUM_SEMID - 1);

const SEMINIB _kernel_seminib_table[TNUM_SEMID] = {
	{ (TA_NULL), (1), (1) },
	{ (TA_TPRI), (0), (2) }
};

SEMCB _kernel_semcb_table[TNUM_SEMID];

/*
 *  Eventflag Functions
 */

const ID _kernel_tmax_flgid = (TMIN_FLGID + TNUM_FLGID - 1);

const FLGINIB _kernel_flginib_table[TNUM_FLGID] = {
	{ (TA_NULL), (0U) },
	{ (TA_TPRI), (0x0001U) }
};

FLGCB _kernel_flgcb_table[TNUM_FLGID];

/*
 *  Dataqueue Functions
 */

const ID _kernel_tmax_dtqid = (TMIN_DTQID + TNUM_DTQID - 1);

static DTQMB _kernel_dtqmb_DTQ1[10];
const DTQINIB _kernel_dtqinib_table[TNUM_DTQID] = {
	{ (TA_NULL), (10), _kernel_dtqmb_DTQ1 },
	{ (TA_TPRI), (0), NULL }
};

DTQCB _kernel_dtqcb_table[TNUM_DTQID];

/*
 *  Priority Dataqueue Functions
 */

const ID _kernel_tmax_pdqid = (TMIN_PDQID + TNUM_PDQID - 1);

static PDQMB _kernel_pdqmb_PDQ1[10];
const PDQINIB _kernel_pdqinib_table[TNUM_PDQID] = {
	{ (TA_NULL), (10), (16), _kernel_pdqmb_PDQ1 },
	{ (TA_TPRI), (0), (2), NULL }
};

PDQCB _kernel_pdqcb_table[TNUM_PDQID];

/*
 *  Mutex Functions
 */

const ID _kernel_tmax_mtxid = (TMIN_MTXID + TNUM_MTXID - 1);

const MTXINIB _kernel_mtxinib_table[TNUM_MTXID] = {
	{ (TA_NULL), INT_PRIORITY(0) },
	{ (TA_CEILING), INT_PRIORITY(MID_PRIORITY) }
};

MTXCB _kernel_mtxcb_table[TNUM_MTXID];

/*
 *  Fixed-sized Memorypool Functions
 */

const ID _kernel_tmax_mpfid = (TMIN_MPFID + TNUM_MPFID - 1);

static MPF_T _kernel_mpf_MPF1[10 * COUNT_MPF_T(100)];
static MPFMB _kernel_mpfmb_MPF1[10];
static MPFMB _kernel_mpfmb_MPF2[20];
const MPFINIB _kernel_mpfinib_table[TNUM_MPFID] = {
	{ (TA_NULL), (10), ROUND_MPF_T(100), _kernel_mpf_MPF1, _kernel_mpfmb_MPF1 },
	{ (TA_NULL), (20), ROUND_MPF_T(200), (void *)(mpf_MPF2), _kernel_mpfmb_MPF2 }
};

MPFCB _kernel_mpfcb_table[TNUM_MPFID];

/*
 *  Cyclic Notification Functions
 */

const ID _kernel_tmax_cycid = (TMIN_CYCID + TNUM_CYCID - 1);

static void
_kernel_cychdr_CYC2(EXINF exinf)
{
	*((intptr_t *) exinf) = (1);
}

static void
_kernel_cychdr_CYC3(EXINF exinf)
{
	(void) loc_cpu();
	*((intptr_t *) exinf) += 1;
	(void) unl_cpu();
}

const ID _kernel_cychdr_CYC4_etskid = TASK2;

static void
_kernel_cychdr_CYC4(EXINF exinf)
{
	ER	ercd;

	ercd = act_tsk((ID) exinf);
	if (ercd != E_OK) {
		(void) act_tsk(_kernel_cychdr_CYC4_etskid);
	}
}

const ID _kernel_cychdr_CYC5_etskid = TASK2;

static void
_kernel_cychdr_CYC5(EXINF exinf)
{
	ER	ercd;

	ercd = wup_tsk((ID) exinf);
	if (ercd != E_OK) {
		(void) wup_tsk(_kernel_cychdr_CYC5_etskid);
	}
}

const ID _kernel_cychdr_CYC6_esemid = SEM2;

static void
_kernel_cychdr_CYC6(EXINF exinf)
{
	ER	ercd;

	ercd = sig_sem((ID) exinf);
	if (ercd != E_OK) {
		(void) sig_sem(_kernel_cychdr_CYC6_esemid);
	}
}

const ID _kernel_cychdr_CYC7_eflgid = FLG1;

static void
_kernel_cychdr_CYC7(EXINF exinf)
{
	ER	ercd;

	ercd = set_flg(((ID) exinf), 0x01);
	if (ercd != E_OK) {
		(void) set_flg(_kernel_cychdr_CYC7_eflgid, 0x02);
	}
}

const ID _kernel_cychdr_CYC8_edtqid = DTQ2;

static void
_kernel_cychdr_CYC8(EXINF exinf)
{
	ER	ercd;

	ercd = psnd_dtq(((ID) exinf), 0x00);
	if (ercd != E_OK) {
		(void) psnd_dtq(_kernel_cychdr_CYC8_edtqid, (intptr_t) ercd);
	}
}

intptr_t *const _kernel_cychdr_CYC9_p_evar = (intptr_t *)(&nvar1);

static void
_kernel_cychdr_CYC9(EXINF exinf)
{
	ER	ercd;

	ercd = act_tsk((ID) exinf);
	if (ercd != E_OK) {
		*_kernel_cychdr_CYC9_p_evar = (intptr_t) ercd;
	}
}

intptr_t *const _kernel_cychdr_CYC10_p_evar = (intptr_t *)(&nvar2);

static void
_kernel_cychdr_CYC10(EXINF exinf)
{
	ER	ercd;

	ercd = act_tsk((ID) exinf);
	if (ercd != E_OK) {
		(void) loc_cpu();
		*_kernel_cychdr_CYC10_p_evar += 1;
		(void) unl_cpu();
	}
}

const CYCINIB _kernel_cycinib_table[TNUM_CYCID] = {
	{ (TA_NULL), (intptr_t)(1), (NFYHDR)(cyclic1_handler), (1000), (1000) },
	{ (TA_STA), (intptr_t)(&nvar1), _kernel_cychdr_CYC2, (1000), (1000) },
	{ (TA_NULL), (intptr_t)(&nvar2), _kernel_cychdr_CYC3, (1000), (1000) },
	{ (TA_NULL), (intptr_t)(TASK1), _kernel_cychdr_CYC4, (1000), (1000) },
	{ (TA_NULL), (intptr_t)(TASK1), _kernel_cychdr_CYC5, (1000), (1000) },
	{ (TA_NULL), (intptr_t)(SEM1), _kernel_cychdr_CYC6, (1000), (1000) },
	{ (TA_NULL), (intptr_t)(FLG1), _kernel_cychdr_CYC7, (1000), (1000) },
	{ (TA_NULL), (intptr_t)(DTQ1), _kernel_cychdr_CYC8, (1000), (1000) },
	{ (TA_NULL), (intptr_t)(TASK1), _kernel_cychdr_CYC9, (1000), (1000) },
	{ (TA_NULL), (intptr_t)(TASK1), _kernel_cychdr_CYC10, (1000), (1000) }
};

CYCCB _kernel_cyccb_table[TNUM_CYCID];

/*
 *  Alarm Notification Functions
 */

const ID _kernel_tmax_almid = (TMIN_ALMID + TNUM_ALMID - 1);

static void
_kernel_almhdr_ALM2(EXINF exinf)
{
	*((intptr_t *) exinf) = (1);
}

static void
_kernel_almhdr_ALM3(EXINF exinf)
{
	(void) loc_cpu();
	*((intptr_t *) exinf) += 1;
	(void) unl_cpu();
}

const ID _kernel_almhdr_ALM4_etskid = TASK2;

static void
_kernel_almhdr_ALM4(EXINF exinf)
{
	ER	ercd;

	ercd = act_tsk((ID) exinf);
	if (ercd != E_OK) {
		(void) act_tsk(_kernel_almhdr_ALM4_etskid);
	}
}

const ID _kernel_almhdr_ALM5_etskid = TASK2;

static void
_kernel_almhdr_ALM5(EXINF exinf)
{
	ER	ercd;

	ercd = wup_tsk((ID) exinf);
	if (ercd != E_OK) {
		(void) wup_tsk(_kernel_almhdr_ALM5_etskid);
	}
}

const ID _kernel_almhdr_ALM6_esemid = SEM2;

static void
_kernel_almhdr_ALM6(EXINF exinf)
{
	ER	ercd;

	ercd = sig_sem((ID) exinf);
	if (ercd != E_OK) {
		(void) sig_sem(_kernel_almhdr_ALM6_esemid);
	}
}

const ID _kernel_almhdr_ALM7_eflgid = FLG1;

static void
_kernel_almhdr_ALM7(EXINF exinf)
{
	ER	ercd;

	ercd = set_flg(((ID) exinf), 0x01);
	if (ercd != E_OK) {
		(void) set_flg(_kernel_almhdr_ALM7_eflgid, 0x02);
	}
}

const ID _kernel_almhdr_ALM8_edtqid = DTQ2;

static void
_kernel_almhdr_ALM8(EXINF exinf)
{
	ER	ercd;

	ercd = psnd_dtq(((ID) exinf), 0x00);
	if (ercd != E_OK) {
		(void) psnd_dtq(_kernel_almhdr_ALM8_edtqid, (intptr_t) ercd);
	}
}

intptr_t *const _kernel_almhdr_ALM9_p_evar = (intptr_t *)(&nvar1);

static void
_kernel_almhdr_ALM9(EXINF exinf)
{
	ER	ercd;

	ercd = act_tsk((ID) exinf);
	if (ercd != E_OK) {
		*_kernel_almhdr_ALM9_p_evar = (intptr_t) ercd;
	}
}

intptr_t *const _kernel_almhdr_ALM10_p_evar = (intptr_t *)(&nvar2);

static void
_kernel_almhdr_ALM10(EXINF exinf)
{
	ER	ercd;

	ercd = act_tsk((ID) exinf);
	if (ercd != E_OK) {
		(void) loc_cpu();
		*_kernel_almhdr_ALM10_p_evar += 1;
		(void) unl_cpu();
	}
}

const ALMINIB _kernel_alminib_table[TNUM_ALMID] = {
	{ (TA_NULL), (intptr_t)(1), (NFYHDR)(alarm1_handler) },
	{ (TA_NULL), (intptr_t)(&nvar1), _kernel_almhdr_ALM2 },
	{ (TA_NULL), (intptr_t)(&nvar2), _kernel_almhdr_ALM3 },
	{ (TA_NULL), (intptr_t)(TASK1), _kernel_almhdr_ALM4 },
	{ (TA_NULL), (intptr_t)(TASK1), _kernel_almhdr_ALM5 },
	{ (TA_NULL), (intptr_t)(SEM1), _kernel_almhdr_ALM6 },
	{ (TA_NULL), (intptr_t)(FLG1), _kernel_almhdr_ALM7 },
	{ (TA_NULL), (intptr_t)(DTQ1), _kernel_almhdr_ALM8 },
	{ (TA_NULL), (intptr_t)(TASK1), _kernel_almhdr_ALM9 },
	{ (TA_NULL), (intptr_t)(TASK1), _kernel_almhdr_ALM10 }
};

ALMCB _kernel_almcb_table[TNUM_ALMID];

/*
 *  Interrupt Management Functions
 */

#ifndef LOG_ISR_ENTER
#define LOG_ISR_ENTER(isrid)
#endif /* LOG_ISR_ENTER */

#ifndef LOG_ISR_LEAVE
#define LOG_ISR_LEAVE(isrid)
#endif /* LOG_ISR_LEAVE */

void
_kernel_inthdr_5(void)
{
	LOG_ISR_ENTER(ISR3);
	((ISR)(isr3))((EXINF)(3));
	LOG_ISR_LEAVE(ISR3);

	if (_kernel_sense_lock()) {
		_kernel_unlock_cpu();
	}

	LOG_ISR_ENTER(ISR1);
	((ISR)(isr1))((EXINF)(1));
	LOG_ISR_LEAVE(ISR1);

	if (_kernel_sense_lock()) {
		_kernel_unlock_cpu();
	}

	LOG_ISR_ENTER(ISR2);
	((ISR)(isr2))((EXINF)(2));
	LOG_ISR_LEAVE(ISR2);
}

#define TNUM_DEF_INHNO	3
const uint_t _kernel_tnum_def_inhno = TNUM_DEF_INHNO;

INTHDR_ENTRY(INHNO_HRT, 0, _kernel_target_hrt_handler)
INTHDR_ENTRY(INHNO1, 4, int1_handler)
INTHDR_ENTRY(5, 5, _kernel_inthdr_5)

const INHINIB _kernel_inhinib_table[TNUM_DEF_INHNO] = {
	{ (INHNO_HRT), (TA_NULL), (FP)(INT_ENTRY(INHNO_HRT, _kernel_target_hrt_handler)) },
	{ (INHNO1), (TA_NULL), (FP)(INT_ENTRY(INHNO1, int1_handler)) },
	{ (5), (TA_NULL), (FP)(INT_ENTRY(5, _kernel_inthdr_5)) }
};

#define TNUM_CFG_INTNO	4
const uint_t _kernel_tnum_cfg_intno = TNUM_CFG_INTNO;

const INTINIB _kernel_intinib_table[TNUM_CFG_INTNO] = {
	{ (INTNO_HRT), (TA_ENAINT | INTATR_HRT), (INTPRI_HRT) },
	{ (INTNO1), (TA_NULL), (TMAX_INTPRI) },
	{ (INTNO2), (TA_ENAINT), (TMIN_INTPRI) },
	{ (INTNO3), (TA_NULL), (TMAX_INTPRI) }
};

/*
 *  CPU Exception Management Functions
 */

#define TNUM_DEF_EXCNO	2
const uint_t _kernel_tnum_def_excno = TNUM_DEF_EXCNO;

EXCHDR_ENTRY(CPUEXC1, 1, cpuexc1_handler)
EXCHDR_ENTRY(CPUEXC2, 2, cpuexc2_handler)

const EXCINIB _kernel_excinib_table[TNUM_DEF_EXCNO] = {
	{ (CPUEXC1), (TA_NULL), (FP)(EXC_ENTRY(CPUEXC1, cpuexc1_handler)) },
	{ (CPUEXC2), (TA_NULL), (FP)(EXC_ENTRY(CPUEXC2, cpuexc2_handler)) }
};

/*
 *  Stack Area for Non-task Context
 */

const size_t _kernel_istksz = (10 * STACK_SIZE);
STK_T *const _kernel_istk = (void *)(istack);

#ifdef TOPPERS_ISTKPT
STK_T *const _kernel_istkpt = TOPPERS_ISTKPT((void *)(istack), (10 * STACK_SIZE));
#endif /* TOPPERS_ISTKPT */

/*
 *  Time Event Management
 */

TMEVTN   _kernel_tmevt_heap[1 + TNUM_TSKID + TNUM_CYCID + TNUM_ALMID];

/*
 *  Module Initialization Function
 */

void
_kernel_initialize_object(void)
{
	_kernel_initialize_task();
	_kernel_initialize_semaphore();
	_kernel_initialize_eventflag();
	_kernel_initialize_dataqueue();
	_kernel_initialize_pridataq();
	_kernel_initialize_mutex();
	_kernel_initialize_mempfix();
	_kernel_initialize_cyclic();
	_kernel_initialize_alarm();
	_kernel_initialize_interrupt();
	_kernel_initialize_exception();
}

/*
 *  Initialization Routine
 */

const uint_t _kernel_tnum_inirtn = TNUM_INIRTN;

const INIRTNB _kernel_inirtnb_table[TNUM_INIRTN] = {
	{ (INIRTN)(_kernel_target_hrt_initialize), (EXINF)(0) },
	{ (INIRTN)(initialize1_routine), (EXINF)(1) },
	{ (INIRTN)(initialize2_routine), (EXINF)(2) }
};

/*
 *  Termination Routine
 */

const uint_t _kernel_tnum_terrtn = TNUM_TERRTN;

const TERRTNB _kernel_terrtnb_table[TNUM_TERRTN] = {
	{ (TERRTN)(terminate2_routine), (EXINF)(2) },
	{ (TERRTN)(terminate1_routine), (EXINF)(1) },
	{ (TERRTN)(_kernel_target_hrt_terminate), (EXINF)(0) }
};

