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
 *  @(#) $Id: nTECSInfo_tCelltypeInfo.c 2663 2017-07-08 23:09:53Z okuma-top $
 */

/* #[<PREAMBLE>]#
 * #[<...>]# から #[</...>]# で囲まれたコメントは編集しないでください
 * tecsmerge によるマージに使用されます
 *
 * 属性アクセスマクロ #_CAAM_#
 * name             char_t*          ATTR_name       
 * b_singleton      bool_t           ATTR_b_singleton
 * b_IDX_is_ID_act  bool_t           ATTR_b_IDX_is_ID_act
 * sizeOfCB         uint32_t         ATTR_sizeOfCB   
 * sizeOfINIB       uint32_t         ATTR_sizeOfINIB 
 * n_cellInLinkUnit uint32_t         ATTR_n_cellInLinkUnit
 * n_cellInSystem   uint32_t         ATTR_n_cellInSystem
 *
 * 呼び口関数 #_TCPF_#
 * call port: cEntryInfo signature: nTECSInfo_sEntryInfo context:task optional:true
 *   bool_t     is_cEntryInfo_joined(int subscript)        check if joined
 *   ER             cEntryInfo_getName( subscript, char_t* name, int_t max_len );
 *   uint16_t       cEntryInfo_getNameLength( subscript );
 *   void           cEntryInfo_getSignatureInfo( subscript, Descriptor( nTECSInfo_sSignatureInfo )* desc );
 *   uint32_t       cEntryInfo_getArraySize( subscript );
 *   bool_t         cEntryInfo_isInline( subscript );
 *       subscript:  0...(NCP_cEntryInfo-1)
 *   [ref_desc]
 *      Descriptor( nTECSInfo_sEntryInfo ) cEntryInfo_refer_to_descriptor( int_t subscript );
 *      Descriptor( nTECSInfo_sEntryInfo ) cEntryInfo_ref_desc( int_t subscript )      (same as above; abbreviated version);
 * call port: cCallInfo signature: nTECSInfo_sCallInfo context:task optional:true
 *   bool_t     is_cCallInfo_joined(int subscript)        check if joined
 *   ER             cCallInfo_getName( subscript, char_t* name, int_t max_len );
 *   uint16_t       cCallInfo_getNameLength( subscript );
 *   void           cCallInfo_getSignatureInfo( subscript, Descriptor( nTECSInfo_sSignatureInfo )* desc );
 *   uint32_t       cCallInfo_getArraySize( subscript );
 *   void           cCallInfo_getSpecifierInfo( subscript, bool_t* b_optional, bool_t* b_dynamic, bool_t* b_ref_desc, bool_t* b_omit );
 *   void           cCallInfo_getInternalInfo( subscript, bool_t* b_allocator_port, bool_t* b_require_port );
 *   void           cCallInfo_getLocationInfo( subscript, uint32_t* offset, int8_t* place );
 *   void           cCallInfo_getOptimizeInfo( subscript, bool_t* b_VMT_useless, bool_t* b_skelton_useless, bool_t* b_cell_unique );
 *       subscript:  0...(NCP_cCallInfo-1)
 *   [ref_desc]
 *      Descriptor( nTECSInfo_sCallInfo ) cCallInfo_refer_to_descriptor( int_t subscript );
 *      Descriptor( nTECSInfo_sCallInfo ) cCallInfo_ref_desc( int_t subscript )      (same as above; abbreviated version);
 * call port: cAttrInfo signature: nTECSInfo_sVarDeclInfo context:task optional:true
 *   bool_t     is_cAttrInfo_joined(int subscript)        check if joined
 *   ER             cAttrInfo_getName( subscript, char_t* name, int_t max_len );
 *   uint16_t       cAttrInfo_getNameLength( subscript );
 *   uint32_t       cAttrInfo_getOffset( subscript );
 *   void           cAttrInfo_getTypeInfo( subscript, Descriptor( nTECSInfo_sTypeInfo )* desc );
 *   void           cAttrInfo_getSizeIsExpr( subscript, char_t* expr_str, int32_t max_len );
 *   ER             cAttrInfo_getSizeIs( subscript, uint32_t* size, const void* p_cb );
 *       subscript:  0...(NCP_cAttrInfo-1)
 *   [ref_desc]
 *      Descriptor( nTECSInfo_sVarDeclInfo ) cAttrInfo_refer_to_descriptor( int_t subscript );
 *      Descriptor( nTECSInfo_sVarDeclInfo ) cAttrInfo_ref_desc( int_t subscript )      (same as above; abbreviated version);
 * call port: cVarInfo signature: nTECSInfo_sVarDeclInfo context:task optional:true
 *   bool_t     is_cVarInfo_joined(int subscript)        check if joined
 *   ER             cVarInfo_getName( subscript, char_t* name, int_t max_len );
 *   uint16_t       cVarInfo_getNameLength( subscript );
 *   uint32_t       cVarInfo_getOffset( subscript );
 *   void           cVarInfo_getTypeInfo( subscript, Descriptor( nTECSInfo_sTypeInfo )* desc );
 *   void           cVarInfo_getSizeIsExpr( subscript, char_t* expr_str, int32_t max_len );
 *   ER             cVarInfo_getSizeIs( subscript, uint32_t* size, const void* p_cb );
 *       subscript:  0...(NCP_cVarInfo-1)
 *   [ref_desc]
 *      Descriptor( nTECSInfo_sVarDeclInfo ) cVarInfo_refer_to_descriptor( int_t subscript );
 *      Descriptor( nTECSInfo_sVarDeclInfo ) cVarInfo_ref_desc( int_t subscript )      (same as above; abbreviated version);
 *
 * #[</PREAMBLE>]# */

/* Put prototype declaration and/or variale definition here #_PAC_# */
#include <string.h>
#include "nTECSInfo_tCelltypeInfo_tecsgen.h"

#ifndef E_OK
#define	E_OK	0		/* success */
#define	E_ID	(-18)	/* illegal ID */
#endif

/* entry port function #_TEPF_# */
/* #[<ENTRY_PORT>]# eCelltypeInfo
 * entry port: eCelltypeInfo
 * signature:  nTECSInfo_sCelltypeInfo
 * context:    task
 * #[</ENTRY_PORT>]# */

/* #[<ENTRY_FUNC>]# eCelltypeInfo_getName
 * name:         eCelltypeInfo_getName
 * global_name:  nTECSInfo_tCelltypeInfo_eCelltypeInfo_getName
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
ER
eCelltypeInfo_getName(CELLIDX idx, char_t* name, int_t max_len)
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

/* #[<ENTRY_FUNC>]# eCelltypeInfo_getNameLength
 * name:         eCelltypeInfo_getNameLength
 * global_name:  nTECSInfo_tCelltypeInfo_eCelltypeInfo_getNameLength
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
uint16_t
eCelltypeInfo_getNameLength(CELLIDX idx)
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

/* #[<ENTRY_FUNC>]# eCelltypeInfo_getNAttr
 * name:         eCelltypeInfo_getNAttr
 * global_name:  nTECSInfo_tCelltypeInfo_eCelltypeInfo_getNAttr
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
uint32_t
eCelltypeInfo_getNAttr(CELLIDX idx)
{
    CELLCB	*p_cellcb;
    if (VALID_IDX(idx)) {
        p_cellcb = GET_CELLCB(idx);
    }
    else {
        /* Write error processing code here */
    } /* end if VALID_IDX(idx) */

    /* Put statements here #_TEFB_# */
    return NCP_cAttrInfo;
}

