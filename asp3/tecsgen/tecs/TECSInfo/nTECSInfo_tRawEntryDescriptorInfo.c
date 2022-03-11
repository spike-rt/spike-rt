/* #[<PREAMBLE>]#
 * #[<...>]# から #[</...>]# で囲まれたコメントは編集しないでください
 * tecsmerge によるマージに使用されます
 *
 * 属性アクセスマクロ #_CAAM_#
 * size             int_t            ATTR_size       
 * rawEntryDescriptor void**           ATTR_rawEntryDescriptor
 *
 * #[</PREAMBLE>]# */

/* プロトタイプ宣言や変数の定義をここに書きます #_PAC_# */
#include "nTECSInfo_tRawEntryDescriptorInfo_tecsgen.h"

#ifndef E_OK
#define	E_OK	0		/* success */
#define	E_ID	(-18)	/* illegal ID */
#endif

/* 受け口関数 #_TEPF_# */
/* #[<ENTRY_PORT>]# eRawEntryDescriptor
 * entry port: eRawEntryDescriptor
 * signature:  nTECSInfo_sRawEntryDescriptorInfo
 * context:    task
 * #[</ENTRY_PORT>]# */

/* #[<ENTRY_FUNC>]# eRawEntryDescriptor_getNRawEntryDescriptorInfo
 * name:         eRawEntryDescriptor_getNRawEntryDescriptorInfo
 * global_name:  nTECSInfo_tRawEntryDescriptorInfo_eRawEntryDescriptor_getNRawEntryDescriptorInfo
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
uint16_t
eRawEntryDescriptor_getNRawEntryDescriptorInfo(CELLIDX idx)
{
    CELLCB	*p_cellcb;
    if (VALID_IDX(idx)) {
        p_cellcb = GET_CELLCB(idx);
    }
    else {
        /* エラー処理コードをここに記述します */
    } /* end if VALID_IDX(idx) */

    /* ここに処理本体を記述します #_TEFB_# */
    return ATTR_size;
}

/* #[<ENTRY_FUNC>]# eRawEntryDescriptor_getRawDescriptor
 * name:         eRawEntryDescriptor_getRawDescriptor
 * global_name:  nTECSInfo_tRawEntryDescriptorInfo_eRawEntryDescriptor_getRawDescriptor
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
ER
eRawEntryDescriptor_getRawDescriptor(CELLIDX idx, int_t ith, void** rawDesc)
{
	CELLCB	*p_cellcb;
  ER      ercd = E_OK;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		/* エラー処理コードをここに記述します */
	} /* end if VALID_IDX(idx) */

	/* ここに処理本体を記述します #_TEFB_# */
  if( 0 <= ith && ith < ATTR_size ){
      *rawDesc = (void *)ATTR_rawEntryDescriptor[ ith ];
  }
  else {
      ercd = E_ID;
  }
  return ercd;
}

/* #[<POSTAMBLE>]#
 *   これより下に非受け口関数を書きます
 * #[</POSTAMBLE>]#*/
