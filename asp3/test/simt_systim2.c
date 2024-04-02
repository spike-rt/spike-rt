/*
 *  TOPPERS Software
 *      Toyohashi Open Platform for Embedded Real-Time Systems
 * 
 *  Copyright (C) 2014-2018 by Embedded and Real-Time Systems Laboratory
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
 *  $Id: simt_systim2.c 1439 2020-05-22 20:02:23Z ertl-hiro $
 */

/* 
 *		システム時刻管理機能のテスト(2)
 *
 * 【テストの目的】
 *
 *	システム時刻管理機能の標準のサービスコール（set_tim，get_tim，
 *	adj_tim）を，要求ベースでテストする．
 *
 *	テスト対象のサービスコール処理関数およびそこから呼び出される以下の
 *	関数のC1カバレッジを達成する．
 *		set_tim … 関数内に分岐がない
 *		get_tim … 関数内に分岐がない
 *		adj_tim
 *		check_adjtim
 *		update_current_evttim（TCYC_HRTCNTが定義されていない場合）
 *
 * 【テスト項目】
 *
 *  (A) set_timの要求ベーステスト
 *	  (A-1) 非タスクコンテキストからの呼出し［NGKI3564］
 *	  (A-2) CPUロック状態からの呼出し［NGKI3565］
 *	  (A-3) システム時刻の現在値が正しく設定されること［NGKI3567］
 *  (B) get_timの要求ベーステスト
 *	  (B-1) 非タスクコンテキストからの呼出し［NGKI2350］
 *	  (B-2) CPUロック状態からの呼出し［NGKI2351］
 *	  (B-3) システム時刻の現在値が正しく参照されること［NGKI2354］
 *	  (B-4) adj_timによってシステム時刻を戻した場合，システム時刻が最も
 *			進んでいた時のシステム時刻を返すこと［NGKI3591］
 *	  (B-5) 参照するシステム時刻の進みが止まっている間に，set_timによ
 *			りシステム時刻を設定した場合でも，参照するシステム時刻の進
 *			みが止まっている時間は変化しないこと［NGKI3592］
 *  (C) adj_timの要求ベーステスト
 *	  (C-1) CPUロック状態からの呼出し［NGKI3583］
 *	  (C-2) adjtimが小さすぎる［NGKI3584］
 *	  (C-3) adjtimが大きすぎる［NGKI3584］
 *	  (C-4) システム時刻にadjtimが加えられること［NGKI3586］
 *	  (C-5) システム時刻の経過をきっかけに発生するタイムイベントが発生
 *			するまでの相対時間も調整されること［NGKI3587］
 *	  (C-6) (adj_tim > 0)で，1秒以上過去の発生時刻を持つタイムイベント
 *			が残っている場合に，E_OBJエラーとなること［NGKI3588］
 *	  (C-7) (adj_tim < 0)で，現在のシステム時刻が，システム時刻が最も進
 *			んでいた時のシステム時刻より1秒以上戻っている場合に，E_OBJ
 *			エラーとなること［NGKI3589］
 *	  (C-8) 非タスクコンテキストからの呼出しで，システム時刻にadjtimが
 *			加えられること
 *  (D) adj_timの実行/分岐パスの網羅
 *	  (D-1) (adjtim <= 0)の場合
 *	  (D-2) (adjtim > 0)で，最も進んでいた時のイベント時刻の更新が必要
 *			ない場合
 *	  (D-3) (adjtim > 0)で，最も進んでいた時のイベント時刻の更新が必要
 *			で，システム時刻のオフセットを進める必要がない場合
 *	  (D-4) (adjtim > 0)で，最も進んでいた時のイベント時刻の更新が必要
 *			で，システム時刻のオフセットを進める必要がある場合
 *  (E) check_adjtimの実行/分岐パス/戻り値の網羅
 *	  (E-1) (adjtim > 0)で，タイムイベントが登録されていない場合
 *	  (E-2) (adjtim > 0)で，先頭のタイムイベントの発生時刻が1秒前以降で
 *			ある場合
 *	  (E-3) (adjtim > 0)で，先頭のタイムイベントの発生時刻が1秒前以前で
 *			ある場合（adj_timがE_OBJエラーとなる）
 *	  (E-4) (adjtim < 0)で，現在のシステム時刻が，システム時刻が最も進
 *			んでいた時のシステム時刻より1秒以上戻っていない場合
 *	  (E-5) (adjtim < 0)で，現在のシステム時刻が，システム時刻が最も進
 *			んでいた時のシステム時刻より1秒以上戻っている場合（adj_tim
 *			がE_OBJエラーとなる）
 *	  (E-6) (adjtim == 0)の場合
 *  (F) update_current_evttimの実行/分岐パスの網羅
 *	  (F-1) 最も進んでいた時のイベント時刻の更新が必要ない場合
 *	  (F-2) 最も進んでいた時のイベント時刻の更新が必要で，システム時刻
 *			のオフセットを進める必要がない場合
 *	  (F-3) 最も進んでいた時のイベント時刻の更新が必要で，システム時刻
 *			のオフセットを進める必要がある場合
 *
 * 【使用リソース】
 *
 *	高分解能タイマモジュールの性質：HRT_CONFIG1
 *		TCYC_HRTCNT		未定義（2^32の意味）
 *		TSTEP_HRTCNT	1U
 *		HRTCNT_BOUND	4000000002U
 *
 *	タイマドライバシミュレータのパラメータ
 *		SIMTIM_INIT_CURRENT		10
 *		SIMTIM_OVERHEAD_HRTINT	10
 *
 *	TASK1: 中優先度タスク，メインタスク，最初から起動
 *	ALM1:  アラームハンドラ
 *	ALM2:  アラームハンドラ
 *	ALM3:  アラームハンドラ
 *
 * 【補足説明】
 *
 *	タイムイベントが登録されていない時に高分解能タイマに設定する相対は，
 *	ドリフト調整機能を持たない場合はHRTCNT_BOUNDであるのに対して，ドリ
 *	フト調整機能を持つ場合はTMAX_RELTIMをイベント時刻の伸縮率で割った値
 *	とHRTCNT_BOUNDの小さい方の値（このテストでは，ドリフト量を設定せず，
 *	HRTCNT_BOUND＞TMAX_RELTIMであるため，TMAX_RELTIMに一致）となる．そ
 *	こで，HRTCNT_EMPTYをこの値に定義し，ドリフト調整機能の有無によらず
 *	同じテストシーケンスが使えるようにする．
 *
 *	以下のテストシーケンスのコメント中で，「時刻：yyy」とは，高分解能
 *	タイマのカウント値がyyyになっていることを意味する．また，「発生：
 *	xxx」とは，高分解能タイマのカウント値がxxxになった時にタイムイベン
 *	トが発生することを意味する．タイムイベントのイベント発生時刻ではな
 *	いことに注意せよ．
 *
 * 【テストシーケンス】
 *
 *	== START ==
 *	// カーネル起動．高分解能タイマのカウント値とイベント時刻は10ずれる
 *	1:		[hook_hrt_set_event <- HRTCNT_EMPTY]
 *	== TASK1（優先度：中）==
 *	// CPUロック状態で各サービスコールを呼び出す
 *	2:	assert(fch_hrt() == 10U)							// 時刻：10
 *	3:	loc_cpu()
 *		set_tim(1000000LLU) -> E_CTX			... (A-2)
 *		get_tim(&systim) -> E_CTX				... (B-2)
 *		adj_tim(100) -> E_CTX					... (C-1)
 *		unl_cpu()
 *	// まずはget_timの一般的な動作を確認
 *	4:	DO(simtim_advance(30U))
 *		assert(fch_hrt() == 40U)							// 時刻：40
 *	5:	get_tim(&systim)						... (B-3)
 *		assert(systim == 30U)
 *	// システム時刻を32ビットを超える値に設定
 *	6:	DO(simtim_advance(10U))
 *		assert(fch_hrt() == 50U)							// 時刻：50
 *	7:	set_tim(2LLU << 32)						... (A-3)(F-2)
 *	// システム時刻が期待通りに設定されていることを確認
 *	8:	DO(simtim_advance(10U))
 *		assert(fch_hrt() == 60U)							// 時刻：60
 *	9:	get_tim(&systim)						... (B-3)
 *		assert(systim == (2LLU << 32) + 10U)
 *	10:	tslp_tsk(TMAX_RELTIM) -> E_TMOUT			// TMOUTの発生：4000000061
 *	11:		[hook_hrt_set_event <- 4000000001U]
 *	// ここで長時間経過したことを想定
 *	== HRT_HANDLER ==										// 時刻：4000000061
 *	// ここでタイムアウト処理が行われる						// 時刻：4000000071
 *	12:		[hook_hrt_set_event <- HRTCNT_EMPTY]
 *	== TASK1（続き）==
 *	13:	DO(simtim_advance(29U))
 *		assert(fch_hrt() == 4000000100U)					// 時刻：4000000100
 *	14:	get_tim(&systim)						... (B-3)
 *		assert(systim == (2LLU << 32) + 4000000050U)
 *	// ここで時間が経過して，HRTが巡回したことを想定
 *	15:	DO(simtim_advance((1LLU << 32) - 4000000000U))
 *		assert(fch_hrt() == 100U)							// 時刻：100
 *	16:	get_tim(&systim)						... (B-3)
 *		assert(systim == (3LLU << 32) + 50U)
 *	// adj_timのパラメータエラーのテスト
 *	17:	adj_tim(-1000001) -> E_PAR				... (C-2)
 *		adj_tim(+1000001) -> E_PAR				... (C-3)
 *
 *	// adj_timでシステム時刻を進めるテスト
 *	// タイムイベントを1つ登録
 *	18:	sta_alm(ALM1, 2000000U)							// ALM1の発生：2000101
 *	19:		[hook_hrt_set_event <- 2000001U]
 *	20:	get_tim(&systim)
 *		assert(systim == (3LLU << 32) + 50U)
 *	// adj_timでシステム時刻を進める
 *	21:	adj_tim(+1000000)						... (C-4)(D-3)(E-2)(C-5)
 *														// ALM1の発生：1000101
 *	22:		[hook_hrt_set_event <- 1000001U]
 *	23:	DO(simtim_advance(50U))
 *		assert(fch_hrt() == 150U)							// 時刻：150
 *	24:	get_tim(&systim)
 *		assert(systim == (3LLU << 32) + 1000100U)
 *	// タイムイベント発生までの時間をチェック
 *	25:	ref_alm(ALM1, &ralm)
 *		assert(ralm.lefttim == 999950U)
 *	// ここで長時間経過したことを想定
 *	26:	DO(simtim_advance(999951U))
 *	== HRT_HANDLER ==										// 時刻：1000101
 *	== ALM1-1（1回目）==
 *	27:	assert(fch_hrt() == 1000111U)						// 時刻：1000111
 *	// 非タスクコンテキストから各サービスコールを呼び出す
 *	28:	set_tim(1LLU << 32) -> E_CTX			... (A-1)
 *		get_tim(&systim) -> E_CTX				... (B-1)
 *		RETURN
 *	29:		[hook_hrt_set_event <- HRTCNT_EMPTY]
 *	== TASK1（続き）==
 *	30:	DO(simtim_advance(89U))
 *		assert(fch_hrt() == 1000200U)						// 時刻：1000200
 *
 *	// adj_timでシステム時刻を戻すテスト
 *	// タイムイベントを1つ登録
 *	31:	sta_alm(ALM1, 2000000U)							// ALM1の発生：3000201
 *	32:		[hook_hrt_set_event <- 2000001U]
 *	// システム時刻を確認しておく
 *	33:	get_tim(&systim)
 *		assert(systim == (3LLU << 32) + 2000150U)
 *	// adj_timでシステム時刻を戻す
 *	34:	adj_tim(-1000000)						... (C-4)(D-1)(E-4)(C-5)
 *														// ALM1の発生：4000201
 *	35:		[hook_hrt_set_event <- 3000001U]
 *	// システム時刻が戻っていないことをチェック
 *	// adj_timを呼び出した時点のシステム時刻になっているはず
 *	36:	get_tim(&systim)						... (B-4)
 *		assert(systim == (3LLU << 32) + 2000150U)
 *	// タイムイベント発生までの時間をチェック
 *	37:	ref_alm(ALM1, &ralm)
 *		assert(ralm.lefttim == 3000000U)
 *	// ここで時間が経過したことを想定
 *	38:	DO(simtim_advance(500000U))
 *		assert(fch_hrt() == 1500200U)						// 時刻：1500200
 *	// システム時刻が進んでいないことをチェック
 *	39:	get_tim(&systim)						... (B-4)
 *		assert(systim == (3LLU << 32) + 2000150U)
 *	// ここで時間が経過したことを想定
 *	40:	DO(simtim_advance(500100U))
 *		assert(fch_hrt() == 2000300U)						// 時刻：2000300
 *	// システム時刻が進んでいることをチェック
 *	41:	get_tim(&systim)						... (B-4)
 *		assert(systim == (3LLU << 32) + 2000250U)
 *	// adj_timでシステム時刻を戻す
 *	42:	adj_tim(-1000000)						... (C-4)(D-1)(E-4)(C-5)
 *														// ALM1の発生：5000201
 *	43:		[hook_hrt_set_event <- 2999901U]
 *	// システム時刻が戻っていないことをチェック
 *	// adj_timを呼び出した時点のシステム時刻になっているはず
 *	44:	DO(simtim_advance(20U))
 *		assert(fch_hrt() == 2000320U)						// 時刻：2000320
 *	45:	get_tim(&systim)						... (B-4)
 *		assert(systim == (3LLU << 32) + 2000250U)
 *	// ここでシステム時刻を設定
 *	// この時点で，get_timで参照するシステム時刻は20止まっていた
 *	// これ以降，さらに999980の間，システム時刻は止まっているはず
 *	46:	set_tim(4LLU << 32)
 *	47:	DO(simtim_advance(20U))
 *		assert(fch_hrt() == 2000340U)						// 時刻：2000340
 *	// システム時刻が進んでいないことをチェック
 *	48:	get_tim(&systim)						... (B-5)
 *		assert(systim == (4LLU << 32))
 *	// ここで時間が経過したことを想定
 *	49:	DO(simtim_advance(500000U))
 *		assert(fch_hrt() == 2500340U)						// 時刻：2500340
 *	// システム時刻が進んでいないことをチェック
 *		get_tim(&systim)						... (B-5)
 *		assert(systim == (4LLU << 32))
 *	// ここで時間が経過したことを想定
 *	50:	DO(simtim_advance(500100U))
 *		assert(fch_hrt() == 3000440U)						// 時刻：3000440
 *	// システム時刻が進んでいることをチェック
 *	51:	get_tim(&systim)						... (B-5)
 *		assert(systim == (4LLU << 32) + 140U)
 *
 *	// adj_timで繰り返しシステム時刻を進めるテスト
 *	// タイムイベントを2つ登録
 *	52:	sta_alm(ALM1, 1000U)							// ALM1の発生：3001441
 *	53:		[hook_hrt_set_event <- HRTCNT_EMPTY]			// ALM1の停止
 *	54:		[hook_hrt_set_event <- 1001U]
 *	55:	DO(simtim_advance(10U))
 *		assert(fch_hrt() == 3000450U)						// 時刻：3000450
 *	56:	sta_alm(ALM2, 1000U)							// ALM2の発生：3001451
 *	// ここで時間が経過したことを想定
 *	57:	DO(simtim_advance(991U))
 *	== HRT_HANDLER ==										// 時刻：3001441
 *	== ALM1-2（2回目）==
 *	58:	assert(fch_hrt() == 3001451U)						// 時刻：3001451
 *	59:	adj_tim(+1000000)						... (C-8)(D-3)(E-2)
 *														// ALM2の発生：2001451
 *	60:	adj_tim(+1000000) -> E_OBJ				... (C-6)(E-3)
 *		RETURN
 *	== ALM2-1（1回目）==
 *	61:	assert(fch_hrt() == 3001451U)						// 時刻：3001451
 *		RETURN
 *	62:		[hook_hrt_set_event <- HRTCNT_EMPTY]
 *	== TASK1（続き）==
 *	63:	DO(simtim_advance(49U))
 *		assert(fch_hrt() == 3001500U)						// 時刻：3001500
 *
 *	// (D-4)のテスト
 *	// ここで長時間経過したことを想定
 *	64:	DO(simtim_advance((1LLU << 32) - 3001500U - 110U))
 *	== HRT_HANDLER ==										// 時刻が10加算
 *	65:		[hook_hrt_set_event <- HRTCNT_EMPTY]
 *	== TASK1（続き）==
 *	66:	assert(fch_hrt() == -100U)							// 時刻：-100
 *	// システム時刻を設定する（この後の計算を楽にするため）
 *	67:	set_tim(5LLU << 32)
 *	// adj_timでシステム時刻を進める
 *	68:	adj_tim(200)								... (C-4)(D-4)(E-1)
 *	69:		[hook_hrt_set_event <- HRTCNT_EMPTY]
 *	// システム時刻を確認
 *	70:	get_tim(&systim)
 *	71:	assert(systim == (5LLU << 32) + 200U)
 *
 *	// adj_timで繰り返しシステム時刻を戻すテスト
 *	// ここで少し時間が経過したことを想定
 *	72:	DO(simtim_advance(200U))
 *		assert(fch_hrt() == 100U)							// 時刻：100
 *	// システム時刻を設定する（この後の計算を楽にするため）
 *	73:	set_tim(6LLU << 32)
 *	// adj_timでシステム時刻を戻す
 *	74:	adj_tim(-200)								... (C-4)(D-1)(E-4)
 *	75:		[hook_hrt_set_event <- HRTCNT_EMPTY]
 *	// システム時刻を確認
 *	76:	get_tim(&systim)
 *	77:	assert(systim == (6LLU << 32))
 *	// adj_timでシステム時刻をさらに戻す
 *	78:	adj_tim(-1000000)							... (C-4)(D-1)(E-4)
 *	79:		[hook_hrt_set_event <- HRTCNT_EMPTY]
 *	80:	adj_tim(-1000000) -> E_OBJ					... (C-7)(E-5)
 *	81:	adj_tim(+1000000)							... (C-4)(D-2)(E-1)
 *	82:		[hook_hrt_set_event <- HRTCNT_EMPTY]
 *	// システム時刻を確認
 *	// adj_tim(-200)を呼び出した時点でのシステム時刻になっているはず
 *	83:	get_tim(&systim)
 *		assert(systim == (6LLU << 32))
 *	// ここで時間が経過したことを想定
 *	84:	DO(simtim_advance(300U))
 *		assert(fch_hrt() == 400U)							// 時刻：400
 *	85:	get_tim(&systim)
 *		assert(systim == (6LLU << 32) + 100U)
 *	// adj_tim(0)の動作を確認
 *	86:	adj_tim(0)									... (C-4)(D-1)(E-6)
 *	87:		[hook_hrt_set_event <- HRTCNT_EMPTY]
 *	88:	DO(simtim_advance(20U))
 *		assert(fch_hrt() == 420U)							// 時刻：420
 *	// システム時刻を確認
 *	89:	get_tim(&systim)
 *		assert(systim == (6LLU << 32) + 120U)
 *	90:	END
 */

