/*
 *  TOPPERS Software
 *      Toyohashi Open Platform for Embedded Real-Time Systems
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
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
 */

/*
 *	Pybricksで実装されているBluetoothシリアルを利用するためのインタフェースドライバ
 */

#include <kernel.h>
#include "tPybricksBluetoothSerialPortMain_tecsgen.h"
#include <t_syslog.h>

#include <pbio/error.h>
#include <pbdrv/bluetooth.h>
#include <pbsys/bluetooth.h>

/*
 *  ポインタのインクリメント
 */
#define INC_PTR(ptr, bufsz) do {	\
	if (++(ptr) == (bufsz)) {		\
		(ptr) = 0;					\
	 }								\
} while (false)

/*
 *  サービスコール呼出しマクロ
 *
 *  サービスコール呼出しを含む式expを評価し，返値がエラー（負の値）の場
 *  合には，ercにercd_expを評価した値を代入し，error_exitにgotoする．
 */
#define SVC(exp, ercd_exp) do {		\
	if ((exp) < 0) {				\
		ercd = (ercd_exp);			\
		goto error_exit;			\
	}								\
} while (false)

/*
 *  E_SYSエラーの生成
 */
static ER
gen_ercd_sys(CELLCB *p_cellcb)
{
	VAR_errorFlag = true;
	return(E_SYS);
}

/*
 *  待ちに入るサービスコールからのエラーの変換
 */
static ER
gen_ercd_wait(ER rercd, CELLCB *p_cellcb)
{
	switch (MERCD(rercd)) {
	case E_RLWAI:
	case E_DLT:
	case E_RASTER:
		return(rercd);
	default:
		VAR_errorFlag = true;
		return(E_SYS);
	}
}

/*
 *  シリアルポートのオープン（受け口関数）
 */
ER
eSerialPort_open(CELLIDX idx)
{
	CELLCB	*p_cellcb;
	ER		ercd;

	if (sns_dpn()) {				/* コンテキストのチェック */
		return(E_CTX);
	}
	if (!VALID_IDX(idx)) {
		return(E_ID);				/* ポート番号のチェック */
	}
	p_cellcb = GET_CELLCB(idx);

	/*
   *  Bluetooth が接続されるまで待つ．
	 */
  while (!pbdrv_bluetooth_is_connected(PBDRV_BLUETOOTH_CONNECTION_UART)) {
    dly_tsk(1000*1000);
  }

	SVC(dis_dsp(), gen_ercd_sys(p_cellcb));
	if (VAR_openFlag) {				/* オープン済みかのチェック */
		ercd = E_OBJ;
	}
	else {
		/*
		 *  変数の初期化
		 */
		VAR_ioControl = (IOCTL_ECHO | IOCTL_CRLF | IOCTL_FCSND | IOCTL_FCRCV);

		/*
		 *  これ以降，割込みを禁止する．
		 */
		if (loc_cpu() < 0) {
			ercd = E_SYS;
			goto error_exit_enadsp;
		}

		VAR_openFlag = true;
		VAR_errorFlag = false;

		if (unl_cpu() < 0) {
			VAR_errorFlag = true;
			ercd = E_SYS;
			goto error_exit_enadsp;
		}
		ercd = E_OK;
	}

  error_exit_enadsp:
	SVC(ena_dsp(), gen_ercd_sys(p_cellcb));

  error_exit:
	return(ercd);
}

/*
 *  シリアルポートのクローズ（受け口関数）
 */
ER
eSerialPort_close(CELLIDX idx)
{
	CELLCB	*p_cellcb;
	ER		ercd;
	bool_t	eflag = false;

	if (sns_dpn()) {				/* コンテキストのチェック */
		return(E_CTX);
	}
	if (!VALID_IDX(idx)) {
		return(E_ID);				/* ポート番号のチェック */
	}
	p_cellcb = GET_CELLCB(idx);

	SVC(dis_dsp(), gen_ercd_sys(p_cellcb));
	if (!VAR_openFlag) {			/* オープン済みかのチェック */
		ercd = E_OBJ;
	}
	else {
		/*
		 *  ハードウェア依存のクローズ処理
		 */
		if (loc_cpu() < 0) {
			eflag = true;
		}
		//cSIOPort_close();
		VAR_openFlag = false;
		if (unl_cpu() < 0) {
			eflag = true;
		}

		/*
		 *  セマフォの初期化
		 */
		if (cSendSemaphore_initialize() < 0) {
			eflag = true;
		}
		if (cReceiveSemaphore_initialize() < 0) {
			eflag = true;
		}

		/*
		 *  エラーコードの設定
		 */
		if (eflag) {
			ercd = gen_ercd_sys(p_cellcb);
		}
		else {
			ercd = E_OK;
		}
	}
	SVC(ena_dsp(), gen_ercd_sys(p_cellcb));

  error_exit:
	return(ercd);
}

/*
 *  シリアルポートへの文字列送信（受け口関数）
 */
