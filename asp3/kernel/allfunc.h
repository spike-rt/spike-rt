/*
 *  TOPPERS/ASP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Advanced Standard Profile Kernel
 * 
 *  Copyright (C) 2005-2022 by Embedded and Real-Time Systems Laboratory
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
 *  $Id: allfunc.h 1631 2022-09-23 09:07:24Z ertl-hiro $
 */

/*
 *		すべての関数をコンパイルするための定義
 */

#ifndef TOPPERS_ALLFUNC_H
#define TOPPERS_ALLFUNC_H

/* startup.c */
#define TOPPERS_sta_ker
#define TOPPERS_ext_ker

/* task.c */
#define TOPPERS_tskini
#define TOPPERS_tsksched
#define TOPPERS_tskrun
#define TOPPERS_tsknrun
#define TOPPERS_tskdmt
#define TOPPERS_tskact
#define TOPPERS_tskpri
#define TOPPERS_tskrot
#define TOPPERS_tskterm

/* taskhook.c */
#define TOPPERS_mtxhook

/* wait.c */
#define TOPPERS_waimake
#define TOPPERS_waicmp
#define TOPPERS_waitmo
#define TOPPERS_waitmook
#define TOPPERS_wobjwai
#define TOPPERS_wobjwaitmo
#define TOPPERS_iniwque

/* time_event.c */
#define TOPPERS_tmeini
#define TOPPERS_tmeup
#define TOPPERS_tmedown
#define TOPPERS_tmecur
#define TOPPERS_tmeset
#define TOPPERS_tmereg
#define TOPPERS_tmeenqrel
#define TOPPERS_tmedeq
#define TOPPERS_tmechk
#define TOPPERS_tmeltim
#define TOPPERS_sigtim

/* task_manage.c */
#define TOPPERS_act_tsk
#define TOPPERS_can_act
#define TOPPERS_get_tst
#define TOPPERS_chg_pri
#define TOPPERS_get_pri
#define TOPPERS_get_inf

/* task_refer.c */
#define TOPPERS_ref_tsk

/* task_sync.c */
#define TOPPERS_slp_tsk
#define TOPPERS_tslp_tsk
#define TOPPERS_wup_tsk
#define TOPPERS_can_wup
#define TOPPERS_rel_wai
#define TOPPERS_sus_tsk
#define TOPPERS_rsm_tsk
#define TOPPERS_dly_tsk

/* task_term.c */
#define TOPPERS_ext_tsk
#define TOPPERS_ras_ter
#define TOPPERS_dis_ter
#define TOPPERS_ena_ter
#define TOPPERS_sns_ter
#define TOPPERS_ter_tsk

/* semaphore.c */
#define TOPPERS_semini
#define TOPPERS_sig_sem
#define TOPPERS_wai_sem
#define TOPPERS_pol_sem
#define TOPPERS_twai_sem
#define TOPPERS_ini_sem
#define TOPPERS_ref_sem

/* eventflag.c */
#define TOPPERS_flgini
#define TOPPERS_flgcnd
#define TOPPERS_set_flg
#define TOPPERS_clr_flg
#define TOPPERS_wai_flg
#define TOPPERS_pol_flg
#define TOPPERS_twai_flg
#define TOPPERS_ini_flg
#define TOPPERS_ref_flg

/* dataqueue.c */
#define TOPPERS_dtqini
#define TOPPERS_dtqenq
#define TOPPERS_dtqfenq
#define TOPPERS_dtqdeq
#define TOPPERS_dtqsnd
#define TOPPERS_dtqfsnd
#define TOPPERS_dtqrcv
#define TOPPERS_snd_dtq
#define TOPPERS_psnd_dtq
#define TOPPERS_tsnd_dtq
#define TOPPERS_fsnd_dtq
#define TOPPERS_rcv_dtq
#define TOPPERS_prcv_dtq
#define TOPPERS_trcv_dtq
#define TOPPERS_ini_dtq
#define TOPPERS_ref_dtq

/* pridataq.c */
#define TOPPERS_pdqini
#define TOPPERS_pdqenq
#define TOPPERS_pdqdeq
#define TOPPERS_pdqsnd
#define TOPPERS_pdqrcv
#define TOPPERS_snd_pdq
#define TOPPERS_psnd_pdq
#define TOPPERS_tsnd_pdq
#define TOPPERS_rcv_pdq
#define TOPPERS_prcv_pdq
#define TOPPERS_trcv_pdq
#define TOPPERS_ini_pdq
#define TOPPERS_ref_pdq

/* mutex.c */
#define TOPPERS_mtxini
#define TOPPERS_mtxchk
#define TOPPERS_mtxacq
#define TOPPERS_mtxrel
#define TOPPERS_mtxrela
#define TOPPERS_loc_mtx
#define TOPPERS_ploc_mtx
#define TOPPERS_tloc_mtx
#define TOPPERS_unl_mtx
#define TOPPERS_ini_mtx
#define TOPPERS_ref_mtx

/* mempfix.c */
#define TOPPERS_mpfini
#define TOPPERS_mpfget
#define TOPPERS_get_mpf
#define TOPPERS_pget_mpf
#define TOPPERS_tget_mpf
#define TOPPERS_rel_mpf
#define TOPPERS_ini_mpf
#define TOPPERS_ref_mpf

/* time_manage.c */
#define TOPPERS_set_tim
#define TOPPERS_get_tim
#define TOPPERS_adj_tim
#define TOPPERS_fch_hrt

/* cyclic.c */
#define TOPPERS_cycini
#define TOPPERS_sta_cyc
#define TOPPERS_stp_cyc
#define TOPPERS_ref_cyc
#define TOPPERS_cyccal

/* alarm.c */
#define TOPPERS_almini
#define TOPPERS_sta_alm
#define TOPPERS_stp_alm
#define TOPPERS_ref_alm
#define TOPPERS_almcal

/* sys_manage.c */
#define TOPPERS_rot_rdq
#define TOPPERS_get_tid
#define TOPPERS_get_lod
#define TOPPERS_get_nth
#define TOPPERS_loc_cpu
#define TOPPERS_unl_cpu
#define TOPPERS_dis_dsp
#define TOPPERS_ena_dsp
#define TOPPERS_sns_ctx
#define TOPPERS_sns_loc
#define TOPPERS_sns_dsp
#define TOPPERS_sns_dpn
#define TOPPERS_sns_ker

/* interrupt.c */
#define TOPPERS_intini
#define TOPPERS_dis_int
#define TOPPERS_ena_int
#define TOPPERS_clr_int
#define TOPPERS_ras_int
#define TOPPERS_prb_int
#define TOPPERS_chg_ipm
#define TOPPERS_get_ipm

/* exception.c */
#define TOPPERS_excini
#define TOPPERS_xsns_dpn

#endif /* TOPPERS_ALLFUNC_H */
