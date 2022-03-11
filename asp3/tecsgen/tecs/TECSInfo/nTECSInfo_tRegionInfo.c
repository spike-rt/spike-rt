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
 *  @(#) $Id: nTECSInfo_tRegionInfo.c 2656 2017-06-24 11:57:31Z okuma-top $
 */

/* #[<PREAMBLE>]#
 * #[<...>]# から #[</...>]# で囲まれたコメントは編集しないでください
 * tecsmerge によるマージに使用されます
 *
 * 属性アクセスマクロ #_CAAM_#
 * name             char*            ATTR_name       
 *
 * 呼び口関数 #_TCPF_#
 * call port: cCellInfo signature: nTECSInfo_sCellInfo context:task optional:true
 *   bool_t     is_cCellInfo_joined(int subscript)        check if joined
 *   ER             cCellInfo_getName( subscript, char_t* name, int_t max_len );
 *   uint16_t       cCellInfo_getNameLength( subscript );
 *   void           cCellInfo_getCelltypeInfo( subscript, Descriptor( nTECSInfo_sCelltypeInfo )* desc );
 *   void           cCellInfo_getCBP( subscript, void** cbp );
 *   void           cCellInfo_getINIBP( subscript, void** inibp );
 *       subscript:  0...(NCP_cCellInfo-1)
 *   [ref_desc]
 *      Descriptor( nTECSInfo_sCellInfo ) cCellInfo_refer_to_descriptor( int_t subscript );
 *      Descriptor( nTECSInfo_sCellInfo ) cCellInfo_ref_desc( int_t subscript )      (same as above; abbreviated version);
 * call port: cRegionInfo signature: nTECSInfo_sRegionInfo context:task optional:true
 *   bool_t     is_cRegionInfo_joined(int subscript)        check if joined
 *   ER             cRegionInfo_getName( subscript, char_t* name, int_t max_len );
 *   uint16_t       cRegionInfo_getNameLength( subscript );
 *   uint32_t       cRegionInfo_getNCell( subscript );
 *   ER             cRegionInfo_getCellInfo( subscript, uint32_t ith, Descriptor( nTECSInfo_sCellInfo )* des );
 *   uint32_t       cRegionInfo_getNRegion( subscript );
 *   ER             cRegionInfo_getRegionInfo( subscript, uint32_t ith, Descriptor( nTECSInfo_sRegionInfo )* des );
 *       subscript:  0...(NCP_cRegionInfo-1)
 *   [ref_desc]
 *      Descriptor( nTECSInfo_sRegionInfo ) cRegionInfo_refer_to_descriptor( int_t subscript );
 *      Descriptor( nTECSInfo_sRegionInfo ) cRegionInfo_ref_desc( int_t subscript )      (same as above; abbreviated version);
 *
 * #[</PREAMBLE>]# */

/* Put prototype declaration and/or variale definition here #_PAC_# */
#include <string.h>
#include "nTECSInfo_tRegionInfo_tecsgen.h"

#ifndef E_OK
#define	E_OK	0		/* success */
#define	E_ID	(-18)	/* illegal ID */
#endif

/* entry port function #_TEPF_# */
/* #[<ENTRY_PORT>]# eRegionInfo
 * entry port: eRegionInfo
 * signature:  nTECSInfo_sRegionInfo
 * context:    task
 * #[</ENTRY_PORT>]# */

/* #[<ENTRY_FUNC>]# eRegionInfo_getName
 * name:         eRegionInfo_getName
 * global_name:  nTECSInfo_tRegionInfo_eRegionInfo_getName
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
ER
eRegionInfo_getName(CELLIDX idx, char_t* name, int_t max_len)
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

/* #[<ENTRY_FUNC>]# eRegionInfo_getNameLength
 * name:         eRegionInfo_getNameLength
 * global_name:  nTECSInfo_tRegionInfo_eRegionInfo_getNameLength
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
uint16_t
eRegionInfo_getNameLength(CELLIDX idx)
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

/* #[<ENTRY_FUNC>]# eRegionInfo_getNCell
 * name:         eRegionInfo_getNCell
 * global_name:  nTECSInfo_tRegionInfo_eRegionInfo_getNCell
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
uint32_t
eRegionInfo_getNCell(CELLIDX idx)
{
	CELLCB	*p_cellcb;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		/* Write error processing code here */
	} /* end if VALID_IDX(idx) */

	/* Put statements here #_TEFB_# */
  return NCP_cCellInfo;
}

/* #[<ENTRY_FUNC>]# eRegionInfo_getCellInfo
 * name:         eRegionInfo_getCellInfo
 * global_name:  nTECSInfo_tRegionInfo_eRegionInfo_getCellInfo
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
ER
eRegionInfo_getCellInfo(CELLIDX idx, uint32_t ith, Descriptor( nTECSInfo_sCellInfo )* des)
{
	CELLCB	*p_cellcb;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		/* Write error processing code here */
	} /* end if VALID_IDX(idx) */

	/* Put statements here #_TEFB_# */
  *des = cCellInfo_refer_to_descriptor( ith );
  return E_OK;
}

/* #[<ENTRY_FUNC>]# eRegionInfo_getNRegion
 * name:         eRegionInfo_getNRegion
 * global_name:  nTECSInfo_tRegionInfo_eRegionInfo_getNRegion
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
uint32_t
eRegionInfo_getNRegion(CELLIDX idx)
{
	CELLCB	*p_cellcb;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		/* Write error processing code here */
	} /* end if VALID_IDX(idx) */

	/* Put statements here #_TEFB_# */
  return NCP_cRegionInfo;
}

/* #[<ENTRY_FUNC>]# eRegionInfo_getRegionInfo
 * name:         eRegionInfo_getRegionInfo
 * global_name:  nTECSInfo_tRegionInfo_eRegionInfo_getRegionInfo
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
ER
eRegionInfo_getRegionInfo(CELLIDX idx, uint32_t ith, Descriptor( nTECSInfo_sRegionInfo )* des)
{
	CELLCB	*p_cellcb;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		/* Write error processing code here */
	} /* end if VALID_IDX(idx) */

	/* Put statements here #_TEFB_# */
  *des = cRegionInfo_refer_to_descriptor( ith );
  return E_OK;
}

/* #[<POSTAMBLE>]#
 *   これより下に非受け口関数を書きます
 * #[</POSTAMBLE>]#*/
