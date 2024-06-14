/*
 *  TOPPERS/ASP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Advanced Standard Profile Kernel
 * 
 *  Copyright (C) 2006-2022 by Embedded and Real-Time Systems Laboratory
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
 *  $Id: posix_serial.h 1709 2022-10-18 14:11:45Z ertl-hiro $
 */

/*
 *		シリアルインタフェースドライバのターゲット依存部（POSIX用）
 */

#ifndef TOPPERS_POSIX_SERIAL_H
#define TOPPERS_POSIX_SERIAL_H

#include <t_stddef.h>

/*
 *  SIOポート数の定義
 */
#define TNUM_SIOP		1		/* サポートするSIOポートの数 */

/*
 *  SIO割込みを登録するための定義
 */
#define ISRPRI_SIO		1		/* SIOの割込みサービスルーチンのISR優先度 */

#ifndef TOPPERS_MACRO_ONLY

/*
 *  SIOポート管理ブロックの定義
 */
typedef struct sio_port_control_block	SIOPCB;

/*
 *  コールバックルーチンの識別番号
 */
#define SIO_RDY_SND		1U		/* 送信可能コールバック */
#define SIO_RDY_RCV		2U		/* 受信通知コールバック */

/*
 *  SIOドライバの初期化
 */
extern void		sio_initialize(EXINF exinf);

/*
 *  SIOドライバの終了処理
 */
extern void		sio_terminate(EXINF exinf);

/*
 *  SIOの割込みサービスルーチン
 */
extern void		sio_isr(EXINF exinf);

/*
 *  SIOポートのオープン
 */
extern SIOPCB	*sio_opn_por(ID siopid, EXINF exinf);

/*
 *  SIOポートのクローズ
 */
extern void		sio_cls_por(SIOPCB *p_siopcb);

/*
 *  SIOポートへの文字送信
 */
extern bool_t	sio_snd_chr(SIOPCB *siopcb, char c);

/*
 *  SIOポートからの文字受信
 */
extern int_t	sio_rcv_chr(SIOPCB *siopcb);

/*
 *  SIOポートからのコールバックの許可
 */
extern void		sio_ena_cbr(SIOPCB *siopcb, uint_t cbrtn);

/*
 *  SIOポートからのコールバックの禁止
 */
extern void		sio_dis_cbr(SIOPCB *siopcb, uint_t cbrtn);

/*
 *  SIOポートからの送信可能コールバック
 */
extern void		sio_irdy_snd(EXINF exinf);

/*
 *  SIOポートからの受信通知コールバック
 */
extern void		sio_irdy_rcv(EXINF exinf);

#endif /* TOPPERS_MACRO_ONLY */
#endif /* TOPPERS_POSIX_SERIAL_H */
