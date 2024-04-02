/*
 *  TOPPERS Software
 *      Toyohashi Open Platform for Embedded Real-Time Systems
 * 
 *  Copyright (C) 2013-2020 by Embedded and Real-Time Systems Laboratory
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
 *  $Id: target_serial.c 1437 2020-05-20 12:12:16Z ertl-hiro $
 */

/*
 *		シリアルインタフェースドライバのターゲット依存部（ダミーターゲット用）
 *		（非TECS版専用）
 */

#include "target_serial.h"

/*
 *  SIOポート初期化ブロックの定義
 */
typedef struct sio_port_initialization_block {
	int_t		dummy;			/* ダミーフィールド */
	/* SIOのレジスタの番地など */
} SIOPINIB;

/*
 *  SIOポート管理ブロックの定義
 */
struct sio_port_control_block {
	const SIOPINIB *p_siopinib;	/* SIOポート初期化ブロック */
	EXINF		exinf;			/* 拡張情報 */
	bool_t		opened;			/* オープン済みフラグ */
	int_t		dummy;			/* ダミーフィールド */
	/* SIOの状態など */
};

/*
 *  SIOポート初期化ブロック
 */
const SIOPINIB siopinib_table[TNUM_SIOP] = {
	{ 0 }
};

/*
 *  SIOポート管理ブロックのエリア
 */
SIOPCB	siopcb_table[TNUM_SIOP];

/*
 *  SIOポートIDから管理ブロックを取り出すためのマクロ
 */
#define INDEX_SIOP(siopid)	((uint_t)((siopid) - 1))
#define get_siopcb(siopid)	(&(siopcb_table[INDEX_SIOP(siopid)]))

/*
 *  SIOドライバの初期化
 */
void
sio_initialize(EXINF exinf)
{
	SIOPCB	*p_siopcb;
	uint_t	i;

	/*
	 *  SIOポート管理ブロックの初期化
	 */
	for (i = 0; i < TNUM_SIOP; i++) {
		p_siopcb = &(siopcb_table[i]);
		p_siopcb->p_siopinib = &(siopinib_table[i]);
		p_siopcb->opened = false;
		/* その他のフィールドの初期化 */
	}
}

/*
 *  SIOドライバの終了処理
 */
void
sio_terminate(EXINF exinf)
{
	uint_t	i;

	/*
	 *  オープンされているSIOポートのクローズ
	 */
	for (i = 0; i < TNUM_SIOP; i++) {
		sio_cls_por(&(siopcb_table[i]));
	}
}

/*
 *  SIOポートのオープン
 */
SIOPCB *
sio_opn_por(ID siopid, EXINF exinf)
{
	SIOPCB			*p_siopcb;
	const SIOPINIB	*p_siopinib;

	p_siopcb = get_siopcb(siopid);
	p_siopinib = p_siopcb->p_siopinib;

	if (!(p_siopcb->opened)) {
		/* SIOのオープン処理 */

		p_siopcb->exinf = exinf;
		p_siopcb->opened = true;
		p_siopcb->dummy = p_siopinib->dummy;
	}
	return(p_siopcb);
}

/*
 *  SIOポートのクローズ
 */
void
sio_cls_por(SIOPCB *p_siopcb)
{
	if (p_siopcb->opened) {
		/* SIOのクローズ処理 */

		p_siopcb->opened = false;
	}
}

/*
 *  SIOの割込みサービスルーチン
 */
void
sio_isr(EXINF exinf)
{
	SIOPCB	*p_siopcb = &(siopcb_table[(int_t) exinf]);
	
	if (1/* 送信完了割込みの場合 */) {
		sio_irdy_snd(p_siopcb->exinf);
	}
	if (1/* 受信完了割込みの場合 */) {
		sio_irdy_rcv(p_siopcb->exinf);
	}
}

/*
 *  SIOポートへの文字送信
 */
bool_t
sio_snd_chr(SIOPCB *p_siopcb, char c)
{
	if (1/* 送信レジスタに空きがある場合 */) {
		/* 送信レジスタに文字cを入れる */
		return(true);
	}
	else {
		return(false);
	}
}

/*
 *  SIOポートからの文字受信
 */
int_t
sio_rcv_chr(SIOPCB *p_siopcb)
{
	char	c;

	if (1/* 受信レジスタに文字がある場合 */) {
		/* 受信レジスタの文字をcに入れる */
		c = '\0';
		return((int_t) c);
	}
	else {
		return(-1);
	}
}

/*
 *  SIOポートからのコールバックの許可
 */
void
sio_ena_cbr(SIOPCB *p_siopcb, uint_t cbrtn)
{
	switch (cbrtn) {
	case SIO_RDY_SND:
		/* 送信完了割込みを許可 */
		break;
	case SIO_RDY_RCV:
		/* 受信完了割込みを許可 */
		break;
	}
}

/*
 *  SIOポートからのコールバックの禁止
 */
void
sio_dis_cbr(SIOPCB *p_siopcb, uint_t cbrtn)
{
	switch (cbrtn) {
	case SIO_RDY_SND:
		/* 送信完了割込みを禁止 */
		break;
	case SIO_RDY_RCV:
		/* 受信完了割込みを禁止 */
		break;
	}
}

/*
 *		システムログの低レベル出力（本来は別のファイルにすべき）
 */

/*
 *  低レベル出力への文字出力
 */
void
target_fput_log(char c)
{
	/* 文字cを表示/出力/保存する */
}
