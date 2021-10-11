/*
 *  TOPPERS Software
 *      Toyohashi Open Platform for Embedded Real-Time Systems
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
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
 *  $Id: xuartps.h 1133 2018-12-29 02:08:25Z ertl-hiro $
 */

/*
 *		XUartPsに関する定義と簡易SIOドライバ
 */

#ifndef TOPPERS_XUARTPS_H
#define TOPPERS_XUARTPS_H

/*
 *		XUartPsに関する定義
 */

/*
 *  UARTレジスタの番地の定義
 */
#define XUARTPS_CR(base)		((uint32_t *)((base) + 0x00U))
#define XUARTPS_MR(base)		((uint32_t *)((base) + 0x04U))
#define XUARTPS_IER(base)		((uint32_t *)((base) + 0x08U))
#define XUARTPS_IDR(base)		((uint32_t *)((base) + 0x0cU))
#define XUARTPS_ISR(base)		((uint32_t *)((base) + 0x14U))
#define XUARTPS_BAUDGEN(base)	((uint32_t *)((base) + 0x18U))
#define XUARTPS_RXTOUT(base)	((uint32_t *)((base) + 0x1cU))
#define XUARTPS_RXWM(base)		((uint32_t *)((base) + 0x20U))
#define XUARTPS_SR(base)		((uint32_t *)((base) + 0x2cU))
#define XUARTPS_FIFO(base)		((uint32_t *)((base) + 0x30U))
#define XUARTPS_BAUDDIV(base)	((uint32_t *)((base) + 0x34U))

/*
 *  コントロールレジスタ（XUARTPS_CR）の設定値
 */
#define XUARTPS_CR_STOPBRK		UINT_C(0x0100)	/* 送信ブレーク停止 */
#define XUARTPS_CR_TX_DIS		UINT_C(0x0020)	/* 送信ディスエーブル */
#define XUARTPS_CR_TX_EN		UINT_C(0x0010)	/* 送信イネーブル */
#define XUARTPS_CR_RX_DIS		UINT_C(0x0008)	/* 受信ディスエーブル */
#define XUARTPS_CR_RX_EN		UINT_C(0x0004)	/* 受信イネーブル */
#define XUARTPS_CR_TXRST		UINT_C(0x0002)	/* 送信リセット */
#define XUARTPS_CR_RXRST		UINT_C(0x0001)	/* 受信リセット */

/*
 *  モードレジスタ（XUARTPS_MR）の設定値
 */
#define XUARTPS_MR_STOPBIT_1	UINT_C(0x0000)	/* ストップビット：1 */
#define XUARTPS_MR_PARITY_NONE	UINT_C(0x0020)	/* パリティなし */
#define XUARTPS_MR_CHARLEN_8	UINT_C(0x0000)	/* データ長：8ビット */
#define XUARTPS_MR_CLKSEL		UINT_C(0x0001)	/* 入力クロック選択 */
#define XUARTPS_MR_CCLK			UINT_C(0x0400)	/* 入力クロック選択 */

/*
 *  割込みイネーブルレジスタ（XUARTPS_IER）と割込みディスエーブルレジ
 *  スタ（XUARTPS_IDR）の設定値，割込み状態レジスタ（XUARTPS_ISR）の参
 *  照／設定値
 */
#define XUARTPS_IXR_TXEMPTY		UINT_C(0x0008)	/* 送信FIFOエンプティ割込み */
#define XUARTPS_IXR_RXTRIG		UINT_C(0x0001)	/* 受信FIFOトリガ割込み */
#define XUARTPS_IXR_ALL			UINT_C(0x1fff)	/* 全割込み */

/*
 *  チャネル状態レジスタ（XUARTPS_SR）の参照値
 */
#define XUARTPS_SR_TXFULL		UINT_C(0x0010)	/* 送信FIFOフル */
#define XUARTPS_SR_TXEMPTY		UINT_C(0x0008)	/* 送信FIFOエンプティ */
#define XUARTPS_SR_RXEMPTY		UINT_C(0x0002)	/* 受信FIFOエンプティ */

