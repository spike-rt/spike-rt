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
 *  @(#) $Id: nTECSInfo_tSignatureInfo.c 2656 2017-06-24 11:57:31Z okuma-top $
 */

/* #[<PREAMBLE>]#
 * #[<...>]# から #[</...>]# で囲まれたコメントは編集しないでください
 * tecsmerge によるマージに使用されます
 *
 * 属性アクセスマクロ #_CAAM_#
 * name             char*            ATTR_name       
 *
 * 呼び口関数 #_TCPF_#
 * call port: cFunctionInfo signature: nTECSInfo_sFunctionInfo context:task optional:true
 *   bool_t     is_cFunctionInfo_joined(int subscript)        check if joined
 *   ER             cFunctionInfo_getName( subscript, char_t* name, int_t max_len );
 *   uint16_t       cFunctionInfo_getNameLength( subscript );
 *   void           cFunctionInfo_getReturnTypeInfo( subscript, Descriptor( nTECSInfo_sTypeInfo )* desc );
 *   uint32_t       cFunctionInfo_getNParam( subscript );
 *   ER             cFunctionInfo_getParamInfo( subscript, uint32_t ith, Descriptor( nTECSInfo_sParamInfo )* param );
 *       subscript:  0...(NCP_cFunctionInfo-1)
 *   [ref_desc]
 *      Descriptor( nTECSInfo_sFunctionInfo ) cFunctionInfo_refer_to_descriptor( int_t subscript );
 *      Descriptor( nTECSInfo_sFunctionInfo ) cFunctionInfo_ref_desc( int_t subscript )      (same as above; abbreviated version);
 *
 * #[</PREAMBLE>]# */

/* Put prototype declaration and/or variale definition here #_PAC_# */
#include <string.h>
#include "nTECSInfo_tSignatureInfo_tecsgen.h"

#ifndef E_OK
#define	E_OK	0		/* success */
#define	E_ID	(-18)	/* illegal ID */
#endif

/* entry port function #_TEPF_# */
/* #[<ENTRY_PORT>]# eSignatureInfo
 * entry port: eSignatureInfo
 * signature:  nTECSInfo_sSignatureInfo
 * context:    task
 * #[</ENTRY_PORT>]# */

/* #[<ENTRY_FUNC>]# eSignatureInfo_getName
 * name:         eSignatureInfo_getName
 * global_name:  nTECSInfo_tSignatureInfo_eSignatureInfo_getName
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
ER
eSignatureInfo_getName(CELLIDX idx, char_t* name, int_t max_len)
{
	ER		ercd = E_OK;
	CELLCB	*p_cellcb;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		return(E_ID);
	} /* end if VALID_IDX(idx) */

	/* Put statements here #_TEFB_# */
	name[ max_len - 1 ] = '\0';
	strncpy( name, ATTR_name, max_len );
  if( name[ max_len - 1 ] ){
      name[ max_len - 1 ] = '\0';
      ercd = E_NOMEM;
  }

	return(ercd);
}

/* #[<ENTRY_FUNC>]# eSignatureInfo_getNameLength
 * name:         eSignatureInfo_getNameLength
 * global_name:  nTECSInfo_tSignatureInfo_eSignatureInfo_getNameLength
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
uint16_t
eSignatureInfo_getNameLength(CELLIDX idx)
{
	CELLCB	*p_cellcb;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		/* Write error processing code here */
	} /* end if VALID_IDX(idx) */

	/* Put statements here #_TEFB_# */
	return strlen( ATTR_name ) + 1;
      /* 終端文字 (NULL) を含む */
}

/* #[<ENTRY_FUNC>]# eSignatureInfo_getNFunction
 * name:         eSignatureInfo_getNFunction
 * global_name:  nTECSInfo_tSignatureInfo_eSignatureInfo_getNFunction
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
uint32_t
eSignatureInfo_getNFunction(CELLIDX idx)
{
	CELLCB	*p_cellcb;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		/* Write error processing code here */
	} /* end if VALID_IDX(idx) */

	/* Put statements here #_TEFB_# */
  return NCP_cFunctionInfo;
}

/* #[<ENTRY_FUNC>]# eSignatureInfo_getFunctionInfo
 * name:         eSignatureInfo_getFunctionInfo
 * global_name:  nTECSInfo_tSignatureInfo_eSignatureInfo_getFunctionInfo
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
ER
eSignatureInfo_getFunctionInfo(CELLIDX idx, uint32_t ith, Descriptor( nTECSInfo_sFunctionInfo )* desc)
{
	CELLCB	*p_cellcb;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		/* Write error processing code here */
	} /* end if VALID_IDX(idx) */

	/* Put statements here #_TEFB_# */
  *desc = cFunctionInfo_ref_desc( ith );
  	return E_OK;
}

/* #[<POSTAMBLE>]#
 *   これより下に非受け口関数を書きます
 * #[</POSTAMBLE>]#*/
