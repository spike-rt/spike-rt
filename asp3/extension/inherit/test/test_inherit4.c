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
 *  $Id: test_inherit4.c 1618 2022-09-19 02:57:29Z ertl-hiro $
 */

/* 
 *		優先度継承ミューテックスのテスト(4)
 *
 * 【テストの目的】
 *
 *  優先度継承ミューテックスに対するini_mtxによる優先度制御についてテ
 *  ストする．
 *
 * 【テスト項目】
 *
 *	(A) 自タスクがロックしている優先度継承ミューテックスXに対して
 *		ini_mtxを発行し，自タスクがミューテックスXをロック解除すること
 *		伴う，自タスクの優先度制御
 *		(A-1) ミューテックスXに，自タスクAと同じ現在優先度のタスクBが
 *			  待ち状態となっており，ミューテックスのロック解除で自タス
 *			  クの現在優先度が下がるべき場合に，現在優先度が適切に下げ
 *			  られること．また，同じ優先度内での優先順位が最高になるこ
 *			  と
 *		(A-2) ミューテックスXに，自タスクAと同じ現在優先度のタスクBが
 *			  待ち状態となっており，ミューテックスのロック解除で自タス
 *			  クの現在優先度が変わるべきでない場合に，現在優先度が変わ
 *			  らないこと．また，同じ優先度内での優先順位が変わらないこ
 *			  と
 *		(A-3) ミューテックスXに，自タスクAより低い現在優先度のタスクし
 *			  か待ち状態となっていない場合に，自タスクの現在優先度が変
 *			  わらないこと．また，同じ優先度内での優先順位が変わらない
 *			  こと
 *	(B) タスクA（実行可能状態）がロックしている優先度継承ミューテック
 *		スXに対してini_mtxを発行し，タスクAがミューテックスXをロック解
 *		除することに伴う，タスクAの優先度制御
 *		(B-1) ミューテックスXに，タスクAと同じ現在優先度のタスクBが待
 *			  ち状態となっており，ミューテックスのロック解除でタスクA
 *			  の現在優先度が下がるべき場合に，現在優先度が適切に下げら
 *			  れること．また，同じ優先度内での優先順位が最高になること
 *		(B-2) ミューテックスXに，タスクAと同じ現在優先度のタスクBが待
 *			  ち状態となっており，ミューテックスのロック解除でタスクA
 *			  の現在優先度が変わるべきでない場合に，現在優先度が変わら
 *			  ないこと．また，同じ優先度内での優先順位が変わらないこと
 *		(B-3) ミューテックスXに，タスクAより低い現在優先度のタスクしか
 *			  待ち状態となっていない場合に，タスクAの現在優先度が変わ
 *			  らないこと．また，同じ優先度内での優先順位が変わらないこ
 *			  と
 *	(C) タスクA（優先度継承ミューテックスYに対するロック待ち状態）がロッ
 *		クしている優先度継承ミューテックスXに対してini_mtxを発行し，タ
 *		スクAがミューテックスXをロック解除することに伴う，タスクAの優
 *		先度制御
 *		(C-1) ミューテックスXに，タスクAと同じ現在優先度のタスクBが待
 *			  ち状態となっており，ミューテックスのロック解除でタスクA
 *			  の現在優先度が下がるべき場合に，現在優先度が適切に下げら
 *			  れること．また，待ち行列中で同じ優先度内での順序が最後に
 *			  なること
 *		(C-2) ミューテックスXに，タスクAと同じ現在優先度のタスクBが待
 *			  ち状態となっており，ミューテックスのロック解除でタスクA
 *			  の現在優先度が下がるべき場合に，現在優先度が適切に下げら
 *			  れること．また，待ち行列中で同じ優先度内での順序が最後に
 *			  なること．さらに，ミューテックスYをロックしているタスクC
 *			  に対して，推移的な優先度継承が起こること
 *		(C-3) ミューテックスXに，タスクAと同じ現在優先度のタスクBが待
 *			  ち状態となっており，ミューテックスのロック解除でタスクA
 *			  の現在優先度が変わるべきでない場合に，現在優先度が変わら
 *			  ないこと．また，待ち行列中で順序が変わらないこと
 *		(C-4) ミューテックスXに，タスクAより低い現在優先度のタスクしか
 *			  待ち状態となっていない場合に，タスクAの現在優先度が変わ
 *			  らないこと．また，待ち行列中で順序が変わらないこと
 *
 * 【テスト項目の実現方法】
 *
 *	(A-1)
 *		優先度継承ミューテックスX（MTX1）を，中優先度タスクA（TASK1）
 *		がロックし，高優先度タスクB（TASK6）が待っている状態で，TASK1
 *		にini_mtx(MTX1)を発行させ，TASK1が中優先度になることを確認する．
 *		また，他の中優先度のタスク（TASK2）を実行できる状態にしておき，
 *		それよりも優先順位が高くなることを確認する．
 *	(A-2)
 *		優先度継承ミューテックスX（MTX1）を，中優先度タスクA（TASK1）
 *		がロックし，中優先度タスクB（TASK2）が待っている状態で，TASK1
 *		にini_mtx(MTX1)を発行させ，TASK1が中優先度のままであることを確
 *		認する．また，実行可能状態の中優先度のタスク2つ（TASK3，TASK4）
 *		を用意にしておき，TASK1の優先順位が変わらないことを確認する．
 *		ディスパッチ禁止状態で実施する．
 *	(A-3)
 *		優先度継承ミューテックスX（MTX1）を，中優先度タスクA（TASK1）
 *		がロックし，低優先度タスク（TASK8）が待っている状態で，TASK1に
 *		ini_mtx(MTX1)を発行させ，TASK1が中優先度のままであることを確認
 *		する．また，実行可能状態の中優先度のタスク2つ（TASK3，TASK4）
 *		を用意にしておき，TASK1の優先順位が変わらないことを確認する．
 *		ディスパッチ禁止状態で実施する．
 *	(B-1)
 *		優先度継承ミューテックスX（MTX1）を，中優先度タスクA（TASK1）
 *		がロックし，高優先度タスクB（TASK6）が待っている状態で，TASK7
 *		にini_mtx(MTX1)を発行させ，TASK1が中優先度になることを確認する．
 *		また，他の中優先度のタスク（TASK3）を実行できる状態にしておき，
 *		それよりも優先順位が高くなることを確認する．
 *	(B-2)
 *		優先度継承ミューテックスX（MTX1）を，中優先度タスクA（TASK1）
 *		がロックし，中優先度タスクB（TASK2）が待っている状態で，TASK7
 *		にini_mtx(MTX1)を発行させ，TASK1が中優先度のままであることを確
 *		認する．また，実行可能状態の中優先度のタスク2つ（TASK3，TASK4）
 *		を用意にしておき，TASK1の優先順位が変わらないことを確認する．
 *	(B-3)
 *		優先度継承ミューテックスX（MTX1）を，中優先度タスクA（TASK1）
 *		がロックし，低優先度タスク（TASK8）Bが待っている状態で，TASK7
 *		にini_mtx(MTX1)を発行させ，TASK1が中優先度のままであることを確
 *		認する．また，実行可能状態の中優先度のタスク2つ（TASK3，TASK4）
 *		を用意にしておき，TASK1の優先順位が変わらないことを確認する．
 *	(C-1)
 *		高優先度タスク（TASK2を高優先度にして用いる）に優先度継承ミュー
 *		テックスY（MTX2）をロックさせる．中優先度タスクA（TASK1）に優
 *		先度継承ミューテックスX（MTX1）をロックさせ，さらにMTX2に対す
 *		る待ち状態にする．高優先度タスクB（TASK6）を，MTX1に対する待ち
 *		状態にする．TASK7にini_mtx(MTX1)を発行させ，TASK1が中優先度に
 *		なることを確認する．また，MTX2に対する待ち状態の中優先度タスク
 *		（TASK3）を用意しておき，待ち行列中での順序がTASK3よりも後にな
 *		ることを確認する．
 *	(C-2)
 *		中優先度タスクC（TASK2）に優先度継承ミューテックスY（MTX2）を
 *		ロックさせる．中優先度タスクA（TASK1）に優先度継承ミューテック
 *		スX（MTX1）をロックさせ，さらにMTX2に対する待ち状態にする．高
 *		優先度タスクB（TASK6）を，MTX1に対する待ち状態にする．TASK7に
 *		ini_mtx(MTX1)を発行させ，TASK1が中優先度になることを確認する．
 *		また，MTX2に対する待ち状態の中優先度タスク（TASK3）を用意して
 *		おき，待ち行列中での順序がTASK3よりも後になることを確認する．
 *		さらに，TASK2の優先度が中優先度になることを確認する．
 *	(C-3)
 *		中優先度タスク（TASK2）に優先度継承ミューテックスY（MTX2）をロッ
 *		クさせる．中優先度タスクA（TASK1）に優先度継承ミューテックスX
 *		（MTX1）をロックさせ，さらにMTX2に対する待ち状態にする．中優先
 *		度タスクB（TASK5）を，MTX1に対する待ち状態にする．TASK7に
 *		ini_mtx(MTX1)を発行させ，TASK1の現在優先度が変わらないことを確
 *		認する．また，MTX2に対する待ち状態の中優先度タスク2つ（TASK3，
 *		TASK4）を用意しておき，TASK1の待ち行列中での順序が変わらないこ
 *		とを確認する
 *	(C-4)
 *		中優先度タスク（TASK2）に優先度継承ミューテックスY（MTX2）をロッ
 *		クさせる．中優先度タスクA（TASK1）に優先度継承ミューテックスX
 *		（MTX1）をロックさせ，さらにMTX2に対する待ち状態にする．低優先
 *		度タスク（TASK8）を，MTX1に対する待ち状態にする．TASK7に
 *		ini_mtx(MTX1)を発行させ，TASK1の現在優先度が変わらないことを確
 *		認する．また，MTX2に対する待ち状態の中優先度タスク2つ（TASK3，
 *		TASK4）を用意しておき，TASK1の待ち行列中での順序が変わらないこ
 *		とを確認する
 *
 * 【使用リソース】
 *
 *	TASK1: 中優先度タスク，メインタスク，最初から起動
 *	TASK2: 中優先度タスク
 *	TASK3: 中優先度タスク
 *	TASK4: 中優先度タスク
 *	TASK5: 中優先度タスク
 *	TASK6: 高優先度タスク
 *	TASK7: 高優先度タスク
 *	TASK8: 低優先度タスク
 *	TASK9: 低優先度タスク
 *	MTX1: ミューテックス（優先度継承）
 *	MTX2: ミューテックス（優先度継承）
 *
 * 【テストシーケンス】
 *
 *	== TASK1（優先度：中）==
 *	//		中：TASK1
 *	1:	loc_mtx(MTX1)
 *	//		中：TASK1，MTX1：TASK1
 *	2:	act_tsk(TASK2)
 *	//		中：TASK1→TASK2，MTX1：TASK1
 *	3:	act_tsk(TASK6)
 *	//		高：TASK6，中：TASK1→TASK2，MTX1：TASK1
 *	== TASK6-1（優先度：高）==
 *	4:	loc_mtx(MTX1) -> E_DLT
 *	//		高：TASK1，中：TASK2，MTX1：TASK1→TASK6
 *	== TASK1（続き）==
 *	5:	ini_mtx(MTX1)									... (A-1)
 *	//		高：TASK6，中：TASK1→TASK2
 *	== TASK6-1（続き）==
 *	6:	ref_mtx(MTX1, &rmtx)
 *		assert(rmtx.htskid == TSK_NONE)
 *		assert(rmtx.wtskid == TSK_NONE)
 *		ref_tsk(TASK1, &rtsk)
 *		assert(rtsk.tskpri == MID_PRIORITY)
 *		ext_tsk()
 *	//		中：TASK1→TASK2
 *
 *	== TASK1（続き）==
 *	7:	loc_mtx(MTX1)
 *	//		中：TASK1→TASK2，MTX1：TASK1
 *		rot_rdq(MID_PRIORITY)
 *	//		中：TASK2→TASK1，MTX1：TASK1
 *	== TASK2-1（優先度：中）==
 *	8:	loc_mtx(MTX1) -> E_DLT
 *	//		中：TASK1，MTX1：TASK1→TASK2
 *	== TASK1（続き）==
 *	9:	dis_dsp()
 *		act_tsk(TASK3)
 *	//		中：TASK1→TASK3，MTX1：TASK1→TASK2
 *		rot_rdq(MID_PRIORITY)
 *	//		中：TASK3→TASK1，MTX1：TASK1→TASK2
 *		act_tsk(TASK4)
 *	//		中：TASK3→TASK1→TASK4，MTX1：TASK1→TASK2
 *		ini_mtx(MTX1)									... (A-2)
 *	//		中：TASK3→TASK1→TASK4→TASK2
 *		ref_mtx(MTX1, &rmtx)
 *		assert(rmtx.htskid == TSK_NONE)
 *		assert(rmtx.wtskid == TSK_NONE)
 *		ref_tsk(TASK1, &rtsk)
 *		assert(rtsk.tskpri == MID_PRIORITY)
 *		ena_dsp()
 *	== TASK3-1（優先度：中）==
 *	10:	ext_tsk()
 *	//		中：TASK1→TASK4→TASK2
 *	== TASK1（続き）==
 *	11:	rot_rdq(MID_PRIORITY)
 *	//		中：TASK4→TASK2→TASK1
 *	== TASK4-1（優先度：中）==
 *	12:	ext_tsk()
 *	//		中：TASK2→TASK1
 *	== TASK2-1（続き）==
 *	13:	ext_tsk()
 *	//		中：TASK1
 *
 *	== TASK1（続き）==
 *	14:	loc_mtx(MTX1)
 *	//		中：TASK1，MTX1：TASK1
 *		act_tsk(TASK8)
 *	//		中：TASK1，低：TASK8，MTX1：TASK1
 *		act_tsk(TASK9)
 *	//		中：TASK1，低：TASK8→TASK9，MTX1：TASK1
 *		slp_tsk()
 *	//		低：TASK8→TASK9，MTX1：TASK1
 *	== TASK8-1（優先度：低）==
 *	15:	loc_mtx(MTX1) -> E_DLT
 *	//		低：TASK9，MTX1：TASK1→TASK8
 *	== TASK9-1（優先度：低）==
 *	16:	wup_tsk(TASK1)
 *	//		中：TASK1，低：TASK9，MTX1：TASK1→TASK8
 *	== TASK1（続き）==
 *	17:	ter_tsk(TASK9)
 *	//		中：TASK1，MTX1：TASK1→TASK8
 *		dis_dsp()
 *		act_tsk(TASK3)
 *	//		中：TASK1→TASK3，MTX1：TASK1→TASK8
 *		rot_rdq(MID_PRIORITY)
 *	//		中：TASK3→TASK1，MTX1：TASK1→TASK8
 *		act_tsk(TASK4)
 *	//		中：TASK3→TASK1→TASK4，MTX1：TASK1→TASK8
 *		ini_mtx(MTX1)									... (A-3)
 *	//		中：TASK3→TASK1→TASK4，低：TASK8
 *		ref_mtx(MTX1, &rmtx)
 *		assert(rmtx.htskid == TSK_NONE)
 *		assert(rmtx.wtskid == TSK_NONE)
 *		ref_tsk(TASK1, &rtsk)
 *		assert(rtsk.tskpri == MID_PRIORITY)
 *		ena_dsp()
 *	== TASK3-2（優先度：中）==
 *	18:	ext_tsk()
 *	//		中：TASK1→TASK4，低：TASK8
 *	== TASK1（続き）==
 *	19:	chg_pri(TASK1, LOW_PRIORITY)
 *	//		中：TASK4，低：TASK8→TASK1
 *	== TASK4-2（優先度：中）==
 *	20:	ext_tsk()
 *	//		低：TASK8→TASK1
 *	== TASK8-1（続き）==
 *	21:	ext_tsk()
 *	//		低：TASK1
 *	== TASK1（続き）==
 *	22:	chg_pri(TASK1, TPRI_INI)
 *	//		中：TASK1
 *
 *	23:	loc_mtx(MTX1)
 *	//		中：TASK1，MTX1：TASK1
 *		act_tsk(TASK6)
 *	//		高：TASK6，中：TASK1，MTX1：TASK1
 *	== TASK6-2（優先度：高）==
 *	24:	loc_mtx(MTX1) -> E_DLT
 *	//		高：TASK1，MTX1：TASK1→TASK6
 *	== TASK1（続き）==
 *	25:	act_tsk(TASK3)
 *	//		高：TASK1，中：TASK3，MTX1：TASK1→TASK6
 *		act_tsk(TASK7)
 *	//		高：TASK1→TASK7，中：TASK3，MTX1：TASK1→TASK6
 *		rot_rdq(HIGH_PRIORITY)
 *	//		中：TASK7→TASK1，中：TASK3，MTX1：TASK1→TASK6
 *	== TASK7-1（優先度：高）==
 *	26:	ini_mtx(MTX1)									... (B-1)
 *	//		高：TASK7→TASK6，中：TASK1→TASK3
 *		ref_mtx(MTX1, &rmtx)
 *		assert(rmtx.htskid == TSK_NONE)
 *		assert(rmtx.wtskid == TSK_NONE)
 *		ref_tsk(TASK1, &rtsk)
 *		assert(rtsk.tskpri == MID_PRIORITY)
 *		ext_tsk()
 *	//		高：TASK6，中：TASK1→TASK3
 *	== TASK6-2（続き）==
 *	27:	ext_tsk()
 *	//		中：TASK1→TASK3
 *	== TASK1（続き）==
 *	28:	rot_rdq(MID_PRIORITY)
 *	//		中：TASK3→TASK1
 *	== TASK3-3（優先度：中）==
 *	29:	ext_tsk()
 *	//		中：TASK1
 *
 *	== TASK1（続き）==
 *	30:	loc_mtx(MTX1)
 *	//		中：TASK1，MTX1：TASK1
 *		act_tsk(TASK2)
 *	//		中：TASK1→TASK2，MTX1：TASK1
 *		rot_rdq(MID_PRIORITY)
 *	//		中：TASK2→TASK1，MTX1：TASK1
 *	== TASK2-2（優先度：中）
 *	31:	loc_mtx(MTX1) -> E_DLT
 *	//		中：TASK1，MTX1：TASK1→TASK2
 *	== TASK1（続き）==
 *	32:	act_tsk(TASK3)
 *	//		中：TASK1→TASK3，MTX1：TASK1→TASK2
 *		rot_rdq(MID_PRIORITY)
 *	//		中：TASK3→TASK1，MTX1：TASK1→TASK2
 *	== TASK3-4（優先度：中）==
 *	33:	act_tsk(TASK4)
 *	//		中：TASK3→TASK1→TASK4，MTX1：TASK1→TASK2
 *		act_tsk(TASK7)
 *	//		高：TASK7，中：TASK3→TASK1→TASK4，MTX1：TASK1→TASK2
 *	== TASK7-2（優先度：高）==
 *	34:	ini_mtx(MTX1)									... (B-2)
 *	//		高：TASK7，中：TASK3→TASK1→TASK4→TASK2
 *		ref_mtx(MTX1, &rmtx)
 *		assert(rmtx.htskid == TSK_NONE)
 *		assert(rmtx.wtskid == TSK_NONE)
 *		ref_tsk(TASK1, &rtsk)
 *		assert(rtsk.tskpri == MID_PRIORITY)
 *		ext_tsk()
 *	//		中：TASK3→TASK1→TASK4→TASK2
 *	== TASK3-4（続き）==
 *	35:	ext_tsk()
 *	//		中：TASK1→TASK4→TASK2
 *	== TASK1（続き）==
 *	36:	rot_rdq(MID_PRIORITY)
 *	//		中：TASK4→TASK2→TASK1
 *	== TASK4-3（優先度：中）==
 *	37:	ext_tsk()
 *	//		中：TASK2→TASK1
 *	== TASK2-2（続き）==
 *	38:	ext_tsk()
 *	//		中：TASK1
 *
 *	== TASK1（続き）==
 *	39:	loc_mtx(MTX1)
 *	//		中：TASK1，MTX1：TASK1
 *		act_tsk(TASK8)
 *	//		中：TASK1，低：TASK8，MTX1：TASK1
 *		act_tsk(TASK9)
 *	//		中：TASK1，低：TASK8→TASK9，MTX1：TASK1
 *		slp_tsk()
 *	//		低：TASK8→TASK9，MTX1：TASK1
 *	== TASK8-2（優先度：低）==
 *	40:	loc_mtx(MTX1) -> E_DLT
 *	//		低：TASK9，MTX1：TASK1→TASK8
 *	== TASK9-2（優先度：低）==
 *	41:	wup_tsk(TASK1)
 *	//		中：TASK1，低：TASK9，MTX1：TASK1→TASK8
 *	== TASK1（続き）==
 *	42:	ter_tsk(TASK9)
 *	//		中：TASK1，MTX1：TASK1→TASK8
 *	43:	act_tsk(TASK3)
 *	//		中：TASK1→TASK3，MTX1：TASK1→TASK8
 *		rot_rdq(MID_PRIORITY)
 *	//		中：TASK3→TASK1，MTX1：TASK1→TASK8
 *	== TASK3-5（優先度：中）==
 *	44:	act_tsk(TASK4)
 *	//		中：TASK3→TASK1→TASK4，MTX1：TASK1→TASK8
 *		act_tsk(TASK7)
 *	//		高：TASK7，中：TASK3→TASK1→TASK4，MTX1：TASK1→TASK8
 *	== TASK7-3（優先度：高）==
 *	45:	ini_mtx(MTX1)									... (B-3)
 *	//		高：TASK7，中：TASK3→TASK1→TASK4，低：TASK8
 *		ref_mtx(MTX1, &rmtx)
 *		assert(rmtx.htskid == TSK_NONE)
 *		assert(rmtx.wtskid == TSK_NONE)
 *		ref_tsk(TASK1, &rtsk)
 *		assert(rtsk.tskpri == MID_PRIORITY)
 *		ext_tsk()
 *	//		中：TASK3→TASK1→TASK4，低：TASK8
 *	== TASK3-5（続き）==
 *	46:	ext_tsk()
 *	//		中：TASK1→TASK4，低：TASK8
 *	== TASK1（続き）==
 *	47:	chg_pri(TASK1, LOW_PRIORITY)
 *	//		中：TASK4，低：TASK8→TASK1
 *	== TASK4-4（優先度：中）==
 *	48:	ext_tsk()
 *	//		低：TASK8→TASK1
 *	== TASK8-2（続き）==
 *	49:	ext_tsk()
 *	//		低：TASK1
 *	== TASK1（続き）==
 *	50:	chg_pri(TASK1, TPRI_INI)
 *	//		中：TASK1
 *
 *	51:	act_tsk(TASK2)
 *	//		中：TASK1→TASK2
 *		rot_rdq(MID_PRIORITY)
 *	//		中：TASK2→TASK1
 *	== TASK2-3（優先度：中）==
 *	52:	loc_mtx(MTX2)
 *	//		中：TASK2→TASK1，MTX2：TASK2
 *		act_tsk(TASK3)
 *	//		中：TASK2→TASK1→TASK3，MTX2：TASK2
 *		rot_rdq(MID_PRIORITY)
 *	//		中：TASK1→TASK3→TASK2，MTX2：TASK2
 *	== TASK1（続き）==
 *	53:	loc_mtx(MTX1)
 *	//		中：TASK1→TASK3→TASK2，MTX1：TASK1，MTX2：TASK2
 *		loc_mtx(MTX2)
 *	//		中：TASK3→TASK2，MTX1：TASK1，MTX2：TASK2→TASK1
 *	== TASK3-6（優先度：中）==
 *	54:	loc_mtx(MTX2)
 *	//		中：TASK2，MTX1：TASK1，MTX2：TASK2→TASK1→TASK3
 *	== TASK2-3（続き）==
 *	55:	act_tsk(TASK6)
 *	//		高：TASK6，中：TASK2，MTX1：TASK1，MTX2：TASK2→TASK1→TASK3
 *	== TASK6-3（優先度：高）==
 *	56:	act_tsk(TASK7)
 *	//		高：TASK6→TASK7，中：TASK2，MTX1：TASK1，MTX2：TASK2→TASK1→TASK3
 *		chg_pri(TASK2, HIGH_PRIORITY)
 *	//		高：TASK6→TASK7→TASK2，MTX1：TASK1，MTX2：TASK2→TASK1→TASK3
 *		loc_mtx(MTX1) -> E_DLT
 *	//		高：TASK7→TASK2，MTX1：TASK1→TASK6，MTX2：TASK2→TASK1→TASK3
 *	//		優先度継承により，TASK1が高優先度になる。
 *	== TASK7-4（優先度：高）==
 *	57:	ini_mtx(MTX1)									... (C-1)
 *	//		高：TASK7→TASK2→TASK6，MTX2：TASK2→TASK3→TASK1
 *	//		優先度継承の解除により，TASK1が中優先度になる。
 *		ref_mtx(MTX1, &rmtx)
 *		assert(rmtx.htskid == TSK_NONE)
 *		assert(rmtx.wtskid == TSK_NONE)
 *		ref_tsk(TASK1, &rtsk)
 *		assert(rtsk.tskpri == MID_PRIORITY)
 *		ext_tsk()
 *	//		高：TASK2→TASK6，MTX2：TASK2→TASK3→TASK1
 *	== TASK2-3（続き）==
 *	58:	unl_mtx(MTX2)
 *	//		中：TASK2→TASK6→TASK3，MTX2：TASK3→TASK1
 *		ext_tsk()
 *	//		中：TASK6→TASK3，MTX2：TASK3→TASK1
 *	== TASK6-3（続き）==
 *	59:	ext_tsk()
 *	//		中：TASK3，MTX2：TASK3→TASK1
 *	== TASK3-6（続き）==
 *	60:	unl_mtx(MTX2)
 *	//		中：TASK3→TASK1，MTX2：TASK1
 *		ext_tsk()
 *	//		中：TASK1，MTX2：TASK1
 *	== TASK1（続き）==
 *	61:	unl_mtx(MTX2)
 *	//		中：TASK1
 *
 *	62:	act_tsk(TASK2)
 *	//		中：TASK1→TASK2
 *		rot_rdq(MID_PRIORITY)
 *	//		中：TASK2→TASK1
 *	== TASK2-4（優先度：中）==
 *	63:	loc_mtx(MTX2)
 *	//		中：TASK2→TASK1，MTX2：TASK2
 *		act_tsk(TASK3)
 *	//		中：TASK2→TASK1→TASK3，MTX2：TASK2
 *		rot_rdq(MID_PRIORITY)
 *	//		中：TASK1→TASK3→TASK2，MTX2：TASK2
 *	== TASK1（続き）==
 *	64:	loc_mtx(MTX1)
 *	//		中：TASK1→TASK3→TASK2，MTX1：TASK1，MTX2：TASK2
 *		loc_mtx(MTX2)
 *	//		中：TASK3→TASK2，MTX1：TASK1，MTX2：TASK2→TASK1
 *	== TASK3-7（優先度：中）==
 *	65:	loc_mtx(MTX2)
 *	//		中：TASK2，MTX1：TASK1，MTX2：TASK2→TASK1→TASK3
 *	== TASK2-4（続き）==
 *	66:	act_tsk(TASK6)
 *	//		高：TASK6，中：TASK2，MTX1：TASK1，MTX2：TASK2→TASK1→TASK3
 *	== TASK6-4（優先度：高）==
 *	67:	act_tsk(TASK7)
 *	//		高：TASK6→TASK7，中：TASK2，MTX1：TASK1，MTX2：TASK2→TASK1→TASK3
 *		loc_mtx(MTX1) -> E_DLT
 *	//		高：TASK2→TASK7，MTX1：TASK1→TASK6，MTX2：TASK2→TASK1→TASK3
 *	//		優先度継承により，TASK1とTASK2が高優先度になる。
 *	== TASK2-4（続き）==
 *	68:	rot_rdq(HIGH_PRIORITY)
 *	//		高：TASK7→TASK2，MTX1：TASK1→TASK6，MTX2：TASK2→TASK1→TASK3
 *	== TASK7-5（優先度：高）==
 *	69:	ini_mtx(MTX1)									... (C-2)
 *	//		高：TASK7→TASK6，中：TASK2，MTX2：TASK2→TASK3→TASK1
 *	//		優先度継承の解除により，TASK1とTASK2が中優先度になる。
 *		ref_mtx(MTX1, &rmtx)
 *		assert(rmtx.htskid == TSK_NONE)
 *		assert(rmtx.wtskid == TSK_NONE)
 *		ref_tsk(TASK1, &rtsk)
 *		assert(rtsk.tskpri == MID_PRIORITY)
 *		ref_tsk(TASK2, &rtsk)
 *		assert(rtsk.tskpri == MID_PRIORITY)
 *		ext_tsk()
 *	//		高：TASK6，中：TASK2，MTX2：TASK2→TASK3→TASK1
 *	== TASK6-4（続き）==
 *	70:	ext_tsk()
 *	//		中：TASK2，MTX2：TASK2→TASK3→TASK1
 *	== TASK2-4（続き）==
 *	71:	unl_mtx(MTX2)
 *	//		中：TASK2→TASK3，MTX2：TASK3→TASK1
 *		ext_tsk()
 *	//		中：TASK3，MTX2：TASK3→TASK1
 *	== TASK3-7（続き）==
 *	72:	unl_mtx(MTX2)
 *	//		中：TASK3→TASK1，MTX2：TASK1
 *		ext_tsk()
 *	//		中：TASK1，MTX2：TASK1
 *	== TASK1（続き）==
 *	73:	unl_mtx(MTX2)
 *	//		中：TASK1
 *
 *	74:	act_tsk(TASK2)
 *	//		中：TASK1→TASK2
 *		act_tsk(TASK3)
 *	//		中：TASK1→TASK2→TASK3
 *		rot_rdq(MID_PRIORITY)
 *	//		中：TASK2→TASK3→TASK1
 *	== TASK2-5（優先度：中）==
 *	75:	loc_mtx(MTX2)
 *	//		中：TASK2→TASK3→TASK1，MTX2：TASK2
 *		rot_rdq(MID_PRIORITY)
 *	//		中：TASK3→TASK1→TASK2，MTX2：TASK2
 *	== TASK3-8（優先度：中）==
 *	76:	loc_mtx(MTX2)
 *	//		中：TASK1→TASK2，MTX2：TASK2→TASK3
 *	== TASK1（続き）==
 *	77:	loc_mtx(MTX1)
 *	//		中：TASK1→TASK2，MTX1：TASK1，MTX2：TASK2→TASK3
 *		loc_mtx(MTX2)
 *	//		中：TASK2，MTX1：TASK1，MTX2：TASK2→TASK3→TASK1
 *	== TASK2-5（続き）==
 *	78:	act_tsk(TASK4)
 *	//		中：TASK2→TASK4，MTX1：TASK1，MTX2：TASK2→TASK3→TASK1
 *		rot_rdq(MID_PRIORITY)
 *	//		中：TASK4→TASK2，MTX1：TASK1，MTX2：TASK2→TASK3→TASK1
 *	== TASK4-5（優先度：中）==
 *	79:	loc_mtx(MTX2)
 *	//		中：TASK2，MTX1：TASK1，MTX2：TASK2→TASK3→TASK1→TASK4
 *	== TASK2-5（続き）==
 *	80:	act_tsk(TASK5)
 *	//		中：TASK2→TASK5，MTX1：TASK1，MTX2：TASK2→TASK3→TASK1→TASK4
 *		rot_rdq(MID_PRIORITY)
 *	//		中：TASK5→TASK2，MTX1：TASK1，MTX2：TASK2→TASK3→TASK1→TASK4
 *	== TASK5-1（優先度：中）==
 *	81:	loc_mtx(MTX1) -> E_DLT
 *	//		中：TASK2，MTX1：TASK1→TASK5，MTX2：TASK2→TASK3→TASK1→TASK4
 *	== TASK2-5（続き）==
 *	82:	act_tsk(TASK7)
 *	//		高：TASK7，中：TASK2，MTX1：TASK1→TASK5，MTX2：TASK2→TASK3→TASK1→TASK4
 *	== TASK7-6（優先度：高）==
 *	83:	ini_mtx(MTX1)									... (C-3)
 *	//		高：TASK7，中：TASK2→TASK5，MTX2：TASK2→TASK3→TASK1→TASK4
 *		ref_mtx(MTX1, &rmtx)
 *		assert(rmtx.htskid == TSK_NONE)
 *		assert(rmtx.wtskid == TSK_NONE)
 *		ref_tsk(TASK1, &rtsk)
 *		assert(rtsk.tskpri == MID_PRIORITY)
 *		ext_tsk()
 *	//		中：TASK2→TASK5，MTX2：TASK2→TASK3→TASK1→TASK4
 *	== TASK2-5（続き）==
 *	84:	unl_mtx(MTX2)
 *	//		中：TASK2→TASK5→TASK3，MTX2：TASK3→TASK1→TASK4
 *		ext_tsk()
 *	//		中：TASK5→TASK3，MTX2：TASK3→TASK1→TASK4
 *	== TASK5-1（続き）==
 *	85:	ext_tsk()
 *	//		中：TASK3，MTX2：TASK3→TASK1→TASK4
 *	== TASK3-8（続き）==
 *	86:	unl_mtx(MTX2)
 *	//		中：TASK3→TASK1，MTX2：TASK1→TASK4
 *		ext_tsk()
 *	//		中：TASK1，MTX2：TASK1→TASK4
 *	== TASK1（続き）==
 *	87:	unl_mtx(MTX2)
 *	//		中：TASK1→TASK4，MTX2：TASK4
 *	88:	rot_rdq(MID_PRIORITY)
 *	//		中：TASK4→TASK1，MTX2：TASK4
 *	== TASK4-5（続き）==
 *	89:	unl_mtx(MTX2)
 *	//		中：TASK4→TASK1
 *		ext_tsk()
 *	//		中：TASK1
 *
 *	== TASK1（続き）==
 *	90:	act_tsk(TASK2)
 *	//		中：TASK1→TASK2
 *		act_tsk(TASK3)
 *	//		中：TASK1→TASK2→TASK3
 *		rot_rdq(MID_PRIORITY)
 *	//		中：TASK2→TASK3→TASK1
 *	== TASK2-6（優先度：中）==
 *	91:	loc_mtx(MTX2)
 *	//		中：TASK2→TASK3→TASK1，MTX2：TASK2
 *		rot_rdq(MID_PRIORITY)
 *	//		中：TASK3→TASK1→TASK2，MTX2：TASK2
 *	== TASK3-9（優先度：中）==
 *	92:	loc_mtx(MTX2)
 *	//		中：TASK1→TASK2，MTX2：TASK2→TASK3
 *	== TASK1（続き）==
 *	93:	loc_mtx(MTX1)
 *	//		中：TASK1→TASK2，MTX1：TASK1，MTX2：TASK2→TASK3
 *		loc_mtx(MTX2)
 *	//		中：TASK2，MTX1：TASK1，MTX2：TASK2→TASK3→TASK1
 *	== TASK2-6（続き）==
 *	94:	act_tsk(TASK4)
 *	//		中：TASK2→TASK4，MTX1：TASK1，MTX2：TASK2→TASK3→TASK1
 *		rot_rdq(MID_PRIORITY)
 *	//		中：TASK4→TASK2，MTX1：TASK1，MTX2：TASK2→TASK3→TASK1
 *	== TASK4-6（優先度：中）==
 *	95:	loc_mtx(MTX2)
 *	//		中：TASK2，MTX1：TASK1，MTX2：TASK2→TASK3→TASK1→TASK4
 *	== TASK2-6（続き）==
 *	96:	act_tsk(TASK8)
 *	//		中：TASK2，低：TASK8，MTX1：TASK1，MTX2：TASK2→TASK3→TASK1→TASK4
 *		act_tsk(TASK9)
 *	//		中：TASK2，低：TASK8→TASK9，MTX1：TASK1，MTX2：TASK2→TASK3→TASK1→TASK4
 *		slp_tsk()
 *	//		低：TASK8→TASK9，MTX1：TASK1，MTX2：TASK2→TASK3→TASK1→TASK4
 *	== TASK8-3（優先度：低）==
 *	97:	loc_mtx(MTX1) -> E_DLT
 *	//		低：TASK9，MTX1：TASK1→TASK8，MTX2：TASK2→TASK3→TASK1→TASK4
 *	== TASK9-3（優先度：低）==
 *	98:	wup_tsk(TASK2)
 *	//		中：TASK2，低：TASK9，MTX1：TASK1→TASK8，MTX2：TASK2→TASK3→TASK1→TASK4
 *	== TASK2-6（続き）==
 *	99:	ter_tsk(TASK9)
 *	//		中：TASK2，MTX1：TASK1→TASK8，MTX2：TASK2→TASK3→TASK1→TASK4
 *		act_tsk(TASK7)
 *	//		高：TASK7，中：TASK2，MTX1：TASK1→TASK8，MTX2：TASK2→TASK3→TASK1→TASK4
 *	== TASK7-7（優先度：高）==
 *	100:ini_mtx(MTX1)								... (C-4)
 *	//		高：TASK7，中：TASK2，低：TASK8，MTX2：TASK2→TASK3→TASK1→TASK4
 *		ref_mtx(MTX1, &rmtx)
 *		assert(rmtx.htskid == TSK_NONE)
 *		assert(rmtx.wtskid == TSK_NONE)
 *		ref_tsk(TASK1, &rtsk)
 *		assert(rtsk.tskpri == MID_PRIORITY)
 *		ext_tsk()
 *	//		中：TASK2，低：TASK8，MTX2：TASK2→TASK3→TASK1→TASK4
 *	== TASK2-6（続き）==
 *	101:unl_mtx(MTX2)
 *	//		中：TASK2→TASK3，低：TASK8，MTX2：TASK3→TASK1→TASK4
 *		ext_tsk()
 *	//		中：TASK3，低：TASK8，MTX2：TASK3→TASK1→TASK4
 *	== TASK3-9（続き）==
 *	102:unl_mtx(MTX2)
 *	//		中：TASK3→TASK1，低：TASK8，MTX2：TASK1→TASK4
 *		ext_tsk()
 *	//		中：TASK1，低：TASK8，MTX2：TASK1→TASK4
 *	== TASK1（続き）==
 *	103:unl_mtx(MTX2)
 *	//		中：TASK1→TASK4，低：TASK8，MTX2：TASK4
 *		chg_pri(TASK1, LOW_PRIORITY)
 *	//		中：TASK4，低：TASK8→TASK1，MTX2：TASK4
 *	== TASK4-6（続き）==
 *	104:unl_mtx(MTX2)
 *	//		中：TASK4，低：TASK8→TASK1
 *		ext_tsk()
 *	//		低：TASK8→TASK1
 *	== TASK8-3（続き）==
 *	105:ext_tsk()
 *	//		低：TASK1，MTX1：TASK1
 *	== TASK1（続き）==
 *	106:END
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
	T_RMTX	rmtx;
	T_RTSK	rtsk;

	test_start(__FILE__);

	check_point(1);
	ercd = loc_mtx(MTX1);
	check_ercd(ercd, E_OK);

	check_point(2);
	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	check_point(3);
	ercd = act_tsk(TASK6);
	check_ercd(ercd, E_OK);

	check_point(5);
	ercd = ini_mtx(MTX1);
	check_ercd(ercd, E_OK);

	check_point(7);
	ercd = loc_mtx(MTX1);
	check_ercd(ercd, E_OK);

	ercd = rot_rdq(MID_PRIORITY);
	check_ercd(ercd, E_OK);

	check_point(9);
	ercd = dis_dsp();
	check_ercd(ercd, E_OK);

	ercd = act_tsk(TASK3);
	check_ercd(ercd, E_OK);

	ercd = rot_rdq(MID_PRIORITY);
	check_ercd(ercd, E_OK);

	ercd = act_tsk(TASK4);
	check_ercd(ercd, E_OK);

	ercd = ini_mtx(MTX1);
	check_ercd(ercd, E_OK);

	ercd = ref_mtx(MTX1, &rmtx);
	check_ercd(ercd, E_OK);

	check_assert(rmtx.htskid == TSK_NONE);

	check_assert(rmtx.wtskid == TSK_NONE);

	ercd = ref_tsk(TASK1, &rtsk);
	check_ercd(ercd, E_OK);

	check_assert(rtsk.tskpri == MID_PRIORITY);

	ercd = ena_dsp();
	check_ercd(ercd, E_OK);

	check_point(11);
	ercd = rot_rdq(MID_PRIORITY);
	check_ercd(ercd, E_OK);

	check_point(14);
	ercd = loc_mtx(MTX1);
	check_ercd(ercd, E_OK);

	ercd = act_tsk(TASK8);
	check_ercd(ercd, E_OK);

	ercd = act_tsk(TASK9);
	check_ercd(ercd, E_OK);

	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(17);
	ercd = ter_tsk(TASK9);
	check_ercd(ercd, E_OK);

	ercd = dis_dsp();
	check_ercd(ercd, E_OK);

	ercd = act_tsk(TASK3);
	check_ercd(ercd, E_OK);

	ercd = rot_rdq(MID_PRIORITY);
	check_ercd(ercd, E_OK);

	ercd = act_tsk(TASK4);
	check_ercd(ercd, E_OK);

	ercd = ini_mtx(MTX1);
	check_ercd(ercd, E_OK);

	ercd = ref_mtx(MTX1, &rmtx);
	check_ercd(ercd, E_OK);

	check_assert(rmtx.htskid == TSK_NONE);

	check_assert(rmtx.wtskid == TSK_NONE);

	ercd = ref_tsk(TASK1, &rtsk);
	check_ercd(ercd, E_OK);

	check_assert(rtsk.tskpri == MID_PRIORITY);

	ercd = ena_dsp();
	check_ercd(ercd, E_OK);

	check_point(19);
	ercd = chg_pri(TASK1, LOW_PRIORITY);
	check_ercd(ercd, E_OK);

	check_point(22);
	ercd = chg_pri(TASK1, TPRI_INI);
	check_ercd(ercd, E_OK);

	check_point(23);
	ercd = loc_mtx(MTX1);
	check_ercd(ercd, E_OK);

	ercd = act_tsk(TASK6);
	check_ercd(ercd, E_OK);

	check_point(25);
	ercd = act_tsk(TASK3);
	check_ercd(ercd, E_OK);

	ercd = act_tsk(TASK7);
	check_ercd(ercd, E_OK);

	ercd = rot_rdq(HIGH_PRIORITY);
	check_ercd(ercd, E_OK);

	check_point(28);
	ercd = rot_rdq(MID_PRIORITY);
	check_ercd(ercd, E_OK);

	check_point(30);
	ercd = loc_mtx(MTX1);
	check_ercd(ercd, E_OK);

	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	ercd = rot_rdq(MID_PRIORITY);
	check_ercd(ercd, E_OK);

	check_point(32);
	ercd = act_tsk(TASK3);
	check_ercd(ercd, E_OK);

	ercd = rot_rdq(MID_PRIORITY);
	check_ercd(ercd, E_OK);

	check_point(36);
	ercd = rot_rdq(MID_PRIORITY);
	check_ercd(ercd, E_OK);

	check_point(39);
	ercd = loc_mtx(MTX1);
	check_ercd(ercd, E_OK);

	ercd = act_tsk(TASK8);
	check_ercd(ercd, E_OK);

	ercd = act_tsk(TASK9);
	check_ercd(ercd, E_OK);

	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(42);
	ercd = ter_tsk(TASK9);
	check_ercd(ercd, E_OK);

	check_point(43);
	ercd = act_tsk(TASK3);
	check_ercd(ercd, E_OK);

	ercd = rot_rdq(MID_PRIORITY);
	check_ercd(ercd, E_OK);

	check_point(47);
	ercd = chg_pri(TASK1, LOW_PRIORITY);
	check_ercd(ercd, E_OK);

	check_point(50);
	ercd = chg_pri(TASK1, TPRI_INI);
	check_ercd(ercd, E_OK);

	check_point(51);
	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	ercd = rot_rdq(MID_PRIORITY);
	check_ercd(ercd, E_OK);

	check_point(53);
	ercd = loc_mtx(MTX1);
	check_ercd(ercd, E_OK);

	ercd = loc_mtx(MTX2);
	check_ercd(ercd, E_OK);

	check_point(61);
	ercd = unl_mtx(MTX2);
	check_ercd(ercd, E_OK);

	check_point(62);
	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	ercd = rot_rdq(MID_PRIORITY);
	check_ercd(ercd, E_OK);

	check_point(64);
	ercd = loc_mtx(MTX1);
	check_ercd(ercd, E_OK);

	ercd = loc_mtx(MTX2);
	check_ercd(ercd, E_OK);

	check_point(73);
	ercd = unl_mtx(MTX2);
	check_ercd(ercd, E_OK);

	check_point(74);
	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	ercd = act_tsk(TASK3);
	check_ercd(ercd, E_OK);

	ercd = rot_rdq(MID_PRIORITY);
	check_ercd(ercd, E_OK);

	check_point(77);
	ercd = loc_mtx(MTX1);
	check_ercd(ercd, E_OK);

	ercd = loc_mtx(MTX2);
	check_ercd(ercd, E_OK);

	check_point(87);
	ercd = unl_mtx(MTX2);
	check_ercd(ercd, E_OK);

	check_point(88);
	ercd = rot_rdq(MID_PRIORITY);
	check_ercd(ercd, E_OK);

	check_point(90);
	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	ercd = act_tsk(TASK3);
	check_ercd(ercd, E_OK);

	ercd = rot_rdq(MID_PRIORITY);
	check_ercd(ercd, E_OK);

	check_point(93);
	ercd = loc_mtx(MTX1);
	check_ercd(ercd, E_OK);

	ercd = loc_mtx(MTX2);
	check_ercd(ercd, E_OK);

	check_point(103);
	ercd = unl_mtx(MTX2);
	check_ercd(ercd, E_OK);

	ercd = chg_pri(TASK1, LOW_PRIORITY);
	check_ercd(ercd, E_OK);

	check_finish(106);
	check_assert(false);
}

static uint_t	task2_count = 0;

void
task2(EXINF exinf)
{
	ER_UINT	ercd;

	switch (++task2_count) {
	case 1:
		check_point(8);
		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_DLT);

		check_point(13);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 2:
		check_point(31);
		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_DLT);

		check_point(38);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 3:
		check_point(52);
		ercd = loc_mtx(MTX2);
		check_ercd(ercd, E_OK);

		ercd = act_tsk(TASK3);
		check_ercd(ercd, E_OK);

		ercd = rot_rdq(MID_PRIORITY);
		check_ercd(ercd, E_OK);

		check_point(55);
		ercd = act_tsk(TASK6);
		check_ercd(ercd, E_OK);

		check_point(58);
		ercd = unl_mtx(MTX2);
		check_ercd(ercd, E_OK);

		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 4:
		check_point(63);
		ercd = loc_mtx(MTX2);
		check_ercd(ercd, E_OK);

		ercd = act_tsk(TASK3);
		check_ercd(ercd, E_OK);

		ercd = rot_rdq(MID_PRIORITY);
		check_ercd(ercd, E_OK);

		check_point(66);
		ercd = act_tsk(TASK6);
		check_ercd(ercd, E_OK);

		check_point(68);
		ercd = rot_rdq(HIGH_PRIORITY);
		check_ercd(ercd, E_OK);

		check_point(71);
		ercd = unl_mtx(MTX2);
		check_ercd(ercd, E_OK);

		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 5:
		check_point(75);
		ercd = loc_mtx(MTX2);
		check_ercd(ercd, E_OK);

		ercd = rot_rdq(MID_PRIORITY);
		check_ercd(ercd, E_OK);

		check_point(78);
		ercd = act_tsk(TASK4);
		check_ercd(ercd, E_OK);

		ercd = rot_rdq(MID_PRIORITY);
		check_ercd(ercd, E_OK);

		check_point(80);
		ercd = act_tsk(TASK5);
		check_ercd(ercd, E_OK);

		ercd = rot_rdq(MID_PRIORITY);
		check_ercd(ercd, E_OK);

		check_point(82);
		ercd = act_tsk(TASK7);
		check_ercd(ercd, E_OK);

		check_point(84);
		ercd = unl_mtx(MTX2);
		check_ercd(ercd, E_OK);

		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 6:
		check_point(91);
		ercd = loc_mtx(MTX2);
		check_ercd(ercd, E_OK);

		ercd = rot_rdq(MID_PRIORITY);
		check_ercd(ercd, E_OK);

		check_point(94);
		ercd = act_tsk(TASK4);
		check_ercd(ercd, E_OK);

		ercd = rot_rdq(MID_PRIORITY);
		check_ercd(ercd, E_OK);

		check_point(96);
		ercd = act_tsk(TASK8);
		check_ercd(ercd, E_OK);

		ercd = act_tsk(TASK9);
		check_ercd(ercd, E_OK);

		ercd = slp_tsk();
		check_ercd(ercd, E_OK);

		check_point(99);
		ercd = ter_tsk(TASK9);
		check_ercd(ercd, E_OK);

		ercd = act_tsk(TASK7);
		check_ercd(ercd, E_OK);

		check_point(101);
		ercd = unl_mtx(MTX2);
		check_ercd(ercd, E_OK);

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
		check_point(10);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 2:
		check_point(18);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 3:
		check_point(29);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 4:
		check_point(33);
		ercd = act_tsk(TASK4);
		check_ercd(ercd, E_OK);

		ercd = act_tsk(TASK7);
		check_ercd(ercd, E_OK);

		check_point(35);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 5:
		check_point(44);
		ercd = act_tsk(TASK4);
		check_ercd(ercd, E_OK);

		ercd = act_tsk(TASK7);
		check_ercd(ercd, E_OK);

		check_point(46);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 6:
		check_point(54);
		ercd = loc_mtx(MTX2);
		check_ercd(ercd, E_OK);

		check_point(60);
		ercd = unl_mtx(MTX2);
		check_ercd(ercd, E_OK);

		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 7:
		check_point(65);
		ercd = loc_mtx(MTX2);
		check_ercd(ercd, E_OK);

		check_point(72);
		ercd = unl_mtx(MTX2);
		check_ercd(ercd, E_OK);

		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 8:
		check_point(76);
		ercd = loc_mtx(MTX2);
		check_ercd(ercd, E_OK);

		check_point(86);
		ercd = unl_mtx(MTX2);
		check_ercd(ercd, E_OK);

		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 9:
		check_point(92);
		ercd = loc_mtx(MTX2);
		check_ercd(ercd, E_OK);

		check_point(102);
		ercd = unl_mtx(MTX2);
		check_ercd(ercd, E_OK);

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
		check_point(20);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 3:
		check_point(37);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 4:
		check_point(48);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 5:
		check_point(79);
		ercd = loc_mtx(MTX2);
		check_ercd(ercd, E_OK);

		check_point(89);
		ercd = unl_mtx(MTX2);
		check_ercd(ercd, E_OK);

		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 6:
		check_point(95);
		ercd = loc_mtx(MTX2);
		check_ercd(ercd, E_OK);

		check_point(104);
		ercd = unl_mtx(MTX2);
		check_ercd(ercd, E_OK);

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
		check_point(81);
		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_DLT);

		check_point(85);
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
	T_RMTX	rmtx;
	T_RTSK	rtsk;

	switch (++task6_count) {
	case 1:
		check_point(4);
		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_DLT);

		check_point(6);
		ercd = ref_mtx(MTX1, &rmtx);
		check_ercd(ercd, E_OK);

		check_assert(rmtx.htskid == TSK_NONE);

		check_assert(rmtx.wtskid == TSK_NONE);

		ercd = ref_tsk(TASK1, &rtsk);
		check_ercd(ercd, E_OK);

		check_assert(rtsk.tskpri == MID_PRIORITY);

		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 2:
		check_point(24);
		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_DLT);

		check_point(27);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 3:
		check_point(56);
		ercd = act_tsk(TASK7);
		check_ercd(ercd, E_OK);

		ercd = chg_pri(TASK2, HIGH_PRIORITY);
		check_ercd(ercd, E_OK);

		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_DLT);

		check_point(59);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 4:
		check_point(67);
		ercd = act_tsk(TASK7);
		check_ercd(ercd, E_OK);

		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_DLT);

		check_point(70);
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
	T_RMTX	rmtx;
	T_RTSK	rtsk;

	switch (++task7_count) {
	case 1:
		check_point(26);
		ercd = ini_mtx(MTX1);
		check_ercd(ercd, E_OK);

		ercd = ref_mtx(MTX1, &rmtx);
		check_ercd(ercd, E_OK);

		check_assert(rmtx.htskid == TSK_NONE);

		check_assert(rmtx.wtskid == TSK_NONE);

		ercd = ref_tsk(TASK1, &rtsk);
		check_ercd(ercd, E_OK);

		check_assert(rtsk.tskpri == MID_PRIORITY);

		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 2:
		check_point(34);
		ercd = ini_mtx(MTX1);
		check_ercd(ercd, E_OK);

		ercd = ref_mtx(MTX1, &rmtx);
		check_ercd(ercd, E_OK);

		check_assert(rmtx.htskid == TSK_NONE);

		check_assert(rmtx.wtskid == TSK_NONE);

		ercd = ref_tsk(TASK1, &rtsk);
		check_ercd(ercd, E_OK);

		check_assert(rtsk.tskpri == MID_PRIORITY);

		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 3:
		check_point(45);
		ercd = ini_mtx(MTX1);
		check_ercd(ercd, E_OK);

		ercd = ref_mtx(MTX1, &rmtx);
		check_ercd(ercd, E_OK);

		check_assert(rmtx.htskid == TSK_NONE);

		check_assert(rmtx.wtskid == TSK_NONE);

		ercd = ref_tsk(TASK1, &rtsk);
		check_ercd(ercd, E_OK);

		check_assert(rtsk.tskpri == MID_PRIORITY);

		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 4:
		check_point(57);
		ercd = ini_mtx(MTX1);
		check_ercd(ercd, E_OK);

		ercd = ref_mtx(MTX1, &rmtx);
		check_ercd(ercd, E_OK);

		check_assert(rmtx.htskid == TSK_NONE);

		check_assert(rmtx.wtskid == TSK_NONE);

		ercd = ref_tsk(TASK1, &rtsk);
		check_ercd(ercd, E_OK);

		check_assert(rtsk.tskpri == MID_PRIORITY);

		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 5:
		check_point(69);
		ercd = ini_mtx(MTX1);
		check_ercd(ercd, E_OK);

		ercd = ref_mtx(MTX1, &rmtx);
		check_ercd(ercd, E_OK);

		check_assert(rmtx.htskid == TSK_NONE);

		check_assert(rmtx.wtskid == TSK_NONE);

		ercd = ref_tsk(TASK1, &rtsk);
		check_ercd(ercd, E_OK);

		check_assert(rtsk.tskpri == MID_PRIORITY);

		ercd = ref_tsk(TASK2, &rtsk);
		check_ercd(ercd, E_OK);

		check_assert(rtsk.tskpri == MID_PRIORITY);

		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 6:
		check_point(83);
		ercd = ini_mtx(MTX1);
		check_ercd(ercd, E_OK);

		ercd = ref_mtx(MTX1, &rmtx);
		check_ercd(ercd, E_OK);

		check_assert(rmtx.htskid == TSK_NONE);

		check_assert(rmtx.wtskid == TSK_NONE);

		ercd = ref_tsk(TASK1, &rtsk);
		check_ercd(ercd, E_OK);

		check_assert(rtsk.tskpri == MID_PRIORITY);

		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 7:
		check_point(100);
		ercd = ini_mtx(MTX1);
		check_ercd(ercd, E_OK);

		ercd = ref_mtx(MTX1, &rmtx);
		check_ercd(ercd, E_OK);

		check_assert(rmtx.htskid == TSK_NONE);

		check_assert(rmtx.wtskid == TSK_NONE);

		ercd = ref_tsk(TASK1, &rtsk);
		check_ercd(ercd, E_OK);

		check_assert(rtsk.tskpri == MID_PRIORITY);

		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	default:
		check_assert(false);
	}
	check_assert(false);
}

static uint_t	task8_count = 0;

void
task8(EXINF exinf)
{
	ER_UINT	ercd;

	switch (++task8_count) {
	case 1:
		check_point(15);
		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_DLT);

		check_point(21);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 2:
		check_point(40);
		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_DLT);

		check_point(49);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 3:
		check_point(97);
		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_DLT);

		check_point(105);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	default:
		check_assert(false);
	}
	check_assert(false);
}

static uint_t	task9_count = 0;

void
task9(EXINF exinf)
{
	ER_UINT	ercd;

	switch (++task9_count) {
	case 1:
		check_point(16);
		ercd = wup_tsk(TASK1);
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 2:
		check_point(41);
		ercd = wup_tsk(TASK1);
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 3:
		check_point(98);
		ercd = wup_tsk(TASK2);
		check_ercd(ercd, E_OK);

		check_assert(false);

	default:
		check_assert(false);
	}
	check_assert(false);
}
