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
 *  $Id: test_inherit7.c 1619 2022-09-19 02:57:56Z ertl-hiro $
 */

/* 
 *		優先度継承ミューテックスのテスト(7)
 *
 * 【テストの目的】
 *
 *  優先度継承ミューテックスに関連するchg_priによる優先度制御について
 *  テストする．
 *
 * 【テスト項目】
 *
 *	(A) chg_priの対象タスクが自タスクの場合
 *		(A-1) chg_priを発行した結果，自タスクの現在優先度が高くなる場
 *			  合に，同優先度内で最低優先順位になること［NGKI1194］．
 *		(A-2) chg_priを発行した結果，自タスクの現在優先度が低くなる場
 *			  合に，同優先度内で最低優先順位になること［NGKI1194］．
 *		(A-3) chg_priを発行した結果，自タスクの現在優先度が変化せず，
 *			  自タスクの現在優先度が上昇状態でない場合に，同優先度内で
 *			  最低優先順位になること［NGKI1194］．
 *		(A-4) chg_priを発行した結果，自タスクの現在優先度が変化せず，
 *			  自タスクの現在優先度が上昇状態である場合に，優先順位が変
 *			  わらないこと［NGKI1197］．
 *	(B) chg_priの対象タスクが実行可能状態の場合
 *		(B-1) chg_priを発行した結果，対象タスクの現在優先度が高くなる
 *			  場合に，同優先度内で最低優先順位になること［NGKI1194］．
 *		(B-2) chg_priを発行した結果，対象タスクの現在優先度が低くなる
 *			  場合に，同優先度内で最低優先順位になること［NGKI1194］．
 *		(B-3) chg_priを発行した結果，対象タスクの現在優先度が変化せず，
 *			  対象タスクの現在優先度が上昇状態でない場合に，同優先度内
 *			  で最低優先順位になること［NGKI1194］．
 *		(B-4) chg_priを発行した結果，対象タスクの現在優先度が変化せず，
 *			  対象タスクの現在優先度が上昇状態である場合に，優先順位が
 *			  変わらないこと［NGKI1197］．
 *	(C) chg_priの対象タスクがTA_NULL属性のミューテックス待ち状態の場合
 *		(C-1) chg_priを発行した結果，対象タスクの現在優先度が高くなる
 *			  場合に，待ち行列中での順序が変更されないこと．
 *		(C-2) chg_priを発行した結果，対象タスクの現在優先度が低くなる
 *			  場合に，待ち行列中での順序が変更されないこと．
 *		(C-3) chg_priを発行した結果，対象タスクの現在優先度が変化せず，
 *			  対象タスクの現在優先度が上昇状態でない場合に，待ち行列中
 *			  での順序が変更されないこと．
 *		(C-4) chg_priを発行した結果，対象タスクの現在優先度が変化せず，
 *			  対象タスクの現在優先度が上昇状態である場合に，待ち行列中
 *			  での順序が変更されないこと．
 *	(D) chg_priの対象タスクが優先度継承ミューテックス待ち状態の場合
 *		(D-1) chg_priを発行した結果，対象タスクの現在優先度が高くなる
 *			  場合に，待ち行列中での順序が変更されること［NGKI1195］．
 *			  また，同優先度内で最後になること［NGKI1196］．
 *		(D-2) (D-1)の結果，推移的な優先度継承が起こること．
 *				 ※ 推移的な優先度継承が起こる状況では，待ち行列中での
 *					同じ優先度内での順序が最後になることは，確認できな
 *					い
 *		(D-3) chg_priを発行した結果，対象タスクの現在優先度が低くなる
 *			  場合に，待ち行列中での順序が変更されること［NGKI1195］．
 *			  また，同優先度内で最後になること［NGKI1196］．
 *		(D-4) (D-3)の結果，推移的な優先度継承が起こること．
 *		(D-5) chg_priを発行した結果，対象タスクの現在優先度が変化せず，
 *			  対象タスクの現在優先度が上昇状態でない場合に，待ち行列中
 *			  の同優先度内で最後になること［NGKI1196］．
 *		(D-6) chg_priを発行した結果，対象タスクの現在優先度が変化せず，
 *			  対象タスクの現在優先度が上昇状態である場合に，待ち行列中
 *			  での順序が変更されないこと．
 *
 * 【テスト項目の実現方法】
 *
 *	(A-1)
 *		中優先度タスク（TASK1）がchg_pri(TASK1,HIGH_PRIORITY)を発行す
 *		ると，優先度が変更されること．実行可能状態の高優先度タスク
 *		（TASK6）を用意しておき，それよりも優先順位が低くなることを確
 *		認する．ディスパッチ禁止状態で実施する．
 *	(A-2)
 *		中優先度タスク（TASK1）がchg_pri(TASK1,LOW_PRIORITY)を発行する
 *		と，優先度が変更されること．実行可能状態の低優先度タスク
 *		（TASK7）を用意しておき，それよりも優先順位が低くなることを確
 *		認する．
 *	(A-3)
 *		中優先度タスク（TASK1）がchg_pri(TASK1,MID_PRIORITY)を発行する．
 *		実行可能状態の別の中低優先度タスク（TASK3）を用意しておき，
 *		TASK1がそれよりも優先順位が低くなることを確認する．
 *	(A-4)
 *		中優先度タスク（TASK1）に優先度継承ミューテックス（MTX1） をロッ
 *		クさせ，別の中優先度タスク（TASK3）をMTX1に対する待ち状態にす
 *		る．TASK1がchg_pri(TASK1,MID_PRIORITY)を発行する．実行可能状態
 *		の別の中低優先度タスク2つ（TASK4，TASK5）を用意しておき，優先
 *		順位が変わらないことを確認する．ディスパッチ禁止状態で実施する．
 *	(B-1)
 *		中優先度タスク（TASK1）が，実行可能状態の別の中優先度タスク
 *		（TASK2）に対してchg_pri(TASK2,HIGH_PRIORITY)を発行すると，
 *		TASK2の優先度が変更されること．実行可能状態の高優先度タスク
 *		（TASK6）を用意しておき，それよりも優先順位が低くなることを確
 *		認する．ディスパッチ禁止状態で実施する．
 *	(B-2)
 *		中優先度タスク（TASK1）が，実行可能状態の別の中優先度タスク
 *		（TASK2）に対してchg_pri(TASK2,LOW_PRIORITY)を発行すると，
 *		TASK2の優先度が変更されること．実行可能状態の低優先度タスク
 *		（TASK7）を用意しておき，それよりも優先順位が低くなることを確
 *		認する．
 *	(B-3)
 *		中優先度タスク（TASK1）が，実行可能状態の別の中優先度タスク
 *		（TASK2）に対してchg_pri(TASK2,MID_PRIORITY)を発行する．実行可
 *		能状態の別の中低優先度タスク（TASK3）を用意しておき，TASK2がそ
 *		れよりも優先順位が低くなることを確認する．
 *	(B-4)
 *		中優先度タスク（TASK2）に優先度継承ミューテックス（MTX1）をロッ
 *		クさせ，別の中優先度タスク（TASK3）をMTX1に対する待ち状態にす
 *		る．別の中優先度タスク（TASK1）がchg_pri(TASK2,MID_PRIORITY)を
 *		発行する．実行可能状態の別の中低優先度タスク2つ（TASK4，TASK5）
 *		を用意しておき，優先順位が変わらないことを確認する．
 *	(C-1)
 *		中優先度タスク（TASK1）に，TA_NULL属性のミューテックス（MTX3）
 *		をロックさせる．中優先度タスク（TASK2）を，MTX3に対する待ち状
 *		態にする．TASK1が，chg_pri(TASK2,HIGH_PRIORITY)を発行する．
 *		MTX3に対する待ち状態の中優先度タスク2つ（TASK4，TASK5）を用意
 *		しておき，TASK2の待ち行列中での順序が変更されないことを確認す
 *		る．
 *	(C-2)
 *		中優先度タスク（TASK1）に，TA_NULL属性のミューテックス（MTX3）
 *		をロックさせる．中優先度タスク（TASK2）を，MTX3に対する待ち状
 *		態にする．TASK1が，chg_pri(TASK2,LOW_PRIORITY)を発行する．MTX3
 *		に対する待ち状態の中優先度タスク2つ（TASK4，TASK5）を用意して
 *		おき，TASK2の待ち行列中での順序が変更されないことを確認する．
 *	(C-3)
 *		中優先度タスク（TASK1）に，TA_NULL属性のミューテックス（MTX3）
 *		をロックさせる．中優先度タスク（TASK2）を，MTX3に対する待ち状
 *		態にする．TASK1が，chg_pri(TASK2,MID_PRIORITY)を発行する．MTX3
 *		に対する待ち状態の中優先度タスク2つ（TASK4，TASK5）を用意して
 *		おき，TASK2の待ち行列中での順序が変更されないことを確認する．
 *	(C-4)
 *		中優先度タスク（TASK2）に優先度継承ミューテックス（MTX1）をロッ
 *		クさせ，別の中優先度タスク（TASK3）をMTX1に対する待ち状態にす
 *		る．別の中優先度タスク（TASK1）に，TA_NULL属性のミューテックス
 *		（MTX3）をロックさせる．中優先度タスク（TASK2）を，MTX3に対す
 *		る待ち状態にする．TASK1が，chg_pri(TASK2,MID_PRIORITY)を発行す
 *		る．MTX3に対する待ち状態の中優先度タスク2つ（TASK4，TASK5）を
 *		用意しておき，TASK2の待ち行列中での順序が変更されないことを確
 *		認する．
 *	(D-1)
 *		中優先度タスク（TASK1）に，優先度継承ミューテックス（MTX2）を
 *		ロックさせる．中優先度タスク（TASK2）を，MTX2に対する待ち状態
 *		にする．TASK1が，chg_pri(TASK2,HIGH_PRIORITY)を発行する．MTX3
 *		に対する待ち状態の中優先度タスク（TASK4）と高優先度タスク
 *		（TASK6）を用意しておき，TASK2の待ち行列中での順序が同優先度内
 *		で最後に変更されることを確認する．
 *	(D-2)
 *		中優先度タスク（TASK1）に，優先度継承ミューテックス（MTX2）を
 *		ロックさせる．中優先度タスク（TASK2）を，MTX2に対する待ち状態
 *		にする．TASK1が，chg_pri(TASK2,HIGH_PRIORITY)を発行する．MTX3
 *		に対する待ち状態の中優先度タスク（TASK4）を用意しておき，TASK2
 *		の待ち行列中での順序が変更されることを確認する．また，TASK1の
 *		現在優先度が変更されることを確認する．
 *	(D-3)
 *		中優先度タスク（TASK1）に，優先度継承ミューテックス（MTX2）を
 *		ロックさせる．中優先度タスク（TASK2）を，MTX2に対する待ち状態
 *		にする．TASK1が，chg_pri(TASK2,LOW_PRIORITY)を発行する．MTX3に
 *		対する待ち状態の低優先度タスク（TASK7）を用意しておき，TASK2の
 *		待ち行列中での順序が同優先度内で最後に変更されることを確認する．
 *	(D-4)
 *		低優先度タスク（TASK1を低優先度にして用いる）に，優先度継承ミュー
 *		テックス（MTX2）をロックさせる．中優先度タスク（TASK2）を，MTX2
 *		に対する待ち状態にする．TASK1が，chg_pri(TASK2,LOW_PRIORITY)
 *		を発行する．MTX3に対する待ち状態の低優先度タスク（TASK6）を用
 *		意しておき，TASK2の待ち行列中での順序が同優先度内で最後に変更
 *		されることを確認する．また，TASK1の現在優先度が変更されること
 *		を確認する．
 *	(D-5)
 *		中優先度タスク（TASK1）に，優先度継承ミューテックス（MTX2）を
 *		ロックさせる．中優先度タスク（TASK2）を，MTX2に対する待ち状態
 *		にする．TASK1が，chg_pri(TASK2,MID_PRIORITY)を発行する．MTX3に
 *		対する待ち状態の中優先度タスク（TASK4）を用意しておき，TASK2の
 *		待ち行列中での順序が同優先度内で最後に変更されることを確認する．
 *	(D-6)
 *		中優先度タスク（TASK2）に優先度継承ミューテックス（MTX1）をロッ
 *		クさせ，別の中優先度タスク（TASK3）をMTX1に対する待ち状態にす
 *		る．別の中優先度タスク（TASK1）に，優先度継承ミューテックス
 *		（MTX2）をロックさせる．中優先度タスク（TASK2）を，MTX2に対す
 *		る待ち状態にする．TASK1が，chg_pri(TASK2,MID_PRIORITY)を発行す
 *		る．MTX1に対する待ち状態の中優先度タスク2つ（TASK4，TASK5）を
 *		用意しておき，TASK2の待ち行列中での順序が変更されないことを確
 *		認する．
 *
 * 【使用リソース】
 *
 *	TASK1: 中優先度タスク，メインタスク，最初から起動
 *	TASK2: 中優先度タスク
 *	TASK3: 中優先度タスク
 *	TASK4: 中優先度タスク
 *	TASK5: 中優先度タスク
 *	TASK6: 高優先度タスク
 *	TASK7: 低優先度タスク
 *	MTX1: ミューテックス（優先度継承）
 *	MTX2: ミューテックス（優先度継承）
 *	MTX3: ミューテックス（TA_NULL属性）
 *
 * 【テストシーケンス】
 *
 *	== TASK1（優先度：中）==
 *	//		中：TASK1
 *	1:	dis_dsp()
 *		act_tsk(TASK6)
 *	//		高：TASK6，中：TASK1
 *		chg_pri(TASK1, HIGH_PRIORITY)					... (A-1)
 *	//		高：TASK6→TASK1
 *		ref_tsk(TASK1, &rtsk)
 *		assert(rtsk.tskpri == HIGH_PRIORITY)
 *		ena_dsp()
 *	== TASK6-1（優先度：高）==
 *	2:	ext_tsk()
 *	//		高：TASK1
 *	== TASK1（続き）==
 *	3:	chg_pri(TASK1, TPRI_INI)
 *	//		中：TASK1
 *
 *	== TASK1（続き）==
 *	4:	act_tsk(TASK7)
 *	//		中：TASK1，低：TASK7
 *		chg_pri(TASK1, LOW_PRIORITY)					... (A-2)
 *	//		低：TASK7→TASK1
 *	== TASK7-1（優先度：低）==
 *	5:	ref_tsk(TASK1, &rtsk)
 *		assert(rtsk.tskpri == LOW_PRIORITY)
 *		ext_tsk()
 *	//		低：TASK1
 *	== TASK1（続き）==
 *	6:	chg_pri(TASK1, TPRI_INI)
 *	//		中：TASK1
 *
 *	== TASK1（続き）==
 *	7:	act_tsk(TASK3)
 *	//		中：TASK1→TASK3
 *		chg_pri(TASK1, MID_PRIORITY)					... (A-3)
 *	//		低：TASK3→TASK1
 *	== TASK3-1（優先度：中）==
 *	8:	ref_tsk(TASK1, &rtsk)
 *		assert(rtsk.tskpri == MID_PRIORITY)
 *		ext_tsk()
 *	//		中：TASK1
 *
 *	== TASK1（続き）==
 *	9:	loc_mtx(MTX1)
 *	//		中：TASK1，MTX1：TASK1
 *		act_tsk(TASK3)
 *	//		中：TASK1→TASK3，MTX1：TASK1
 *		rot_rdq(MID_PRIORITY)
 *	//		中：TASK3→TASK1，MTX1：TASK1
 *	== TASK3-2（優先度：中）==
 *	10:	loc_mtx(MTX1)
 *	//		中：TASK1，MTX1：TASK1→TASK3
 *	== TASK1（続き）==
 *	11:	dis_dsp()
 *		act_tsk(TASK4)
 *	//		中：TASK1→TASK4，MTX1：TASK1→TASK3
 *		rot_rdq(MID_PRIORITY)
 *	//		中：TASK4→TASK1，MTX1：TASK1→TASK3
 *		act_tsk(TASK5)
 *	//		中：TASK4→TASK1→TASK5，MTX1：TASK1→TASK3
 *		chg_pri(TASK1, MID_PRIORITY)					... (A-4)
 *	//		中：TASK4→TASK1→TASK5，MTX1：TASK1→TASK3
 *		ref_tsk(TASK1, &rtsk)
 *		assert(rtsk.tskpri == MID_PRIORITY)
 *		ena_dsp()
 *	== TASK4-1（優先度：中）==
 *	12:	ext_tsk()
 *	//		中：TASK1→TASK5，MTX1：TASK1→TASK3
 *	== TASK1（続き）==
 *	13:	unl_mtx(MTX1)
 *	//		中：TASK1→TASK5→TASK3，MTX1：TASK3
 *		rot_rdq(MID_PRIORITY)
 *	//		中：TASK5→TASK3→TASK1，MTX1：TASK3
 *	== TASK5-1（優先度：中）==
 *	14:	ext_tsk()
 *	//		中：TASK3→TASK1，MTX1：TASK3
 *	== TASK3-2（続き）==
 *	15:	ext_tsk()
 *	//		中：TASK1
 *
 *	== TASK1（続き）==
 *	16:	act_tsk(TASK2)
 *	//		中：TASK1→TASK2
 *		dis_dsp()
 *		act_tsk(TASK6)
 *	//		高：TASK6，中：TASK1→TASK2
 *		chg_pri(TASK2, HIGH_PRIORITY)					... (B-1)
 *	//		高：TASK6→TASK2，中：TASK1
 *		ref_tsk(TASK2, &rtsk)
 *		assert(rtsk.tskpri == HIGH_PRIORITY)
 *		ena_dsp()
 *	== TASK6-2（優先度：高）==
 *	17:	ext_tsk()
 *	//		高：TASK2，中：TASK1
 *	== TASK2-1（優先度：高）==
 *	18:	ext_tsk()
 *	//		中：TASK1
 *
 *	== TASK1（続き）==
 *	19:	act_tsk(TASK2)
 *	//		中：TASK1→TASK2
 *		act_tsk(TASK7)
 *	//		中：TASK1→TASK2，低：TASK7
 *		chg_pri(TASK2, LOW_PRIORITY)					... (B-2)
 *	//		中：TASK1，低：TASK7→TASK2
 *		ref_tsk(TASK2, &rtsk)
 *		assert(rtsk.tskpri == LOW_PRIORITY)
 *		chg_pri(TASK1, LOW_PRIORITY)
 *	//		低：TASK7→TASK2→TASK1
 *	== TASK7-2（優先度：低）==
 *	20:	ext_tsk()
 *	//		低：TASK2→TASK1
 *	== TASK2-2（続き）==
 *	21:	ext_tsk()
 *	//		低：TASK1
 *	== TASK1（続き）==
 *	22:	chg_pri(TASK1, TPRI_INI)
 *	//		中：TASK1
 *
 *	== TASK1（続き）==
 *	23:	act_tsk(TASK2)
 *	//		中：TASK1→TASK2
 *		act_tsk(TASK3)
 *	//		中：TASK1→TASK2→TASK3
 *		chg_pri(TASK2, MID_PRIORITY)					... (B-3)
 *	//		中：TASK1→TASK3→TASK2
 *		ref_tsk(TASK2, &rtsk)
 *		assert(rtsk.tskpri == MID_PRIORITY)
 *		rot_rdq(MID_PRIORITY)
 *	//		中：TASK3→TASK2→TASK1
 *	== TASK3-3（優先度：中）==
 *	24:	ext_tsk()
 *	//		中：TASK2→TASK1
 *
 *	== TASK2-3（優先度：中）==
 *	25:	loc_mtx(MTX1)
 *	//		中：TASK2→TASK1，MTX1：TASK2
 *		rot_rdq(MID_PRIORITY)
 *	//		中：TASK1→TASK2，MTX1：TASK2
 *	== TASK1（続き）==
 *	26:	act_tsk(TASK3)
 *	//		中：TASK1→TASK2→TASK3，MTX1：TASK2
 *		rot_rdq(MID_PRIORITY)
 *	//		中：TASK2→TASK3→TASK1，MTX1：TASK2
 *	== TASK2-3（続き）==
 *	27:	act_tsk(TASK4)
 *	//		中：TASK2→TASK3→TASK1→TASK4，MTX1：TASK2
 *		rot_rdq(MID_PRIORITY)
 *	//		中：TASK3→TASK1→TASK4→TASK2，MTX1：TASK2
 *	== TASK3-4（優先度：中）==
 *	28:	act_tsk(TASK5)
 *	//		中：TASK3→TASK1→TASK4→TASK2→TASK5，MTX1：TASK2
 *		loc_mtx(MTX1)
 *	//		中：TASK1→TASK4→TASK2→TASK5，MTX1：TASK2→TASK3
 *	== TASK1（続き）==
 *	29:	chg_pri(TASK2, MID_PRIORITY)					... (B-4)
 *	//		中：TASK1→TASK4→TASK2→TASK5，MTX1：TASK2→TASK3
 *		rot_rdq(MID_PRIORITY)
 *	//		中：TASK4→TASK2→TASK5→TASK1，MTX1：TASK2→TASK3
 *	== TASK4-2（優先度：中）==
 *	30:	ext_tsk()
 *	//		中：TASK2→TASK5→TASK1，MTX1：TASK2→TASK3
 *	== TASK2-3（続き）==
 *	31:	ext_tsk()
 *	//		中：TASK5→TASK1→TASK3，MTX1：TASK3
 *	== TASK5-2（優先度：中）==
 *	32:	ext_tsk()
 *	//		中：TASK1→TASK3，MTX1：TASK3
 *	== TASK1（続き）==
 *	33:	rot_rdq(MID_PRIORITY)
 *	//		中：TASK3→TASK1，MTX1：TASK3
 *	== TASK3-4（続き）==
 *	34:	ext_tsk()
 *	//		中：TASK1
 *
 *	== TASK1（続き）==
 *	35:	loc_mtx(MTX3)
 *	//		中：TASK1，MTX3：TASK1
 *		act_tsk(TASK4)
 *	//		中：TASK1→TASK4，MTX3：TASK1
 *		act_tsk(TASK2)
 *	//		中：TASK1→TASK4→TASK2，MTX3：TASK1
 *		act_tsk(TASK5)
 *	//		中：TASK1→TASK4→TASK2→TASK5，MTX3：TASK1
 *		rot_rdq(MID_PRIORITY)
 *	//		中：TASK4→TASK2→TASK5→TASK1，MTX3：TASK1
 *	== TASK4-3（優先度：中）==
 *	36:	loc_mtx(MTX3)
 *	//		中：TASK2→TASK5→TASK1，MTX3：TASK1→TASK4
 *	== TASK2-4（優先度：中）==
 *	37:	loc_mtx(MTX3)
 *	//		中：TASK5→TASK1，MTX3：TASK1→TASK4→TASK2
 *	== TASK5-3（優先度：中）==
 *	38:	loc_mtx(MTX3)
 *	//		中：TASK1，MTX3：TASK1→TASK4→TASK2→TASK5
 *	== TASK1（続き）==
 *	39:	chg_pri(TASK2, HIGH_PRIORITY)					... (C-1)
 *	//		中：TASK1，MTX3：TASK1→TASK4→TASK2→TASK5
 *		ref_tsk(TASK2, &rtsk)
 *		assert(rtsk.tskpri == HIGH_PRIORITY)
 *		chg_pri(TASK1, LOW_PRIORITY)
 *	//		低：TASK1，MTX3：TASK1→TASK4→TASK2→TASK5
 *		unl_mtx(MTX3)
 *	//		中：TASK4，低：TASK1，MTX3：TASK4→TASK2→TASK5
 *	== TASK4-3（続き）==
 *	40:	ext_tsk()
 *	//		高：TASK2，低：TASK1，MTX3：TASK2→TASK5
 *	== TASK2-4（続き）==
 *	41:	ext_tsk()
 *	//		中：TASK5，低：TASK1，MTX3：TASK5
 *	== TASK5-3（続き）==
 *	42:	ext_tsk()
 *	//		低：TASK1
 *	== TASK1（続き）==
 *	43:	chg_pri(TASK1, TPRI_INI)
 *	//		中：TASK1
 *
 *	== TASK1（続き）==
 *	44:	loc_mtx(MTX3)
 *	//		中：TASK1，MTX3：TASK1
 *		act_tsk(TASK4)
 *	//		中：TASK1→TASK4，MTX3：TASK1
 *		act_tsk(TASK2)
 *	//		中：TASK1→TASK4→TASK2，MTX3：TASK1
 *		act_tsk(TASK5)
 *	//		中：TASK1→TASK4→TASK2→TASK5，MTX3：TASK1
 *		rot_rdq(MID_PRIORITY)
 *	//		中：TASK4→TASK2→TASK5→TASK1，MTX3：TASK1
 *	== TASK4-4（優先度：中）==
 *	45:	loc_mtx(MTX3)
 *	//		中：TASK2→TASK5→TASK1，MTX3：TASK1→TASK4
 *	== TASK2-5（優先度：中）==
 *	46:	loc_mtx(MTX3)
 *	//		中：TASK5→TASK1，MTX3：TASK1→TASK4→TASK2
 *	== TASK5-4（優先度：中）==
 *	47:	loc_mtx(MTX3)
 *	//		中：TASK1，MTX3：TASK1→TASK4→TASK2→TASK5
 *	== TASK1（続き）==
 *	48:	chg_pri(TASK2, LOW_PRIORITY)					... (C-2)
 *	//		中：TASK1，MTX3：TASK1→TASK4→TASK2→TASK5
 *		ref_tsk(TASK2, &rtsk)
 *		assert(rtsk.tskpri == LOW_PRIORITY)
 *		chg_pri(TASK1, LOW_PRIORITY)
 *	//		低：TASK1，MTX3：TASK1→TASK4→TASK2→TASK5
 *		unl_mtx(MTX3)
 *	//		中：TASK4，低：TASK1，MTX3：TASK4→TASK2→TASK5
 *	== TASK4-4（続き）==
 *	49:	ext_tsk()
 *	//		低：TASK1→TASK2，MTX3：TASK2→TASK5
 *	== TASK1（続き）==
 *	50:	chg_pri(TASK1, LOW_PRIORITY)
 *	//		低：TASK2→TASK1，MTX3：TASK2→TASK5
 *	== TASK2-5（続き）==
 *	51:	ext_tsk()
 *	//		中：TASK5，低：TASK1，MTX3：TASK5
 *	== TASK5-4（続き）==
 *	52:	ext_tsk()
 *	//		低：TASK1
 *	== TASK1（続き）==
 *	53:	chg_pri(TASK1, TPRI_INI)
 *	//		中：TASK1
 *
 *	54:	loc_mtx(MTX3)
 *	//		中：TASK1，MTX3：TASK1
 *		act_tsk(TASK4)
 *	//		中：TASK1→TASK4，MTX3：TASK1
 *		act_tsk(TASK2)
 *	//		中：TASK1→TASK4→TASK2，MTX3：TASK1
 *		act_tsk(TASK5)
 *	//		中：TASK1→TASK4→TASK2→TASK5，MTX3：TASK1
 *		rot_rdq(MID_PRIORITY)
 *	//		中：TASK4→TASK2→TASK5→TASK1，MTX3：TASK1
 *	== TASK4-5（優先度：中）==
 *	55:	loc_mtx(MTX3)
 *	//		中：TASK2→TASK5→TASK1，MTX3：TASK1→TASK4
 *	== TASK2-6（優先度：中）==
 *	56:	loc_mtx(MTX3)
 *	//		中：TASK5→TASK1，MTX3：TASK1→TASK4→TASK2
 *	== TASK5-5（優先度：中）==
 *	57:	loc_mtx(MTX3)
 *	//		中：TASK1，MTX3：TASK1→TASK4→TASK2→TASK5
 *	== TASK1（続き）==
 *	58:	chg_pri(TASK2, MID_PRIORITY)					... (C-3)
 *	//		中：TASK1，MTX3：TASK1→TASK4→TASK2→TASK5
 *		ref_tsk(TASK2, &rtsk)
 *		assert(rtsk.tskpri == MID_PRIORITY)
 *		chg_pri(TASK1, LOW_PRIORITY)
 *	//		低：TASK1，MTX3：TASK1→TASK4→TASK2→TASK5
 *		unl_mtx(MTX3)
 *	//		中：TASK4，低：TASK1，MTX3：TASK4→TASK2→TASK5
 *	== TASK4-5（続き）==
 *	59:	ext_tsk()
 *	//		中：TASK2，低：TASK1，MTX3：TASK2→TASK5
 *	== TASK2-6（続き）==
 *	60:	ext_tsk()
 *	//		中：TASK5，低：TASK1，MTX3：TASK5
 *	== TASK5-5（続き）==
 *	61:	ext_tsk()
 *	//		低：TASK1
 *	== TASK1（続き）==
 *	62:	chg_pri(TASK1, TPRI_INI)
 *	//		中：TASK1
 *
 *	63:	loc_mtx(MTX3)
 *	//		中：TASK1，MTX3：TASK1
 *		act_tsk(TASK4)
 *	//		中：TASK1→TASK4，MTX3：TASK1
 *		act_tsk(TASK2)
 *	//		中：TASK1→TASK4→TASK2，MTX3：TASK1
 *		act_tsk(TASK3)
 *	//		中：TASK1→TASK4→TASK2→TASK3，MTX3：TASK1
 *		act_tsk(TASK5)
 *	//		中：TASK1→TASK4→TASK2→TASK3→TASK5，MTX3：TASK1
 *		rot_rdq(MID_PRIORITY)
 *	//		中：TASK4→TASK2→TASK3→TASK5→TASK1，MTX3：TASK1
 *	== TASK4-6（優先度：中）==
 *	64:	loc_mtx(MTX3)
 *	//		中：TASK2→TASK3→TASK5→TASK1，MTX3：TASK1→TASK4
 *	== TASK2-7（優先度：中）==
 *	65:	loc_mtx(MTX1)
 *	//		中：TASK2→TASK3→TASK5→TASK1，MTX1：TASK2，MTX3：TASK1→TASK4
 *		loc_mtx(MTX3)
 *	//		中：TASK3→TASK5→TASK1，MTX1：TASK2，MTX3：TASK1→TASK4→TASK2
 *	== TASK3-5（優先度：中）==
 *	66:	loc_mtx(MTX1)
 *	//		中：TASK5→TASK1，MTX1：TASK2→TASK3，MTX3：TASK1→TASK4→TASK2
 *	== TASK5-6（優先度：中）==
 *	67:	loc_mtx(MTX3)
 *	//		中：TASK1，MTX1：TASK2→TASK3，MTX3：TASK1→TASK4→TASK2→TASK5
 *	== TASK1（続き）==
 *	68:	chg_pri(TASK2, MID_PRIORITY)					... (C-4)
 *	//		中：TASK1，MTX1：TASK2→TASK3，MTX3：TASK1→TASK4→TASK2→TASK5
 *		ref_tsk(TASK2, &rtsk)
 *		assert(rtsk.tskpri == MID_PRIORITY)
 *		chg_pri(TASK1, LOW_PRIORITY)
 *	//		低：TASK1，MTX1：TASK2→TASK3，MTX3：TASK1→TASK4→TASK2→TASK5
 *		unl_mtx(MTX3)
 *	//		中：TASK4，低：TASK1，MTX1：TASK2→TASK3，MTX3：TASK4→TASK2→TASK5
 *	== TASK4-6（続き）==
 *	69:	ext_tsk()
 *	//		中：TASK2，低：TASK1，MTX1：TASK2→TASK3，MTX3：TASK2→TASK5
 *	== TASK2-7（続き）==
 *	70:	ext_tsk()
 *	//		中：TASK5→TASK3，低：TASK1，MTX1：TASK3，MTX3：TASK5
 *	== TASK5-6（続き）==
 *	71:	ext_tsk()
 *	//		中：TASK3，低：TASK1，MTX1：TASK3
 *	== TASK3-5（続き）==
 *	72:	ext_tsk()
 *	//		低：TASK1
 *	== TASK1（続き）==
 *	73:	chg_pri(TASK1, TPRI_INI)
 *	//		中：TASK1
 *
 *	74:	loc_mtx(MTX2)
 *	//		中：TASK1，MTX2：TASK1
 *		act_tsk(TASK4)
 *	//		中：TASK1→TASK4，MTX2：TASK1
 *		act_tsk(TASK2)
 *	//		中：TASK1→TASK4→TASK2，MTX2：TASK1
 *		rot_rdq(MID_PRIORITY)
 *	//		中：TASK4→TASK2→TASK1，MTX2：TASK1
 *	== TASK4-7（優先度：中）==
 *	75:	loc_mtx(MTX2)
 *	//		中：TASK2→TASK1，MTX2：TASK1→TASK4
 *	== TASK2-8（優先度：中）==
 *	76:	loc_mtx(MTX2)
 *	//		中：TASK1，MTX2：TASK1→TASK4→TASK2
 *	== TASK1（続き）==
 *	77:	act_tsk(TASK6)
 *	//		高：TASK6，中：TASK1，MTX2：TASK1→TASK4→TASK2
 *	== TASK6-3（優先度：高）==
 *	78:	loc_mtx(MTX2)
 *	//		高：TASK1，MTX2：TASK1→TASK6→TASK4→TASK2
 *	== TASK1（続き）==
 *	79:	chg_pri(TASK2, HIGH_PRIORITY)					... (D-1)
 *	//		高：TASK1，MTX2：TASK1→TASK6→TASK4→TASK2
 *		ref_tsk(TASK2, &rtsk)
 *		assert(rtsk.tskpri == HIGH_PRIORITY)
 *		unl_mtx(MTX2)
 *	//		高：TASK6，中：TASK1，MTX2：TASK6→TASK2→TASK4
 *	== TASK6-3（続き）==
 *	80:	ext_tsk()
 *	//		高：TASK2，中：TASK1，MTX2：TASK2→TASK4
 *	== TASK2-8（続き）==
 *	81:	ext_tsk()
 *	//		中：TASK1→TASK4，MTX2：TASK4
 *	== TASK1（続き）==
 *	82:	chg_pri(TASK1, LOW_PRIORITY)
 *	//		中：TASK4，低：TASK1，MTX2：TASK4
 *	== TASK4-7（続き）==
 *	83:	ext_tsk()
 *	//		低：TASK1
 *	== TASK1（続き）==
 *	84:	chg_pri(TASK1, TPRI_INI)
 *	//		中：TASK1
 *
 *	85:	loc_mtx(MTX2)
 *	//		中：TASK1，MTX2：TASK1
 *		act_tsk(TASK4)
 *	//		中：TASK1→TASK4，MTX2：TASK1
 *		act_tsk(TASK2)
 *	//		中：TASK1→TASK4→TASK2，MTX2：TASK1
 *		rot_rdq(MID_PRIORITY)
 *	//		中：TASK4→TASK2→TASK1，MTX2：TASK1
 *	== TASK4-8（優先度：中）==
 *	86:	loc_mtx(MTX2)
 *	//		中：TASK2→TASK1，MTX2：TASK1→TASK4
 *	== TASK2-9（優先度：中）==
 *	87:	loc_mtx(MTX2)
 *	//		中：TASK1，MTX2：TASK1→TASK4→TASK2
 *	== TASK1（続き）==
 *	88:	chg_pri(TASK2, HIGH_PRIORITY)					... (D-2)
 *	//		高：TASK1，MTX2：TASK1→TASK2→TASK4
 *	//		優先度継承により，TASK1とTASK2が高優先度になる。
 *		ref_tsk(TASK2, &rtsk)
 *		assert(rtsk.tskpri == HIGH_PRIORITY)
 *		ref_tsk(TASK1, &rtsk)
 *		assert(rtsk.tskpri == HIGH_PRIORITY)
 *		unl_mtx(MTX2)
 *	//		高：TASK2，中：TASK1，MTX2：TASK2→TASK4
 *	== TASK2-9（続き）==
 *	89:	ext_tsk()
 *	//		中：TASK1→TASK4，MTX2：TASK4
 *	== TASK1（続き）==
 *	90:	chg_pri(TASK1, LOW_PRIORITY)
 *	//		中：TASK4，低：TASK1，MTX2：TASK4
 *	== TASK4-8（続き）==
 *	91:	ext_tsk()
 *	//		低：TASK1
 *	== TASK1（続き）==
 *	92:	chg_pri(TASK1, TPRI_INI)
 *	//		中：TASK1
 *
 *	93:	loc_mtx(MTX2)
 *	//		中：TASK1，MTX2：TASK1
 *		act_tsk(TASK7)
 *	//		中：TASK1，低：TASK7，MTX2：TASK1
 *		chg_pri(TASK1, LOW_PRIORITY)
 *	//		低：TASK7→TASK1，MTX2：TASK1
 *	== TASK7-3（優先度：低）==
 *	94:	loc_mtx(MTX2)
 *	//		低：TASK1，MTX2：TASK1→TASK7
 *	== TASK1（続き）==
 *	95:	act_tsk(TASK2)
 *	//		中：TASK2，低：TASK1，MTX2：TASK1→TASK7
 *	== TASK2-10（優先度：中）==
 *	96:	chg_pri(TASK1, TPRI_INI)
 *	//		中： TASK2→TASK1，MTX2：TASK1→TASK7
 *		loc_mtx(MTX2)
 *	//		中：TASK1，MTX2：TASK1→TASK2→TASK7
 *	== TASK1（続き）==
 *	97:	chg_pri(TASK2, LOW_PRIORITY)					... (D-3)
 *	//		中：TASK1，MTX2：TASK1→TASK7→TASK2
 *		ref_tsk(TASK2, &rtsk)
 *		assert(rtsk.tskpri == LOW_PRIORITY)
 *		unl_mtx(MTX2)
 *	//		中：TASK1，低：TASK7，MTX2：TASK7→TASK2
 *		slp_tsk()
 *	//		低：TASK7，MTX2：TASK7→TASK2
 *	== TASK7-3（続き）==
 *		ext_tsk()
 *	//		低：TASK2，MTX2：TASK2
 *	== TASK2-10（続き）==
 *	98:	chg_pri(TASK2, TPRI_INI)
 *	//		中：TASK2，MTX2：TASK2
 *		wup_tsk(TASK1)
 *	//		中：TASK2→TASK1，MTX2：TASK2
 *		ext_tsk()
 *	//		中：TASK1
 *
 *	== TASK1（続き）==
 *	99:	loc_mtx(MTX2)
 *	//		中：TASK1，MTX2：TASK1
 *		act_tsk(TASK7)
 *	//		中：TASK1，低：TASK7，MTX2：TASK1
 *		chg_pri(TASK1, LOW_PRIORITY)
 *	//		低：TASK7→TASK1，MTX2：TASK1
 *	== TASK7-4（優先度：低）==
 *	100:loc_mtx(MTX2)
 *	//		低：TASK1，MTX2：TASK1→TASK7
 *	== TASK1（続き）==
 *	101:act_tsk(TASK2)
 *	//		中：TASK2，低：TASK1，MTX2：TASK1→TASK7
 *	== TASK2-11（優先度：中）==
 *	102:loc_mtx(MTX2)
 *	//		中：TASK1，MTX2：TASK1→TASK2→TASK7
 *	== TASK1（続き）==
 *	103:chg_pri(TASK2, LOW_PRIORITY)					... (D-4)
 *	//		低：TASK1，MTX2：TASK1→TASK7→TASK2
 *		ref_tsk(TASK2, &rtsk)
 *		assert(rtsk.tskpri == LOW_PRIORITY)
 *		ref_tsk(TASK1, &rtsk)
 *		assert(rtsk.tskpri == LOW_PRIORITY)
 *		unl_mtx(MTX2)
 *	//		低：TASK1→TASK7，MTX2：TASK7→TASK2
 *		slp_tsk()
 *	//		低：TASK7，MTX2：TASK7→TASK2
 *	== TASK7-4（続き）==
 *	104:ext_tsk()
 *	//		低：TASK2，MTX2：TASK2
 *	== TASK2-11（続き）==
 *	105:wup_tsk(TASK1)
 *	//		低：TASK2→TASK1，MTX2：TASK2
 *		ext_tsk()
 *	//		低：TASK1
 *	== TASK1（続き）==
 *	106:chg_pri(TASK1, TPRI_INI)
 *	//		中：TASK1
 *
 *	107:loc_mtx(MTX2)
 *	//		中：TASK1，MTX2：TASK1
 *		act_tsk(TASK2)
 *	//		中：TASK1→TASK2，MTX2：TASK1
 *		act_tsk(TASK4)
 *	//		中：TASK1→TASK2→TASK4，MTX2：TASK1
 *		rot_rdq(MID_PRIORITY)
 *	//		中：TASK2→TASK4→TASK1，MTX2：TASK1
 *	== TASK2-12（優先度：中）==
 *	108:loc_mtx(MTX2)
 *	//		中：TASK4→TASK1，MTX2：TASK1→TASK2
 *	== TASK4-9（優先度：中）==
 *	109:loc_mtx(MTX2)
 *	//		中：TASK1，MTX2：TASK1→TASK2→TASK4
 *	== TASK1（続き）==
 *	110:chg_pri(TASK2, MID_PRIORITY)					... (D-5)
 *	//		中：TASK1，MTX2：TASK1→TASK4→TASK2
 *		ref_tsk(TASK2, &rtsk)
 *		assert(rtsk.tskpri == MID_PRIORITY)
 *		unl_mtx(MTX2)
 *	//		中：TASK1→TASK4，MTX2：TASK4→TASK2
 *		chg_pri(TASK1, LOW_PRIORITY)
 *	//		中：TASK4，低：TASK1，MTX2：TASK4→TASK2
 *	== TASK4-9（続き）==
 *	111:ext_tsk()
 *	//		中：TASK2，低：TASK1，MTX2：TASK2
 *	== TASK2-12（続き）==
 *	112:ext_tsk()
 *	//		低：TASK1
 *	== TASK1（続き）==
 *	113:chg_pri(TASK1, TPRI_INI)
 *	//		中：TASK1
 *
 *	114:loc_mtx(MTX2)
 *	//		中：TASK1，MTX2：TASK1
 *		act_tsk(TASK4)
 *	//		中：TASK1→TASK4，MTX2：TASK1
 *		act_tsk(TASK2)
 *	//		中：TASK1→TASK4→TASK2，MTX2：TASK1
 *		act_tsk(TASK3)
 *	//		中：TASK1→TASK4→TASK2→TASK3，MTX2：TASK1
 *		act_tsk(TASK5)
 *	//		中：TASK1→TASK4→TASK2→TASK3→TASK5，MTX2：TASK1
 *		rot_rdq(MID_PRIORITY)
 *	//		中：TASK4→TASK2→TASK3→TASK5→TASK1，MTX2：TASK1
 *	== TASK4-10（優先度：中）==
 *	115:loc_mtx(MTX2)
 *	//		中：TASK2→TASK3→TASK5→TASK1，MTX2：TASK1→TASK4
 *	== TASK2-13（優先度：中）==
 *	116:loc_mtx(MTX1)
 *	//		中：TASK2→TASK3→TASK5→TASK1，MTX1：TASK2，MTX2：TASK1→TASK4
 *		loc_mtx(MTX2)
 *	//		中：TASK3→TASK5→TASK1，MTX1：TASK2，MTX2：TASK1→TASK4→TASK2
 *	== TASK3-6（優先度：中）==
 *	117:loc_mtx(MTX1)
 *	//		中：TASK5→TASK1，MTX1：TASK2→TASK3，MTX2：TASK1→TASK4→TASK2
 *	== TASK5-7（優先度：中）==
 *	118:loc_mtx(MTX2)
 *	//		中：TASK1，MTX1：TASK2→TASK3，MTX2：TASK1→TASK4→TASK2→TASK5
 *	== TASK1（続き）==
 *	119:chg_pri(TASK2, MID_PRIORITY)					... (D-6)
 *	//		中：TASK1，MTX1：TASK2→TASK3，MTX2：TASK1→TASK4→TASK2→TASK5
 *		ref_tsk(TASK2, &rtsk)
 *		assert(rtsk.tskpri == MID_PRIORITY)
 *		chg_pri(TASK1, LOW_PRIORITY)
 *	//		中：TASK1，MTX1：TASK2→TASK3，MTX2：TASK1→TASK4→TASK2→TASK5
 *		unl_mtx(MTX2)
 *	//		中：TASK4，低：TASK1，MTX1：TASK2→TASK3，MTX2：TASK4→TASK2→TASK5
 *	== TASK4-10（続き）==
 *	120:ext_tsk()
 *	//		中：TASK2，低：TASK1，MTX1：TASK2→TASK3，MTX2：TASK2→TASK5
 *	== TASK2-13（続き）==
 *	121:ext_tsk()
 *	//		中：TASK5→TASK3，低：TASK1，MTX1：TASK3，MTX2：TASK5
 *	== TASK5-7（続き）==
 *	122:ext_tsk()
 *	//		中：TASK3，低：TASK1，MTX1：TASK3
 *	== TASK3-6（続き）==
 *	123:ext_tsk()
 *	//		低：TASK1
 *	== TASK1（続き）==
 *	124:chg_pri(TASK1, TPRI_INI)
 *	//		中：TASK1
 *	125:END
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
	T_RTSK	rtsk;

	test_start(__FILE__);

	check_point(1);
	ercd = dis_dsp();
	check_ercd(ercd, E_OK);

	ercd = act_tsk(TASK6);
	check_ercd(ercd, E_OK);

	ercd = chg_pri(TASK1, HIGH_PRIORITY);
	check_ercd(ercd, E_OK);

	ercd = ref_tsk(TASK1, &rtsk);
	check_ercd(ercd, E_OK);

	check_assert(rtsk.tskpri == HIGH_PRIORITY);

	ercd = ena_dsp();
	check_ercd(ercd, E_OK);

	check_point(3);
	ercd = chg_pri(TASK1, TPRI_INI);
	check_ercd(ercd, E_OK);

	check_point(4);
	ercd = act_tsk(TASK7);
	check_ercd(ercd, E_OK);

	ercd = chg_pri(TASK1, LOW_PRIORITY);
	check_ercd(ercd, E_OK);

	check_point(6);
	ercd = chg_pri(TASK1, TPRI_INI);
	check_ercd(ercd, E_OK);

	check_point(7);
	ercd = act_tsk(TASK3);
	check_ercd(ercd, E_OK);

	ercd = chg_pri(TASK1, MID_PRIORITY);
	check_ercd(ercd, E_OK);

	check_point(9);
	ercd = loc_mtx(MTX1);
	check_ercd(ercd, E_OK);

	ercd = act_tsk(TASK3);
	check_ercd(ercd, E_OK);

	ercd = rot_rdq(MID_PRIORITY);
	check_ercd(ercd, E_OK);

	check_point(11);
	ercd = dis_dsp();
	check_ercd(ercd, E_OK);

	ercd = act_tsk(TASK4);
	check_ercd(ercd, E_OK);

	ercd = rot_rdq(MID_PRIORITY);
	check_ercd(ercd, E_OK);

	ercd = act_tsk(TASK5);
	check_ercd(ercd, E_OK);

	ercd = chg_pri(TASK1, MID_PRIORITY);
	check_ercd(ercd, E_OK);

	ercd = ref_tsk(TASK1, &rtsk);
	check_ercd(ercd, E_OK);

	check_assert(rtsk.tskpri == MID_PRIORITY);

	ercd = ena_dsp();
	check_ercd(ercd, E_OK);

	check_point(13);
	ercd = unl_mtx(MTX1);
	check_ercd(ercd, E_OK);

	ercd = rot_rdq(MID_PRIORITY);
	check_ercd(ercd, E_OK);

	check_point(16);
	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	ercd = dis_dsp();
	check_ercd(ercd, E_OK);

	ercd = act_tsk(TASK6);
	check_ercd(ercd, E_OK);

	ercd = chg_pri(TASK2, HIGH_PRIORITY);
	check_ercd(ercd, E_OK);

	ercd = ref_tsk(TASK2, &rtsk);
	check_ercd(ercd, E_OK);

	check_assert(rtsk.tskpri == HIGH_PRIORITY);

	ercd = ena_dsp();
	check_ercd(ercd, E_OK);

	check_point(19);
	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	ercd = act_tsk(TASK7);
	check_ercd(ercd, E_OK);

	ercd = chg_pri(TASK2, LOW_PRIORITY);
	check_ercd(ercd, E_OK);

	ercd = ref_tsk(TASK2, &rtsk);
	check_ercd(ercd, E_OK);

	check_assert(rtsk.tskpri == LOW_PRIORITY);

	ercd = chg_pri(TASK1, LOW_PRIORITY);
	check_ercd(ercd, E_OK);

	check_point(22);
	ercd = chg_pri(TASK1, TPRI_INI);
	check_ercd(ercd, E_OK);

	check_point(23);
	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	ercd = act_tsk(TASK3);
	check_ercd(ercd, E_OK);

	ercd = chg_pri(TASK2, MID_PRIORITY);
	check_ercd(ercd, E_OK);

	ercd = ref_tsk(TASK2, &rtsk);
	check_ercd(ercd, E_OK);

	check_assert(rtsk.tskpri == MID_PRIORITY);

	ercd = rot_rdq(MID_PRIORITY);
	check_ercd(ercd, E_OK);

	check_point(26);
	ercd = act_tsk(TASK3);
	check_ercd(ercd, E_OK);

	ercd = rot_rdq(MID_PRIORITY);
	check_ercd(ercd, E_OK);

	check_point(29);
	ercd = chg_pri(TASK2, MID_PRIORITY);
	check_ercd(ercd, E_OK);

	ercd = rot_rdq(MID_PRIORITY);
	check_ercd(ercd, E_OK);

	check_point(33);
	ercd = rot_rdq(MID_PRIORITY);
	check_ercd(ercd, E_OK);

	check_point(35);
	ercd = loc_mtx(MTX3);
	check_ercd(ercd, E_OK);

	ercd = act_tsk(TASK4);
	check_ercd(ercd, E_OK);

	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	ercd = act_tsk(TASK5);
	check_ercd(ercd, E_OK);

	ercd = rot_rdq(MID_PRIORITY);
	check_ercd(ercd, E_OK);

	check_point(39);
	ercd = chg_pri(TASK2, HIGH_PRIORITY);
	check_ercd(ercd, E_OK);

	ercd = ref_tsk(TASK2, &rtsk);
	check_ercd(ercd, E_OK);

	check_assert(rtsk.tskpri == HIGH_PRIORITY);

	ercd = chg_pri(TASK1, LOW_PRIORITY);
	check_ercd(ercd, E_OK);

	ercd = unl_mtx(MTX3);
	check_ercd(ercd, E_OK);

	check_point(43);
	ercd = chg_pri(TASK1, TPRI_INI);
	check_ercd(ercd, E_OK);

	check_point(44);
	ercd = loc_mtx(MTX3);
	check_ercd(ercd, E_OK);

	ercd = act_tsk(TASK4);
	check_ercd(ercd, E_OK);

	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	ercd = act_tsk(TASK5);
	check_ercd(ercd, E_OK);

	ercd = rot_rdq(MID_PRIORITY);
	check_ercd(ercd, E_OK);

	check_point(48);
	ercd = chg_pri(TASK2, LOW_PRIORITY);
	check_ercd(ercd, E_OK);

	ercd = ref_tsk(TASK2, &rtsk);
	check_ercd(ercd, E_OK);

	check_assert(rtsk.tskpri == LOW_PRIORITY);

	ercd = chg_pri(TASK1, LOW_PRIORITY);
	check_ercd(ercd, E_OK);

	ercd = unl_mtx(MTX3);
	check_ercd(ercd, E_OK);

	check_point(50);
	ercd = chg_pri(TASK1, LOW_PRIORITY);
	check_ercd(ercd, E_OK);

	check_point(53);
	ercd = chg_pri(TASK1, TPRI_INI);
	check_ercd(ercd, E_OK);

	check_point(54);
	ercd = loc_mtx(MTX3);
	check_ercd(ercd, E_OK);

	ercd = act_tsk(TASK4);
	check_ercd(ercd, E_OK);

	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	ercd = act_tsk(TASK5);
	check_ercd(ercd, E_OK);

	ercd = rot_rdq(MID_PRIORITY);
	check_ercd(ercd, E_OK);

	check_point(58);
	ercd = chg_pri(TASK2, MID_PRIORITY);
	check_ercd(ercd, E_OK);

	ercd = ref_tsk(TASK2, &rtsk);
	check_ercd(ercd, E_OK);

	check_assert(rtsk.tskpri == MID_PRIORITY);

	ercd = chg_pri(TASK1, LOW_PRIORITY);
	check_ercd(ercd, E_OK);

	ercd = unl_mtx(MTX3);
	check_ercd(ercd, E_OK);

	check_point(62);
	ercd = chg_pri(TASK1, TPRI_INI);
	check_ercd(ercd, E_OK);

	check_point(63);
	ercd = loc_mtx(MTX3);
	check_ercd(ercd, E_OK);

	ercd = act_tsk(TASK4);
	check_ercd(ercd, E_OK);

	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	ercd = act_tsk(TASK3);
	check_ercd(ercd, E_OK);

	ercd = act_tsk(TASK5);
	check_ercd(ercd, E_OK);

	ercd = rot_rdq(MID_PRIORITY);
	check_ercd(ercd, E_OK);

	check_point(68);
	ercd = chg_pri(TASK2, MID_PRIORITY);
	check_ercd(ercd, E_OK);

	ercd = ref_tsk(TASK2, &rtsk);
	check_ercd(ercd, E_OK);

	check_assert(rtsk.tskpri == MID_PRIORITY);

	ercd = chg_pri(TASK1, LOW_PRIORITY);
	check_ercd(ercd, E_OK);

	ercd = unl_mtx(MTX3);
	check_ercd(ercd, E_OK);

	check_point(73);
	ercd = chg_pri(TASK1, TPRI_INI);
	check_ercd(ercd, E_OK);

	check_point(74);
	ercd = loc_mtx(MTX2);
	check_ercd(ercd, E_OK);

	ercd = act_tsk(TASK4);
	check_ercd(ercd, E_OK);

	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	ercd = rot_rdq(MID_PRIORITY);
	check_ercd(ercd, E_OK);

	check_point(77);
	ercd = act_tsk(TASK6);
	check_ercd(ercd, E_OK);

	check_point(79);
	ercd = chg_pri(TASK2, HIGH_PRIORITY);
	check_ercd(ercd, E_OK);

	ercd = ref_tsk(TASK2, &rtsk);
	check_ercd(ercd, E_OK);

	check_assert(rtsk.tskpri == HIGH_PRIORITY);

	ercd = unl_mtx(MTX2);
	check_ercd(ercd, E_OK);

	check_point(82);
	ercd = chg_pri(TASK1, LOW_PRIORITY);
	check_ercd(ercd, E_OK);

	check_point(84);
	ercd = chg_pri(TASK1, TPRI_INI);
	check_ercd(ercd, E_OK);

	check_point(85);
	ercd = loc_mtx(MTX2);
	check_ercd(ercd, E_OK);

	ercd = act_tsk(TASK4);
	check_ercd(ercd, E_OK);

	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	ercd = rot_rdq(MID_PRIORITY);
	check_ercd(ercd, E_OK);

	check_point(88);
	ercd = chg_pri(TASK2, HIGH_PRIORITY);
	check_ercd(ercd, E_OK);

	ercd = ref_tsk(TASK2, &rtsk);
	check_ercd(ercd, E_OK);

	check_assert(rtsk.tskpri == HIGH_PRIORITY);

	ercd = ref_tsk(TASK1, &rtsk);
	check_ercd(ercd, E_OK);

	check_assert(rtsk.tskpri == HIGH_PRIORITY);

	ercd = unl_mtx(MTX2);
	check_ercd(ercd, E_OK);

	check_point(90);
	ercd = chg_pri(TASK1, LOW_PRIORITY);
	check_ercd(ercd, E_OK);

	check_point(92);
	ercd = chg_pri(TASK1, TPRI_INI);
	check_ercd(ercd, E_OK);

	check_point(93);
	ercd = loc_mtx(MTX2);
	check_ercd(ercd, E_OK);

	ercd = act_tsk(TASK7);
	check_ercd(ercd, E_OK);

	ercd = chg_pri(TASK1, LOW_PRIORITY);
	check_ercd(ercd, E_OK);

	check_point(95);
	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	check_point(97);
	ercd = chg_pri(TASK2, LOW_PRIORITY);
	check_ercd(ercd, E_OK);

	ercd = ref_tsk(TASK2, &rtsk);
	check_ercd(ercd, E_OK);

	check_assert(rtsk.tskpri == LOW_PRIORITY);

	ercd = unl_mtx(MTX2);
	check_ercd(ercd, E_OK);

	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(99);
	ercd = loc_mtx(MTX2);
	check_ercd(ercd, E_OK);

	ercd = act_tsk(TASK7);
	check_ercd(ercd, E_OK);

	ercd = chg_pri(TASK1, LOW_PRIORITY);
	check_ercd(ercd, E_OK);

	check_point(101);
	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	check_point(103);
	ercd = chg_pri(TASK2, LOW_PRIORITY);
	check_ercd(ercd, E_OK);

	ercd = ref_tsk(TASK2, &rtsk);
	check_ercd(ercd, E_OK);

	check_assert(rtsk.tskpri == LOW_PRIORITY);

	ercd = ref_tsk(TASK1, &rtsk);
	check_ercd(ercd, E_OK);

	check_assert(rtsk.tskpri == LOW_PRIORITY);

	ercd = unl_mtx(MTX2);
	check_ercd(ercd, E_OK);

	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(106);
	ercd = chg_pri(TASK1, TPRI_INI);
	check_ercd(ercd, E_OK);

	check_point(107);
	ercd = loc_mtx(MTX2);
	check_ercd(ercd, E_OK);

	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	ercd = act_tsk(TASK4);
	check_ercd(ercd, E_OK);

	ercd = rot_rdq(MID_PRIORITY);
	check_ercd(ercd, E_OK);

	check_point(110);
	ercd = chg_pri(TASK2, MID_PRIORITY);
	check_ercd(ercd, E_OK);

	ercd = ref_tsk(TASK2, &rtsk);
	check_ercd(ercd, E_OK);

	check_assert(rtsk.tskpri == MID_PRIORITY);

	ercd = unl_mtx(MTX2);
	check_ercd(ercd, E_OK);

	ercd = chg_pri(TASK1, LOW_PRIORITY);
	check_ercd(ercd, E_OK);

	check_point(113);
	ercd = chg_pri(TASK1, TPRI_INI);
	check_ercd(ercd, E_OK);

	check_point(114);
	ercd = loc_mtx(MTX2);
	check_ercd(ercd, E_OK);

	ercd = act_tsk(TASK4);
	check_ercd(ercd, E_OK);

	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	ercd = act_tsk(TASK3);
	check_ercd(ercd, E_OK);

	ercd = act_tsk(TASK5);
	check_ercd(ercd, E_OK);

	ercd = rot_rdq(MID_PRIORITY);
	check_ercd(ercd, E_OK);

	check_point(119);
	ercd = chg_pri(TASK2, MID_PRIORITY);
	check_ercd(ercd, E_OK);

	ercd = ref_tsk(TASK2, &rtsk);
	check_ercd(ercd, E_OK);

	check_assert(rtsk.tskpri == MID_PRIORITY);

	ercd = chg_pri(TASK1, LOW_PRIORITY);
	check_ercd(ercd, E_OK);

	ercd = unl_mtx(MTX2);
	check_ercd(ercd, E_OK);

	check_point(124);
	ercd = chg_pri(TASK1, TPRI_INI);
	check_ercd(ercd, E_OK);

	check_finish(125);
	check_assert(false);
}

static uint_t	task2_count = 0;

void
task2(EXINF exinf)
{
	ER_UINT	ercd;

	switch (++task2_count) {
	case 1:
		check_point(18);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 2:
		check_point(21);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 3:
		check_point(25);
		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_OK);

		ercd = rot_rdq(MID_PRIORITY);
		check_ercd(ercd, E_OK);

		check_point(27);
		ercd = act_tsk(TASK4);
		check_ercd(ercd, E_OK);

		ercd = rot_rdq(MID_PRIORITY);
		check_ercd(ercd, E_OK);

		check_point(31);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 4:
		check_point(37);
		ercd = loc_mtx(MTX3);
		check_ercd(ercd, E_OK);

		check_point(41);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 5:
		check_point(46);
		ercd = loc_mtx(MTX3);
		check_ercd(ercd, E_OK);

		check_point(51);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 6:
		check_point(56);
		ercd = loc_mtx(MTX3);
		check_ercd(ercd, E_OK);

		check_point(60);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 7:
		check_point(65);
		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_OK);

		ercd = loc_mtx(MTX3);
		check_ercd(ercd, E_OK);

		check_point(70);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 8:
		check_point(76);
		ercd = loc_mtx(MTX2);
		check_ercd(ercd, E_OK);

		check_point(81);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 9:
		check_point(87);
		ercd = loc_mtx(MTX2);
		check_ercd(ercd, E_OK);

		check_point(89);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 10:
		check_point(96);
		ercd = chg_pri(TASK1, TPRI_INI);
		check_ercd(ercd, E_OK);

		ercd = loc_mtx(MTX2);
		check_ercd(ercd, E_OK);

		check_point(98);
		ercd = chg_pri(TASK2, TPRI_INI);
		check_ercd(ercd, E_OK);

		ercd = wup_tsk(TASK1);
		check_ercd(ercd, E_OK);

		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 11:
		check_point(102);
		ercd = loc_mtx(MTX2);
		check_ercd(ercd, E_OK);

		check_point(105);
		ercd = wup_tsk(TASK1);
		check_ercd(ercd, E_OK);

		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 12:
		check_point(108);
		ercd = loc_mtx(MTX2);
		check_ercd(ercd, E_OK);

		check_point(112);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 13:
		check_point(116);
		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_OK);

		ercd = loc_mtx(MTX2);
		check_ercd(ercd, E_OK);

		check_point(121);
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
	T_RTSK	rtsk;

	switch (++task3_count) {
	case 1:
		check_point(8);
		ercd = ref_tsk(TASK1, &rtsk);
		check_ercd(ercd, E_OK);

		check_assert(rtsk.tskpri == MID_PRIORITY);

		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 2:
		check_point(10);
		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_OK);

		check_point(15);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 3:
		check_point(24);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 4:
		check_point(28);
		ercd = act_tsk(TASK5);
		check_ercd(ercd, E_OK);

		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_OK);

		check_point(34);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 5:
		check_point(66);
		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_OK);

		check_point(72);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 6:
		check_point(117);
		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_OK);

		check_point(123);
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
		check_point(12);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 2:
		check_point(30);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 3:
		check_point(36);
		ercd = loc_mtx(MTX3);
		check_ercd(ercd, E_OK);

		check_point(40);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 4:
		check_point(45);
		ercd = loc_mtx(MTX3);
		check_ercd(ercd, E_OK);

		check_point(49);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 5:
		check_point(55);
		ercd = loc_mtx(MTX3);
		check_ercd(ercd, E_OK);

		check_point(59);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 6:
		check_point(64);
		ercd = loc_mtx(MTX3);
		check_ercd(ercd, E_OK);

		check_point(69);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 7:
		check_point(75);
		ercd = loc_mtx(MTX2);
		check_ercd(ercd, E_OK);

		check_point(83);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 8:
		check_point(86);
		ercd = loc_mtx(MTX2);
		check_ercd(ercd, E_OK);

		check_point(91);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 9:
		check_point(109);
		ercd = loc_mtx(MTX2);
		check_ercd(ercd, E_OK);

		check_point(111);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 10:
		check_point(115);
		ercd = loc_mtx(MTX2);
		check_ercd(ercd, E_OK);

		check_point(120);
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
		check_point(14);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 2:
		check_point(32);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 3:
		check_point(38);
		ercd = loc_mtx(MTX3);
		check_ercd(ercd, E_OK);

		check_point(42);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 4:
		check_point(47);
		ercd = loc_mtx(MTX3);
		check_ercd(ercd, E_OK);

		check_point(52);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 5:
		check_point(57);
		ercd = loc_mtx(MTX3);
		check_ercd(ercd, E_OK);

		check_point(61);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 6:
		check_point(67);
		ercd = loc_mtx(MTX3);
		check_ercd(ercd, E_OK);

		check_point(71);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 7:
		check_point(118);
		ercd = loc_mtx(MTX2);
		check_ercd(ercd, E_OK);

		check_point(122);
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
		check_point(2);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 2:
		check_point(17);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 3:
		check_point(78);
		ercd = loc_mtx(MTX2);
		check_ercd(ercd, E_OK);

		check_point(80);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	default:
		check_assert(false);
	}
	check_assert(false);
}

static uint_t	task7_count = 0;

void
task7(EXINF exinf)
{
	ER_UINT	ercd;
	T_RTSK	rtsk;

	switch (++task7_count) {
	case 1:
		check_point(5);
		ercd = ref_tsk(TASK1, &rtsk);
		check_ercd(ercd, E_OK);

		check_assert(rtsk.tskpri == LOW_PRIORITY);

		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 2:
		check_point(20);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 3:
		check_point(94);
		ercd = loc_mtx(MTX2);
		check_ercd(ercd, E_OK);

		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 4:
		check_point(100);
		ercd = loc_mtx(MTX2);
		check_ercd(ercd, E_OK);

		check_point(104);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	default:
		check_assert(false);
	}
	check_assert(false);
}