#include <kernel.h>
#include <t_syslog.h>
#include "syssvc/test_svc.h"
#include "arch/simtimer/sim_timer_cntl.h"
#include "kernel_cfg.h"
#include "simt_systim2.h"

#ifndef HRT_CONFIG1
#error Compiler option "-DHRT_CONFIG1" is missing.
#endif

#ifndef HOOK_HRT_EVENT
#error Compiler option "-DHOOK_HRT_EVENT" is missing.
#endif /* HOOK_HRT_EVENT */

/*
 *  HRTCNT_EMPTYの定義
 */
#ifdef TOPPERS_SUPPORT_DRIFT
#define HRTCNT_EMPTY	TMAX_RELTIM
#else /* TOPPERS_SUPPORT_DRIFT */
#define HRTCNT_EMPTY	HRTCNT_BOUND
#endif /* TOPPERS_SUPPORT_DRIFT */

void
hook_hrt_raise_event(void)
{
	check_point(0);
}

/* DO NOT DELETE THIS LINE -- gentest depends on it. */

static uint_t	alarm1_count = 0;

void
alarm1_handler(EXINF exinf)
{
	ER_UINT	ercd;
	SYSTIM	systim;

	switch (++alarm1_count) {
	case 1:
		check_point(27);
		check_assert(fch_hrt() == 1000111U);

		check_point(28);
		ercd = set_tim(1LLU << 32);
		check_ercd(ercd, E_CTX);

		ercd = get_tim(&systim);
		check_ercd(ercd, E_CTX);

		return;

		check_point(0);

	case 2:
		check_point(58);
		check_assert(fch_hrt() == 3001451U);

		check_point(59);
		ercd = adj_tim(+1000000);
		check_ercd(ercd, E_OK);

		check_point(60);
		ercd = adj_tim(+1000000);
		check_ercd(ercd, E_OBJ);

		return;

		check_point(0);

	default:
		check_point(0);
	}
	check_point(0);
}