#ifdef TOPPERS_OMIT_TECS
/*
 *		XUartPs用 簡易SIOドライバ
 */
#include <sil.h>

/*
 *  SIOポート数の定義
 */
#define TNUM_SIOP		1		/* サポートするSIOポートの数 */

/*
 *  コールバックルーチンの識別番号
 */
#define SIO_RDY_SND		1U		/* 送信可能コールバック */
#define SIO_RDY_RCV		2U		/* 受信通知コールバック */

#ifndef TOPPERS_MACRO_ONLY

/*
 *  SIOポート管理ブロックの定義
 */
typedef struct sio_port_control_block	SIOPCB;

/*
 *  プリミティブな送信／受信関数
 */

/*
 *  受信バッファに文字があるか？
 */
Inline bool_t
xuartps_getready(uintptr_t base)
{
	return((sil_rew_mem(XUARTPS_SR(base)) & XUARTPS_SR_RXEMPTY) == 0U);
}

/*
 *  送信バッファに空きがあるか？
 */
Inline bool_t
xuartps_putready(uintptr_t base)
{
	return((sil_rew_mem(XUARTPS_SR(base)) & XUARTPS_SR_TXFULL) == 0U);
}

/*
 *  受信した文字の取出し
 */
Inline char
xuartps_getchar(uintptr_t base)
{
	return((char) sil_rew_mem(XUARTPS_FIFO(base)));
}

/*
 *  送信する文字の書込み
 */
Inline void
xuartps_putchar(uintptr_t base, char c)
{
	sil_wrw_mem(XUARTPS_FIFO(base), (uint32_t) c);
}

/*
 *  送信割込みイネーブル
 */
Inline void
xuartps_enable_send(uintptr_t base)
{
	sil_wrw_mem(XUARTPS_IER(base), XUARTPS_IXR_TXEMPTY);
}

/*
 *  送信割込みディスエーブル
 */
Inline void
xuartps_disable_send(uintptr_t base)
{
	sil_wrw_mem(XUARTPS_IDR(base), XUARTPS_IXR_TXEMPTY);
}

/*
 *  受信割込みイネーブル
 */
Inline void
xuartps_enable_receive(uintptr_t base)
{
	sil_wrw_mem(XUARTPS_IER(base), XUARTPS_IXR_RXTRIG);
}

/*
 *  受信割込みディスエーブル
 */
Inline void
xuartps_disable_receive(uintptr_t base)
{
	sil_wrw_mem(XUARTPS_IDR(base), XUARTPS_IXR_RXTRIG);
}

/*
 *  シリアルインタフェースドライバに提供する機能
 */

/*
 *  SIOドライバの初期化
 */
extern void		xuartps_initialize(void);

/*
 *  SIOドライバの終了処理
 */
extern void		xuartps_terminate(void);

/*
 *  SIOの割込みサービスルーチン
 */
extern void		xuartps_isr(ID siopid);

/*
 *  SIOポートのオープン
 */
extern SIOPCB	*xuartps_opn_por(ID siopid, intptr_t exinf);

/*
 *  SIOポートのクローズ
 */
extern void		xuartps_cls_por(SIOPCB *siopcb);

/*
 *  SIOポートへの文字送信
 */
extern bool_t	xuartps_snd_chr(SIOPCB *siopcb, char c);

/*
 *  SIOポートからの文字受信
 */
extern int_t	xuartps_rcv_chr(SIOPCB *siopcb);

/*
 *  SIOポートからのコールバックの許可
 */
extern void		xuartps_ena_cbr(SIOPCB *siopcb, uint_t cbrtn);

/*
 *  SIOポートからのコールバックの禁止
 */
extern void		xuartps_dis_cbr(SIOPCB *siopcb, uint_t cbrtn);

/*
 *  SIOポートからの送信可能コールバック
 */
extern void		xuartps_irdy_snd(intptr_t exinf);

/*
 *  SIOポートからの受信通知コールバック
 */
extern void		xuartps_irdy_rcv(intptr_t exinf);

#endif /* TOPPERS_MACRO_ONLY */
#endif /* TOPPERS_OMIT_TECS */
#endif /* TOPPERS_XUARTPS_H */
