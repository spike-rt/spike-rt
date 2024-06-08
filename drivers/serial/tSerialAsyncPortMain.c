/*
 *  TOPPERS Software
 *      Toyohashi Open Platform for Embedded Real-Time Systems
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2006-2023 by Embedded and Real-Time Systems Laboratory
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
 * シリアルインタフェースドライバ
 */

#include <kernel.h>
#include "tSerialAsyncPortMain_tecsgen.h"
#include <t_syslog.h>
#include <string.h>


#define MIN(a, b) ((a>b) ? b: a)

/*
 *  ポインタのインクリメント
 */
#define INC_PTR(ptr, bufsz) do {  \
  if (++(ptr) == (bufsz)) {    \
    (ptr) = 0;          \
   }                \
} while (false)

/*
 *  ポインタの加算
 */
#define ADD_PTR(ptr, bufsz, adder) do {  \
  (ptr) += (adder);         \
  if ((ptr) >= (bufsz)) {    \
    (ptr) -= bufsz;          \
  }                \
} while (false)

/*
 *  サービスコール呼出しマクロ
 *
 *  サービスコール呼出しを含む式expを評価し，返値がエラー（負の値）の場
 *  合には，ercにercd_expを評価した値を代入し，error_exitにgotoする．
 */
#define SVC(exp, ercd_exp) do {    \
  if ((exp) < 0) {        \
    ercd = (ercd_exp);      \
    goto error_exit;      \
  }                \
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
  syslog(LOG_EMERG, "rercd : %d\n", rercd);
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
  CELLCB  *p_cellcb;
  ER    ercd;

  if (sns_dpn()) {        /* コンテキストのチェック */
    return(E_CTX);
  }
  if (!VALID_IDX(idx)) {
    return(E_ID);        /* ポート番号のチェック */
  }
  p_cellcb = GET_CELLCB(idx);

  /*
   *  ハードウェア依存のオープン処理
   */
  cSIOPort_open();

  SVC(dis_dsp(), gen_ercd_sys(p_cellcb));
  if (VAR_openFlag) {        /* オープン済みかのチェック */
    ercd = E_OBJ;
  }
  else {
    /*
     *  変数の初期化
     */
    VAR_ioControl = (IOCTL_ECHO | IOCTL_CRLF | IOCTL_FCSND | IOCTL_FCRCV);

    VAR_receiveReadPointer = 0U;
    VAR_receiveWritePointer = 0U;
    VAR_receiveCount = 0U;

    VAR_sendReadPointer = 0U;
    VAR_sendWritePointer = 0U;
    VAR_sendCount = 0U;

    /*
     *  これ以降，割込みを禁止する．
     */
    if (loc_cpu() < 0) {
      ercd = E_SYS;
      goto error_exit_enadsp;
    }


    /*
     *  送信コールバックを禁止する．
     */
    cSIOPort_disableCBR(SIOAsyncSendPop);

    /*
     *  受信コールバックを許可する．
     */
    cSIOPort_enableCBR(SIOAsyncReceivePush);
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
  CELLCB  *p_cellcb;
  ER    ercd;
  bool_t  eflag = false;

  if (sns_dpn()) {        /* コンテキストのチェック */
    return(E_CTX);
  }
  if (!VALID_IDX(idx)) {
    return(E_ID);        /* ポート番号のチェック */
  }
  p_cellcb = GET_CELLCB(idx);

  SVC(dis_dsp(), gen_ercd_sys(p_cellcb));
  if (!VAR_openFlag) {      /* オープン済みかのチェック */
    ercd = E_OBJ;
  }
  else {
    /*
     *  ハードウェア依存のクローズ処理
     */
    if (loc_cpu() < 0) {
      eflag = true;
    }
    cSIOPort_close();
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

Inline void
serialPort_writeBegin(CELLCB *p_cellcb)
{
  if (VAR_sendCount > 0) {
    cSIOPort_putNotify();
    // cSIOPort_enableCBR(SIOAsyncSendPop);
  }
}

/*
 *  送信用バッファへの1文字書き込み．
 */
static ER_BOOL
serialPort_writeChar(CELLCB *p_cellcb, char c)
{
  bool_t  buffer_full;
  ER_BOOL  ercd;
  ER    rercd;

  /*
   *  LFの前にCRを送信する．
   */
  if (c == '\n' && (VAR_ioControl & IOCTL_CRLF) != 0U) {
    /*
     *  以下のコードは再帰呼出しになっているが，引数cが'\n'の場合に
     *  引数cを'\r'として呼び出すことから，この再帰呼出しは2回目の
     *  呼び出しで必ず止まる．
     */
    SVC(rercd = serialPort_writeChar(p_cellcb, '\r'), rercd);
    if ((bool_t) rercd) {
      serialPort_writeBegin(p_cellcb);
      SVC(rercd = cSendSemaphore_wait(),
                    gen_ercd_wait(rercd, p_cellcb));
    }
  }

  /*
   * コールバックとの競合状態を防ぐためにロックする．
   */ 
  SVC(loc_cpu(), gen_ercd_sys(p_cellcb));

  VAR_sendBuffer[VAR_sendWritePointer] = c;
  INC_PTR(VAR_sendWritePointer, ATTR_sendBufferSize);
  VAR_sendCount++;
  buffer_full = (VAR_sendCount == ATTR_sendBufferSize);

  if (VAR_sendCount == 1) {
    /*
     * VAR_sendCount 0 -> 1 の場合，送信コールバックを許可する．
     */
    cSIOPort_enableCBR(SIOAsyncSendPop);
  }

  SVC(unl_cpu(), gen_ercd_sys(p_cellcb));

  ercd = (ER_BOOL) buffer_full;

  error_exit:
  return(ercd);
}

/*
 *  シリアルポートへの文字列送信（受け口関数）
 */
ER_UINT
eSerialPort_write(CELLIDX idx, const char *buffer, uint_t length)
{
  CELLCB  *p_cellcb;
  bool_t  buffer_full = false;
  uint_t  wricnt = 0U;
  ER    ercd, rercd;

  if (sns_dpn()) {        /* コンテキストのチェック */
    return(E_CTX);
  }
  if (!VALID_IDX(idx)) {      /* ポート番号のチェック */
    return(E_ID);
  }

  p_cellcb = GET_CELLCB(idx);
  if (!VAR_openFlag) {      /* オープン済みかのチェック */
    return(E_OBJ);
  }
  if (VAR_errorFlag) {      /* エラー状態かのチェック */
    return(E_SYS);
  }

  SVC(rercd = cSendSemaphore_wait(), gen_ercd_wait(rercd, p_cellcb));
  while (wricnt < length) {
    if (buffer_full) {
      /* 
       * 送信バッファに空きがなければ，
       * 空きができるのを待ってから，バッファに書き込む
       */
      serialPort_writeBegin(p_cellcb);
      SVC(rercd = cSendSemaphore_wait(),
                    gen_ercd_wait(rercd, p_cellcb));
    }
    SVC(rercd = serialPort_writeChar(p_cellcb, *buffer++), rercd);
    wricnt++;
    buffer_full = (bool_t) rercd;
  }
  serialPort_writeBegin(p_cellcb);
  if (!buffer_full) {
    /* 
     * 送信バッファがフルでない場合は，セマフォを解放する．
     * フルの場合は，送信用コールバックにより空きができた時に解放される．
     */
    SVC(cSendSemaphore_signal(), gen_ercd_sys(p_cellcb));
  }
  ercd = E_OK;

  error_exit:
  return(wricnt > 0U ? (ER_UINT) wricnt : ercd);
}

/*
 *  シリアルポートからの1文字受信
 */
static ER_BOOL
serialPort_readChar(CELLCB *p_cellcb, char *p_c)
{
  bool_t  buffer_empty;
  ER_BOOL  ercd;

	SVC(loc_cpu(), gen_ercd_sys(p_cellcb));

  /*
   *  受信バッファから文字を取り出す．
   */
  *p_c = VAR_receiveBuffer[VAR_receiveReadPointer];
  INC_PTR(VAR_receiveReadPointer, ATTR_receiveBufferSize);
  VAR_receiveCount--;
  buffer_empty = (VAR_receiveCount == 0U);

	SVC(unl_cpu(), gen_ercd_sys(p_cellcb));
  ercd = (ER_BOOL) buffer_empty;

  error_exit:
  return(ercd);
}

/*
 *  シリアルポートからの文字列受信（受け口関数）
 */
ER_UINT
eSerialPort_read(CELLIDX idx, char *buffer, uint_t length)
{
  CELLCB  *p_cellcb;
  bool_t  buffer_empty;
  uint_t  reacnt = 0U;
  char  c = '\0';    /* コンパイラの警告を抑止するために初期化する */
  ER    ercd, rercd;

  if (sns_dpn()) {        /* コンテキストのチェック */
    return(E_CTX);
  }
  if (!VALID_IDX(idx)) {      /* ポート番号のチェック */
    return(E_ID);
  }

  p_cellcb = GET_CELLCB(idx);
  if (!VAR_openFlag) {      /* オープン済みかのチェック */
    return(E_OBJ);
  }
  if (VAR_errorFlag) {      /* エラー状態かのチェック */
    return(E_SYS);
  }

  buffer_empty = true;      /* ループの1回めはwai_semする */
  while (reacnt < length) {
    if (buffer_empty) {
      SVC(rercd = cReceiveSemaphore_wait(),
                    gen_ercd_wait(rercd, p_cellcb));
    }
    SVC(rercd = serialPort_readChar(p_cellcb, &c), rercd);
    *buffer++ = c;
    reacnt++;
    buffer_empty = (bool_t) rercd;

    /*
     *  エコーバック処理．
     */
    if ((VAR_ioControl & IOCTL_ECHO) != 0U) {
      SVC(rercd = cSendSemaphore_wait(),
                    gen_ercd_wait(rercd, p_cellcb));
      SVC(rercd = serialPort_writeChar(p_cellcb, c), rercd);
      if (!((bool_t) rercd)) {
        SVC(cSendSemaphore_signal(), gen_ercd_sys(p_cellcb));
      }
      serialPort_writeBegin(p_cellcb);
    }
  }
  if (!buffer_empty) {
    /* 
     * 受信バッファが空でない場合は，取り出し処理が終わったのでセマフォを解放する．
     * 空の場合は，受信用コールバックにより受信データが存在する時に解放される．
     */
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
  CELLCB  *p_cellcb;

  if (sns_dpn()) {        /* コンテキストのチェック */
    return(E_CTX);
  }
  if (!VALID_IDX(idx)) {
    return(E_ID);        /* ポート番号のチェック */
  }

  p_cellcb = GET_CELLCB(idx);
  if (!VAR_openFlag) {      /* オープン済みかのチェック */
    return(E_OBJ);
  }
  if (VAR_errorFlag) {      /* エラー状態かのチェック */
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
  CELLCB  *p_cellcb;

  if (sns_dpn()) {        /* コンテキストのチェック */
    return(E_CTX);
  }
  if (!VALID_IDX(idx)) {
    return(E_ID);        /* ポート番号のチェック */
  }

  p_cellcb = GET_CELLCB(idx);
  if (!VAR_openFlag) {      /* オープン済みかのチェック */
    return(E_OBJ);
  }
  if (VAR_errorFlag) {      /* エラー状態かのチェック */
    return(E_SYS);
  }

  pk_rpor->reacnt = VAR_receiveCount;
  pk_rpor->wricnt = VAR_sendCount;
  return(E_OK);
}

/*
 * 送信バッファのサイズを返すコールバック．
 */
ER_UINT
eSIOCBR_sizeSend(CELLIDX idx)
{
  CELLCB  *p_cellcb;
  assert(VALID_IDX(idx));
  p_cellcb = GET_CELLCB(idx);

  return VAR_sendCount;
}

/*
 * 送信バッファから１文字を取り出すコールバック．
 */
ER_UINT
eSIOCBR_popSend(CELLIDX idx, char *dst)
{
  CELLCB  *p_cellcb;
  uint_t send_size = 0U;
  ER    ercd;

  assert(VALID_IDX(idx));
  p_cellcb = GET_CELLCB(idx);

  if (VAR_sendCount > 0U) {
    /*
     *  送信バッファから文字を取り出す．
     */
    *dst = VAR_sendBuffer[VAR_sendReadPointer];
    INC_PTR(VAR_sendReadPointer, ATTR_sendBufferSize);
    if (VAR_sendCount == ATTR_sendBufferSize) {
      SVC(cSendSemaphore_signal(), gen_ercd_sys(p_cellcb));
    }
    VAR_sendCount--;
    send_size = 1;
  }

  if (VAR_sendCount == 0U) {
    /*
     * これ以上送るべき文字が存在しない場合は，送信コールバックを禁止する．
     */
    cSIOPort_disableCBR(SIOAsyncSendPop);
  }

  ercd = (ER_UINT)send_size;

  error_exit:
  return(ercd);
}


#if 0
/*
 * 送信バッファから指定された文字数以下の文字列を取り出すコールバック．
 */
ER_UINT
eSIOCBR_popSend(CELLIDX idx, char *dst_data, uint_t max_size)
{
  CELLCB  *p_cellcb;
  uint_t send_size = 0U;
  int buffer_tail_size;
  ER    ercd;

  assert(VALID_IDX(idx));
  p_cellcb = GET_CELLCB(idx);

  // TODO: 競合状態は発生する？
  //SVC(loc_cpu(), gen_ercd_sys(p_cellcb));
  if (VAR_sendCount < ATTR_sendBufferSize) {
    /*
     * 送信バッファに空きがある場合，セマフォを獲得する．
     * フルである場合は，eSerialPort_write() により獲得済みなので，
     * 獲得しない．
     */
    SVC(rercd = cSendSemaphore_wait(), gen_ercd_wait(rercd, p_cellcb));
  }
  //SVC(unl_cpu(), gen_ercd_sys(p_cellcb));

  if (VAR_sendCount > 0U) {
    /*
     *  送信バッファに送信するべき文字が存在する場合．
     */
    send_size = MIN(max_size, VAR_sendCount);
    buffer_tail_size = ATTR_sendBufferSize - VAR_sendReadPointer;
    if (send_size <= buffer_tail_size) {
      memcpy(dst_data, &VAR_sendBuffer[VAR_sendReadPointer], send_size);
    }
    else {
      memcpy(dst_data, &VAR_sendBuffer[VAR_sendReadPointer], buffer_tail_size);
      memcpy(dst_data, &VAR_sendBuffer[0], send_size - buffer_tail_size);
    }
    ADD_PTR(VAR_sendReadPointer, ATTR_sendBufferSize, send_size);
    VAR_sendCount -= send_size;
  }

  if (VAR_sendCount == 0U) {
    /*
     * これ以上送るべき文字が存在しない場合は，送信コールバックを禁止する．
     */
    cSIOPort_disableCBR(SIOAsyncSendPop);
  }

  SVC(cSendSemaphore_signal(), gen_ercd_sys(p_cellcb));
  ercd = (ER_UINT)send_size;

  error_exit:
  return(ercd);
}
#endif

/*
 *  シリアルポートからの受信通知コールバック（受け口関数）
 */
ER_UINT
eSIOCBR_pushReceive(CELLIDX idx, char src)
{
  CELLCB  *p_cellcb;
  uint_t receive_size = 0U;
  ER    ercd;

  assert(VALID_IDX(idx));
  p_cellcb = GET_CELLCB(idx);

  if (VAR_receiveCount == ATTR_receiveBufferSize) {
    /*
     *  バッファフルの場合，受信した文字を捨てる．
     */
  }
  else {
    /*
     *  受信した文字を受信バッファに入れる．
     */
    VAR_receiveBuffer[VAR_receiveWritePointer] = src;
    INC_PTR(VAR_receiveWritePointer, ATTR_receiveBufferSize);
    if (VAR_receiveCount == 0U) {
      SVC(cReceiveSemaphore_signal(), gen_ercd_sys(p_cellcb));
    }
    VAR_receiveCount++;
    receive_size = 1;
  }
  ercd = (ER_UINT)receive_size;

  error_exit:
  return(ercd);
}

#if 0
/*
 *  シリアルポートからの受信通知コールバック（受け口関数）
 */
ER_UINT
eSIOCBR_pushReceive(CELLIDX idx, const char *src_data, uint_t size)
{
  CELLCB  *p_cellcb;
  bool_t  buffer_empty;
  uint_t receive_size = 0U;
  int buffer_tail_size;
  ER    ercd, rercd;

  assert(VALID_IDX(idx));
  p_cellcb = GET_CELLCB(idx);

  //SVC(loc_cpu(), gen_ercd_sys(p_cellcb));
  // TODO: 競合状態は発生する？
  if (VAR_receiveCount == 0) {
    /*
     * 受信バッファが空ではない場合，セマフォを獲得済みのタスクが存在しないので
     * セマフォを獲得する．この獲得では，待ち状態にならない．
     * フルである場合は，セマフォが初期状態(0)
     * または，eSerialPort_read()によって獲得済みなので，獲得しない．
     */
    SVC(rercd = cReceiveSemaphore_wait(), gen_ercd_wait(rercd, p_cellcb));
  }
  //SVC(unl_cpu(), gen_ercd_sys(p_cellcb));
  
  if (VAR_receiveCount == ATTR_receiveBufferSize) {
    /*
     *  バッファフルの場合，受信した文字を捨てる．
     */
  }
  else {
    /*
     *  受信した文字をできるだけ受信バッファに入れる．
     */
    receive_size = MIN(ATTR_receiveBufferSize - VAR_receiveCount, size);
    buffer_tail_size = ATTR_receiveBufferSize - VAR_receiveWritePointer;
    if (receive_size <= buffer_tail_size) {
      memcpy(&VAR_receiveBuffer[VAR_receiveWritePointer], src_data, receive_size);
    }
    else {
      memcpy(&VAR_receiveBuffer[VAR_receiveWritePointer], src_data, buffer_tail_size);
      memcpy(&VAR_receiveBuffer[0], src_data, receive_size - buffer_tail_size);
    }
    ADD_PTR(VAR_receiveWritePointer, ATTR_receiveBufferSize, receive_size);
    VAR_receiveCount += receive_size;
  }
  SVC(cReceiveSemaphore_signal(), gen_ercd_sys(p_cellcb));
  ercd = (ER_UINT)receive_size;

  error_exit:
  return(ercd);
}
#endif

/*
 *  シリアルインタフェースドライバからの未送信文字の取出し
 */
bool_t
enSerialPortManage_getChar(CELLIDX idx, char *p_c)
{
  CELLCB  *p_cellcb;

  if (VALID_IDX(idx)) {            /* ポート番号のチェック */
    p_cellcb = GET_CELLCB(idx);
    if (VAR_openFlag) {            /* オープン済みかのチェック */
      if (VAR_sendCount > 0U) {
        *p_c = VAR_sendBuffer[VAR_sendReadPointer];
        INC_PTR(VAR_sendReadPointer, ATTR_sendBufferSize);
        VAR_sendCount--;
        return(true);
      }
    }
  }
  return(false);
}