static uint_t	alarm2_count = 0;

void
alarm2_handler(EXINF exinf)
{

	switch (++alarm2_count) {
	case 1:
		check_point(61);
		check_assert(fch_hrt() == 3001451U);

		return;

		check_point(0);

	default:
		check_point(0);
	}
	check_point(0);
}

void
task1(EXINF exinf)
{
	ER_UINT	ercd;
	SYSTIM	systim;
	T_RALM	ralm;

	check_point(2);
	check_assert(fch_hrt() == 10U);

	check_point(3);
	ercd = loc_cpu();
	check_ercd(ercd, E_OK);

	ercd = set_tim(1000000LLU);
	check_ercd(ercd, E_CTX);

	ercd = get_tim(&systim);
	check_ercd(ercd, E_CTX);

	ercd = adj_tim(100);
	check_ercd(ercd, E_CTX);

	ercd = unl_cpu();
	check_ercd(ercd, E_OK);

	check_point(4);
	simtim_advance(30U);

	check_assert(fch_hrt() == 40U);

	check_point(5);
	ercd = get_tim(&systim);
	check_ercd(ercd, E_OK);

	check_assert(systim == 30U);

	check_point(6);
	simtim_advance(10U);

	check_assert(fch_hrt() == 50U);

	check_point(7);
	ercd = set_tim(2LLU << 32);
	check_ercd(ercd, E_OK);

	check_point(8);
	simtim_advance(10U);

	check_assert(fch_hrt() == 60U);

	check_point(9);
	ercd = get_tim(&systim);
	check_ercd(ercd, E_OK);

	check_assert(systim == (2LLU << 32) + 10U);

	check_point(10);
	ercd = tslp_tsk(TMAX_RELTIM);
	check_ercd(ercd, E_TMOUT);

	check_point(13);
	simtim_advance(29U);

	check_assert(fch_hrt() == 4000000100U);

	check_point(14);
	ercd = get_tim(&systim);
	check_ercd(ercd, E_OK);

	check_assert(systim == (2LLU << 32) + 4000000050U);

	check_point(15);
	simtim_advance((1LLU << 32) - 4000000000U);

	check_assert(fch_hrt() == 100U);

	check_point(16);
	ercd = get_tim(&systim);
	check_ercd(ercd, E_OK);

	check_assert(systim == (3LLU << 32) + 50U);

	check_point(17);
	ercd = adj_tim(-1000001);
	check_ercd(ercd, E_PAR);

	ercd = adj_tim(+1000001);
	check_ercd(ercd, E_PAR);

	check_point(18);
	ercd = sta_alm(ALM1, 2000000U);
	check_ercd(ercd, E_OK);

	check_point(20);
	ercd = get_tim(&systim);
	check_ercd(ercd, E_OK);

	check_assert(systim == (3LLU << 32) + 50U);

	check_point(21);
	ercd = adj_tim(+1000000);
	check_ercd(ercd, E_OK);

	check_point(23);
	simtim_advance(50U);

	check_assert(fch_hrt() == 150U);

	check_point(24);
	ercd = get_tim(&systim);
	check_ercd(ercd, E_OK);

	check_assert(systim == (3LLU << 32) + 1000100U);

	check_point(25);
	ercd = ref_alm(ALM1, &ralm);
	check_ercd(ercd, E_OK);

	check_assert(ralm.lefttim == 999950U);

	check_point(26);
	simtim_advance(999951U);

	check_point(30);
	simtim_advance(89U);

	check_assert(fch_hrt() == 1000200U);

	check_point(31);
	ercd = sta_alm(ALM1, 2000000U);
	check_ercd(ercd, E_OK);

	check_point(33);
	ercd = get_tim(&systim);
	check_ercd(ercd, E_OK);

	check_assert(systim == (3LLU << 32) + 2000150U);

	check_point(34);
	ercd = adj_tim(-1000000);
	check_ercd(ercd, E_OK);

	check_point(36);
	ercd = get_tim(&systim);
	check_ercd(ercd, E_OK);

	check_assert(systim == (3LLU << 32) + 2000150U);

	check_point(37);
	ercd = ref_alm(ALM1, &ralm);
	check_ercd(ercd, E_OK);

	check_assert(ralm.lefttim == 3000000U);

	check_point(38);
	simtim_advance(500000U);

	check_assert(fch_hrt() == 1500200U);

	check_point(39);
	ercd = get_tim(&systim);
	check_ercd(ercd, E_OK);

	check_assert(systim == (3LLU << 32) + 2000150U);

	check_point(40);
	simtim_advance(500100U);

	check_assert(fch_hrt() == 2000300U);

	check_point(41);
	ercd = get_tim(&systim);
	check_ercd(ercd, E_OK);

	check_assert(systim == (3LLU << 32) + 2000250U);

	check_point(42);
	ercd = adj_tim(-1000000);
	check_ercd(ercd, E_OK);

	check_point(44);
	simtim_advance(20U);

	check_assert(fch_hrt() == 2000320U);

	check_point(45);
	ercd = get_tim(&systim);
	check_ercd(ercd, E_OK);

	check_assert(systim == (3LLU << 32) + 2000250U);

	check_point(46);
	ercd = set_tim(4LLU << 32);
	check_ercd(ercd, E_OK);

	check_point(47);
	simtim_advance(20U);

	check_assert(fch_hrt() == 2000340U);

	check_point(48);
	ercd = get_tim(&systim);
	check_ercd(ercd, E_OK);

	check_assert(systim == (4LLU << 32));

	check_point(49);
	simtim_advance(500000U);

	check_assert(fch_hrt() == 2500340U);

	ercd = get_tim(&systim);
	check_ercd(ercd, E_OK);

	check_assert(systim == (4LLU << 32));

	check_point(50);
	simtim_advance(500100U);

	check_assert(fch_hrt() == 3000440U);

	check_point(51);
	ercd = get_tim(&systim);
	check_ercd(ercd, E_OK);

	check_assert(systim == (4LLU << 32) + 140U);

	check_point(52);
	ercd = sta_alm(ALM1, 1000U);
	check_ercd(ercd, E_OK);

	check_point(55);
	simtim_advance(10U);

	check_assert(fch_hrt() == 3000450U);

	check_point(56);
	ercd = sta_alm(ALM2, 1000U);
	check_ercd(ercd, E_OK);

	check_point(57);
	simtim_advance(991U);

	check_point(63);
	simtim_advance(49U);

	check_assert(fch_hrt() == 3001500U);

	check_point(64);
	simtim_advance((1LLU << 32) - 3001500U - 110U);

	check_point(66);
	check_assert(fch_hrt() == -100U);

	check_point(67);
	ercd = set_tim(5LLU << 32);
	check_ercd(ercd, E_OK);

	check_point(68);
	ercd = adj_tim(200);
	check_ercd(ercd, E_OK);

	check_point(70);
	ercd = get_tim(&systim);
	check_ercd(ercd, E_OK);

	check_point(71);
	check_assert(systim == (5LLU << 32) + 200U);

	check_point(72);
	simtim_advance(200U);

	check_assert(fch_hrt() == 100U);

	check_point(73);
	ercd = set_tim(6LLU << 32);
	check_ercd(ercd, E_OK);

	check_point(74);
	ercd = adj_tim(-200);
	check_ercd(ercd, E_OK);

	check_point(76);
	ercd = get_tim(&systim);
	check_ercd(ercd, E_OK);

	check_point(77);
	check_assert(systim == (6LLU << 32));

	check_point(78);
	ercd = adj_tim(-1000000);
	check_ercd(ercd, E_OK);

	check_point(80);
	ercd = adj_tim(-1000000);
	check_ercd(ercd, E_OBJ);

	check_point(81);
	ercd = adj_tim(+1000000);
	check_ercd(ercd, E_OK);

	check_point(83);
	ercd = get_tim(&systim);
	check_ercd(ercd, E_OK);

	check_assert(systim == (6LLU << 32));

	check_point(84);
	simtim_advance(300U);

	check_assert(fch_hrt() == 400U);

	check_point(85);
	ercd = get_tim(&systim);
	check_ercd(ercd, E_OK);

	check_assert(systim == (6LLU << 32) + 100U);

	check_point(86);
	ercd = adj_tim(0);
	check_ercd(ercd, E_OK);

	check_point(88);
	simtim_advance(20U);

	check_assert(fch_hrt() == 420U);

	check_point(89);
	ercd = get_tim(&systim);
	check_ercd(ercd, E_OK);

	check_assert(systim == (6LLU << 32) + 120U);

	check_finish(90);
	check_point(0);
}

