/*
 *  TOPPERS Software
 *      Toyohashi Open Platform for Embedded Real-Time Systems
 * 
 *  Copyright (C) 2022 by Embedded and Real-Time Systems Laboratory
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
 *  $Id: test_subprio3.c 1646 2022-09-25 17:51:54Z ertl-hiro $
 */

/* 
 *		サブ優先度機能のテスト(3)
 *
 *	テストの目的とテスト項目の設計については，「サブ優先度機能のテスト」
 *	（test_subprio.txt）を参照すること．
 *
 * 【テスト項目】
 *
 *	(I) chg_priによる優先順位制御
 *	  (I-1) 対象タスクが実行できる状態で，優先度上昇状態でなく，優先度
 *		 	がサブ優先度を使用すると設定されている場合
 *		(I-1-1) 同優先度の他のタスクよりサブ優先度が高い場合に，そのタ
 *				スクより優先順位が高くなること（タスク切換えが起こる場
 *				合）
 *		(I-1-2) 同優先度の他のタスクよりサブ優先度が高い場合に，そのタ
 *				スクより優先順位が高くなること（タスク切換えが起こらな
 *				い場合）
 *		(I-1-3) 同優先度の他のタスクと同じサブ優先度の場合に，そのタス
 *				クより優先順位が低くなること［NGKI1194］
 *		(I-1-4) 同優先度の他のタスクよりサブ優先度が低い場合に，そのタ
 *				スクより優先順位が低くなること
 *	  (I-2) 対象タスクが実行できる状態で，優先度上昇状態でなく，優先度
 *		 	がサブ優先度を使用すると設定されていない場合
 *		(I-2-1) 同優先度の他のタスクよりサブ優先度が高い場合に，サブ優
 *				先度に関係なく，そのタスクより優先順位が低くなること
 *				［NGKI1194］
 *	  (I-3) 対象タスクが実行できる状態で，優先度上昇状態であり（すなわ
 *		 	ち，現在優先度が変化せず），優先度がサブ優先度を使用すると
 *		 	設定されている場合
 *		(I-3-1) 優先順位が変わらないこと［NGKI1197］
 *	  (I-4) 対象タスクが待ち状態でタスクの優先度順の待ち行列につながれ
 *			ている場合で，優先度上昇状態でなく，優先度がサブ優先度を使
 *			用すると設定されている場合
 *		(I-4-1) 対象タスクの待ち行列の中での順序が，サブ優先度に関係な
 *				く，同優先度内で最後になること［NGKI1195］［NGKI1196］
 *	  (I-5) 対象タスクが待ち状態でタスクの優先度順の待ち行列につながれ
 *			ている場合で，優先度上昇状態であり（すなわち，現在優先度が
 *			変化せず），優先度がサブ優先度を使用すると設定されている場
 *			合
 *		(I-5-1) 待ち行列の中での順番が変わらないこと［NGKI1197］
 *	(J) chg_sprによる優先順位制御
 *	  (J-1) 対象タスクが実行できる状態で，優先度上昇状態でなく，優先度
 *		 	がサブ優先度を使用すると設定されている場合
 *		(J-1-1) 同優先度の他のタスクよりサブ優先度が高くなる場合に，そ
 *				のタスクより優先順位が高くなること（タスク切換えが起こ
 *				る場合）
 *		(J-1-2) 同優先度の他のタスクよりサブ優先度が高くなる場合に，そ
 *				のタスクより優先順位が高くなること（タスク切換えが起こ
 *				らない場合）
 *		(J-1-3) 同優先度の他のタスクと同じサブ優先度になる場合に，その
 *				タスクより優先順位が低くなること（タスク切換えが起こる
 *				場合）［NGKI3673］
 *		(J-1-4) 同優先度の他のタスクと同じサブ優先度になる場合に，その
 *				タスクより優先順位が低くなること（タスク切換えが起こら
 *				ない場合）［NGKI3673］
 *		(J-1-5) 同優先度の他のタスクよりサブ優先度が低くなる場合に，サ
 *				ブ優先度が変更され，そのタスクより優先順位が低くなるこ
 *				と（タスク切換えが起こる場合）
 *		(J-1-6) 同優先度の他のタスクよりサブ優先度が低くなる場合に，サ
 *				ブ優先度が変更され，そのタスクより優先順位が低くなるこ
 *				と（タスク切換えが起こらない場合）
 *	  (J-2) 対象タスクが実行できる状態で，優先度上昇状態でなく，優先度
 *		 	がサブ優先度を使用すると設定されていない場合
 *		(J-2-1) 同優先度の他のタスクよりサブ優先度が高い場合に，サブ優
 *				先度に関係なく，そのタスクより優先順位が低くなること
 *	  (J-3) 対象タスクが実行できる状態で，優先度上昇状態であり，優先度
 *		 	がサブ優先度を使用すると設定されている場合
 *		(J-3-1) 優先順位が変わらないこと
 *	  (J-4) 対象タスクが待ち状態でタスクの優先度順の待ち行列につながれ
 *			ている場合で，優先度上昇状態でなく，優先度がサブ優先度を使
 *			用すると設定されている場合
 *		(J-4-1) 対象タスクの待ち行列の中での順序が変わらないこと
 *
 * 【使用リソース】
 *
 *	TASK1: 高優先度タスク，メインタスク，最初から起動
 *	TASK2: 中優先度タスク
 *	TASK3: 中優先度タスク
 *	TASK4: 中優先度タスク
 *	TASK5: 低優先度タスク
 *	TASK6: 低優先度タスク
 *	TASK7: 最低優先度タスク，最初から起動
 *	中優先度を，サブ優先度を使用して優先順位を決めるように設定
 *	MTX1:  ミューテックス（TA_CEILING属性，上限は中優先度）
 *	MTX2:  ミューテックス（TA_CEILING属性，上限は中優先度）
 *
 * 【凡例】
 *
 *	「中：[0]TASK2」という記述の中の[0]は，TASK2のサブ優先度が0（最高
 *	値）であることを示す。また，[M]はサブ優先度がUINT_MAX（最低値）で
 *	あることを，[B]は現在優先度が上昇中でサブ優先度が0（最高値）とみな
 *	されていることを示す。
 *
 * 【テストシーケンス】
 *
 *	== TASK1（優先度：高）==
 *	1:	act_tsk(TASK2)
 *		act_tsk(TASK5)
 *		chg_spr(TASK2, 20)
 *		chg_spr(TASK5, 10)
 *		// 高：TASK1，中：[20]TASK2，低：TASK5
 *	2:	slp_tsk()
 *		// 中：[20]TASK2，低：TASK5
 *	== TASK2-1（優先度：中）==
 *	3:	chg_pri(TASK5, MID_PRIORITY)				... (I-1-1)
 *		// 中：[10]TASK5→[20]TASK2
 *	== TASK5-1（優先度：中←低）==
 *	4:	ext_tsk()
 *		// 中：[20]TASK2
 *
 *	== TASK2-1（続き）==
 *	5:	wup_tsk(TASK1)
 *		// 高：TASK1，中：[20]TASK2
 *	== TASK1（続き）==
 *	6:	act_tsk(TASK5)
 *		// 高：TASK1，中：[20]TASK2，低：TASK5
 *		chg_pri(TASK5, MID_PRIORITY)				... (I-1-2)
 *		// 高：TASK1，中：[10]TASK5→[20]TASK2
 *	7:	slp_tsk()
 *		// 中：[10]TASK5→[20]TASK2
 *	== TASK5-2（優先度：中←低）==
 *	8:	ext_tsk()
 *		// 中：[20]TASK2
 *
 *	== TASK2-1（続き）==
 *	9:	chg_spr(TASK2, 10)
 *		// 中：[10]TASK2
 *		wup_tsk(TASK1)
 *		// 高：TASK1，中：[10]TASK2
 *	== TASK1（続き）==
 *	10:	act_tsk(TASK5)
 *		// 高：TASK1，中：[10]TASK2，低：TASK5
 *		chg_pri(TASK5, MID_PRIORITY)				... (I-1-3)
 *		// 高：TASK1，中：[10]TASK2→[10]TASK5
 *	11:	slp_tsk()
 *		// 中：[10]TASK2→[10]TASK5
 *	== TASK2-1（続き）==
 *	12:	ext_tsk()
 *		// 中：[10]TASK5
 *
 *	== TASK5-3（優先度：中←低）==
 *	13:	chg_pri(TASK5, TPRI_INI)
 *		// 低：TASK5
 *		act_tsk(TASK2)
 *		// 中：[10]TASK2，低：TASK5
 *	== TASK2-2（優先度：中）==
 *	14:	chg_spr(TASK2, 0)
 *		// 中：[0]TASK2，低：TASK5
 *		wup_tsk(TASK1)
 *		// 高：TASK1，中：[0]TASK2，低：TASK5
 *	== TASK1（続き）==
 *	15:	chg_pri(TASK5, MID_PRIORITY)				... (I-1-4)
 *		// 高：TASK1，中：[0]TASK2→[10]TASK5
 *	16:	slp_tsk()
 *		// 中：[0]TASK2→[10]TASK5
 *
 *	== TASK2-2（続き）==
 *	17:	chg_pri(TASK5, TPRI_INI)
 *		// 中：[0]TASK2，低：TASK5
 *		wup_tsk(TASK1)
 *		// 高：TASK1，中：[0]TASK2，低：TASK5
 *	== TASK1（続き）==
 *	18:	chg_pri(TASK2, LOW_PRIORITY)				... (I-2-1)
 *		// 高：TASK1，低：TASK5→TASK2
 *	19:	slp_tsk()
 *		// 低：TASK5→TASK2
 *	== TASK5-3（続き）==
 *	20:	ext_tsk()
 *		// 低：TASK2
 *	== TASK2-2（続き）==
 *	21:	chg_pri(TASK2, TPRI_INI)
 *		// 中：[0]TASK2
 *
 *	22:	wup_tsk(TASK1)
 *		// 高：TASK1，中：[0]TASK2
 *	== TASK1（続き）==
 *	23:	chg_spr(TASK3, 0)
 *		act_tsk(TASK3)
 *		// 高：TASK1，中：[0]TASK2→[0]TASK3
 *		act_tsk(TASK4)
 *		// 高：TASK1，中：[0]TASK2→[0]TASK3→[M]TASK4
 *		slp_tsk()
 *		// 中：[0]TASK2→[0]TASK3→[M]TASK4
 *	== TASK2-2（続き）==
 *	24:	loc_mtx(MTX1)
 *		// 中：[B]TASK2→[0]TASK3→[M]TASK4，MTX1：TASK2
 *		slp_tsk()
 *		// 中：[0]TASK3→[M]TASK4，MTX1：TASK2
 *	== TASK3-1（優先度：中）==
 *	25:	loc_mtx(MTX1)
 *		// 中：[M]TASK4，MTX1：TASK2→TASK3
 *	== TASK4-1（優先度：中）==
 *	26:	wup_tsk(TASK2)
 *		// 中：[B]TASK2→[M]TASK4，MTX1：TASK2→TASK3
 *	== TASK2-2（続き）==
 *	27:	unl_mtx(MTX1)
 *		// 中：[0]TASK2→[B]TASK3→[M]TASK4，MTX1：TASK3
 *		wup_tsk(TASK1)
 *		// 高：TASK1，中：[0]TASK2→[B]TASK3→[M]TASK4，MTX1：TASK3
 *	== TASK1（続き）==
 *	28:	chg_spr(TASK4, 0)
 *		// 高：TASK1，中：[0]TASK2→[B]TASK3→[0]TASK4，MTX1：TASK3
 *		chg_pri(TASK3, MID_PRIORITY)				... (I-3-1)
 *		// 高：TASK1，中：[0]TASK2→[B]TASK3→[0]TASK4，MTX1：TASK3
 *		get_nth(MID_PRIORITY, 0U, &tskid)
 *		assert(tskid == TASK2)
 *		get_nth(MID_PRIORITY, 1U, &tskid)
 *		assert(tskid == TASK3)
 *		get_nth(MID_PRIORITY, 2U, &tskid)
 *		assert(tskid == TASK4)
 *	29:	chg_spr(TASK3, 10)							... (J-3-1)
 *		// 高：TASK1，中：[0]TASK2→[B]TASK3→[0]TASK4，MTX1：TASK3
 *	30:	slp_tsk()
 *		// 中：[0]TASK2→[B]TASK3→[0]TASK4，MTX1：TASK3
 *	== TASK2-2（続き）==
 *	31:	chg_spr(TASK2, 20)
 *		// 中：[B]TASK3→[0]TASK4→[20]TASK2，MTX1：TASK3
 *	== TASK3-1（続き）==
 *	32:	ext_tsk()
 *		// 中：[0]TASK4→[20]TASK2
 *	== TASK4-1（続き）==
 *	33:	ext_tsk()
 *		// 中：[20]TASK2
 *
 *	== TASK2-2（続き）==
 *	34:	loc_mtx(MTX1)
 *		// 中：[B]TASK2，MTX1：TASK2
 *		act_tsk(TASK6)
 *		// 中：[B]TASK2，低：TASK6，MTX1：TASK2
 *		act_tsk(TASK5)
 *		// 中：[B]TASK2，低：TASK6→TASK5，MTX1：TASK2
 *		slp_tsk()
 *		// 低：TASK6→TASK5，MTX1：TASK2
 *	== TASK6-1（優先度：低）==
 *	35:	loc_mtx(MTX1)
 *		// 低：TASK5，MTX1：TASK2→TASK6
 *	== TASK5-4（優先度：低）==
 *	36:	loc_mtx(MTX1)
 *		// MTX1：TASK2→TASK6→TASK5
 *	== TASK7（優先度：最低）==
 *	37:	wup_tsk(TASK2)
 *		// 中：[B]TASK2，MTX1：TASK2→TASK6→TASK5
 *	== TASK2-2（続き）==
 *	38:	chg_pri(TASK5, MID_PRIORITY)				... (I-4-1)
 *		// 中：[B]TASK2，MTX1：TASK2→TASK5→TASK6
 *		unl_mtx(MTX1)
 *		// 中：[B]TASK5→[20]TASK2，MTX1：TASK5→TASK6
 *	== TASK5-4（続き）==
 *	39:	unl_mtx(MTX1)
 *		// 中：[B]TASK6→[10]TASK5→[20]TASK2，MTX1：TASK6
 *	== TASK6-1（続き）==
 *	40:	unl_mtx(MTX1)
 *		// 中：[10]TASK5→[20]TASK2，低：TASK6
 *	== TASK5-4（続き）==
 *	41:	ext_tsk()
 *		// 中：[20]TASK2，低：TASK6
 *
 *	== TASK2-2（続き）==
 *	42:	loc_mtx(MTX1)
 *		// 中：[B]TASK2，低：TASK6，MTX1：TASK2
 *		act_tsk(TASK3)
 *		// 中：[B]TASK2→[10]TASK3，低：TASK6，MTX1：TASK2
 *		slp_tsk()
 *		// 中：[10]TASK3，低：TASK6，MTX1：TASK2
 *	== TASK3-2（優先度：中）==
 *	43:	loc_mtx(MTX1)
 *		// 低：TASK6，MTX1：TASK2→TASK3
 *	== TASK6-1（続き）==
 *	44:	loc_mtx(MTX2)
 *		// 中：[B]TASK6，MTX1：TASK2→TASK3，MTX2：TASK6
 *		loc_mtx(MTX1)
 *		// MTX1：TASK2→TASK3→TASK6，MTX2：TASK6
 *	== TASK7（続き）
 *	45:	act_tsk(TASK4)
 *		// 中：[0]TASK4，MTX1：TASK2→TASK3→TASK6，MTX2：TASK6
 *	== TASK4-2（優先度：中）==
 *	46:	loc_mtx(MTX1)
 *		// MTX1：TASK2→TASK3→TASK6→TASK4，MTX2：TASK6
 *	== TASK7（続き）
 *	47:	wup_tsk(TASK2)
 *		// 中：[B]TASK2，MTX1：TASK2→TASK3→TASK6→TASK4，MTX2：TASK6
 *	== TASK2-2（続き）==
 *	48:	chg_pri(TASK6, MID_PRIORITY)				... (I-5-1)
 *		// 中：[B]TASK2，MTX1：TASK2→TASK3→TASK6→TASK4，MTX2：TASK6
 *	49:	unl_mtx(MTX1)
 *		// 中：[B]TASK3→[20]TASK2，MTX1：TASK3→TASK6→TASK4，MTX2：TASK6
 *	== TASK3-2（続き）==
 *	50:	ext_tsk()
 *		// 中：[B]TASK6→[20]TASK2，MTX1：TASK6→TASK4，MTX2：TASK6
 *	== TASK6-1（続き）==
 *	51:	ext_tsk()
 *		// 中：[B]TASK4→[20]TASK2，MTX1：TASK4
 *	== TASK4-2（続き）==
 *	52:	unl_mtx(MTX1)
 *		// 中：[0]TASK4→[20]TASK2
 *
 *	53:	chg_spr(TASK4, 20)							... (J-1-3)
 *		// 中：[20]TASK2→[20]TASK4
 *	== TASK2-2（続き）==
 *	54:	chg_spr(TASK4, 10)							... (J-1-1)
 *		// 中：[10]TASK4→[20]TASK2
 *	== TASK4-2（続き）==
 *	55:	chg_spr(TASK4, 30)							... (J-1-5)
 *		// 中：[20]TASK2→[30]TASK4
 *	== TASK2-2（続き）==
 *	56:	wup_tsk(TASK1)
 *		// 高：TASK1，中：[20]TASK2→[30]TASK4
 *	== TASK1（続き）==
 *	57:	chg_spr(TASK4, 10)							... (J-1-2)
 *		// 高：TASK1，中：[10]TASK4→[20]TASK2
 *		slp_tsk()
 *		// 中：[10]TASK4→[20]TASK2
 *	== TASK4-2（続き）==
 *	58:	wup_tsk(TASK1)
 *		// 高：TASK1，中：[10]TASK4→[20]TASK2
 *	== TASK1（続き）==
 *	59:	chg_spr(TASK4, 20)							... (J-1-4)
 *		// 高：TASK1，中：[20]TASK2→[20]TASK4
 *		slp_tsk()
 *		// 中：[20]TASK2→[20]TASK4
 *	== TASK2-2（続き）==
 *	60:	wup_tsk(TASK1)
 *		// 高：TASK1，中：[20]TASK2→[20]TASK4
 *	== TASK1（続き）==
 *	61:	chg_spr(TASK2, 30)							... (J-1-6)
 *		// 高：TASK1，中：[20]TASK4→[30]TASK2
 *		slp_tsk()
 *		// 中：[20]TASK4→[30]TASK2
 *	== TASK4-2（続き）==
 *	62:	ext_tsk()
 *		// 中：[30]TASK2
 *
 *	== TASK2-2（続き）==
 *	63:	act_tsk(TASK5)
 *		// 中：[30]TASK2，低：TASK5
 *		act_tsk(TASK6)
 *		// 中：[30]TASK2，低：TASK5→TASK6
 *		chg_spr(TASK6, 0)							... (J-2-1)
 *		// 中：[30]TASK2，低：TASK5→TASK6
 *	64:	ext_tsk()
 *		// 低：TASK5→TASK6
 *	== TASK5-5（優先度：低）==
 *	65:	ext_tsk()
 *		// 低：TASK6
 *
 *	== TASK6-2（優先度：低）==
 *	66:	loc_mtx(MTX1)
 *		// 中：[B]TASK6，MTX1：TASK6
 *		act_tsk(TASK2)
 *		// 中：[B]TASK6→[30]TASK2，MTX1：TASK6
 *		act_tsk(TASK3)
 *		// 中：[B]TASK6→[10]TASK3→[30]TASK2，MTX1：TASK6
 *		act_tsk(TASK4)
 *		// 中：[B]TASK6→[10]TASK3→[20]TASK4→[30]TASK2，MTX1：TASK6
 *		slp_tsk()
 *		// 中：[10]TASK3→[20]TASK4→[30]TASK2，MTX1：TASK6
 *	== TASK3-3（優先度：中）==
 *	67:	loc_mtx(MTX1)
 *		// 中：[20]TASK4→[30]TASK2，MTX1：TASK6→TASK3
 *	== TASK4-3（優先度：中）==
 *	68:	loc_mtx(MTX1)
 *		// 中：[30]TASK2，MTX1：TASK6→TASK3→TASK4
 *	== TASK2-3（優先度：中）==
 *	69:	loc_mtx(MTX1)
 *		// MTX1：TASK6→TASK3→TASK4→TASK2
 *	== TASK7（続き）
 *	70:	wup_tsk(TASK6)
 *		// 中：[B]TASK6，MTX1：TASK6→TASK3→TASK4→TASK2
 *	== TASK6-2（続き）==
 *	71:	chg_spr(TASK4, 0)							... (J-4-1)
 *		// 中：[B]TASK6，MTX1：TASK6→TASK3→TASK4→TASK2
 *		unl_mtx(MTX1)
 *		// 中：[B]TASK2，低：TASK6，MTX1：TASK3→TASK4→TASK2
 *	== TASK3-3（続き）==
 *	72:	ext_tsk()
 *		// 中：[B]TASK3，低：TASK6，MTX1：TASK4→TASK2
 *	== TASK4-3（続き）==
 *	73:	ext_tsk()
 *		// 中：[B]TASK4，低：TASK6，MTX1：TASK2
 *	== TASK2-3（続き）==
 *	74:	ext_tsk()
 *		// 低：TASK6
 *	== TASK6-2（続き）==
 *	75:	END
 */

