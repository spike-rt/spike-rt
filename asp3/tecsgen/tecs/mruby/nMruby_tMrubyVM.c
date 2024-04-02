/*
 *  Copyright (C) 2008-2017 by TOPPERS Project
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
 *  @(#) $Id: nMruby_tMrubyVM.c 2640 2017-06-03 11:27:12Z okuma-top $
 */

/* #[<PREAMBLE>]#
 * #[<...>]# から #[</...>]# で囲まれたコメントは編集しないでください
 * tecsmerge によるマージに使用されます
 *
 * 属性アクセスマクロ #_CAAM_#
 * irep             uint8_t*         ATTR_irep       
 * var_irep         mrb_irep*        VAR_var_irep    
 * mrb              mrb_state*       VAR_mrb         
 * rproc            struct RProc*    VAR_rproc       
 *
 * 呼び口関数 #_TCPF_#
 * call port: cInit signature: nMruby_sInitializeBridge context:task optional:true
 *   bool_t     is_cInit_joined()                     check if joined
 *   void           cInit_initializeBridge( const mrb_state* mrb );
 * call port: cMalloc signature: sMalloc context:task
 *   int            cMalloc_initializeMemoryPool( );
 *   void*          cMalloc_calloc( size_t nelem, size_t elem_size );
 *   void*          cMalloc_malloc( size_t size );
 *   void*          cMalloc_realloc( const void* ptr, size_t new_size );
 *   void           cMalloc_free( const void* ptr );
 *
 * #[</PREAMBLE>]# */

/* プロトタイプ宣言や変数の定義をここに書きます #_PAC_# */
#include "nMruby_tMrubyVM_tecsgen.h"
#include <string.h>

#include <mruby.h>
#include <mruby/proc.h>
#include <mruby/data.h>
#include <mruby/compile.h>
#include "t_syslog.h"

MRB_API mrb_state* mrb_open_TECS( CELLCB *p_cellcb);
static mrb_value mrb_run__(mrb_state *mrb, struct RProc *proc, mrb_value self);

#ifndef E_OK
#define	E_OK	0		/* success */
#define	E_ID	(-18)	/* illegal ID */
#endif

/* 受け口関数 #_TEPF_# */
/* #[<ENTRY_PORT>]# eMrubyVM
 * entry port: eMrubyVM
 * signature:  nMruby_sMrubyVM
 * context:    task
 * #[</ENTRY_PORT>]# */

/* #[<ENTRY_FUNC>]# eMrubyVM_initialize
 * name:         eMrubyVM_initialize
 * global_name:  nMruby_tMrubyVM_eMrubyVM_initialize
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
bool_t
eMrubyVM_initialize(CELLIDX idx)
{
	CELLCB	*p_cellcb;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		/* エラー処理コードをここに記述します */
    return false;
	} /* end if VALID_IDX(idx) */

  cMalloc_initializeMemoryPool();
	/* ここに処理本体を記述します #_TEFB_# */
  /* new interpreter instance */
  //VAR_mrb = mrb_open();
  VAR_mrb = mrb_open_TECS(p_cellcb);
  if (VAR_mrb == NULL) {
    syslog(LOG_EMERG, "Invalid mrb_state, exiting test driver");
//    cSerialPort_write("Invalid mrb_state, exiting test driver\n", 39);
    return false;
  }

  if(is_cInit_joined()){
    // TECS Bridge
    cInit_initializeBridge( VAR_mrb );
  }
  VAR_var_irep = mrb_read_irep(VAR_mrb, ATTR_irep);
  VAR_rproc = mrb_proc_new(VAR_mrb, VAR_var_irep);
  return true;
}

/* #[<ENTRY_FUNC>]# eMrubyVM_run
 * name:         eMrubyVM_run
 * global_name:  nMruby_tMrubyVM_eMrubyVM_run
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
bool_t
eMrubyVM_run(CELLIDX idx)
{
	CELLCB	*p_cellcb;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		/* エラー処理コードをここに記述します */
	} /* end if VALID_IDX(idx) */

	/* ここに処理本体を記述します #_TEFB_# */
  mrb_run__(VAR_mrb, VAR_rproc, mrb_top_self(VAR_mrb));
  if (VAR_mrb->exc) {
    mrb_p(VAR_mrb, mrb_obj_value(VAR_mrb->exc));
    return false;
  }
  return true;
}

/* #[<ENTRY_FUNC>]# eMrubyVM_funcall
 * name:         eMrubyVM_funcall
 * global_name:  nMruby_tMrubyVM_eMrubyVM_funcall
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
void
eMrubyVM_funcall(CELLIDX idx, const char_t* name)
{
	CELLCB	*p_cellcb;
  mrb_int  argc;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		/* エラー処理コードをここに記述します */
	} /* end if VALID_IDX(idx) */

	/* ここに処理本体を記述します #_TEFB_# */
  argc = 0;
  (void)mrb_funcall(VAR_mrb, mrb_top_self(VAR_mrb), name, argc );
}

/* #[<ENTRY_FUNC>]# eMrubyVM_finalize
 * name:         eMrubyVM_finalize
 * global_name:  nMruby_tMrubyVM_eMrubyVM_finalize
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
void
eMrubyVM_finalize(CELLIDX idx)
{
	CELLCB	*p_cellcb;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		/* エラー処理コードをここに記述します */
	} /* end if VALID_IDX(idx) */

	/* ここに処理本体を記述します #_TEFB_# */
	syslog(LOG_EMERG, "end of mruby program");
  mrb_close(VAR_mrb);
}

/* #[<POSTAMBLE>]#
 *   これより下に非受け口関数を書きます
 * #[</POSTAMBLE>]#*/

#include "tlsf.h"
void*
mrb_TECS_allocf(mrb_state *mrb, void *p, size_t size, void *ud)
{
	CELLCB	*p_cellcb= (CELLCB *)ud;
  if (size == 0) {
      cMalloc_free(p);
      //tlsf_free(p);
      return NULL;
  }
  else if (p){
      //return tlsf_realloc(p,size);
      return cMalloc_realloc(p, size);
  }
  else {
      //return tlsf_malloc(size);
      return cMalloc_malloc(size);
  }
}

// p_cellcb は VM のもの
// VM セルタイプに、呼び口 call sMalloc cMalloc; を設ける
// mrb_TECS_allocf を参照
MRB_API mrb_state*
mrb_open_TECS( CELLCB *p_cellcb)
{
    return mrb_open_allocf(mrb_TECS_allocf, (void *)p_cellcb);
};

// mruby V3.0.0 から mrb_run が非公開となったため、代わりをこちらに記述する
// mruby 3.0.0 の state.c mrb_run のソースコードを参考にしている。
// mrb_state(mrb) の中を直接のぞくのは、お行儀のよくないので、
// mrb->c->ci->argc == 0 を仮定して呼び出す
static mrb_value
mrb_run__(mrb_state *mrb, struct RProc *proc, mrb_value self)
{
  // syslog( LOG_NOTICE, "nMruby_tMruby.c: mrb_run: argc=%d",mrb->c->ci->argc );

// mruby の state.c より
// mrb->c->ci->argc == 0 を仮定
  return mrb_vm_run(mrb, proc, self, 2); /* argc + 2 (receiver and block) */
}

// ダミーを外す
#if 0
/*** ダミー関数 ***/
#ifndef EV3_SOUND
void _write(){}
void _close(){}
void _lseek(){}
void _read(){}
//void _fstat(){}
//void _isatty(){}
#endif

//void _exit(){}
//void _getpid(){}
//void _kill(){}
void _gettimeofday(){}
void _fini(){}
#endif