/* #[<ENTRY_FUNC>]# eCelltypeInfo_getAttrInfo
 * name:         eCelltypeInfo_getAttrInfo
 * global_name:  nTECSInfo_tCelltypeInfo_eCelltypeInfo_getAttrInfo
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
ER
eCelltypeInfo_getAttrInfo(CELLIDX idx, uint32_t ith, Descriptor( nTECSInfo_sVarDeclInfo )* desc)
{
    CELLCB	*p_cellcb;
    ER      ercd = E_OK;
    if (VALID_IDX(idx)) {
        p_cellcb = GET_CELLCB(idx);
    }
    else {
        /* Write error processing code here */
    } /* end if VALID_IDX(idx) */

    /* Put statements here #_TEFB_# */
    if( 0 <= ith && ith < NCP_cAttrInfo ){
        if( is_cAttrInfo_joined( ith ) )
            *desc = cAttrInfo_refer_to_descriptor( ith );
        else
            ercd = E_NOEXS;
    }
    else
        ercd = E_ID;
    return ercd;
}

/* #[<ENTRY_FUNC>]# eCelltypeInfo_getNVar
 * name:         eCelltypeInfo_getNVar
 * global_name:  nTECSInfo_tCelltypeInfo_eCelltypeInfo_getNVar
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
uint32_t
eCelltypeInfo_getNVar(CELLIDX idx)
{
    CELLCB	*p_cellcb;
    if (VALID_IDX(idx)) {
        p_cellcb = GET_CELLCB(idx);
    }
    else {
        /* Write error processing code here */
    } /* end if VALID_IDX(idx) */

    /* Put statements here #_TEFB_# */
    return NCP_cVarInfo;
}

