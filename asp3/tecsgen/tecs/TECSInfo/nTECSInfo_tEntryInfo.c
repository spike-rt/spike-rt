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
 *  @(#) $Id: nTECSInfo_tEntryInfo.c 2656 2017-06-24 11:57:31Z okuma-top $
 */

/* #[<PREAMBLE>]#
 * #[<...>]# から #[</...>]# で囲まれたコメントは編集しないでください
 * tecsmerge によるマージに使用されます
 *
 * 属性アクセスマクロ #_CAAM_#
 * name             char_t*          ATTR_name       
 * b_inline         bool_t           ATTR_b_inline   
 * array_size       uint32_t         ATTR_array_size 
 *
 * 呼び口関数 #_TCPF_#
 * call port: cSignatureInfo signature: nTECSInfo_sSignatureInfo context:task
 *   ER             cSignatureInfo_getName( char_t* name, int_t max_len );
 *   uint16_t       cSignatureInfo_getNameLength( );
 *   uint32_t       cSignatureInfo_getNFunction( );
 *   ER             cSignatureInfo_getFunctionInfo( uint32_t ith, Descriptor( nTECSInfo_sFunctionInfo )* desc );
 *   [ref_desc]
 *      Descriptor( nTECSInfo_sSignatureInfo ) cSignatureInfo_refer_to_descriptor();
 *      Descriptor( nTECSInfo_sSignatureInfo ) cSignatureInfo_ref_desc()      (same as above; abbreviated version);
 *
 * #[</PREAMBLE>]# */

/* Put prototype declaration and/or variale definition here #_PAC_# */
#include <string.h>
#include "nTECSInfo_tEntryInfo_tecsgen.h"

#ifndef E_OK
#define	E_OK	0		/* success */
#define	E_ID	(-18)	/* illegal ID */
#endif

/* entry port function #_TEPF_# */
/* #[<ENTRY_PORT>]# eEntryInfo
 * entry port: eEntryInfo
 * signature:  nTECSInfo_sEntryInfo
 * context:    task
 * #[</ENTRY_PORT>]# */

/* #[<ENTRY_FUNC>]# eEntryInfo_getName
 * name:         eEntryInfo_getName
 * global_name:  nTECSInfo_tEntryInfo_eEntryInfo_getName
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
ER
eEntryInfo_getName(CELLIDX idx, char_t* name, int_t max_len)
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

/* #[<ENTRY_FUNC>]# eEntryInfo_getNameLength
 * name:         eEntryInfo_getNameLength
 * global_name:  nTECSInfo_tEntryInfo_eEntryInfo_getNameLength
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
uint16_t
eEntryInfo_getNameLength(CELLIDX idx)
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
}

/* #[<ENTRY_FUNC>]# eEntryInfo_getSignatureInfo
 * name:         eEntryInfo_getSignatureInfo
 * global_name:  nTECSInfo_tEntryInfo_eEntryInfo_getSignatureInfo
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
void
eEntryInfo_getSignatureInfo(CELLIDX idx, Descriptor( nTECSInfo_sSignatureInfo )* desc)
{
	CELLCB	*p_cellcb;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		/* Write error processing code here */
	} /* end if VALID_IDX(idx) */

	/* Put statements here #_TEFB_# */
  *desc = cSignatureInfo_refer_to_descriptor();
}

/* #[<ENTRY_FUNC>]# eEntryInfo_getArraySize
 * name:         eEntryInfo_getArraySize
 * global_name:  nTECSInfo_tEntryInfo_eEntryInfo_getArraySize
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
uint32_t
eEntryInfo_getArraySize(CELLIDX idx)
{
	CELLCB	*p_cellcb;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		/* Write error processing code here */
	} /* end if VALID_IDX(idx) */

	/* Put statements here #_TEFB_# */
  return ATTR_array_size;
}

/* #[<ENTRY_FUNC>]# eEntryInfo_isInline
 * name:         eEntryInfo_isInline
 * global_name:  nTECSInfo_tEntryInfo_eEntryInfo_isInline
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
bool_t
eEntryInfo_isInline(CELLIDX idx)
{
	CELLCB	*p_cellcb;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		/* Write error processing code here */
	} /* end if VALID_IDX(idx) */

	/* Put statements here #_TEFB_# */
  return ATTR_b_inline;
}

/* #[<POSTAMBLE>]#
 *   これより下に非受け口関数を書きます
 * #[</POSTAMBLE>]#*/