#include <kernel.h>
#include <t_syslog.h>
#include "syssvc/test_svc.h"
#include "kernel_cfg.h"
#include "test_common.h"

/* DO NOT DELETE THIS LINE -- gentest depends on it. */

void
task1(EXINF exinf)
{
	ER_UINT	ercd;
	ID		tskid;

	test_start(__FILE__);

	check_point(1);
	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	ercd = act_tsk(TASK5);
	check_ercd(ercd, E_OK);

	ercd = chg_spr(TASK2, 20);
	check_ercd(ercd, E_OK);

	ercd = chg_spr(TASK5, 10);
	check_ercd(ercd, E_OK);

	check_point(2);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(6);
	ercd = act_tsk(TASK5);
	check_ercd(ercd, E_OK);

	ercd = chg_pri(TASK5, MID_PRIORITY);
	check_ercd(ercd, E_OK);

	check_point(7);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(10);
	ercd = act_tsk(TASK5);
	check_ercd(ercd, E_OK);

	ercd = chg_pri(TASK5, MID_PRIORITY);
	check_ercd(ercd, E_OK);

	check_point(11);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(15);
	ercd = chg_pri(TASK5, MID_PRIORITY);
	check_ercd(ercd, E_OK);

	check_point(16);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(18);
	ercd = chg_pri(TASK2, LOW_PRIORITY);
	check_ercd(ercd, E_OK);

	check_point(19);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(23);
	ercd = chg_spr(TASK3, 0);
	check_ercd(ercd, E_OK);

	ercd = act_tsk(TASK3);
	check_ercd(ercd, E_OK);

	ercd = act_tsk(TASK4);
	check_ercd(ercd, E_OK);

	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(28);
	ercd = chg_spr(TASK4, 0);
	check_ercd(ercd, E_OK);

	ercd = chg_pri(TASK3, MID_PRIORITY);
	check_ercd(ercd, E_OK);

	ercd = get_nth(MID_PRIORITY, 0U, &tskid);
	check_ercd(ercd, E_OK);

	check_assert(tskid == TASK2);

	ercd = get_nth(MID_PRIORITY, 1U, &tskid);
	check_ercd(ercd, E_OK);

	check_assert(tskid == TASK3);

	ercd = get_nth(MID_PRIORITY, 2U, &tskid);
	check_ercd(ercd, E_OK);

	check_assert(tskid == TASK4);

	check_point(29);
	ercd = chg_spr(TASK3, 10);
	check_ercd(ercd, E_OK);

	check_point(30);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(57);
	ercd = chg_spr(TASK4, 10);
	check_ercd(ercd, E_OK);

	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(59);
	ercd = chg_spr(TASK4, 20);
	check_ercd(ercd, E_OK);

	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(61);
	ercd = chg_spr(TASK2, 30);
	check_ercd(ercd, E_OK);

	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_assert(false);
}

