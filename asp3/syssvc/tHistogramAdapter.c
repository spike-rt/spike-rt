/*
 *  TOPPERS Software
 *      Toyohashi Open Platform for Embedded Real-Time Systems
 * 
 *  Copyright (C) 2016-2018 by Embedded and Real-Time Systems Laboratory
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
 *  $Id: tHistogramAdapter.c 958 2018-04-28 13:09:47Z ertl-hiro $
 */

/*
 *		C言語で記述されたアプリケーションから，TECSベースの実行時間分布
 *		集計サービスを呼び出すためのアダプタ
 */

#include "tHistogramAdapter_tecsgen.h"
#include "histogram.h"

/*
 *  実行時間分布計測IDの範囲の判定
 */
#define VALID_HISTID(histid)	(1 <= histid && histid <= N_CP_cHistogram)

/*
 *  実行時間分布計測の初期化（サービスコール）
 */
ER
init_hist(ID histid)
{
	ER		ercd;

	if (!VALID_HISTID(histid)) {
		ercd = E_ID;
	}
	else {
		ercd = cHistogram_initialize(histid - 1);
	}
	return(ercd);
}

/*
 *  実行時間計測の開始
 */
ER
begin_measure(ID histid)
{
	ER		ercd;

	if (!VALID_HISTID(histid)) {
		ercd = E_ID;
	}
	else {
		ercd = cHistogram_beginMeasure(histid - 1);
	}
	return(ercd);
}

/*
 *  実行時間計測の終了
 */
ER
end_measure(ID histid)
{
	ER		ercd;

	if (!VALID_HISTID(histid)) {
		ercd = E_ID;
	}
	else {
		ercd = cHistogram_endMeasure(histid - 1);
	}
	return(ercd);
}

/*
 *  実行時間分布計測の表示
 */
ER
print_hist(ID histid)
{
	ER		ercd;

	if (!VALID_HISTID(histid)) {
		ercd = E_ID;
	}
	else {
		ercd = cHistogram_print(histid - 1);
	}
	return(ercd);
}