static uint_t	hook_hrt_set_event_count = 0;

void
hook_hrt_set_event(HRTCNT hrtcnt)
{

	switch (++hook_hrt_set_event_count) {
	case 1:
		test_start(__FILE__);

		check_point(1);
		check_assert(hrtcnt == HRTCNT_EMPTY);

		return;

		check_point(0);

	case 2:
		check_point(11);
		check_assert(hrtcnt == 4000000001U);

		return;

		check_point(0);

	case 3:
		check_point(12);
		check_assert(hrtcnt == HRTCNT_EMPTY);

		return;

		check_point(0);

	case 4:
		check_point(19);
		check_assert(hrtcnt == 2000001U);

		return;

		check_point(0);

	case 5:
		check_point(22);
		check_assert(hrtcnt == 1000001U);

		return;

		check_point(0);

	case 6:
		check_point(29);
		check_assert(hrtcnt == HRTCNT_EMPTY);

		return;

		check_point(0);

	case 7:
		check_point(32);
		check_assert(hrtcnt == 2000001U);

		return;

		check_point(0);

	case 8:
		check_point(35);
		check_assert(hrtcnt == 3000001U);

		return;

		check_point(0);

	case 9:
		check_point(43);
		check_assert(hrtcnt == 2999901U);

		return;

		check_point(0);

	case 10:
		check_point(53);
		check_assert(hrtcnt == HRTCNT_EMPTY);

		return;

		check_point(0);

	case 11:
		check_point(54);
		check_assert(hrtcnt == 1001U);

		return;

		check_point(0);

	case 12:
		check_point(62);
		check_assert(hrtcnt == HRTCNT_EMPTY);

		return;

		check_point(0);

	case 13:
		check_point(65);
		check_assert(hrtcnt == HRTCNT_EMPTY);

		return;

		check_point(0);

	case 14:
		check_point(69);
		check_assert(hrtcnt == HRTCNT_EMPTY);

		return;

		check_point(0);

	case 15:
		check_point(75);
		check_assert(hrtcnt == HRTCNT_EMPTY);

		return;

		check_point(0);

	case 16:
		check_point(79);
		check_assert(hrtcnt == HRTCNT_EMPTY);

		return;

		check_point(0);

	case 17:
		check_point(82);
		check_assert(hrtcnt == HRTCNT_EMPTY);

		return;

		check_point(0);

	case 18:
		check_point(87);
		check_assert(hrtcnt == HRTCNT_EMPTY);

		return;

		check_point(0);

	default:
		check_point(0);
	}
	check_point(0);
}
