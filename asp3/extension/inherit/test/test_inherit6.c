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
 *  $Id: test_inherit6.c 1616 2022-09-18 14:22:03Z ertl-hiro $
 */

/* 
 *		優先度継承ミューテックスのテスト(6)
 *
 * 【テストの目的】
 *
 *  タスクの終了処理（task_terminate）時の優先度継承ミューテックスによ
 *  る優先度制御についてテストする．
 *
 * 【テスト項目】
 *
 *	(A) 自タスクの終了時のミューテックスのロック解除
 *		(A-1) ロックしていたミューテックス（1つ）がロック解除されること．
 *		(A-2) ロックしていたミューテックス（1つ）がロック解除され，ロッ
 *			  クを待っていたタスクがそれをロックし，待ち解除されること．
 *		(A-3) ロックしていたミューテックス（複数）がロックしたのと逆順
 *			  でロック解除されること．
 *		(A-4) ロックしていたミューテックス（複数）がロックしたのと逆順
 *			  でロック解除され，ロックを待っていたタスク（複数）がそれ
 *			  ぞれをロックし，待ち解除されること（後にロックしたミュー
 *			  テックスを待っていたタスクの方が，優先順位が高くなること
 *			  を確認する）．
 *	(B) 実行可能状態のタスクの終了時のミューテックスのロック解除
 *		(B-1) ロックしていたミューテックス（1つ）がロック解除されること．
 *		(B-2) ロックしていたミューテックス（1つ）がロック解除され，ロッ
 *			  クを待っていたタスクがそれをロックし，待ち解除されること．
 *		(B-3) (B-2)の結果，タスクディスパッチが起こること．
 *		(B-4) ロックしていたミューテックス（複数）がすべてロック解除さ
 *			  れること．
 *		(B-5) ロックしていたミューテックス（複数）がロックしたのと逆順
 *			  でロック解除され，ロックを待っていたタスク（複数）がそれ
 *			  ぞれをロックし，待ち解除されること．（後にロックしたミュー
 *			  テックスを待っていたタスクの方が，優先順位が高くなること
 *			  を確認する）．
 *		(B-6) (B-5)の結果，タスクディスパッチが起こること．
 *	(C) 優先度継承ミューテックスのロック待ち状態のタスクの終了時のミュー
 *		テックスのロック解除
 *		(C-1) ロックしていたミューテックス（1つ）がロック解除されること．
 *		(C-2) ロックしていたミューテックス（1つ）がロック解除され，ロッ
 *			  クを待っていたタスクがそれをロックし，待ち解除されること．
 *		(C-3) (C-2)の結果，タスクディスパッチが起こること．
 *		(C-4) ロックしていたミューテックス（複数）がすべてロック解除さ
 *			  れること．
 *		(C-5) ロックしていたミューテックス（複数）がロックしたのと逆順
 *			  でロック解除され，ロックを待っていたタスク（複数）がそれ
 *			  ぞれをロックし，待ち解除されること．（後にロックしたミュー
 *			  テックスを待っていたタスクの方が，優先順位が高くなること
 *			  を確認する）．
 *		(C-6) (C-5)の結果，タスクディスパッチが起こること．
 *
 * 【テスト項目の実現方法】
 *
 *	(A-1)
 *		中優先度タスク（TASK2）に優先度継承ミューテックス（MTX1）をロッ
 *		クさせ，TASK2がext_tsk()すると，ロックが解除されることを確認す
 *		る．
 *	(A-2)
 *		中優先度タスク（TASK2）に優先度継承ミューテックス（MTX1）をロッ
 *		クさせ，別の中優先度タスク（TASK3）にMTX1を待たせた状態で，
 *		TASK2がext_tsk()すると，TASK3がMTX1をロックし，待ち解除される
 *		ことを確認する．
 *	(A-3)
 *		中優先度タスク（TASK2）に優先度継承ミューテックス2つ（MTX1，
 *		MTX2）をロックさせ，TASK2がext_tsk()すると，両方のロックが解除
 *		されることを確認する．
 *	(A-4)
 *		中優先度タスク（TASK2）に優先度継承ミューテックス2つ（MTX1，
 *		MTX2）をこの順でロックさせ，別の中優先度タスク2つ（TASK3，
 *		TASK4）にそれぞれMTX1とMTX2を待たせた状態で，TASK2がext_tsk()
 *		すると，TASK3とTASK4がそれぞれをロックし，待ち解除されることを
 *		確認する．また，後にロックしていたMTX2を待っていたTASK4が，
 *		TASK3よりも優先順位が高くなることを確認する．
 *	(B-1)
 *		中優先度タスク（TASK2）に優先度継承ミューテックス（MTX1）をロッ
 *		クさせ，高優先度タスク（TASK5）がter_tsk(TASK2)すると，ロック
 *		が解除されることを確認する．
 *	(B-2)
 *		中優先度タスク（TASK2）に優先度継承ミューテックス（MTX1）をロッ
 *		クさせ，別の中優先度タスク（TASK3）にMTX1を待たせた状態で，高
 *		優先度タスク（TASK5）がter_tsk(TASK2)すると，TASK3がMTX1をロッ
 *		クし，待ち解除されることを確認する．
 *	(B-3)
 *		中優先度タスク（TASK2）に優先度継承ミューテックス（MTX1）をロッ
 *		クさせた後，待ち状態にする．高優先度タスク（TASK5）にMTX1を待
 *		たせた状態で，低優先度タスク（TASK1）がter_tsk(TASK2)すると，
 *		TASK5がMTX1をロックし，待ち解除され，ディスパッチが起こること
 *		を確認する．
 *	(B-4)
 *		中優先度タスク（TASK2）に優先度継承ミューテックス2つ（MTX1，
 *		MTX2）をロックさせ，高優先度タスク（TASK5）がter_tsk(TASK2)す
 *		ると，両方のロックが解除されることを確認する．
 *	(B-5)
 *		中優先度タスク（TASK2）に優先度継承ミューテックス2つ（MTX1，
 *		MTX2）をこの順でロックさせ，別の中優先度タスク2つ（TASK3，
 *		TASK4）にそれぞれMTX1とMTX2を待たせた状態で，高優先度タスク
 *		（TASK5）がter_tsk(TASK2)すると，TASK3とTASK4がそれぞれをロッ
 *		クし，待ち解除されることを確認する．また，後にロックしていた
 *		MTX2を待っていたTASK4が，TASK3よりも優先順位が高くなることを確
 *		認する．
 *	(B-6)
 *		中優先度タスク（TASK2）に優先度継承ミューテックス2つ（MTX1，
 *		MTX2）をこの順でロックさせた後，待ち状態にする．高優先度タスク
 *		2つ（TASK5，TASK6）にそれぞれMTX1とMTX2を待たせた状態で，低優
 *		先度タスク（TASK1）がter_tsk(TASK2)すると，TASK5とTASK6がそれ
 *		ぞれをロックし，待ち解除されることを確認する．また，後にロック
 *		していたMTX2を待っていたTASK6が，TASK5よりも優先順位が高くなる
 *		ことと，ディスパッチが起こることを確認する．
 *	(C-1)
 *		低優先度タスク（TASK1）に優先度継承ミューテックス（MTX3）をロッ
 *		クさせる．中優先度タスク（TASK2）に優先度継承ミューテックス
 *		（MTX1）をロックさせ，さらにMTX3に対する待ち状態にする．高優先
 *		度タスク（TASK5）がter_tsk(TASK2)すると，ロックが解除されるこ
 *		とを確認する．
 *	(C-2)
 *		低優先度タスク（TASK1）に優先度継承ミューテックス（MTX3）をロッ
 *		クさせる．中優先度タスク（TASK2）に優先度継承ミューテックス
 *		（MTX1）をロックさせ，さらにMTX3に対する待ち状態にする．別の中
 *		優先度タスク（TASK3）にMTX1を待たせた状態で，高優先度タスク
 *		（TASK5）がter_tsk(TASK2)すると，TASK3がMTX1をロックし，待ち解
 *		除されることを確認する．
 *	(C-3)
 *		低優先度タスク（TASK1）に優先度継承ミューテックス（MTX3）をロッ
 *		クさせる．中優先度タスク（TASK2）に優先度継承ミューテックス
 *		（MTX1）をロックさせ，さらにMTX3に対する待ち状態にする．高優先
 *		度タスク（TASK5）にMTX1を待たせた状態で，TASK1がter_tsk(TASK2)
 *		すると，TASK5がMTX1をロックし，待ち解除され，ディスパッチが起
 *		こることを確認する．
 *	(C-4)
 *		低優先度タスク（TASK1）に優先度継承ミューテックス（MTX3）をロッ
 *		クさせる．中優先度タスク（TASK2）に優先度継承ミューテックス2つ
 *		（MTX1，MTX2）をロックさせ，さらにMTX3に対する待ち状態にする．
 *		高優先度タスク（TASK5）がter_tsk(TASK2)すると，両方のロックが
 *		解除されることを確認する．
 *	(C-5)
 *		低優先度タスク（TASK1）に優先度継承ミューテックス（MTX3）をロッ
 *		クさせる．中優先度タスク（TASK2）に優先度継承ミューテックス2つ
 *		（MTX1，MTX2）をこの順でロックさせ，さらにMTX3に対する待ち状態
 *		にする．別の中優先度タスク2つ（TASK3，TASK4）にそれぞれMTX1と
 *		MTX2を待たせた状態で，高優先度タスク（TASK5）がter_tsk(TASK2)
 *		すると，TASK3とTASK4がそれぞれをロックし，待ち解除されることを
 *		確認する．また，後にロックしていたMTX2を待っていたTASK4が，
 *		TASK3よりも優先順位が高くなることを確認する．
 *	(C-6)
 *		低優先度タスク（TASK1）に優先度継承ミューテックス（MTX3）をロッ
 *		クさせる．中優先度タスク（TASK2）に優先度継承ミューテックス2つ
 *		（MTX1，MTX2）をこの順でロックさせ，さらにMTX3に対する待ち状態
 *		にする．高優先度タスク2つ（TASK5，TASK6）にそれぞれMTX1とMTX2
 *		を待たせた状態で，低優先度タスク（TASK1）がter_tsk(TASK2)する
 *		と，TASK5とTASK6がそれぞれをロックし，待ち解除されることを確認
 *		する．また，後にロックしていたMTX2を待っていたTASK4が，TASK3よ
 *		りも優先順位が高くなることと，ディスパッチが起こることを確認す
 *		る．
 *
 * 【使用リソース】
 *
 *	TASK1: 低優先度タスク，メインタスク，最初から起動
 *	TASK2: 中優先度タスク
 *	TASK3: 中優先度タスク
 *	TASK4: 中優先度タスク
 *	TASK5: 高優先度タスク
 *	TASK6: 高優先度タスク
 *	MTX1: ミューテックス（優先度継承）
 *	MTX2: ミューテックス（優先度継承）
 *	MTX3: ミューテックス（優先度継承）
 *
 * 【テストシーケンス】
 *
 *	== TASK1（優先度：低）==
 *	//		低：TASK1
 *	1:	act_tsk(TASK2)
 *	//		中：TASK2，低：TASK1
 *	== TASK2-1（優先度：中）==
 *	2:	loc_mtx(MTX1)
 *	//		中：TASK2，低：TASK1，MTX1：TASK2
 *		ext_tsk()										... (A-1)
 *	//		低：TASK1
 *	== TASK1（続き）==
 *	3:	ref_mtx(MTX1, &rmtx)
 *		assert(rmtx.htskid == TSK_NONE)
 *		assert(rmtx.wtskid == TSK_NONE)
 *
 *	4:	act_tsk(TASK2)
 *	//		中：TASK2，低：TASK1
 *	== TASK2-2（優先度：中）==
 *	5:	loc_mtx(MTX1)
 *	//		中：TASK2，低：TASK1，MTX1：TASK2
 *		act_tsk(TASK3)
 *	//		中：TASK2→TASK3，低：TASK1，MTX1：TASK2
 *		rot_rdq(MID_PRIORITY)
 *	//		中：TASK3→TASK2，低：TASK1，MTX1：TASK2
 *	== TASK3-1（優先度：中）==
 *	6:	loc_mtx(MTX1)
 *	//		中：TASK2，低：TASK1，MTX1：TASK2→TASK3
 *	== TASK2-2（続き）==
 *	7:	ext_tsk()										... (A-2)
 *	//		中：TASK3，低：TASK1，MTX1：TASK3
 *	== TASK3-1（続き）==
 *	8:	ref_mtx(MTX1, &rmtx)
 *		assert(rmtx.htskid == TASK3)
 *		assert(rmtx.wtskid == TSK_NONE)
 *		ext_tsk()
 *	//		低：TASK1
 *
 *	== TASK1（続き）==
 *	9:	act_tsk(TASK2)
 *	//		中：TASK2，低：TASK1
 *	== TASK2-3（優先度：中）==
 *	10:	loc_mtx(MTX1)
 *	//		中：TASK2，低：TASK1，MTX1：TASK2
 *		loc_mtx(MTX2)
 *	//		中：TASK2，低：TASK1，MTX1：TASK2，MTX2：TASK2
 *		ext_tsk()										... (A-3)
 *	//		低：TASK1
 *	== TASK1（続き）==
 *	11:	ref_mtx(MTX1, &rmtx)
 *		assert(rmtx.htskid == TSK_NONE)
 *		assert(rmtx.wtskid == TSK_NONE)
 *		ref_mtx(MTX2, &rmtx)
 *		assert(rmtx.htskid == TSK_NONE)
 *		assert(rmtx.wtskid == TSK_NONE)
 *
 *	12:	act_tsk(TASK2)
 *	//		中：TASK2，低：TASK1
 *	== TASK2-4（優先度：中）==
 *	13:	loc_mtx(MTX1)
 *	//		中：TASK2，低：TASK1，MTX1：TASK2
 *		loc_mtx(MTX2)
 *	//		中：TASK2，低：TASK1，MTX1：TASK2，MTX2：TASK2
 *		act_tsk(TASK3)
 *	//		中：TASK2→TASK3，低：TASK1，MTX1：TASK2，MTX2：TASK2
 *		act_tsk(TASK4)
 *	//		中：TASK2→TASK3→TASK4，低：TASK1，MTX1：TASK2，MTX2：TASK2
 *		rot_rdq(MID_PRIORITY)
 *	//		中：TASK3→TASK4→TASK2，低：TASK1，MTX1：TASK2，MTX2：TASK2
 *	== TASK3-2（優先度：中）==
 *	14:	loc_mtx(MTX1)
 *	//		中：TASK4→TASK2，低：TASK1，MTX1：TASK2→TASK3，MTX2：TASK2
 *	== TASK4-1（優先度：中）==
 *	15:	loc_mtx(MTX2)
 *	//		中：TASK2，低：TASK1，MTX1：TASK2→TASK3，MTX2：TASK2→TASK4
 *	== TASK2-4（続き）==
 *	16:	ext_tsk()										... (A-4)
 *	//		中：TASK4→TASK3，低：TASK1，MTX1：TASK3，MTX2：TASK4
 *	== TASK4-1（続き）==
 *	17:	ref_mtx(MTX1, &rmtx)
 *		assert(rmtx.htskid == TASK3)
 *		assert(rmtx.wtskid == TSK_NONE)
 *		ref_mtx(MTX2, &rmtx)
 *		assert(rmtx.htskid == TASK4)
 *		assert(rmtx.wtskid == TSK_NONE)
 *		ext_tsk()
 *	//		中：TASK3，低：TASK1，MTX1：TASK3
 *	== TASK3-2（続き）==
 *	18:	ext_tsk()
 *	//		低：TASK1
 *
 *	== TASK1（続き）==
 *	19:	act_tsk(TASK2)
 *	//		中：TASK2，低：TASK1
 *	== TASK2-5（優先度：中）==
 *	20:	loc_mtx(MTX1)
 *	//		中：TASK2，低：TASK1，MTX1：TASK2
 *		act_tsk(TASK5)
 *	//		高：TASK5，中：TASK2，低：TASK1，MTX1：TASK2
 *	== TASK5-1（優先度：高）==
 *	21:	ter_tsk(TASK2)									... (B-1)
 *	//		高：TASK5，低：TASK1
 *		ref_mtx(MTX1, &rmtx)
 *		assert(rmtx.htskid == TSK_NONE)
 *		assert(rmtx.wtskid == TSK_NONE)
 *		ext_tsk()
 *	//		低：TASK1
 *
 *	== TASK1（続き）==
 *	22:	act_tsk(TASK2)
 *	//		中：TASK2，低：TASK1
 *	== TASK2-6（優先度：中）==
 *	23:	act_tsk(TASK3)
 *	//		中：TASK2→TASK3，低：TASK1
 *		loc_mtx(MTX1)
 *	//		中：TASK2→TASK3，低：TASK1，MTX1：TASK2
 *		rot_rdq(MID_PRIORITY)
 *	//		中：TASK3→TASK2，低：TASK1，MTX1：TASK2
 *	== TASK3-3（優先度：中）==
 *	24:	loc_mtx(MTX1)
 *	//		中：TASK2，低：TASK1，MTX1：TASK2→TASK3
 *	== TASK2-6（続き）==
 *	25:	act_tsk(TASK5)
 *	//		高：TASK5，中：TASK2，低：TASK1，MTX1：TASK2→TASK3
 *	== TASK5-2（優先度：高）==
 *	26:	ter_tsk(TASK2)									... (B-2)
 *	//		高：TASK5，中：TASK3，低：TASK1，MTX1：TASK3
 *		ref_mtx(MTX1, &rmtx)
 *		assert(rmtx.htskid == TASK3)
 *		assert(rmtx.wtskid == TSK_NONE)
 *		ext_tsk()
 *	//		中：TASK3，低：TASK1，MTX1：TASK3
 *	== TASK3-3（続き）==
 *	27:	ext_tsk()
 *	//		低：TASK1
 *
 *	== TASK1（続き）==
 *	28:	act_tsk(TASK2)
 *	//		中：TASK2，低：TASK1
 *	== TASK2-7（優先度：中）==
 *	29:	loc_mtx(MTX1)
 *	//		中：TASK2，低：TASK1，MTX1：TASK2
 *		slp_tsk()
 *	//		低：TASK1，MTX1：TASK2
 *	== TASK1（続き）==
 *	30:	act_tsk(TASK5)
 *	//		高：TASK5，低：TASK1，MTX1：TASK2
 *	== TASK5-3（優先度：高）==
 *	31:	loc_mtx(MTX1)
 *	//		低：TASK1，MTX1：TASK2→TASK5
 *	== TASK1（続き）==
 *	32:	ter_tsk(TASK2)									... (B-3)
 *	//		高：TASK5，低：TASK1，MTX1：TASK5
 *	== TASK5-3（続き）
 *	33:	ref_mtx(MTX1, &rmtx)
 *		assert(rmtx.htskid == TASK5)
 *		assert(rmtx.wtskid == TSK_NONE)
 *		ext_tsk()
 *	//		低：TASK1
 *
 *	== TASK1（続き）==
 *	34:	act_tsk(TASK2)
 *	//		中：TASK2，低：TASK1
 *	== TASK2-8（優先度：中）==
 *	35:	loc_mtx(MTX1)
 *	//		中：TASK2，低：TASK1，MTX1：TASK2
 *		loc_mtx(MTX2)
 *	//		中：TASK2，低：TASK1，MTX1：TASK2，MTX2：TASK2
 *		act_tsk(TASK5)
 *	//		高：TASK5，中：TASK2，低：TASK1，MTX1：TASK2，MTX2：TASK2
 *	== TASK5-4（優先度：高）==
 *	36:	ter_tsk(TASK2)									... (B-4)
 *	//		高：TASK5，低：TASK1
 *		ref_mtx(MTX1, &rmtx)
 *		assert(rmtx.htskid == TSK_NONE)
 *		assert(rmtx.wtskid == TSK_NONE)
 *		ref_mtx(MTX2, &rmtx)
 *		assert(rmtx.htskid == TSK_NONE)
 *		assert(rmtx.wtskid == TSK_NONE)
 *		ext_tsk()
 *	//		低：TASK1
 *
 *	== TASK1（続き）==
 *	37:	act_tsk(TASK2)
 *	//		中：TASK2，低：TASK1
 *	== TASK2-9（優先度：中）==
 *	38:	act_tsk(TASK3)
 *	//		中：TASK2→TASK3，低：TASK1
 *		act_tsk(TASK4)
 *	//		中：TASK2→TASK3→TASK4，低：TASK1
 *		loc_mtx(MTX1)
 *	//		中：TASK2→TASK3→TASK4，低：TASK1，MTX1：TASK2
 *		loc_mtx(MTX2)
 *	//		中：TASK2→TASK3→TASK4，低：TASK1，MTX1：TASK2，MTX2：TASK2
 *		rot_rdq(MID_PRIORITY)
 *	//		中：TASK3→TASK4→TASK2，低：TASK1，MTX1：TASK2，MTX2：TASK2
 *	== TASK3-4（優先度：中）==
 *	39:	loc_mtx(MTX1)
 *	//		中：TASK4→TASK2，低：TASK1，MTX1：TASK2→TASK3，MTX2：TASK2
 *	== TASK4-2（優先度：中）==
 *	40:	loc_mtx(MTX2)
 *	//		中：TASK2，低：TASK1，MTX1：TASK2→TASK3，MTX2：TASK2→TASK4
 *	== TASK2-9（続き）==
 *	41:	act_tsk(TASK5)
 *	//		高：TASK5，中：TASK2，低：TASK1，MTX1：TASK2→TASK3，MTX2：TASK2→TASK4
 *	== TASK5-5（優先度：高）==
 *	42:	ter_tsk(TASK2)									... (B-5)
 *	//		高：TASK5，中：TASK4→TASK3，低：TASK1，MTX1：TASK3，MTX2：TASK4
 *		ref_mtx(MTX1, &rmtx)
 *		assert(rmtx.htskid == TASK3)
 *		assert(rmtx.wtskid == TSK_NONE)
 *		ref_mtx(MTX2, &rmtx)
 *		assert(rmtx.htskid == TASK4)
 *		assert(rmtx.wtskid == TSK_NONE)
 *		ext_tsk()
 *	//		中：TASK4→TASK3，低：TASK1，MTX1：TASK3，MTX2：TASK4
 *	== TASK4-2（続き）==
 *	43:	ext_tsk()
 *	//		中：TASK3，低：TASK1，MTX1：TASK3
 *	== TASK3-4（続き）==
 *	44:	ext_tsk()
 *	//		低：TASK1
 *
 *	== TASK1（続き）==
 *	45:	act_tsk(TASK2)
 *	//		中：TASK2，低：TASK1
 *	== TASK2-10（優先度：中）==
 *	46:	loc_mtx(MTX1)
 *	//		中：TASK2，低：TASK1，MTX1：TASK2
 *		loc_mtx(MTX2)
 *	//		中：TASK2，低：TASK1，MTX1：TASK2，MTX2：TASK2
 *		slp_tsk()
 *	//		低：TASK1，MTX1：TASK2，MTX2：TASK2
 *	== TASK1（続き）==
 *	47:	act_tsk(TASK5)
 *	//		高：TASK5，低：TASK1，MTX1：TASK2，MTX2：TASK2
 *	== TASK5-6（優先度：高）==
 *	48:	loc_mtx(MTX1)
 *	//		低：TASK1，MTX1：TASK2→TASK5，MTX2：TASK2
 *	== TASK1（続き）==
 *	49:	act_tsk(TASK6)
 *	//		高：TASK6，低：TASK1，MTX1：TASK2→TASK5，MTX2：TASK2
 *	== TASK6-1（優先度：高）==
 *	50:	loc_mtx(MTX2)
 *	//		低：TASK1，MTX1：TASK2→TASK5，MTX2：TASK2→TASK6
 *	== TASK1（続き）==
 *	51:	ter_tsk(TASK2)									... (B-6)
 *	//		高：TASK6→TASK5，低：TASK1，MTX1：TASK5，MTX2：TASK6
 *	== TASK6-1（続き）
 *	52:	ref_mtx(MTX1, &rmtx)
 *		assert(rmtx.htskid == TASK5)
 *		assert(rmtx.wtskid == TSK_NONE)
 *		ref_mtx(MTX2, &rmtx)
 *		assert(rmtx.htskid == TASK6)
 *		assert(rmtx.wtskid == TSK_NONE)
 *		ext_tsk()
 *	//		高：TASK5，低：TASK1，MTX1：TASK5
 *	== TASK5-6（続き）
 *	53:	ext_tsk()
 *	//		低：TASK1
 *
 *	== TASK1（続き）==
 *	54:	loc_mtx(MTX3)
 *	//		低：TASK1，MTX3：TASK1
 *		act_tsk(TASK2)
 *	//		中：TASK2，低：TASK1，MTX3：TASK1
 *	== TASK2-11（優先度：中）==
 *	55:	loc_mtx(MTX1)
 *	//		中：TASK2，低：TASK1，MTX1：TASK2，MTX3：TASK1
 *		loc_mtx(MTX3)
 *	//		中：TASK1，MTX1：TASK2，MTX3：TASK1→TASK2
 *	== TASK1（続き）==
 *	56:	act_tsk(TASK5)
 *	//		高：TASK5，中：TASK1，MTX1：TASK2，MTX3：TASK1→TASK2
 *	== TASK5-7（優先度：高）==
 *	57:	ter_tsk(TASK2)									... (C-1)
 *	//		高：TASK5，低：TASK1，MTX3：TASK1
 *		ref_mtx(MTX1, &rmtx)
 *		assert(rmtx.htskid == TSK_NONE)
 *		assert(rmtx.wtskid == TSK_NONE)
 *		ext_tsk()
 *	//		低：TASK1，MTX3：TASK1
 *
 *	== TASK1（続き）==
 *	58:	act_tsk(TASK2)
 *	//		中：TASK2，低：TASK1，MTX3：TASK1
 *	== TASK2-12（優先度：中）==
 *	59:	act_tsk(TASK3)
 *	//		中：TASK2→TASK3，低：TASK1，MTX3：TASK1
 *		loc_mtx(MTX1)
 *	//		中：TASK2→TASK3，低：TASK1，MTX1：TASK2，MTX3：TASK1
 *		loc_mtx(MTX3)
 *	//		中：TASK1→TASK3，MTX1：TASK2，MTX3：TASK1→TASK2
 *	== TASK1（続き）==
 *	60:	rot_rdq(MID_PRIORITY)
 *	//		中：TASK3→TASK1，MTX1：TASK2，MTX3：TASK1→TASK2
 *	== TASK3-5（優先度：中）==
 *	61:	loc_mtx(MTX1)
 *	//		中：TASK1，MTX1：TASK2→TASK3，MTX3：TASK1→TASK2
 *	== TASK1（続き）==
 *	62:	act_tsk(TASK5)
 *	//		高：TASK5，中：TASK1，MTX1：TASK2→TASK3，MTX3：TASK1→TASK2
 *	== TASK5-8（優先度：高）==
 *	63:	ter_tsk(TASK2)									... (C-2)
 *	//		高：TASK5，中：TASK3，低：TASK1，MTX1：TASK3，MTX3：TASK1
 *		ref_mtx(MTX1, &rmtx)
 *		assert(rmtx.htskid == TASK3)
 *		assert(rmtx.wtskid == TSK_NONE)
 *		ext_tsk()
 *	//		中：TASK3，低：TASK1，MTX1：TASK3，MTX3：TASK1
 *	== TASK3-5（続き）==
 *	64:	ext_tsk()
 *	//		低：TASK1，MTX3：TASK1
 *
 *	== TASK1（続き）==
 *	65:	act_tsk(TASK2)
 *	//		中：TASK2，低：TASK1，MTX3：TASK1
 *	== TASK2-13（優先度：中）==
 *	66:	loc_mtx(MTX1)
 *	//		中：TASK2，低：TASK1，MTX1：TASK2，MTX3：TASK1
 *		loc_mtx(MTX3)
 *	//		中：TASK1，MTX1：TASK2，MTX3：TASK1→TASK2
 *	== TASK1（続き）==
 *	67:	act_tsk(TASK5)
 *	//		高：TASK5，中：TASK1，MTX1：TASK2，MTX3：TASK1→TASK2
 *	== TASK5-9（優先度：高）==
 *	68:	loc_mtx(MTX1)
 *	//		中：TASK1，MTX1：TASK2→TASK5，MTX3：TASK1→TASK2
 *	== TASK1（続き）==
 *	69:	ter_tsk(TASK2)									... (C-3)
 *	//		高：TASK5，低：TASK1，MTX1：TASK5，MTX3：TASK1
 *	== TASK5-9（続き）
 *	70:	ref_mtx(MTX1, &rmtx)
 *		assert(rmtx.htskid == TASK5)
 *		assert(rmtx.wtskid == TSK_NONE)
 *		ext_tsk()
 *	//		低：TASK1，MTX3：TASK1
 *
 *	== TASK1（続き）==
 *	71:	act_tsk(TASK2)
 *	//		中：TASK2，低：TASK1，MTX3：TASK1
 *	== TASK2-14（優先度：中）==
 *	72:	loc_mtx(MTX1)
 *	//		中：TASK2，低：TASK1，MTX1：TASK2，MTX3：TASK1
 *		loc_mtx(MTX2)
 *	//		中：TASK2，低：TASK1，MTX1：TASK2，MTX2：TASK2，MTX3：TASK1
 *		loc_mtx(MTX3)
 *	//		中：TASK1，MTX1：TASK2，MTX2：TASK2，MTX3：TASK1→TASK2
 *	== TASK1（続き）==
 *	73:	act_tsk(TASK5)
 *	//		高：TASK5，中：TASK1，MTX1：TASK2，MTX2：TASK2，MTX3：TASK1→TASK2
 *	== TASK5-10（優先度：高）==
 *	74:	ter_tsk(TASK2)									... (C-4)
 *	//		高：TASK5，低：TASK1，MTX3：TASK1
 *		ref_mtx(MTX1, &rmtx)
 *		assert(rmtx.htskid == TSK_NONE)
 *		assert(rmtx.wtskid == TSK_NONE)
 *		ref_mtx(MTX2, &rmtx)
 *		assert(rmtx.htskid == TSK_NONE)
 *		assert(rmtx.wtskid == TSK_NONE)
 *		ext_tsk()
 *	//		低：TASK1，MTX3：TASK1
 *
 *	== TASK1（続き）==
 *	75:	act_tsk(TASK2)
 *	//		中：TASK2，低：TASK1，MTX3：TASK1
 *	== TASK2-15（優先度：中）==
 *	76:	act_tsk(TASK3)
 *	//		中：TASK2→TASK3，低：TASK1，MTX3：TASK1
 *		act_tsk(TASK4)
 *	//		中：TASK2→TASK3→TASK4，低：TASK1，MTX3：TASK1
 *		loc_mtx(MTX1)
 *	//		中：TASK2→TASK3→TASK4，低：TASK1，MTX1：TASK2，MTX3：TASK1
 *		loc_mtx(MTX2)
 *	//		中：TASK2→TASK3→TASK4，低：TASK1，MTX1：TASK2，MTX2：TASK2，MTX3：TASK1
 *		loc_mtx(MTX3)
 *	//		中：TASK1→TASK3→TASK4，MTX1：TASK2，MTX2：TASK2，MTX3：TASK1→TASK2
 *	== TASK1（続き）==
 *	77:	rot_rdq(MID_PRIORITY)
 *	//		中：TASK3→TASK4→TASK1，MTX1：TASK2，MTX2：TASK2，MTX3：TASK1→TASK2
 *	== TASK3-6（優先度：中）==
 *	78:	loc_mtx(MTX1)
 *	//		中：TASK4→TASK1，MTX1：TASK2→TASK3，MTX2：TASK2，MTX3：TASK1→TASK2
 *	== TASK4-3（優先度：中）==
 *	79:	loc_mtx(MTX2)
 *	//		中：TASK1，MTX1：TASK2→TASK3，MTX2：TASK2→TASK4，MTX3：TASK1→TASK2
 *	== TASK1（続き）==
 *	80:	act_tsk(TASK5)
 *	//		高：TASK5，中：TASK1，MTX1：TASK2→TASK3，MTX2：TASK2→TASK4，MTX3：TASK1→TASK2
 *	== TASK5-11（優先度：高）==
 *	81:	ter_tsk(TASK2)									... (C-5)
 *	//		高：TASK5，中：TASK4→TASK3，低：TASK1，MTX1：TASK3，MTX2：TASK4，MTX3：TASK1
 *		ref_mtx(MTX1, &rmtx)
 *		assert(rmtx.htskid == TASK3)
 *		assert(rmtx.wtskid == TSK_NONE)
 *		ref_mtx(MTX2, &rmtx)
 *		assert(rmtx.htskid == TASK4)
 *		assert(rmtx.wtskid == TSK_NONE)
 *		ext_tsk()
 *	//		中：TASK4→TASK3，低：TASK1，MTX1：TASK3，MTX2：TASK4，MTX3：TASK1
 *	== TASK4-3（続き）==
 *	82:	ext_tsk()
 *	//		中：TASK3，低：TASK1，MTX1：TASK3，MTX3：TASK1
 *	== TASK3-6（続き）==
 *	83:	ext_tsk()
 *	//		低：TASK1，MTX3：TASK1
 *
 *	== TASK1（続き）==
 *	84:	act_tsk(TASK2)
 *	//		中：TASK2，低：TASK1，MTX3：TASK1
 *	== TASK2-16（優先度：中）==
 *	85:	loc_mtx(MTX1)
 *	//		中：TASK2，低：TASK1，MTX1：TASK2，MTX3：TASK1
 *		loc_mtx(MTX2)
 *	//		中：TASK2，低：TASK1，MTX1：TASK2，MTX2：TASK2，MTX3：TASK1
 *		loc_mtx(MTX3)
 *	//		中：TASK1，MTX1：TASK2，MTX2：TASK2，MTX3：TASK1→TASK2
 *	== TASK1（続き）==
 *	86:	act_tsk(TASK5)
 *	//		高：TASK5，中：TASK1，MTX1：TASK2，MTX2：TASK2，MTX3：TASK1→TASK2
 *	== TASK5-12（優先度：高）==
 *	87:	loc_mtx(MTX1)
 *	//		高：TASK1，MTX1：TASK2→TASK5，MTX2：TASK2，MTX3：TASK1→TASK2
 *	== TASK1（続き）==
 *	88:	act_tsk(TASK6)
 *	//		高：TASK1→TASK6，MTX1：TASK2→TASK5，MTX2：TASK2，MTX3：TASK1→TASK2
 *		rot_rdq(HIGH_PRIORITY)
 *	//		高：TASK6→TASK1，MTX1：TASK2→TASK5，MTX2：TASK2，MTX3：TASK1→TASK2
 *	== TASK6-2（優先度：高）==
 *	89:	loc_mtx(MTX2)
 *	//		高：TASK1，MTX1：TASK2→TASK5，MTX2：TASK2→TASK6，MTX3：TASK1→TASK2
 *	== TASK1（続き）==
 *	90:	ter_tsk(TASK2)									... (C-6)
 *	//		高：TASK6→TASK5，低：TASK1，MTX1：TASK5，MTX2：TASK6，MTX3：TASK1
 *	== TASK6-2（続き）
 *	91:	ref_mtx(MTX1, &rmtx)
 *		assert(rmtx.htskid == TASK5)
 *		assert(rmtx.wtskid == TSK_NONE)
 *		ref_mtx(MTX2, &rmtx)
 *		assert(rmtx.htskid == TASK6)
 *		assert(rmtx.wtskid == TSK_NONE)
 *		ext_tsk()
 *	//		高：TASK5，低：TASK1，MTX1：TASK5，MTX3：TASK1
 *	== TASK5-12（続き）
 *	92:	ext_tsk()
 *	//		低：TASK1，MTX3：TASK1
 *	== TASK1（続き）==
 *	93:	END
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

	test_start(__FILE__);

	check_point(1);
	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	check_point(3);
	ercd = ref_mtx(MTX1, &rmtx);
	check_ercd(ercd, E_OK);

	check_assert(rmtx.htskid == TSK_NONE);

	check_assert(rmtx.wtskid == TSK_NONE);

	check_point(4);
	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	check_point(9);
	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	check_point(11);
	ercd = ref_mtx(MTX1, &rmtx);
	check_ercd(ercd, E_OK);

	check_assert(rmtx.htskid == TSK_NONE);

	check_assert(rmtx.wtskid == TSK_NONE);

	ercd = ref_mtx(MTX2, &rmtx);
	check_ercd(ercd, E_OK);

	check_assert(rmtx.htskid == TSK_NONE);

	check_assert(rmtx.wtskid == TSK_NONE);

	check_point(12);
	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	check_point(19);
	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	check_point(22);
	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	check_point(28);
	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	check_point(30);
	ercd = act_tsk(TASK5);
	check_ercd(ercd, E_OK);

	check_point(32);
	ercd = ter_tsk(TASK2);
	check_ercd(ercd, E_OK);

	check_point(34);
	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	check_point(37);
	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	check_point(45);
	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	check_point(47);
	ercd = act_tsk(TASK5);
	check_ercd(ercd, E_OK);

	check_point(49);
	ercd = act_tsk(TASK6);
	check_ercd(ercd, E_OK);

	check_point(51);
	ercd = ter_tsk(TASK2);
	check_ercd(ercd, E_OK);

	check_point(54);
	ercd = loc_mtx(MTX3);
	check_ercd(ercd, E_OK);

	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	check_point(56);
	ercd = act_tsk(TASK5);
	check_ercd(ercd, E_OK);

	check_point(58);
	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	check_point(60);
	ercd = rot_rdq(MID_PRIORITY);
	check_ercd(ercd, E_OK);

	check_point(62);
	ercd = act_tsk(TASK5);
	check_ercd(ercd, E_OK);

	check_point(65);
	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	check_point(67);
	ercd = act_tsk(TASK5);
	check_ercd(ercd, E_OK);

	check_point(69);
	ercd = ter_tsk(TASK2);
	check_ercd(ercd, E_OK);

	check_point(71);
	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	check_point(73);
	ercd = act_tsk(TASK5);
	check_ercd(ercd, E_OK);

	check_point(75);
	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	check_point(77);
	ercd = rot_rdq(MID_PRIORITY);
	check_ercd(ercd, E_OK);

	check_point(80);
	ercd = act_tsk(TASK5);
	check_ercd(ercd, E_OK);

	check_point(84);
	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	check_point(86);
	ercd = act_tsk(TASK5);
	check_ercd(ercd, E_OK);

	check_point(88);
	ercd = act_tsk(TASK6);
	check_ercd(ercd, E_OK);

	ercd = rot_rdq(HIGH_PRIORITY);
	check_ercd(ercd, E_OK);

	check_point(90);
	ercd = ter_tsk(TASK2);
	check_ercd(ercd, E_OK);

	check_finish(93);
	check_assert(false);
}

static uint_t	task2_count = 0;

void
task2(EXINF exinf)
{
	ER_UINT	ercd;

	switch (++task2_count) {
	case 1:
		check_point(2);
		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_OK);

		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 2:
		check_point(5);
		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_OK);

		ercd = act_tsk(TASK3);
		check_ercd(ercd, E_OK);

		ercd = rot_rdq(MID_PRIORITY);
		check_ercd(ercd, E_OK);

		check_point(7);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 3:
		check_point(10);
		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_OK);

		ercd = loc_mtx(MTX2);
		check_ercd(ercd, E_OK);

		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 4:
		check_point(13);
		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_OK);

		ercd = loc_mtx(MTX2);
		check_ercd(ercd, E_OK);

		ercd = act_tsk(TASK3);
		check_ercd(ercd, E_OK);

		ercd = act_tsk(TASK4);
		check_ercd(ercd, E_OK);

		ercd = rot_rdq(MID_PRIORITY);
		check_ercd(ercd, E_OK);

		check_point(16);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 5:
		check_point(20);
		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_OK);

		ercd = act_tsk(TASK5);
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 6:
		check_point(23);
		ercd = act_tsk(TASK3);
		check_ercd(ercd, E_OK);

		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_OK);

		ercd = rot_rdq(MID_PRIORITY);
		check_ercd(ercd, E_OK);

		check_point(25);
		ercd = act_tsk(TASK5);
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 7:
		check_point(29);
		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_OK);

		ercd = slp_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 8:
		check_point(35);
		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_OK);

		ercd = loc_mtx(MTX2);
		check_ercd(ercd, E_OK);

		ercd = act_tsk(TASK5);
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 9:
		check_point(38);
		ercd = act_tsk(TASK3);
		check_ercd(ercd, E_OK);

		ercd = act_tsk(TASK4);
		check_ercd(ercd, E_OK);

		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_OK);

		ercd = loc_mtx(MTX2);
		check_ercd(ercd, E_OK);

		ercd = rot_rdq(MID_PRIORITY);
		check_ercd(ercd, E_OK);

		check_point(41);
		ercd = act_tsk(TASK5);
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 10:
		check_point(46);
		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_OK);

		ercd = loc_mtx(MTX2);
		check_ercd(ercd, E_OK);

		ercd = slp_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 11:
		check_point(55);
		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_OK);

		ercd = loc_mtx(MTX3);
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 12:
		check_point(59);
		ercd = act_tsk(TASK3);
		check_ercd(ercd, E_OK);

		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_OK);

		ercd = loc_mtx(MTX3);
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 13:
		check_point(66);
		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_OK);

		ercd = loc_mtx(MTX3);
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 14:
		check_point(72);
		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_OK);

		ercd = loc_mtx(MTX2);
		check_ercd(ercd, E_OK);

		ercd = loc_mtx(MTX3);
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 15:
		check_point(76);
		ercd = act_tsk(TASK3);
		check_ercd(ercd, E_OK);

		ercd = act_tsk(TASK4);
		check_ercd(ercd, E_OK);

		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_OK);

		ercd = loc_mtx(MTX2);
		check_ercd(ercd, E_OK);

		ercd = loc_mtx(MTX3);
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 16:
		check_point(85);
		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_OK);

		ercd = loc_mtx(MTX2);
		check_ercd(ercd, E_OK);

		ercd = loc_mtx(MTX3);
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
	T_RMTX	rmtx;

	switch (++task3_count) {
	case 1:
		check_point(6);
		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_OK);

		check_point(8);
		ercd = ref_mtx(MTX1, &rmtx);
		check_ercd(ercd, E_OK);

		check_assert(rmtx.htskid == TASK3);

		check_assert(rmtx.wtskid == TSK_NONE);

		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 2:
		check_point(14);
		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_OK);

		check_point(18);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 3:
		check_point(24);
		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_OK);

		check_point(27);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 4:
		check_point(39);
		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_OK);

		check_point(44);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 5:
		check_point(61);
		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_OK);

		check_point(64);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 6:
		check_point(78);
		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_OK);

		check_point(83);
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
	T_RMTX	rmtx;

	switch (++task4_count) {
	case 1:
		check_point(15);
		ercd = loc_mtx(MTX2);
		check_ercd(ercd, E_OK);

		check_point(17);
		ercd = ref_mtx(MTX1, &rmtx);
		check_ercd(ercd, E_OK);

		check_assert(rmtx.htskid == TASK3);

		check_assert(rmtx.wtskid == TSK_NONE);

		ercd = ref_mtx(MTX2, &rmtx);
		check_ercd(ercd, E_OK);

		check_assert(rmtx.htskid == TASK4);

		check_assert(rmtx.wtskid == TSK_NONE);

		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 2:
		check_point(40);
		ercd = loc_mtx(MTX2);
		check_ercd(ercd, E_OK);

		check_point(43);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 3:
		check_point(79);
		ercd = loc_mtx(MTX2);
		check_ercd(ercd, E_OK);

		check_point(82);
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
	T_RMTX	rmtx;

	switch (++task5_count) {
	case 1:
		check_point(21);
		ercd = ter_tsk(TASK2);
		check_ercd(ercd, E_OK);

		ercd = ref_mtx(MTX1, &rmtx);
		check_ercd(ercd, E_OK);

		check_assert(rmtx.htskid == TSK_NONE);

		check_assert(rmtx.wtskid == TSK_NONE);

		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 2:
		check_point(26);
		ercd = ter_tsk(TASK2);
		check_ercd(ercd, E_OK);

		ercd = ref_mtx(MTX1, &rmtx);
		check_ercd(ercd, E_OK);

		check_assert(rmtx.htskid == TASK3);

		check_assert(rmtx.wtskid == TSK_NONE);

		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 3:
		check_point(31);
		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_OK);

		check_point(33);
		ercd = ref_mtx(MTX1, &rmtx);
		check_ercd(ercd, E_OK);

		check_assert(rmtx.htskid == TASK5);

		check_assert(rmtx.wtskid == TSK_NONE);

		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 4:
		check_point(36);
		ercd = ter_tsk(TASK2);
		check_ercd(ercd, E_OK);

		ercd = ref_mtx(MTX1, &rmtx);
		check_ercd(ercd, E_OK);

		check_assert(rmtx.htskid == TSK_NONE);

		check_assert(rmtx.wtskid == TSK_NONE);

		ercd = ref_mtx(MTX2, &rmtx);
		check_ercd(ercd, E_OK);

		check_assert(rmtx.htskid == TSK_NONE);

		check_assert(rmtx.wtskid == TSK_NONE);

		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 5:
		check_point(42);
		ercd = ter_tsk(TASK2);
		check_ercd(ercd, E_OK);

		ercd = ref_mtx(MTX1, &rmtx);
		check_ercd(ercd, E_OK);

		check_assert(rmtx.htskid == TASK3);

		check_assert(rmtx.wtskid == TSK_NONE);

		ercd = ref_mtx(MTX2, &rmtx);
		check_ercd(ercd, E_OK);

		check_assert(rmtx.htskid == TASK4);

		check_assert(rmtx.wtskid == TSK_NONE);

		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 6:
		check_point(48);
		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_OK);

		check_point(53);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 7:
		check_point(57);
		ercd = ter_tsk(TASK2);
		check_ercd(ercd, E_OK);

		ercd = ref_mtx(MTX1, &rmtx);
		check_ercd(ercd, E_OK);

		check_assert(rmtx.htskid == TSK_NONE);

		check_assert(rmtx.wtskid == TSK_NONE);

		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 8:
		check_point(63);
		ercd = ter_tsk(TASK2);
		check_ercd(ercd, E_OK);

		ercd = ref_mtx(MTX1, &rmtx);
		check_ercd(ercd, E_OK);

		check_assert(rmtx.htskid == TASK3);

		check_assert(rmtx.wtskid == TSK_NONE);

		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 9:
		check_point(68);
		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_OK);

		check_point(70);
		ercd = ref_mtx(MTX1, &rmtx);
		check_ercd(ercd, E_OK);

		check_assert(rmtx.htskid == TASK5);

		check_assert(rmtx.wtskid == TSK_NONE);

		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 10:
		check_point(74);
		ercd = ter_tsk(TASK2);
		check_ercd(ercd, E_OK);

		ercd = ref_mtx(MTX1, &rmtx);
		check_ercd(ercd, E_OK);

		check_assert(rmtx.htskid == TSK_NONE);

		check_assert(rmtx.wtskid == TSK_NONE);

		ercd = ref_mtx(MTX2, &rmtx);
		check_ercd(ercd, E_OK);

		check_assert(rmtx.htskid == TSK_NONE);

		check_assert(rmtx.wtskid == TSK_NONE);

		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 11:
		check_point(81);
		ercd = ter_tsk(TASK2);
		check_ercd(ercd, E_OK);

		ercd = ref_mtx(MTX1, &rmtx);
		check_ercd(ercd, E_OK);

		check_assert(rmtx.htskid == TASK3);

		check_assert(rmtx.wtskid == TSK_NONE);

		ercd = ref_mtx(MTX2, &rmtx);
		check_ercd(ercd, E_OK);

		check_assert(rmtx.htskid == TASK4);

		check_assert(rmtx.wtskid == TSK_NONE);

		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 12:
		check_point(87);
		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_OK);

		check_point(92);
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

	switch (++task6_count) {
	case 1:
		check_point(50);
		ercd = loc_mtx(MTX2);
		check_ercd(ercd, E_OK);

		check_point(52);
		ercd = ref_mtx(MTX1, &rmtx);
		check_ercd(ercd, E_OK);

		check_assert(rmtx.htskid == TASK5);

		check_assert(rmtx.wtskid == TSK_NONE);

		ercd = ref_mtx(MTX2, &rmtx);
		check_ercd(ercd, E_OK);

		check_assert(rmtx.htskid == TASK6);

		check_assert(rmtx.wtskid == TSK_NONE);

		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 2:
		check_point(89);
		ercd = loc_mtx(MTX2);
		check_ercd(ercd, E_OK);

		check_point(91);
		ercd = ref_mtx(MTX1, &rmtx);
		check_ercd(ercd, E_OK);

		check_assert(rmtx.htskid == TASK5);

		check_assert(rmtx.wtskid == TSK_NONE);

		ercd = ref_mtx(MTX2, &rmtx);
		check_ercd(ercd, E_OK);

		check_assert(rmtx.htskid == TASK6);

		check_assert(rmtx.wtskid == TSK_NONE);

		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	default:
		check_assert(false);
	}
	check_assert(false);
}