ER_UINT
eSerialPort_write(CELLIDX idx, const char *buffer, uint_t length)
{
	CELLCB	*p_cellcb;
	bool_t	buffer_full;
	uint_t	wricnt = 0U;
  uint32_t size;
	ER		ercd, rercd;
  pbio_error_t err;

	if (sns_dpn()) {				/* コンテキストのチェック */
		return(E_CTX);
	}
	if (!VALID_IDX(idx)) {			/* ポート番号のチェック */
		return(E_ID);
	}

	p_cellcb = GET_CELLCB(idx);
	if (!VAR_openFlag) {			/* オープン済みかのチェック */
		return(E_OBJ);
	}
	if (VAR_errorFlag) {			/* エラー状態かのチェック */
		return(E_SYS);
	}

	buffer_full = true;				/* ループの1回めはwai_semする */
	while (wricnt < length) {
		if (buffer_full) {
			SVC(rercd = cSendSemaphore_wait(),
										gen_ercd_wait(rercd, p_cellcb));
		}

    size = length - wricnt;
    err = pbsys_bluetooth_tx((const uint8_t *)buffer, &size);
    // TODO: PBIO_ERROR_AGAIN のときの対応
    if (err == PBIO_SUCCESS) {
      buffer += size;
		  wricnt += size;
    }

    if (err !=PBIO_SUCCESS && err != PBIO_ERROR_AGAIN) {
		  //VAR_errorFlag = true;
		  ercd = E_SYS;
      goto error_exit;
    }

		buffer_full = (bool_t) rercd || (err !=PBIO_SUCCESS && err != PBIO_ERROR_AGAIN);
	}
	if (!buffer_full) {
		SVC(cSendSemaphore_signal(), gen_ercd_sys(p_cellcb));
	}
	ercd = E_OK;

  error_exit:
	return(wricnt > 0U ? (ER_UINT) wricnt : ercd);
}

/*
 *  シリアルポートからの文字列受信（受け口関数）
 */
ER_UINT
eSerialPort_read(CELLIDX idx, char *buffer, uint_t length)
{
	CELLCB	*p_cellcb;
	bool_t	buffer_empty;
	uint_t	reacnt = 0U;
	ER		ercd, rercd;
  uint32_t size;
  pbio_error_t err;

	if (sns_dpn()) {				/* コンテキストのチェック */
		return(E_CTX);
	}
	if (!VALID_IDX(idx)) {			/* ポート番号のチェック */
		return(E_ID);
	}

	p_cellcb = GET_CELLCB(idx);
	if (!VAR_openFlag) {			/* オープン済みかのチェック */
		return(E_OBJ);
	}
	if (VAR_errorFlag) {			/* エラー状態かのチェック */
		return(E_SYS);
	}

	buffer_empty = true;			/* ループの1回めはwai_semする */
	while (reacnt < length) {
		if (buffer_empty) {
			SVC(rercd = cReceiveSemaphore_wait(),
										gen_ercd_wait(rercd, p_cellcb));
		}
  
    size = length - reacnt;
    err = pbsys_bluetooth_rx((uint8_t *)buffer, &size);
    // TODO: PBIO_ERROR_AGAIN のときの対応
    if (err == PBIO_SUCCESS) {
		  buffer += size;
		  reacnt += size;
    }

    if (err !=PBIO_SUCCESS && err != PBIO_ERROR_AGAIN) {
		  //VAR_errorFlag = true;
		  ercd = E_SYS;
      goto error_exit;
    }

		buffer_empty = (bool_t) rercd || (err !=PBIO_SUCCESS && err != PBIO_ERROR_AGAIN);
	}
	if (!buffer_empty) {
		SVC(cReceiveSemaphore_signal(), gen_ercd_sys(p_cellcb));
	}
	ercd = E_OK;

  error_exit:
	return(reacnt > 0U ? (ER_UINT) reacnt : ercd);
}

/*
 *  シリアルポートの制御（受け口関数）
 */
ER
eSerialPort_control(CELLIDX idx, uint_t ioctl)
{
	CELLCB	*p_cellcb;

	if (sns_dpn()) {				/* コンテキストのチェック */
		return(E_CTX);
	}
	if (!VALID_IDX(idx)) {
		return(E_ID);				/* ポート番号のチェック */
	}

	p_cellcb = GET_CELLCB(idx);
	if (!VAR_openFlag) {			/* オープン済みかのチェック */
		return(E_OBJ);
	}
	if (VAR_errorFlag) {			/* エラー状態かのチェック */
		return(E_SYS);
	}

	VAR_ioControl = ioctl;
	return(E_OK);
}

/*
 *  シリアルポート状態の参照（受け口関数）
 */
ER
eSerialPort_refer(CELLIDX idx, T_SERIAL_RPOR* pk_rpor)
{
	CELLCB	*p_cellcb;

	if (sns_dpn()) {				/* コンテキストのチェック */
		return(E_CTX);
	}
	if (!VALID_IDX(idx)) {
		return(E_ID);				/* ポート番号のチェック */
	}

	p_cellcb = GET_CELLCB(idx);
	if (!VAR_openFlag) {			/* オープン済みかのチェック */
		return(E_OBJ);
	}
	if (VAR_errorFlag) {			/* エラー状態かのチェック */
		return(E_SYS);
	}

	//pk_rpor->reacnt = VAR_receiveCount;
	//pk_rpor->wricnt = VAR_sendCount;
	return(E_OK);
}

/*
 *  シリアルインタフェースドライバからの未送信文字の取出し
 */
bool_t
enSerialPortManage_getChar(CELLIDX idx, char *p_c)
{
	CELLCB	*p_cellcb;

	if (VALID_IDX(idx)) {						/* ポート番号のチェック */
		p_cellcb = GET_CELLCB(idx);
		if (VAR_openFlag) {						/* オープン済みかのチェック */
			return(true);
		}
	}
	return(false);
}