static uint_t	task2_count = 0;

void
task2(EXINF exinf)
{
	ER_UINT	ercd;

	switch (++task2_count) {
	case 1:
		check_point(3);
		ercd = chg_pri(TASK5, MID_PRIORITY);
		check_ercd(ercd, E_OK);

		check_point(5);
		ercd = wup_tsk(TASK1);
		check_ercd(ercd, E_OK);

		check_point(9);
		ercd = chg_spr(TASK2, 10);
		check_ercd(ercd, E_OK);

		ercd = wup_tsk(TASK1);
		check_ercd(ercd, E_OK);

		check_point(12);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 2:
		check_point(14);
		ercd = chg_spr(TASK2, 0);
		check_ercd(ercd, E_OK);

		ercd = wup_tsk(TASK1);
		check_ercd(ercd, E_OK);

		check_point(17);
		ercd = chg_pri(TASK5, TPRI_INI);
		check_ercd(ercd, E_OK);

		ercd = wup_tsk(TASK1);
		check_ercd(ercd, E_OK);

		check_point(21);
		ercd = chg_pri(TASK2, TPRI_INI);
		check_ercd(ercd, E_OK);

		check_point(22);
		ercd = wup_tsk(TASK1);
		check_ercd(ercd, E_OK);

		check_point(24);
		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_OK);

		ercd = slp_tsk();
		check_ercd(ercd, E_OK);

		check_point(27);
		ercd = unl_mtx(MTX1);
		check_ercd(ercd, E_OK);

		ercd = wup_tsk(TASK1);
		check_ercd(ercd, E_OK);

		check_point(31);
		ercd = chg_spr(TASK2, 20);
		check_ercd(ercd, E_OK);

		check_point(34);
		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_OK);

		ercd = act_tsk(TASK6);
		check_ercd(ercd, E_OK);

		ercd = act_tsk(TASK5);
		check_ercd(ercd, E_OK);

		ercd = slp_tsk();
		check_ercd(ercd, E_OK);

		check_point(38);
		ercd = chg_pri(TASK5, MID_PRIORITY);
		check_ercd(ercd, E_OK);

		ercd = unl_mtx(MTX1);
		check_ercd(ercd, E_OK);

		check_point(42);
		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_OK);

		ercd = act_tsk(TASK3);
		check_ercd(ercd, E_OK);

		ercd = slp_tsk();
		check_ercd(ercd, E_OK);

		check_point(48);
		ercd = chg_pri(TASK6, MID_PRIORITY);
		check_ercd(ercd, E_OK);

		check_point(49);
		ercd = unl_mtx(MTX1);
		check_ercd(ercd, E_OK);

		check_point(54);
		ercd = chg_spr(TASK4, 10);
		check_ercd(ercd, E_OK);

		check_point(56);
		ercd = wup_tsk(TASK1);
		check_ercd(ercd, E_OK);

		check_point(60);
		ercd = wup_tsk(TASK1);
		check_ercd(ercd, E_OK);

		check_point(63);
		ercd = act_tsk(TASK5);
		check_ercd(ercd, E_OK);

		ercd = act_tsk(TASK6);
		check_ercd(ercd, E_OK);

		ercd = chg_spr(TASK6, 0);
		check_ercd(ercd, E_OK);

		check_point(64);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 3:
		check_point(69);
		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_OK);

		check_point(74);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	default:
		check_assert(false);
	}
	check_assert(false);
}

