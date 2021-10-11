#ifndef tMessageBufferCEP__INLINE_H
#define tMessageBufferCEP__INLINE_H

/* #[<PREAMBLE>]#
 * #[<...>]# から #[</...>]# で囲まれたコメントは編集しないでください
 * tecsmerge によるマージに使用されます
 *
 * 呼び口関数 #_TCPF_#
 * call port: cMessageBuffer0 signature: sMessageBuffer context:task
 *   ER             cMessageBuffer0_send( const uint8_t* message, uint32_t size );
 *   ER             cMessageBuffer0_sendPolling( const uint8_t* message, uint32_t size );
 *   ER             cMessageBuffer0_sendTimeout( const uint8_t* message, uint32_t size, TMO timeout );
 *   ER_UINT        cMessageBuffer0_receive( uint8_t* message );
 *   ER_UINT        cMessageBuffer0_receivePolling( uint8_t* message );
 *   ER_UINT        cMessageBuffer0_receiveTimeout( uint8_t* message, TMO timeout );
 *   ER             cMessageBuffer0_initialize( );
 *   ER             cMessageBuffer0_refer( T_RMBF* pk_messageBufferStatus );
 * call port: cMessageBuffer1 signature: sMessageBuffer context:task
 *   ER             cMessageBuffer1_send( const uint8_t* message, uint32_t size );
 *   ER             cMessageBuffer1_sendPolling( const uint8_t* message, uint32_t size );
 *   ER             cMessageBuffer1_sendTimeout( const uint8_t* message, uint32_t size, TMO timeout );
 *   ER_UINT        cMessageBuffer1_receive( uint8_t* message );
 *   ER_UINT        cMessageBuffer1_receivePolling( uint8_t* message );
 *   ER_UINT        cMessageBuffer1_receiveTimeout( uint8_t* message, TMO timeout );
 *   ER             cMessageBuffer1_initialize( );
 *   ER             cMessageBuffer1_refer( T_RMBF* pk_messageBufferStatus );
 *
 * #[</PREAMBLE>]# */

/* 受け口関数 #_TEPF_# */
/* #[<ENTRY_PORT>]# eChannel
 * entry port: eChannel
 * signature:  sChannel
2 * context:    task
 * #[</ENTRY_PORT>]# */

/* #[<ENTRY_FUNC>]# eChannel_send
 * name:         eChannel_send
 * global_name:  tMessageBufferCEP_eChannel_send
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
Inline ER
eChannel_send(CELLIDX idx, const int8_t* buf, int16_t size, TMO tmo)
{
	ER		ercd = E_OK;
  int16_t write_count;
    CELLCB	*p_cellcb;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		return(E_ID);
	} /* end if VALID_IDX(idx) */
  (void)p_cellcb;   // to avoid unused warning

	/* ここに処理本体を記述します #_TEFB_# */
  if( size < 0 )
      return E_PAR;
  else if( size == 0 )
      return E_OK;

  write_count = 0;
  while( write_count < size ){
      VAR_write_buf[ VAR_write_count++ ] = buf[ write_count++ ];
      if( VAR_write_count >= ATTR_buf_size ){
          ercd = eChannel_flush( idx, tmo);
          if( ercd != E_OK ){
              break;
          }
      }
  }

  // syslog( LOG_INFO, "eChannel_send ercd=%d", ercd );
	return(ercd);
}

/* #[<ENTRY_FUNC>]# eChannel_flush
 * name:         eChannel_flush
 * global_name:  tMessageBufferCEP_eChannel_flush
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
Inline ER
eChannel_flush(CELLIDX idx, TMO tmo)
{
	ER		ercd = E_OK;
	CELLCB	*p_cellcb;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		return(E_ID);
	} /* end if VALID_IDX(idx) */
  (void)p_cellcb;   // to avoid unused warning

	/* ここに処理本体を記述します #_TEFB_# */
  /* flush しなくても、いつも送られる */
  if( VAR_write_count > 0 ){
      ercd = cMessageBuffer0_sendTimeout( (const uint8_t *)VAR_write_buf, VAR_write_count, tmo );
  }
  // syslog( LOG_INFO, "eChannel_flush ercd=%d count=%d", ercd, VAR_write_count );
  VAR_write_count = 0;
	return(ercd);
}

/* #[<ENTRY_FUNC>]# eChannel_receive
 * name:         eChannel_receive
 * global_name:  tMessageBufferCEP_eChannel_receive
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
Inline ER_UINT
eChannel_receive(CELLIDX idx, int8_t* buf, int16_t size, TMO tmo)
{
	ER		   ercd = E_OK;
	CELLCB	 *p_cellcb;
  int16_t  count = 0;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		return(E_ID);
	} /* end if VALID_IDX(idx) */
  (void)p_cellcb;   // to avoid unused warning

	/* ここに処理本体を記述します #_TEFB_# */
  if( size == 0 )
      return 0;

  while( count < size ){
      if( VAR_n_read_buf_filled == 0 ){
          ercd = cMessageBuffer1_receiveTimeout( (uint8_t *)VAR_read_buf, tmo );
          if( ercd < 0  )
              break;
          VAR_n_read_buf_filled = ercd;
      }
      while( count < size && VAR_read_count < VAR_n_read_buf_filled ){
          buf[ count ] = VAR_read_buf[ VAR_read_count ];
          count++;
          VAR_read_count++;
      }
      if( VAR_read_count >= VAR_n_read_buf_filled ){
          VAR_n_read_buf_filled = 0;
          VAR_read_count = 0;
      }
      ercd = count;
  }
  // syslog( LOG_INFO, "eChannel_receive ercd=%d", ercd );

	return(ercd);
}

/* #[<ENTRY_FUNC>]# eChannel_end_receive
 * name:         eChannel_end_receive
 * global_name:  tMessageBufferCEP_eChannel_end_receive
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
Inline ER
eChannel_end_receive(CELLIDX idx)
{
	ER		ercd = E_OK;
	CELLCB	*p_cellcb;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		return(E_ID);
	} /* end if VALID_IDX(idx) */
  (void)p_cellcb;   // to avoid unused warning

	/* ここに処理本体を記述します #_TEFB_# */

	return(ercd);
}

/* #[<ENTRY_FUNC>]# eChannel_reset
 * name:         eChannel_reset
 * global_name:  tMessageBufferCEP_eChannel_reset
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
Inline ER
eChannel_reset(CELLIDX idx)
{
  ER		ercd0 = E_OK, ercd1 = E_OK;
	CELLCB	*p_cellcb;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		return(E_ID);
	} /* end if VALID_IDX(idx) */
  (void)p_cellcb;   // to avoid unused warning

	/* ここに処理本体を記述します #_TEFB_# */
  ercd0 = cMessageBuffer0_initialize( );
  ercd1 = cMessageBuffer1_initialize( );

	return(ercd1 != E_OK ? ercd1 : ercd0);
}

/* #[<POSTAMBLE>]#
 *   これより下に非受け口関数を書きます
 * #[</POSTAMBLE>]#*/

#endif /* tMessageBufferCEP_INLINEH */
