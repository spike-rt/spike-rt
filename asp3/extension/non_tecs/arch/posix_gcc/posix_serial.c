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
 *  $Id: posix_serial.c 1709 2022-10-18 14:11:45Z ertl-hiro $
 */

/*
 *		シリアルインタフェースドライバのターゲット依存部（POSIX用）
 */

#include <t_stddef.h>
#include <t_syslog.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include "target_serial.h"

/*
 *  SIOポート初期化ブロックの定義
 */
typedef struct sio_port_initialization_block {
	char		*path;			/* ファイルのパス名 */
} SIOPINIB;

/*
 *  SIOポート管理ブロックの定義
 */
struct sio_port_control_block {
	const SIOPINIB *p_siopinib;	/* SIOポート初期化ブロック */
	intptr_t	exinf;			/* 拡張情報 */
	bool_t		opened;			/* オープン済みフラグ */
	struct termios saved_term;	/* 元の端末制御情報 */

	int_t		read_fd;		/* 読出し用ファイルディスクリプタ */
	bool_t		rcv_flag;		/* 受信文字バッファ有効フラグ */
	char		rcv_buf;		/* 受信文字バッファ */
	bool_t		rcv_rdy;		/* 受信通知コールバック許可フラグ */

	int_t		write_fd;		/* 書込み用ファイルディスクリプタ */
	bool_t		snd_flag;		/* 送信文字バッファ有効フラグ */
	char		snd_buf;		/* 送信文字バッファ */
	bool_t		snd_rdy;		/* 送信通知コールバック許可フラグ */
};

/*
 *  SIOポート初期化ブロック
 */
const SIOPINIB siopinib_table[TNUM_SIOP] = {
	{ NULL }
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
 *  SIOの割込みサービスルーチン
 */
void
sio_isr(EXINF exinf)
{
	SIOPCB	*p_siopcb = &(siopcb_table[0]);
	int_t	n;

	if (p_siopcb->snd_flag) {
		if ((n = write(p_siopcb->write_fd, &(p_siopcb->snd_buf), 1)) > 0) {
			p_siopcb->snd_flag = false;
			if (p_siopcb->snd_rdy) {
				sio_irdy_snd(p_siopcb->exinf);
			}
		}
	}
	if (!p_siopcb->rcv_flag) {
		if ((n = read(p_siopcb->read_fd, &(p_siopcb->rcv_buf), 1)) > 0) {
			p_siopcb->rcv_flag = true;
			if (p_siopcb->rcv_rdy) {
				sio_irdy_rcv(p_siopcb->exinf);
			}
		}
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
	int_t			fd;
	struct termios	term;

	p_siopcb = get_siopcb(siopid);
	p_siopinib = p_siopcb->p_siopinib;
	
	if (!(p_siopcb->opened)) {
		if (p_siopinib->path != NULL) {
			fd = open(p_siopinib->path, O_RDWR, 0777);
			assert(fd >= 0);
			p_siopcb->read_fd = fd;
			p_siopcb->write_fd = fd;
		}
		else {
			fd = STDIN_FILENO;				/* 標準入出力を使う */
			p_siopcb->read_fd = STDIN_FILENO;
			p_siopcb->write_fd = STDOUT_FILENO;
		}
		fcntl(fd, F_SETOWN, getpid());
		fcntl(fd, F_SETFL, (O_NONBLOCK | O_ASYNC));

		tcgetattr(fd, &(p_siopcb->saved_term));
		term = p_siopcb->saved_term;
		term.c_lflag &= ~(ECHO | ICANON);
		tcsetattr(fd, TCSAFLUSH, &term);

		p_siopcb->exinf = exinf;
		p_siopcb->rcv_flag = false;
		p_siopcb->rcv_rdy = false;
		p_siopcb->snd_flag = false;
		p_siopcb->snd_rdy = false;
		p_siopcb->opened = true;
	}
	return(p_siopcb);
}

/*
 *  SIOポートのクローズ
 */
void
sio_cls_por(SIOPCB *p_siopcb)
{
	int_t	fd;

	if (p_siopcb->opened) {
		fd = p_siopcb->read_fd;
		tcsetattr(fd, TCSAFLUSH, &(p_siopcb->saved_term));
		fcntl(fd, F_SETFL, 0);

		if (p_siopcb->p_siopinib->path != NULL) {
			close(p_siopcb->read_fd);
		}
		p_siopcb->opened = false;
	}
}

/*
 *  SIOポートへの文字送信
 */
bool_t
sio_snd_chr(SIOPCB *p_siopcb, char c)
{
	ssize_t	len;

	if (!p_siopcb->snd_flag) {
		len = write(p_siopcb->write_fd, &c, 1);
		assert(len >= 0 || errno == EAGAIN || errno == EINTR);
		if (len > 0) {
			return(true);
		}
		else {
			p_siopcb->snd_flag = true;
			p_siopcb->snd_buf = c;
			return(true);
		}
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
	ssize_t	len;

	if (p_siopcb->rcv_flag) {
		p_siopcb->rcv_flag = false;
		return((int_t)(uint8_t)(p_siopcb->rcv_buf));
	}
	else {
		len = read(p_siopcb->read_fd, &c, 1);
		assert(len >= 0 || errno == EAGAIN || errno == EINTR);
		if (len > 0) {
			return((int_t)(uint8_t) c);
		}
		else {
			return(-1);
		}
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
		p_siopcb->snd_rdy = true;
		break;
	case SIO_RDY_RCV:
		p_siopcb->rcv_rdy = true;
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
		p_siopcb->snd_rdy = false;
		break;
	case SIO_RDY_RCV:
		p_siopcb->rcv_rdy = false;
		break;
	}
}

/*
 *		システムログの低レベル出力（本来は別のファイルにすべき）
 */

/*
 *  標準エラーへの文字出力
 */
void
target_fput_log(char c)
{
	ssize_t	len;

	do {
		len = write(STDERR_FILENO, &c, 1);
		assert(len >= 0 || errno == EINTR);
	} while (len != 1);
}