static uint_t	task3_count = 0;

void
task3(EXINF exinf)
{
	ER_UINT	ercd;

	switch (++task3_count) {
	case 1:
		check_point(25);
		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_OK);

		check_point(32);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 2:
		check_point(43);
		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_OK);

		check_point(50);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 3:
		check_point(67);
		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_OK);

		check_point(72);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	default:
		check_assert(false);
	}
	check_assert(false);
}

static uint_t	task4_count = 0;

void
task4(EXINF exinf)
{
	ER_UINT	ercd;

	switch (++task4_count) {
	case 1:
		check_point(26);
		ercd = wup_tsk(TASK2);
		check_ercd(ercd, E_OK);

		check_point(33);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 2:
		check_point(46);
		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_OK);

		check_point(52);
		ercd = unl_mtx(MTX1);
		check_ercd(ercd, E_OK);

		check_point(53);
		ercd = chg_spr(TASK4, 20);
		check_ercd(ercd, E_OK);

		check_point(55);
		ercd = chg_spr(TASK4, 30);
		check_ercd(ercd, E_OK);

		check_point(58);
		ercd = wup_tsk(TASK1);
		check_ercd(ercd, E_OK);

		check_point(62);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 3:
		check_point(68);
		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_OK);

		check_point(73);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	default:
		check_assert(false);
	}
	check_assert(false);
}

