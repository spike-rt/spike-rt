/*
 *  TOPPERS Software
 *      Toyohashi Open Platform for Embedded Real-Time Systems
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2004-2022 by Embedded and Real-Time Systems Laboratory
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
 * 
 *  $Id: log_output.c 1695 2022-10-16 06:25:10Z ertl-hiro $
 */

/*
 *		システムログのフォーマット出力
 */

#include <t_stddef.h>
#include <t_syslog.h>
#include <log_output.h>

/*
 *  数値を文字列に変換
 */
#define CONVERT_BUFLEN	((sizeof(ULOGPAR) * CHAR_BIT + 2) / 3)
										/* ULOGPAR型の数値の最大文字数 */
static void
convert(ULOGPAR val, uint_t radix, const char *radchar,
			uint_t width, bool_t minus, bool_t padzero, void (*putc)(char))
{
	char	buf[CONVERT_BUFLEN];
	uint_t	i, j;

	i = 0U;
	do {
		buf[i++] = radchar[val % radix];
		val /= radix;
	} while (i < CONVERT_BUFLEN && val != 0);

	if (minus && width > 0) {
		width -= 1;
	}
	if (minus && padzero) {
		(*putc)('-');
	}
	for (j = i; j < width; j++) {
		(*putc)(padzero ? '0' : ' ');
	}
	if (minus && !padzero) {
		(*putc)('-');
	}
	while (i > 0U) {
		(*putc)(buf[--i]);
	}
}

/*
 *  文字列整形出力
 */
static const char raddec[] = "0123456789";
static const char radhex[] = "0123456789abcdef";
static const char radHEX[] = "0123456789ABCDEF";

void
syslog_printf(const char *format, const LOGPAR args[], void (*putc)(char))
{
	char		c;
	uint_t		width;
	bool_t		padzero;
	LOGPAR		val;
	const char	*str;
	uint_t		i;

	i = 0U;
	while ((c = *format++) != '\0') {
		if (c != '%' || i >= TNUM_LOGPAR - 1) {
			(*putc)(c);
			continue;
		}

		width = 0U;
		padzero = false;
		if ((c = *format++) == '0') {
			padzero = true;
			c = *format++;
		}
		while ('0' <= c && c <= '9') {
			width = width * 10U + c - '0';
			c = *format++;
		}
		if (c == 'l' || c == 't' || c == 'T') {
			c = *format++;
		}
		switch (c) {
		case 'd':
			val = args[i++];
			if (val >= 0) {
				convert((ULOGPAR) val, 10U, raddec,
										width, false, padzero, putc);
			}
			else {
				convert((ULOGPAR)(-val), 10U, raddec,
										width, true, padzero, putc);
			}
			break;
		case 'u':
			convert((ULOGPAR) args[i++], 10U, raddec,
										width, false, padzero, putc);
			break;
		case 'x':
		case 'p':
			convert((ULOGPAR) args[i++], 16U, radhex,
										width, false, padzero, putc);
			break;
		case 'X':
			convert((ULOGPAR) args[i++], 16U, radHEX,
										width, false, padzero, putc);
			break;
		case 'c':
			(*putc)((char) args[i++]);
			break;
		case 's':
			str = (const char *)(args[i++]);
			while ((c = *str++) != '\0') {
				(*putc)(c);
			}
			break;
		case '%':
			(*putc)('%');
			break;
		case '\0':
			format--;
			break;
		default:
			break;
		}
	}
}

/*
 *  ログ情報の出力
 */
void
syslog_print(const SYSLOG *p_syslog, void (*putc)(char))
{
	switch (p_syslog->logtype) {
	case LOG_TYPE_COMMENT:
		syslog_printf((const char *)(p_syslog->logpar[0]),
								&(p_syslog->logpar[1]), putc);
		(*putc)('\n');
		break;
	case LOG_TYPE_ASSERT:
		syslog_printf("%s:%u: Assertion `%s' failed.\007",
								&(p_syslog->logpar[0]), putc);
		(*putc)('\n');
		break;
	default:
		/*
		 *  他の種別のログ情報には対応していない．
		 */
		break;
	}
}

/*
 *  ログ情報喪失メッセージの出力
 */
void
syslog_lostmsg(uint_t lostlog, void (*putc)(char))
{
	LOGPAR	lostinfo[1];

	lostinfo[0] = (LOGPAR) lostlog;
	syslog_printf("%d messages are lost.", lostinfo, putc);
	(*putc)('\n');
}
