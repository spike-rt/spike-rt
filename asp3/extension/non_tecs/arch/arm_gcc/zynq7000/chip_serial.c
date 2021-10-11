/*
 *  TOPPERS Software
 *      Toyohashi Open Platform for Embedded Real-Time Systems
 * 
 *  Copyright (C) 2006-2018 by Embedded and Real-Time Systems Laboratory
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
 *  $Id: chip_serial.c 1095 2018-11-28 00:57:28Z ertl-hiro $
 */

/*
 *		シリアルインタフェースドライバのチップ依存部（Zynq7000用）
 *		（非TECS版専用）
 */

#include <kernel.h>
#include <sil.h>
#include <t_syslog.h>
#include "target_syssvc.h"
#include "target_serial.h"

/*
 *  SIOドライバの初期化
 */
void
sio_initialize(intptr_t exinf)
{
	xuartps_initialize();
}

/*
 *  SIOドライバの終了処理
 */
void
sio_terminate(intptr_t exinf)
{
	xuartps_terminate();
}

/*
 *  SIOの割込みサービスルーチン
 */
void
sio_isr(intptr_t exinf)
{
	xuartps_isr((ID) exinf);
}

/*
 *  SIOポートのオープン
 */
SIOPCB *
sio_opn_por(ID siopid, intptr_t exinf)
{
	SIOPCB		*p_siopcb;

	/*
	 *  デバイス依存のオープン処理
	 */
	p_siopcb = xuartps_opn_por(siopid, exinf);

	/*
	 *  SIOの割込みマスクを解除する．
	 */
	(void) ena_int(INTNO_SIO);
	return(p_siopcb);   
}

/*
 *  SIOポートのクローズ
 */
void
sio_cls_por(SIOPCB *p_siopcb)
{
	/*
	 *  デバイス依存のクローズ処理
	 */
	xuartps_cls_por(p_siopcb);

	/*
	 *  SIOの割込みをマスクする．
	 */
	(void) dis_int(INTNO_SIO);
}

/*
 *  SIOポートへの文字送信
 */
bool_t
sio_snd_chr(SIOPCB *p_siopcb, char c)
{
	return(xuartps_snd_chr(p_siopcb, c));
}

/*
 *  SIOポートからの文字受信
 */
int_t
sio_rcv_chr(SIOPCB *p_siopcb)
{
	return(xuartps_rcv_chr(p_siopcb));
}

/*
 *  SIOポートからのコールバックの許可
 */
void
sio_ena_cbr(SIOPCB *p_siopcb, uint_t cbrtn)
{
	xuartps_ena_cbr(p_siopcb, cbrtn);
}

/*
 *  SIOポートからのコールバックの禁止
 */
void
sio_dis_cbr(SIOPCB *p_siopcb, uint_t cbrtn)
{
	xuartps_dis_cbr(p_siopcb, cbrtn);
}

/*
 *  SIOポートからの送信可能コールバック
 */
void
xuartps_irdy_snd(intptr_t exinf)
{
	sio_irdy_snd(exinf);
}

/*
 *  SIOポートからの受信通知コールバック
 */
void
xuartps_irdy_rcv(intptr_t exinf)
{
	sio_irdy_rcv(exinf);
}