static uint_t	task5_count = 0;

void
task5(EXINF exinf)
{
	ER_UINT	ercd;

	switch (++task5_count) {
	case 1:
		check_point(4);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 2:
		check_point(8);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 3:
		check_point(13);
		ercd = chg_pri(TASK5, TPRI_INI);
		check_ercd(ercd, E_OK);

		ercd = act_tsk(TASK2);
		check_ercd(ercd, E_OK);

		check_point(20);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 4:
		check_point(36);
		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_OK);

		check_point(39);
		ercd = unl_mtx(MTX1);
		check_ercd(ercd, E_OK);

		check_point(41);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 5:
		check_point(65);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	default:
		check_assert(false);
	}
	check_assert(false);
}

static uint_t	task6_count = 0;

void
task6(EXINF exinf)
{
	ER_UINT	ercd;

	switch (++task6_count) {
	case 1:
		check_point(35);
		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_OK);

		check_point(40);
		ercd = unl_mtx(MTX1);
		check_ercd(ercd, E_OK);

		check_point(44);
		ercd = loc_mtx(MTX2);
		check_ercd(ercd, E_OK);

		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_OK);

		check_point(51);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 2:
		check_point(66);
		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_OK);

		ercd = act_tsk(TASK2);
		check_ercd(ercd, E_OK);

		ercd = act_tsk(TASK3);
		check_ercd(ercd, E_OK);

		ercd = act_tsk(TASK4);
		check_ercd(ercd, E_OK);

		ercd = slp_tsk();
		check_ercd(ercd, E_OK);

		check_point(71);
		ercd = chg_spr(TASK4, 0);
		check_ercd(ercd, E_OK);

		ercd = unl_mtx(MTX1);
		check_ercd(ercd, E_OK);

		check_finish(75);
		check_assert(false);

	default:
		check_assert(false);
	}
	check_assert(false);
}

void
task7(EXINF exinf)
{
	ER_UINT	ercd;

	check_point(37);
	ercd = wup_tsk(TASK2);
	check_ercd(ercd, E_OK);

	check_point(45);
	ercd = act_tsk(TASK4);
	check_ercd(ercd, E_OK);

	check_point(47);
	ercd = wup_tsk(TASK2);
	check_ercd(ercd, E_OK);

	check_point(70);
	ercd = wup_tsk(TASK6);
	check_ercd(ercd, E_OK);

	check_assert(false);
}
