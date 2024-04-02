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
 *  $Id: test_inherit3.c 1615 2022-09-18 12:12:57Z ertl-hiro $
 */

/* 
 *		優先度継承ミューテックスのテスト(3)
 *
 * 【テストの目的】
 *
 *  優先度継承ミューテックスに対するloc_mtxとunl_mtxによる優先度制御に
 *  ついてテストする．
 *
 * 【テスト項目】
 *
 *	(A) タスクA（実行可能状態）がロックしている優先度継承ミューテック
 *		スXに対してloc_mtxを発行し，自タスクがミューテックスXに対する
 *		ロック待ち状態になる．これに伴うタスクAの優先度制御
 *		(A-1) 自タスクの現在優先度が，タスクAの現在優先度よりも高い場
 *			  合に，タスクAの現在優先度が自タスクの現在優先度まで上が
 *			  ること．また，タスクAが同じ優先度内で最高優先順位になる
 *			  こと
 *		(A-2) 自タスクの現在優先度が，タスクAの現在優先度と同じ場合に，
 *			  タスクAの現在優先度が変わらないこと．また，タスクAの同じ
 *			  優先度内での優先順位が変わらないこと
 *		(A-3) 自タスクの現在優先度が，タスクAの現在優先度よりも低い状
 *			  況は，ディスパッチ保留状態でしか作ることができず，この状
 *			  態では自タスクがloc_mtxを発行することができないため，こ
 *			  の状況は考えなくても良い
 *	(B) タスクA（優先度継承ミューテックスYに対するロック待ち状態）がロッ
 *		クしている優先度継承ミューテックスXに対してloc_mtxを発行し，自
 *		タスクがミューテックスXに対するロック待ち状態になる．これに伴
 *		うタスクAの優先度制御
 *		(B-1) 自タスクの現在優先度が，タスクAの現在優先度よりも高い場
 *			  合に，タスクAの現在優先度が自タスクの現在優先度まで上が
 *			  ること．また，タスクAの待ち行列中で同じ優先度内での順序
 *			  が最後になること
 *		(B-2) 自タスクの現在優先度が，タスクAの現在優先度よりも高い場
 *			  合に，タスクAの現在優先度が自タスクの現在優先度まで上が
 *			  ること．さらに，ミューテックスYをロックしているタスクに
 *			  対して，推移的な優先度継承が起こること
 *				 ※ 推移的な優先度継承が起こる状況では，待ち行列中での
 *					同じ優先度内での順序が最後になることは，確認できな
 *					い
 *		(B-3) 自タスクの現在優先度が，タスクAの現在優先度と同じ場合に，
 *			  タスクAの現在優先度が変わらないこと．また，待ち行列中で
 *			  の同じ優先度内での順序が変わらないこと
 *		(B-4) 自タスクの現在優先度が，タスクAの現在優先度よりも低い場
 *			  合に，タスクAの現在優先度が変わらないこと．また，待ち行
 *			  列中での同じ優先度内での順序が変わらないこと
 *	(C) 自タスクがロックしている優先度継承ミューテックスXに対して
 * 		unl_mtxを発行したことに伴う，自タスクの優先度制御
 *		(C-1) ミューテックスXに，自タスクと同じ現在優先度のタスクAが待
 *			  ち状態となっており，ミューテックスのロック解除で現在優先
 *			  度が下がるべき場合に，自タスクの現在優先度が適切に下げら
 *			  れること．また，タスクAが同じ優先度内での優先順位が最高
 *			  になること
 *		(C-2) ミューテックスXに，自タスクと同じ現在優先度のタスクAが待
 *			  ち状態となっており，ミューテックスのロック解除で現在優先
 *			  度が変わるべきでない場合に，自タスクの現在優先度が変わら
 *			  ないこと．また，同じ優先度内での優先順位が変わらないこと
 *		(C-3) ミューテックスXに，自タスクより低い現在優先度のタスクし
 *			  か待ち状態となっていなかった場合に，自タスクの現在優先度
 *			  が変わらないこと．また，同じ優先度内での優先順位が変わら
 *			  ないこと
 *
 * 【テスト項目の実現方法】
 *
 *	(A-1)
 *		中優先度タスク（TASK1）が，優先度継承ミューテックス（MTX1）を
 *		ロックした状態で，高優先度タスク（TASK6）にloc_mtx(MTX1)を発行
 *		させ，TASK1が高優先度になることを確認する．また，他の高優先度
 *		のタスク（TASK7）を実行できる状態にしておき，それよりも優先順
 *		位が高くなることを確認する．
 *	(A-2)
 *		中優先度タスク（TASK1）が，優先度継承ミューテックス（MTX1）を
 *		ロックした状態で，中優先度タスク（TASK2）にloc_mtx(MTX1)を発行
 *		させ，TASK1の現在優先度が変わらないことを確認する．また，実行
 *		可能状態の中優先度タスク2つ（TASK3，TASK4）を用意しておき，
 *		TASK1の優先順位が変わらないことを確認する．
 *	(B-1)
 *		中優先度タスク（TASK2）に優先度継承ミューテックス（MTX2）をロッ
 *		クさせる．高優先度タスク（TASK7）をMTX2に対する待ち状態にする．
 *		中優先度タスク（TASK1）に優先度継承ミューテックス（MTX1）をロッ
 *		クさせ，さらにMTX2に対する待ち状態にする．高優先度タスク
 *		（TASK6）にloc_mtx(MTX1)を発行させ，TASK1が高優先度になること
 *		を確認する．また，待ち行列中での順序がTASK7よりも後であること
 *		を確認する．
 *	(B-2)
 *		中優先度タスク（TASK2）に優先度継承ミューテックス（MTX2）をロッ
 *		クさせる．中優先度タスク（TASK1）に優先度継承ミューテックス
 *		（MTX1）をロックさせ，さらにMTX2に対する待ち状態にする．高優先
 *		度タスク（TASK6）にloc_mtx(MTX1)を発行させ，TASK1が高優先度に
 *		なることを確認する．さらに，TASK2が高優先度になることを確認す
 *		る．
 *	(B-3)
 *		中優先度タスク（TASK2）に優先度継承ミューテックス（MTX2）をロッ
 *		クさせる．中優先度タスク（TASK1）に優先度継承ミューテックス
 *		（MTX1）をロックさせ，さらにMTX2に対する待ち状態にする．中優先
 *		度タスク（TASK5）にloc_mtx(MTX1)を発行させ，TASK1の現在優先度
 *		が変わらないことを確認する．また，MTX2に対する待ち状態の中優先
 *		度タスク2つ（TASK3，TASK4）を用意しておき，待ち行列中での同じ
 *		優先度内での順序が変わらないことを確認する．
 *	(B-4)
 *		中優先度タスク（TASK2）に優先度継承ミューテックス（MTX2）をロッ
 *		クさせる．中優先度タスク（TASK1）に優先度継承ミューテックス
 *		（MTX1）をロックさせ，さらにMTX2に対する待ち状態にする．低優先
 *		度タスク（TASK8）にloc_mtx(MTX1)を発行させ，TASK1の現在優先度
 *		が変わらないことを確認する．また，MTX2に対する待ち状態の中優先
 *		度タスク2つ（TASK3，TASK4）を用意しておき，待ち行列中での同じ
 *		優先度内での順序が変わらないことを確認する．
 *	(C-1)
 *		中優先度タスク（TASK1）に優先度継承ミューテックス（MTX1）をロッ
 *		クさせる．高優先度タスク（TASK6）をMTX1に対する待ち状態にする．
 *		TASK1にunl_mtx(MTX1)を発行させ，TASK1が中優先度に下がることを
 *		確認する．また，他の中優先度のタスク（TASK2）を実行できる状態
 *		にしておき，それよりも優先順位が高くなることを確認する．
 *	(C-2)
 *		中優先度タスク（TASK1）に優先度継承ミューテックス（MTX1）をロッ
 *		クさせる．中優先度タスク（TASK2）をMTX1に対する待ち状態にする．
 *		TASK1にunl_mtx(MTX1)を発行させ，TASK1の優先度が変わらないこと
 *		を確認する．また，他の中優先度のタスクを2つ（TASK3，TASK4）実
 *		行できる状態にしておき，TASK1の優先順位が変わらないことを確認
 *		する．ディスパッチ禁止状態で実施する．
 *	(C-3)
 *		中優先度タスク（TASK1）に優先度継承ミューテックス（MTX1）をロッ
 *		クさせる．低優先度タスク（TASK8）をMTX1に対する待ち状態にする．
 *		TASK1にunl_mtx(MTX1)を発行させ，TASK1の優先度が変わらないこと
 *		を確認する．また，他の中優先度のタスクを2つ（TASK3，TASK4）実
 *		行できる状態にしておき，TASK1の優先順位が変わらないことを確認
 *		する．ディスパッチ禁止状態で実施する．
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
 *		act_tsk(TASK2)
 *	//		中：TASK1→TASK2，MTX1：TASK1
 *		act_tsk(TASK6)
 *	//		高：TASK6，中：TASK1→TASK2，MTX1：TASK1
 *	== TASK6-1（優先度：高）==
 *	2:	act_tsk(TASK7)
 *	//		高：TASK6→TASK7，中：TASK1→TASK2，MTX1：TASK1
 *		loc_mtx(MTX1)									... (A-1)
 *	//		高：TASK1→TASK7，中：TASK2，MTX1：TASK1→TASK6
 *	== TASK1（続き）==
 *	3:	ref_tsk(TASK1, &rtsk)
 *		assert(rtsk.tskpri == HIGH_PRIORITY)
 *		unl_mtx(MTX1)									... (C-1)
 *	//		高：TASK7→TASK6，中：TASK1→TASK2，MTX1：TASK6
 *	== TASK7-1（優先度：高）==
 *	4:	ref_tsk(TASK1, &rtsk)
 *		assert(rtsk.tskpri == MID_PRIORITY)
 *		ext_tsk()
 *	//		高：TASK6，中：TASK1→TASK2，MTX1：TASK6
 *	== TASK6-1（続き）==
 *	5:	unl_mtx(MTX1)
 *	//		高：TASK6，中：TASK1→TASK2
 *		ext_tsk()
 *	//		中：TASK1→TASK2
 *
 *	== TASK1（続き）==
 *	6:	loc_mtx(MTX1)
 *	//		中：TASK1→TASK2，MTX1：TASK1
 *		act_tsk(TASK3)
 *	//		中：TASK1→TASK2→TASK3，MTX1：TASK1
 *		rot_rdq(MID_PRIORITY)
 *	//		中：TASK2→TASK3→TASK1，MTX1：TASK1
 *	== TASK2-1（優先度：中）==
 *	7:	act_tsk(TASK4)
 *	//		中：TASK2→TASK3→TASK1→TASK4，MTX1：TASK1
 *		loc_mtx(MTX1)									... (A-2)
 *	//		中：TASK3→TASK1→TASK4，MTX1：TASK1→TASK2
 *	== TASK3-1（優先度：中）==
 *	8:	ext_tsk()
 *	//		中：TASK1→TASK4，MTX1：TASK1→TASK2
 *	== TASK1（続き）==
 *	9:	unl_mtx(MTX1)
 *	//		中：TASK1→TASK4→TASK2，MTX1：TASK2
 *		rot_rdq(MID_PRIORITY)
 *	//		中：TASK4→TASK2→TASK1，MTX1：TASK2
 *	== TASK4-1（優先度：中）==
 *	10:	ext_tsk()
 *	//		中：TASK2→TASK1，MTX1：TASK2
 *	== TASK2-1（続き）==
 *	11:	unl_mtx(MTX1)
 *	//		中：TASK2→TASK1
 *
 *	12:	loc_mtx(MTX2)
 *	//		中：TASK2→TASK1，MTX2：TASK2
 *		act_tsk(TASK3)
 *	//		中：TASK2→TASK1→TASK3，MTX2：TASK2
 *		act_tsk(TASK7)
 *	//		高：TASK7，中：TASK2→TASK1→TASK3，MTX2：TASK2
 *	== TASK7-2（優先度：高）==
 *	13:	loc_mtx(MTX2)
 *	//		高：TASK2，中：TASK1→TASK3，MTX2：TASK2→TASK7
 *	== TASK2-1（続き）==
 *	14:	slp_tsk()
 *	//		中：TASK1→TASK3，MTX2：TASK2→TASK7
 *	== TASK1（続き）==
 *	15:	loc_mtx(MTX1)
 *	//		中：TASK1→TASK3，MTX1：TASK1，MTX2：TASK2→TASK7
 *		loc_mtx(MTX2)
 *	//		中：TASK3，MTX1：TASK1，MTX2：TASK2→TASK7→TASK1
 *	== TASK3-2（優先度：中）==
 *	16:	act_tsk(TASK6)
 *	//		高：TASK6，中：TASK3，MTX1：TASK1，MTX2：TASK2→TASK7→TASK1
 *	== TASK6-2（優先度：高）==
 *	17:	loc_mtx(MTX1)									... (B-1)
 *	//		中：TASK3，MTX1：TASK1→TASK6，MTX2：TASK2→TASK7→TASK1
 *	== TASK3-2（続き）==
 *	18:	ref_tsk(TASK1, &rtsk)
 *		assert(rtsk.tskpri == HIGH_PRIORITY)
 *		wup_tsk(TASK2)
 *	//		高：TASK2，中：TASK3，MTX1：TASK1→TASK6，MTX2：TASK2→TASK7→TASK1
 *	== TASK2-1（続き）==
 *	19:	unl_mtx(MTX2)
 *	//		高：TASK7，中：TASK2→TASK3，MTX1：TASK1→TASK6，MTX2：TASK7→TASK1
 *	== TASK7-2（続き）==
 *	20:	unl_mtx(MTX2)
 *	//		高：TASK7→TASK1，中：TASK2→TASK3，MTX1：TASK1→TASK6，MTX2：TASK1
 *		ext_tsk()
 *	//		高：TASK1，中：TASK2→TASK3，MTX1：TASK1→TASK6，MTX2：TASK1
 *	== TASK1（続き）==
 *	21:	unl_mtx(MTX2)
 *	//		高：TASK6，中：TASK1→TASK2→TASK3，MTX1：TASK1→TASK6
 *		unl_mtx(MTX1)
 *	//		高：TASK6，中：TASK1→TASK2→TASK3，MTX1：TASK6
 *	== TASK6-2（続き）==
 *	22:	unl_mtx(MTX1)
 *	//		高：TASK6，中：TASK1→TASK2→TASK3
 *		ext_tsk()
 *	//		中：TASK1→TASK2→TASK3
 *	== TASK1（続き）==
 *	23:	rot_rdq(MID_PRIORITY)
 *	//		中：TASK2→TASK3→TASK1
 *
 *	== TASK2-1（続き）==
 *	24:	loc_mtx(MTX2)
 *	//		中：TASK2→TASK3→TASK1，MTX2：TASK2
 *		slp_tsk()
 *	//		中：TASK3→TASK1，MTX2：TASK2
 *	== TASK3-2（続き）==
 *	25:	rot_rdq(MID_PRIORITY)
 *	//		中：TASK1→TASK3，MTX2：TASK2
 *	== TASK1（続き）==
 *	26:	loc_mtx(MTX1)
 *	//		中：TASK1→TASK3，MTX1：TASK1，MTX2：TASK2
 *		loc_mtx(MTX2)
 *	//		中：TASK3，MTX1：TASK1，MTX2：TASK2→TASK1
 *	== TASK3-2（続き）==
 *	27:	act_tsk(TASK6)
 *	//		高：TASK6，中：TASK3，MTX1：TASK1，MTX2：TASK2→TASK1
 *	== TASK6-3（優先度：高）==
 *	28:	loc_mtx(MTX1)									... (B-2)
 *	//		中：TASK3，MTX1：TASK1→TASK6，MTX2：TASK2→TASK1
 *	== TASK3-2（続き）==
 *	29:	ref_tsk(TASK1, &rtsk)
 *		assert(rtsk.tskpri == HIGH_PRIORITY)
 *		ref_tsk(TASK2, &rtsk)
 *		assert(rtsk.tskpri == HIGH_PRIORITY)
 *		wup_tsk(TASK2)
 *	//		高：TASK2，中：TASK3，MTX1：TASK1→TASK6，MTX2：TASK2→TASK1
 *	== TASK2-1（続き）==
 *	30:	unl_mtx(MTX2)
 *	//		高：TASK1，中：TASK2→TASK3，MTX1：TASK1→TASK6，MTX2：TASK1
 *	== TASK1（続き）==
 *	31:	unl_mtx(MTX2)
 *	//		高：TASK1，中：TASK2→TASK3，MTX1：TASK1→TASK6
 *		unl_mtx(MTX1)
 *	//		高：TASK6，中：TASK1→TASK2→TASK3，MTX1：TASK6
 *	== TASK6-3（続き）==
 *	32:	unl_mtx(MTX1)
 *	//		高：TASK6，中：TASK1→TASK2→TASK3
 *		ext_tsk()
 *	//		中：TASK1→TASK2→TASK3
 *	== TASK1（続き）==
 *	33:	rot_rdq(MID_PRIORITY)
 *	//		中：TASK2→TASK3→TASK1
 *
 *	== TASK2-1（続き）==
 *	34:	loc_mtx(MTX2)
 *	//		中：TASK2→TASK3→TASK1，MTX2：TASK2
 *		slp_tsk()
 *	//		中：TASK3→TASK1，MTX2：TASK2
 *	== TASK3-2（続き）==
 *	35:	loc_mtx(MTX2)
 *	//		中：TASK1，MTX2：TASK2→TASK3
 *	== TASK1（続き）==
 *	36:	loc_mtx(MTX1)
 *	//		中：TASK1，MTX1：TASK1，MTX2：TASK2→TASK3
 *		act_tsk(TASK5)
 *	//		中：TASK1→TASK5，MTX1：TASK1，MTX2：TASK2→TASK3
 *		loc_mtx(MTX2)
 *	//		中：TASK5，MTX1：TASK1，MTX2：TASK2→TASK3→TASK1
 *	== TASK5-1（優先度：中）==
 *	37:	act_tsk(TASK4)
 *	//		中：TASK5→TASK4，MTX1：TASK1，MTX2：TASK2→TASK3→TASK1
 *		rot_rdq(MID_PRIORITY)
 *	//		中：TASK4→TASK5，MTX1：TASK1，MTX2：TASK2→TASK3→TASK1
 *	== TASK4-2（優先度：中）==
 *	38:	loc_mtx(MTX2)
 *	//		中：TASK5，MTX1：TASK1，MTX2：TASK2→TASK3→TASK1→TASK4
 *	== TASK5-1（続き）==
 *	39:	wup_tsk(TASK2)
 *	//		中：TASK5→TASK2，MTX1：TASK1，MTX2：TASK2→TASK3→TASK1→TASK4
 *		loc_mtx(MTX1)									... (B-3)
 *	//		中：TASK2，MTX1：TASK1→TASK5，MTX2：TASK2→TASK3→TASK1→TASK4
 *	== TASK2-1（続き）==
 *	40:	ref_tsk(TASK1, &rtsk)
 *		assert(rtsk.tskpri == MID_PRIORITY)
 *		unl_mtx(MTX2)
 *	//		中：TASK2→TASK3，MTX1：TASK1→TASK5，MTX2：TASK3→TASK1→TASK4
 *		ext_tsk()
 *	//		中：TASK3，MTX1：TASK1→TASK5，MTX2：TASK3→TASK1→TASK4
 *	== TASK3-2（続き）==
 *	41:	unl_mtx(MTX2)
 *	//		中：TASK3→TASK1，MTX1：TASK1→TASK5，MTX2：TASK1→TASK4
 *		ext_tsk()
 *	//		中：TASK1，MTX1：TASK1→TASK5，MTX2：TASK1→TASK4
 *	== TASK1（続き）==
 *	42:	unl_mtx(MTX2)
 *	//		中：TASK1→TASK4，MTX1：TASK1→TASK5，MTX2：TASK4
 *		unl_mtx(MTX1)
 *	//		中：TASK1→TASK4→TASK5，MTX1：TASK5，MTX2：TASK4
 *		rot_rdq(MID_PRIORITY)
 *	//		中：TASK4→TASK5→TASK1，MTX1：TASK5，MTX2：TASK4
 *	== TASK4-2（続き）==
 *	43:	unl_mtx(MTX2)
 *	//		中：TASK4→TASK5→TASK1，MTX1：TASK5
 *		ext_tsk()
 *	//		中：TASK5→TASK1，MTX1：TASK5
 *	== TASK5-1（続き）==
 *	44:	loc_mtx(MTX2)
 *	//		中：TASK5→TASK1
 *		ext_tsk()
 *	//		中：TASK1
 *
 *	== TASK1（続き）==
 *	45:	act_tsk(TASK2)
 *	//		中：TASK1→TASK2
 *		act_tsk(TASK3)
 *	//		中：TASK1→TASK2→TASK3
 *		rot_rdq(MID_PRIORITY)
 *	//		中：TASK2→TASK1
 *	== TASK2-2（優先度：中）==
 *	46:	loc_mtx(MTX2)
 *	//		中：TASK2→TASK1→TASK3，MTX2：TASK2
 *		slp_tsk()
 *	//		中：TASK3→TASK1，MTX2：TASK2
 *	== TASK3-3（優先度：中）==
 *	47:	loc_mtx(MTX2)
 *	//		中：TASK1，MTX2：TASK2→TASK3
 *	== TASK1（続き）==
 *	48:	loc_mtx(MTX1)
 *	//		中：TASK1，MTX1：TASK1，MTX2：TASK2→TASK3
 *		act_tsk(TASK8)
 *	//		中：TASK1，低：TASK8，MTX1：TASK1，MTX2：TASK2→TASK3
 *	49:	loc_mtx(MTX2)
 *	//		低：TASK8，MTX1：TASK1，MTX2：TASK2→TASK3→TASK1
 *	== TASK8-1（優先度：低）==
 *	50:	act_tsk(TASK4)
 *	//		中：TASK4，低：TASK8，MTX1：TASK1，MTX2：TASK2→TASK3→TASK1
 *	== TASK4-3（優先度：中）==
 *	51:	loc_mtx(MTX2)
 *	//		低：TASK8，MTX1：TASK1，MTX2：TASK2→TASK3→TASK1→TASK4
 *	== TASK8-1（続き）==
 *	52:	act_tsk(TASK9)
 *	//		低：TASK8→TASK9，MTX1：TASK1，MTX2：TASK2→TASK3→TASK1→TASK4
 *	53:	loc_mtx(MTX1)									... (B-4)
 *	//		低：TASK9，MTX1：TASK1→TASK8，MTX2：TASK2→TASK3→TASK1→TASK4
 *	== TASK9-1（優先度：低）==
 *	54:	ref_tsk(TASK1, &rtsk)
 *		assert(rtsk.tskpri == MID_PRIORITY)
 *		wup_tsk(TASK2)
 *	//		中：TASK2，低：TASK9，MTX1：TASK1→TASK8，MTX2：TASK2→TASK3→TASK1→TASK4
 *	== TASK2-2（続き）==
 *	55:	ter_tsk(TASK9)
 *	//		中：TASK2→TASK3，MTX1：TASK1→TASK8，MTX2：TASK3→TASK1→TASK4
 *		unl_mtx(MTX2)
 *	//		中：TASK2→TASK3，MTX1：TASK1→TASK8，MTX2：TASK3→TASK1→TASK4
 *		ext_tsk()
 *	//		中：TASK3，MTX1：TASK1→TASK8，MTX2：TASK3→TASK1→TASK4
 *	== TASK3-3（続き）==
 *	56:	unl_mtx(MTX2)
 *	//		中：TASK3→TASK1，MTX1：TASK1→TASK8，MTX2：TASK1→TASK4
 *		ext_tsk()
 *	//		中：TASK1，MTX1：TASK1→TASK8，MTX2：TASK1→TASK4
 *	== TASK1（続き）==
 *	57:	unl_mtx(MTX2)
 *	//		中：TASK1→TASK4，MTX1：TASK1→TASK8，MTX2：TASK4
 *		unl_mtx(MTX1)
 *	//		中：TASK1→TASK4，低：TASK8，MTX1：TASK8，MTX2：TASK4
 *		rot_rdq(MID_PRIORITY)
 *	//		中：TASK4→TASK1，低：TASK8，MTX1：TASK8，MTX2：TASK4
 *	== TASK4-3（続き）==
 *	58:	unl_mtx(MTX2)
 *	//		中：TASK4→TASK1，低：TASK8，MTX1：TASK8
 *		ext_tsk()
 *	//		中：TASK1，低：TASK8，MTX1：TASK8
 *	== TASK1（続き）==
 *	59:	chg_pri(TASK1, LOW_PRIORITY)
 *	//		低：TASK8→TASK1，MTX1：TASK8
 *	== TASK8-1（続き）==
 *	60:	unl_mtx(MTX1)
 *	//		低：TASK8→TASK1
 *		ext_tsk()
 *	//		低：TASK1
 *	== TASK1（続き）==
 *	61:	chg_pri(TASK1, TPRI_INI)
 *	//		中：TASK1
 *
 *	62:	loc_mtx(MTX1)
 *	//		中：TASK1，MTX1：TASK1
 *		act_tsk(TASK2)
 *	//		中：TASK1→TASK2，MTX1：TASK1
 *		rot_rdq(MID_PRIORITY)
 *	//		中：TASK2→TASK1，MTX1：TASK1
 *	== TASK2-3（優先度：中）==
 *	63:	loc_mtx(MTX1)
 *	//		中：TASK1，MTX1：TASK1→TASK2
 *	== TASK1（続き）==
 *	64:	dis_dsp()
 *		act_tsk(TASK3)
 *	//		中：TASK1→TASK3，MTX1：TASK1→TASK2
 *		rot_rdq(MID_PRIORITY)
 *	//		中：TASK3→TASK1，MTX1：TASK1→TASK2
 *		act_tsk(TASK4)
 *	//		中：TASK3→TASK1→TASK4，MTX1：TASK1→TASK2
 *		unl_mtx(MTX1)									... (C-2)
 *	//		中：TASK3→TASK1→TASK4→TASK2，MTX1：TASK2
 *		ena_dsp()
 *	== TASK3-4（優先度：中）==
 *	65:	ext_tsk()
 *	//		中：TASK1→TASK4→TASK2，MTX1：TASK2
 *	== TASK1（続き）==
 *	66:	rot_rdq(MID_PRIORITY)
 *	//		中：TASK4→TASK2→TASK1，MTX1：TASK2
 *	== TASK4-4（優先度：中）==
 *	67:	ext_tsk()
 *	//		中：TASK2→TASK1，MTX1：TASK2
 *	== TASK2-3（続き）==
 *	68:	unl_mtx(MTX1)
 *	//		中：TASK2→TASK1
 *		slp_tsk()
 *	//		中：TASK1
 *
 *	== TASK1（続き）==
 *	69:	loc_mtx(MTX1)
 *	//		中：TASK1，MTX1：TASK1
 *		act_tsk(TASK8)
 *	//		中：TASK1，低：TASK8，MTX1：TASK1
 *		act_tsk(TASK9)
 *	//		中：TASK1，低：TASK8→TASK9，MTX1：TASK1
 *		slp_tsk()
 *	//		低：TASK8→TASK9，MTX1：TASK1
 *	== TASK8-2（優先度：低）
 *		loc_mtx(MTX1)
 *	//		低：TASK9，MTX1：TASK1→TASK8
 *	== TASK9-2（優先度：低）
 *		wup_tsk(TASK1)
 *	//		 中：TASK1，低：TASK9，MTX1：TASK1→TASK8
 *	== TASK1（続き）==
 *	70:	ter_tsk(TASK9)
 *	//		 中：TASK1，MTX1：TASK1→TASK8
 *		dis_dsp()
 *		act_tsk(TASK3)
 *	//		中：TASK1→TASK3，MTX1：TASK1→TASK8
 *		rot_rdq(MID_PRIORITY)
 *	//		中：TASK3→TASK1，MTX1：TASK1→TASK8
 *		act_tsk(TASK4)
 *	//		中：TASK3→TASK1→TASK4，MTX1：TASK1→TASK8
 *		unl_mtx(MTX1)									... (C-3)
 *	//		 中：TASK3→TASK1→TASK4，低：TASK8，MTX1：TASK8
 *		ena_dsp()
 *	== TASK3-5（優先度：中）==
 *	71:	ext_tsk()
 *	//		 中：TASK1→TASK4，低：TASK8，MTX1：TASK8
 *	== TASK1（続き）==
 *	72:	rot_rdq(MID_PRIORITY)
 *	//		 中：TASK4→TASK1，低：TASK8，MTX1：TASK8
 *	== TASK4-5（優先度：中）==
 *	73:	ext_tsk()
 *	//		 中：TASK1，低：TASK8，MTX1：TASK8
 *	== TASK1（続き）==
 *	74:	chg_pri(TASK1, LOW_PRIORITY)
 *	//		低：TASK8→TASK1，MTX1：TASK8
 *	== TASK8-2（続き）==
 *	75:	unl_mtx(MTX1)
 *	//		低：TASK8→TASK1
 *		ext_tsk()
 *	//		低：TASK1
 *	== TASK1（続き）==
 *	76:	END
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
	ercd = loc_mtx(MTX1);
	check_ercd(ercd, E_OK);

	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	ercd = act_tsk(TASK6);
	check_ercd(ercd, E_OK);

	check_point(3);
	ercd = ref_tsk(TASK1, &rtsk);
	check_ercd(ercd, E_OK);

	check_assert(rtsk.tskpri == HIGH_PRIORITY);

	ercd = unl_mtx(MTX1);
	check_ercd(ercd, E_OK);

	check_point(6);
	ercd = loc_mtx(MTX1);
	check_ercd(ercd, E_OK);

	ercd = act_tsk(TASK3);
	check_ercd(ercd, E_OK);

	ercd = rot_rdq(MID_PRIORITY);
	check_ercd(ercd, E_OK);

	check_point(9);
	ercd = unl_mtx(MTX1);
	check_ercd(ercd, E_OK);

	ercd = rot_rdq(MID_PRIORITY);
	check_ercd(ercd, E_OK);

	check_point(15);
	ercd = loc_mtx(MTX1);
	check_ercd(ercd, E_OK);

	ercd = loc_mtx(MTX2);
	check_ercd(ercd, E_OK);

	check_point(21);
	ercd = unl_mtx(MTX2);
	check_ercd(ercd, E_OK);

	ercd = unl_mtx(MTX1);
	check_ercd(ercd, E_OK);

	check_point(23);
	ercd = rot_rdq(MID_PRIORITY);
	check_ercd(ercd, E_OK);

	check_point(26);
	ercd = loc_mtx(MTX1);
	check_ercd(ercd, E_OK);

	ercd = loc_mtx(MTX2);
	check_ercd(ercd, E_OK);

	check_point(31);
	ercd = unl_mtx(MTX2);
	check_ercd(ercd, E_OK);

	ercd = unl_mtx(MTX1);
	check_ercd(ercd, E_OK);

	check_point(33);
	ercd = rot_rdq(MID_PRIORITY);
	check_ercd(ercd, E_OK);

	check_point(36);
	ercd = loc_mtx(MTX1);
	check_ercd(ercd, E_OK);

	ercd = act_tsk(TASK5);
	check_ercd(ercd, E_OK);

	ercd = loc_mtx(MTX2);
	check_ercd(ercd, E_OK);

	check_point(42);
	ercd = unl_mtx(MTX2);
	check_ercd(ercd, E_OK);

	ercd = unl_mtx(MTX1);
	check_ercd(ercd, E_OK);

	ercd = rot_rdq(MID_PRIORITY);
	check_ercd(ercd, E_OK);

	check_point(45);
	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	ercd = act_tsk(TASK3);
	check_ercd(ercd, E_OK);

	ercd = rot_rdq(MID_PRIORITY);
	check_ercd(ercd, E_OK);

	check_point(48);
	ercd = loc_mtx(MTX1);
	check_ercd(ercd, E_OK);

	ercd = act_tsk(TASK8);
	check_ercd(ercd, E_OK);

	check_point(49);
	ercd = loc_mtx(MTX2);
	check_ercd(ercd, E_OK);

	check_point(57);
	ercd = unl_mtx(MTX2);
	check_ercd(ercd, E_OK);

	ercd = unl_mtx(MTX1);
	check_ercd(ercd, E_OK);

	ercd = rot_rdq(MID_PRIORITY);
	check_ercd(ercd, E_OK);

	check_point(59);
	ercd = chg_pri(TASK1, LOW_PRIORITY);
	check_ercd(ercd, E_OK);

	check_point(61);
	ercd = chg_pri(TASK1, TPRI_INI);
	check_ercd(ercd, E_OK);

	check_point(62);
	ercd = loc_mtx(MTX1);
	check_ercd(ercd, E_OK);

	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	ercd = rot_rdq(MID_PRIORITY);
	check_ercd(ercd, E_OK);

	check_point(64);
	ercd = dis_dsp();
	check_ercd(ercd, E_OK);

	ercd = act_tsk(TASK3);
	check_ercd(ercd, E_OK);

	ercd = rot_rdq(MID_PRIORITY);
	check_ercd(ercd, E_OK);

	ercd = act_tsk(TASK4);
	check_ercd(ercd, E_OK);

	ercd = unl_mtx(MTX1);
	check_ercd(ercd, E_OK);

	ercd = ena_dsp();
	check_ercd(ercd, E_OK);

	check_point(66);
	ercd = rot_rdq(MID_PRIORITY);
	check_ercd(ercd, E_OK);

	check_point(69);
	ercd = loc_mtx(MTX1);
	check_ercd(ercd, E_OK);

	ercd = act_tsk(TASK8);
	check_ercd(ercd, E_OK);

	ercd = act_tsk(TASK9);
	check_ercd(ercd, E_OK);

	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(70);
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

	ercd = unl_mtx(MTX1);
	check_ercd(ercd, E_OK);

	ercd = ena_dsp();
	check_ercd(ercd, E_OK);

	check_point(72);
	ercd = rot_rdq(MID_PRIORITY);
	check_ercd(ercd, E_OK);

	check_point(74);
	ercd = chg_pri(TASK1, LOW_PRIORITY);
	check_ercd(ercd, E_OK);

	check_finish(76);
	check_assert(false);
}

static uint_t	task2_count = 0;

void
task2(EXINF exinf)
{
	ER_UINT	ercd;
	T_RTSK	rtsk;

	switch (++task2_count) {
	case 1:
		check_point(7);
		ercd = act_tsk(TASK4);
		check_ercd(ercd, E_OK);

		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_OK);

		check_point(11);
		ercd = unl_mtx(MTX1);
		check_ercd(ercd, E_OK);

		check_point(12);
		ercd = loc_mtx(MTX2);
		check_ercd(ercd, E_OK);

		ercd = act_tsk(TASK3);
		check_ercd(ercd, E_OK);

		ercd = act_tsk(TASK7);
		check_ercd(ercd, E_OK);

		check_point(14);
		ercd = slp_tsk();
		check_ercd(ercd, E_OK);

		check_point(19);
		ercd = unl_mtx(MTX2);
		check_ercd(ercd, E_OK);

		check_point(24);
		ercd = loc_mtx(MTX2);
		check_ercd(ercd, E_OK);

		ercd = slp_tsk();
		check_ercd(ercd, E_OK);

		check_point(30);
		ercd = unl_mtx(MTX2);
		check_ercd(ercd, E_OK);

		check_point(34);
		ercd = loc_mtx(MTX2);
		check_ercd(ercd, E_OK);

		ercd = slp_tsk();
		check_ercd(ercd, E_OK);

		check_point(40);
		ercd = ref_tsk(TASK1, &rtsk);
		check_ercd(ercd, E_OK);

		check_assert(rtsk.tskpri == MID_PRIORITY);

		ercd = unl_mtx(MTX2);
		check_ercd(ercd, E_OK);

		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 2:
		check_point(46);
		ercd = loc_mtx(MTX2);
		check_ercd(ercd, E_OK);

		ercd = slp_tsk();
		check_ercd(ercd, E_OK);

		check_point(55);
		ercd = ter_tsk(TASK9);
		check_ercd(ercd, E_OK);

		ercd = unl_mtx(MTX2);
		check_ercd(ercd, E_OK);

		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 3:
		check_point(63);
		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_OK);

		check_point(68);
		ercd = unl_mtx(MTX1);
		check_ercd(ercd, E_OK);

		ercd = slp_tsk();
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
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 2:
		check_point(16);
		ercd = act_tsk(TASK6);
		check_ercd(ercd, E_OK);

		check_point(18);
		ercd = ref_tsk(TASK1, &rtsk);
		check_ercd(ercd, E_OK);

		check_assert(rtsk.tskpri == HIGH_PRIORITY);

		ercd = wup_tsk(TASK2);
		check_ercd(ercd, E_OK);

		check_point(25);
		ercd = rot_rdq(MID_PRIORITY);
		check_ercd(ercd, E_OK);

		check_point(27);
		ercd = act_tsk(TASK6);
		check_ercd(ercd, E_OK);

		check_point(29);
		ercd = ref_tsk(TASK1, &rtsk);
		check_ercd(ercd, E_OK);

		check_assert(rtsk.tskpri == HIGH_PRIORITY);

		ercd = ref_tsk(TASK2, &rtsk);
		check_ercd(ercd, E_OK);

		check_assert(rtsk.tskpri == HIGH_PRIORITY);

		ercd = wup_tsk(TASK2);
		check_ercd(ercd, E_OK);

		check_point(35);
		ercd = loc_mtx(MTX2);
		check_ercd(ercd, E_OK);

		check_point(41);
		ercd = unl_mtx(MTX2);
		check_ercd(ercd, E_OK);

		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 3:
		check_point(47);
		ercd = loc_mtx(MTX2);
		check_ercd(ercd, E_OK);

		check_point(56);
		ercd = unl_mtx(MTX2);
		check_ercd(ercd, E_OK);

		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 4:
		check_point(65);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 5:
		check_point(71);
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
		check_point(10);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 2:
		check_point(38);
		ercd = loc_mtx(MTX2);
		check_ercd(ercd, E_OK);

		check_point(43);
		ercd = unl_mtx(MTX2);
		check_ercd(ercd, E_OK);

		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 3:
		check_point(51);
		ercd = loc_mtx(MTX2);
		check_ercd(ercd, E_OK);

		check_point(58);
		ercd = unl_mtx(MTX2);
		check_ercd(ercd, E_OK);

		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 4:
		check_point(67);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 5:
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
		check_point(37);
		ercd = act_tsk(TASK4);
		check_ercd(ercd, E_OK);

		ercd = rot_rdq(MID_PRIORITY);
		check_ercd(ercd, E_OK);

		check_point(39);
		ercd = wup_tsk(TASK2);
		check_ercd(ercd, E_OK);

		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_OK);

		check_point(44);
		ercd = loc_mtx(MTX2);
		check_ercd(ercd, E_OK);

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
		ercd = act_tsk(TASK7);
		check_ercd(ercd, E_OK);

		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_OK);

		check_point(5);
		ercd = unl_mtx(MTX1);
		check_ercd(ercd, E_OK);

		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 2:
		check_point(17);
		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_OK);

		check_point(22);
		ercd = unl_mtx(MTX1);
		check_ercd(ercd, E_OK);

		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 3:
		check_point(28);
		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_OK);

		check_point(32);
		ercd = unl_mtx(MTX1);
		check_ercd(ercd, E_OK);

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
		check_point(4);
		ercd = ref_tsk(TASK1, &rtsk);
		check_ercd(ercd, E_OK);

		check_assert(rtsk.tskpri == MID_PRIORITY);

		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 2:
		check_point(13);
		ercd = loc_mtx(MTX2);
		check_ercd(ercd, E_OK);

		check_point(20);
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

static uint_t	task8_count = 0;

void
task8(EXINF exinf)
{
	ER_UINT	ercd;

	switch (++task8_count) {
	case 1:
		check_point(50);
		ercd = act_tsk(TASK4);
		check_ercd(ercd, E_OK);

		check_point(52);
		ercd = act_tsk(TASK9);
		check_ercd(ercd, E_OK);

		check_point(53);
		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_OK);

		check_point(60);
		ercd = unl_mtx(MTX1);
		check_ercd(ercd, E_OK);

		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 2:
		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_OK);

		check_point(75);
		ercd = unl_mtx(MTX1);
		check_ercd(ercd, E_OK);

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
	T_RTSK	rtsk;

	switch (++task9_count) {
	case 1:
		check_point(54);
		ercd = ref_tsk(TASK1, &rtsk);
		check_ercd(ercd, E_OK);

		check_assert(rtsk.tskpri == MID_PRIORITY);

		ercd = wup_tsk(TASK2);
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 2:
		ercd = wup_tsk(TASK1);
		check_ercd(ercd, E_OK);

		check_assert(false);

	default:
		check_assert(false);
	}
	check_assert(false);
}