/* #[<ENTRY_FUNC>]# eCelltypeInfo_getVarInfo
 * name:         eCelltypeInfo_getVarInfo
 * global_name:  nTECSInfo_tCelltypeInfo_eCelltypeInfo_getVarInfo
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
ER
eCelltypeInfo_getVarInfo(CELLIDX idx, uint32_t ith, Descriptor( nTECSInfo_sVarDeclInfo )* desc)
{
    CELLCB	*p_cellcb;
    ER      ercd = E_OK;
    if (VALID_IDX(idx)) {
        p_cellcb = GET_CELLCB(idx);
    }
    else {
        /* Write error processing code here */
    } /* end if VALID_IDX(idx) */

    /* Put statements here #_TEFB_# */
    if( 0 <= ith &&  ith < NCP_cVarInfo ){
        if( is_cVarInfo_joined( ith ) )
            *desc = cVarInfo_refer_to_descriptor( ith );
        else
            ercd = E_NOEXS;
    }
    else
        ercd = E_ID;
    return ercd;
}

/* #[<ENTRY_FUNC>]# eCelltypeInfo_getNCall
 * name:         eCelltypeInfo_getNCall
 * global_name:  nTECSInfo_tCelltypeInfo_eCelltypeInfo_getNCall
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
uint32_t
eCelltypeInfo_getNCall(CELLIDX idx)
{
    CELLCB	*p_cellcb;
    if (VALID_IDX(idx)) {
        p_cellcb = GET_CELLCB(idx);
    }
    else {
        /* Write error processing code here */
    } /* end if VALID_IDX(idx) */

    /* Put statements here #_TEFB_# */
    return NCP_cCallInfo;
}

/* #[<ENTRY_FUNC>]# eCelltypeInfo_getCallInfo
 * name:         eCelltypeInfo_getCallInfo
 * global_name:  nTECSInfo_tCelltypeInfo_eCelltypeInfo_getCallInfo
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
ER
eCelltypeInfo_getCallInfo(CELLIDX idx, uint32_t ith, Descriptor( nTECSInfo_sCallInfo )* desc)
{
    CELLCB	*p_cellcb;
    ER      ercd = E_OK;
    if (VALID_IDX(idx)) {
        p_cellcb = GET_CELLCB(idx);
    }
    else {
        /* Write error processing code here */
    } /* end if VALID_IDX(idx) */

    /* Put statements here #_TEFB_# */
    if( 0 <= ith && ith < NCP_cCallInfo ){
        if( is_cCallInfo_joined( ith ) )
            *desc = cCallInfo_refer_to_descriptor( ith );
        else
            ercd = E_NOEXS;
    }
    else
        ercd = E_ID;

    return ercd;
}

/* #[<ENTRY_FUNC>]# eCelltypeInfo_getNEntry
 * name:         eCelltypeInfo_getNEntry
 * global_name:  nTECSInfo_tCelltypeInfo_eCelltypeInfo_getNEntry
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
uint32_t
eCelltypeInfo_getNEntry(CELLIDX idx)
{
    CELLCB	*p_cellcb;
    if (VALID_IDX(idx)) {
        p_cellcb = GET_CELLCB(idx);
    }
    else {
        /* Write error processing code here */
    } /* end if VALID_IDX(idx) */

    /* Put statements here #_TEFB_# */
    return NCP_cEntryInfo;
}

