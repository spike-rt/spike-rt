/*
 *   Copyright (C) 2008-2017 by TOPPERS Project
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
 *  @(#) $Id: nMruby_tMruby.c 2640 2017-06-03 11:27:12Z okuma-top $
 */

/*
 * 以前の nMruby_tMrubyProc.c は、mruby の本体に含まれていた 
 *    mrbgems/mruby-bin-mruby/tools/mruby/mruby.c
 * をベースに TECS のコードを足していたが、TOPPERS ライセンスと異なるため
 * 新たに書き下ろした。（小さなコードのため、概ね似たようなものになるが。
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mruby.h"
#include "mruby/string.h"
#include "mruby/array.h"
#include "mruby/compile.h"
#include "mruby/dump.h"
#include "mruby/variable.h"

/* #[<PREAMBLE>]#
 * Don't edit the comments between #[<...>]# and #[</...>]#
 * These comment are used by tecsmerege when merging.
 *
 * attr access macro #_CAAM_#
 * mrb              mrb_state*       VAR_mrb         
 *
 * call port function #_TCPF_#
 * call port: cInit signature: nMruby_sInitializeBridge context:task optional:true
 *   bool_t     is_cInit_joined()                     check if joined
 *   void           cInit_initializeBridge( const mrb_state* mrb );
 *
 * #[</PREAMBLE>]# */

/* プロトタイプ宣言や変数の定義をここに書きます #_PAC_# */
#include "nMruby_tMrubyProc_tecsgen.h"

#ifndef E_OK
#define	E_OK	0		/* success */
#define	E_ID	(-18)	/* illegal ID */
#endif

/* 受け口関数 #_TEPF_# */
/* #[<ENTRY_PORT>]# eMain
 * entry port: eMain
 * signature:  nPosix_sMain
 * context:    task
 * #[</ENTRY_PORT>]# */

/* #[<ENTRY_FUNC>]# eMain_main
 * name:         eMain_main
 * global_name:  nMruby_tMrubyProc_eMain_main
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
int
eMain_main(CELLIDX idx, int argc, const char** argv)
{
	CELLCB	*p_cellcb;
  mrb_state *mrb;
  mrbc_context* context;
  if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		/* エラー処理コードをここに記述します */
      return -1;
	} /* end if VALID_IDX(idx) */

	/* ここに処理本体を記述します #_TEFB_# */
  if (argc != 2) {
      printf("usage: %s script.rb\n", argv[0]);
      goto error_return;
  }
  mrb = mrb_open();
  if( mrb == 0 ){
      printf("%s: mrb_open() failed\n", argv[0]);
      goto error_return;
  }
  context = mrbc_context_new(mrb);
  if (context == 0) {
      printf("%s: mrbc_context_new() failed\n", argv[0]);
      goto error_close;
  }

  /*--------   ARGV   ----------*/
  {
      mrb_value ARGV = mrb_ary_new_capa(mrb, argc);
      int  i;
      for (i = 0; i < argc; i++) {
          mrb_ary_push(mrb, ARGV, mrb_str_new_cstr(mrb, argv[i]));
      }
      mrb_define_global_const(mrb, "ARGV", ARGV);
  }
  /*--------   $0   ----------*/
  {
      mrb_sym zero_sym = mrb_intern_lit(mrb, "$0");
      mrbc_filename(mrb, context, argv[0]);
      mrb_gv_set(mrb, zero_sym, mrb_str_new_cstr(mrb, argv[0]));
  }

  /* ------- register TECS bridge classes -------*/
  if (is_cInit_joined()) {
      cInit_initializeBridge(mrb);
  }

  /*--------  load .rb file -------*/
  {
      FILE *file = fopen(argv[1], "r");
      if (file == NULL) {
          printf("file not found: %s\n", argv[1]);
          goto error_free;
      }

      mrb_value val = mrb_load_file_cxt(mrb, file, context);
      if (mrb->exc) {
          if(! mrb_undef_p(val)){
              mrb_print_error(mrb);
          }
      }
  }

  /*---------- success case --------*/
  mrbc_context_free(mrb, context);
  mrb_close( mrb );
  return 0;

  /*---------- error case ----------*/
error_free:
  mrbc_context_free(mrb, context);
error_close:
  mrb_close( mrb );
error_return:

  return -1;
}

/* #[<POSTAMBLE>]#
 *   Put non-entry functions below.
 * #[</POSTAMBLE>]#*/
