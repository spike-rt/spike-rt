/*
 *  TOPPERS Software
 *      Toyohashi Open Platform for Embedded Real-Time Systems
 * 
 *  Copyright (C) 2005-2018 by Embedded and Real-Time Systems Laboratory
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
 *  $Id: test_svc.h 999 2018-07-27 02:28:18Z ertl-hiro $
 */

/* 
 *		テストプログラム用サービス
 */

#ifndef TOPPERS_TEST_SVC_H
#define TOPPERS_TEST_SVC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <kernel.h>
#include "target_test.h"

/*
 *  テストプログラム用サービスのサービスコール
 */
extern void	test_start(const char *progname);
extern void	check_point(uint_t count);
extern void	check_finish(uint_t count);
extern void	check_assert_error(const char *expr, const char *file, int_t line);
extern void	check_ercd_error(ER ercd, const char *file, int_t line);
extern ER	get_interrupt_priority_mask(PRI *p_ipm);

/*
 *	条件チェック
 */
#define check_assert(exp) \
	((void)(!(exp) ? (check_assert_error(#exp, __FILE__, __LINE__), 0) : 0))

/*
 *	エラーコードチェック
 */
#define check_ercd(ercd, expected_ercd) \
	((void)((ercd) != (expected_ercd) ? \
					(check_ercd_error(ercd, __FILE__, __LINE__), 0) : 0))

/*
 *	システム状態のチェック
 */
#define check_state(ctx, loc, dsp, dpn, ter) do {	\
	check_assert(sns_ctx() == ctx);					\
	check_assert(sns_loc() == loc);					\
	check_assert(sns_dsp() == dsp);					\
	check_assert(sns_dpn() == dpn);					\
	check_assert(sns_ter() == ter);					\
} while (false);

/*
 *	割込み優先度マスクのチェック
 */
#define check_ipm(ipm) do {							\
	PRI		intpri;									\
	ER		ercd;									\
													\
	ercd = get_interrupt_priority_mask(&intpri);	\
	check_ercd(ercd, E_OK);							\
	check_assert(intpri == ipm);					\
} while (false);

#ifdef __cplusplus
}
#endif

#endif /* TOPPERS_TEST_SVC_H */