/* #[<ENTRY_FUNC>]# eCelltypeInfo_getEntryInfo
 * name:         eCelltypeInfo_getEntryInfo
 * global_name:  nTECSInfo_tCelltypeInfo_eCelltypeInfo_getEntryInfo
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
ER
eCelltypeInfo_getEntryInfo(CELLIDX idx, uint32_t ith, Descriptor( nTECSInfo_sEntryInfo )* desc)
{
    CELLCB	*p_cellcb;
    ER      ercd = E_OK;
    if (VALID_IDX(idx)) {
        p_cellcb = GET_CELLCB(idx);
    }
    else {
        /* Write error processing code here */
    } /* end if VALID_IDX(idx) */

    /* Put statements here #_TEFB_# */
    if( 0 <= ith && ith < NCP_cEntryInfo ){
        if( is_cEntryInfo_joined( ith ) )
            *desc = cEntryInfo_refer_to_descriptor( ith );
        else
            ercd = E_NOEXS;
    }
    else
        ercd = E_ID;

    return ercd;
}

/* #[<ENTRY_FUNC>]# eCelltypeInfo_isSingleton
 * name:         eCelltypeInfo_isSingleton
 * global_name:  nTECSInfo_tCelltypeInfo_eCelltypeInfo_isSingleton
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
bool_t
eCelltypeInfo_isSingleton(CELLIDX idx)
{
    CELLCB	*p_cellcb;
    if (VALID_IDX(idx)) {
        p_cellcb = GET_CELLCB(idx);
    }
    else {
        /* Write error processing code here */
    } /* end if VALID_IDX(idx) */

    /* Put statements here #_TEFB_# */
    return ATTR_b_singleton;
}

/* #[<ENTRY_FUNC>]# eCelltypeInfo_isIDX_is_ID
 * name:         eCelltypeInfo_isIDX_is_ID
 * global_name:  nTECSInfo_tCelltypeInfo_eCelltypeInfo_isIDX_is_ID
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
bool_t
eCelltypeInfo_isIDX_is_ID(CELLIDX idx)
{
    CELLCB	*p_cellcb;
    if (VALID_IDX(idx)) {
        p_cellcb = GET_CELLCB(idx);
    }
    else {
        /* Write error processing code here */
    } /* end if VALID_IDX(idx) */

    /* Put statements here #_TEFB_# */
    return ATTR_b_IDX_is_ID_act;
}

/* #[<ENTRY_FUNC>]# eCelltypeInfo_sizeOfCB
 * name:         eCelltypeInfo_sizeOfCB
 * global_name:  nTECSInfo_tCelltypeInfo_eCelltypeInfo_sizeOfCB
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
uint32_t
eCelltypeInfo_sizeOfCB(CELLIDX idx)
{
    CELLCB	*p_cellcb;
    if (VALID_IDX(idx)) {
        p_cellcb = GET_CELLCB(idx);
    }
    else {
        /* エラー処理コードをここに記述します */
    } /* end if VALID_IDX(idx) */

    /* ここに処理本体を記述します #_TEFB_# */
    return ATTR_sizeOfCB;
}

/* #[<ENTRY_FUNC>]# eCelltypeInfo_sizeOfINIB
 * name:         eCelltypeInfo_sizeOfINIB
 * global_name:  nTECSInfo_tCelltypeInfo_eCelltypeInfo_sizeOfINIB
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
uint32_t
eCelltypeInfo_sizeOfINIB(CELLIDX idx)
{
    CELLCB	*p_cellcb;
    if (VALID_IDX(idx)) {
        p_cellcb = GET_CELLCB(idx);
    }
    else {
        /* エラー処理コードをここに記述します */
    } /* end if VALID_IDX(idx) */

    /* ここに処理本体を記述します #_TEFB_# */
    return ATTR_sizeOfINIB;
}

/* #[<POSTAMBLE>]#
 *   これより下に非受け口関数を書きます
 * #[</POSTAMBLE>]#*/
