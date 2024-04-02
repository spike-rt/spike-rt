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
 *  $Id: test_subprio2.c 1646 2022-09-25 17:51:54Z ertl-hiro $
 */

/* 
 *		サブ優先度機能のテスト(2)
 *
 *	テストの目的とテスト項目の設計については，「サブ優先度機能のテスト」
 *	（test_subprio.txt）を参照すること．
 *
 * 【テスト項目】
 *
 *	(B) ENA_SPRの機能［NGKI3679］
 *		(B-1) ENA_SPRによりサブ優先度を使用する優先度を1つ設定できること
 *	(H) ミューテックス機能による優先順位制御
 *	  (H-1) loc_mtx／ploc_mtxで優先度上限ミューテックスをロックしたこ
 *			とによる自タスクの優先順位の制御（対象タスクの優先度が，サ
 *			ブ優先度を使用すると設定されている場合）
 *		(H-1-1) 自タスクの現在優先度が上がり，上がった優先度内でサブ優
 *				先度0の中の最高優先順位になること
 *		(H-1-2) 自タスクの現在優先度はそのままで，あらたに優先度上昇状
 *				態になったため，サブ優先度0の中の最高優先順位になること
 *		(H-1-3) 自タスクの現在優先度がそのままで，優先度上昇状態が続く
 *				ために，サブ優先度0の中での優先順位が変わらないこと
 *	  (H-2) loc_mtx／ploc_mtxで優先度上限ミューテックスをロックしたこ
 *			とによる自タスクの優先順位の制御（対象タスクの優先度が，サ
 *			ブ優先度を使用すると設定されていない場合）
 *			＊(H-1-1)〜(H-1-3)のいずれの状況も，サブ優先度を使用すると
 *			  設定されていない場合にも同じ振舞いとなり，使用していない
 *			  ことが区別できない．そのため，テスト対象から外す．
 *	  (H-3) unl_mtxの結果，他タスクが優先度上限ミューテックスをロック
 *			したことによるロックしたタスクの優先順位の制御（対象タスク
 *			の優先度が，サブ優先度を使用すると設定されている場合）
 *		(H-3-1) ロックしたタスクの現在優先度が上がり，上がった優先度内
 *				でサブ優先度0の中の最低優先順位になること
 *		(H-3-2) ロックしたタスクの現在優先度はそのままで，サブ優先度0
 *				の中の最低優先順位になること
 *	  (H-4) unl_mtxの結果，他タスクが優先度上限ミューテックスをロック
 *			したことによるロックしたタスクの優先順位の制御（対象タスク
 *			の優先度が，サブ優先度を使用すると設定されていない場合）
 *		(H-4-1) ロックしたタスクの現在優先度が上がり，上がった優先度内
 *				でサブ優先度に関係なく最低優先順位になること
 *	  (H-5) unl_mtxで優先度上限ミューテックスをロック解除したことによ
 *			る自タスクの優先順位の制御（対象タスクの優先度が，サブ優先
 *			度を使用すると設定されている場合）
 *		(H-5-1) 自タスクの現在優先度が下がり，下がった優先度内で元のサ
 *				ブ優先度の中の最高優先順位になること（ディスパッチが起
 *				こる場合）
 *		(H-5-2) 自タスクの現在優先度が下がり，下がった優先度内で元のサ
 *				ブ優先度の中の最高優先順位になること（ディスパッチが起
 *				こらない場合）
 *		(H-5-3) 自タスクの現在優先度がそのままで，優先度上昇状態でなく
 *				なったために，元のサブ優先度の中の最高優先順位になるこ
 *				と（ディスパッチが起こる場合）
 *		(H-5-4) 自タスクの現在優先度がそのままで，優先度上昇状態でなく
 *				なったために，元のサブ優先度の中の最高優先順位になるこ
 *				と（ディスパッチが起こらない場合）
 *		(H-5-5) 自タスクの現在優先度がそのままで，優先度上昇状態が続く
 *				ために，サブ優先度0の中での優先順位が変わらないこと
 *	  (H-6) unl_mtxで優先度上限ミューテックスをロック解除したことによ
 *			る自タスクの優先順位の制御（対象タスクの優先度が，サブ優先
 *			度を使用すると設定されていない場合）
 *		(H-6-1) 自タスクの現在優先度が下がり，下がった優先度内でサブ優
 *				先度に関係なく最高優先順位になること
 *	  (H-7) ini_mtxで優先度上限ミューテックスを初期化したことによるロッ
 *			ク解除したタスクの優先順位の制御（ロックしたタスクが実行で
 *			きる状態の場合。対象タスクの優先度が，サブ優先度を使用する
 *			と設定されている場合）
 *		(H-7-1) ロック解除したタスクの現在優先度が下がり，下がった優先
 *				度内で元のサブ優先度の中の最高優先順位になること（ディ
 *				スパッチが起こる場合）
 *		(H-7-2) ロック解除したタスクの現在優先度が下がり，下がった優先
 *				度内で元のサブ優先度の中の最高優先順位になること（ディ
 *				スパッチが起こらない場合）
 *		(H-7-3) ロック解除したタスクの現在優先度がそのままで，優先度上
 *				昇状態でなくなったために，元のサブ優先度の中の最高優先
 *				順位になること（ディスパッチが起こる場合）
 *		(H-7-4) ロック解除したタスクの現在優先度がそのままで，優先度上
 *				昇状態でなくなったために，元のサブ優先度の中の最高優先
 *				順位になること（ディスパッチが起こらない場合）
 *		(H-7-5) ロック解除したタスクの現在優先度がそのままで，優先度上
 *				昇状態が続くために，サブ優先度0の中での優先順位が変わ
 *				らないこと
 *	  (H-8) ini_mtxで優先度上限ミューテックスを初期化したことによるロッ
 *			ク解除したタスクの優先順位の制御（ロックしたタスクが実行で
 *			きる状態の場合。対象タスクの優先度が，サブ優先度を使用する
 *			と設定されていない場合）
 *		(H-8-1) ロック解除したタスクの現在優先度が下がり，下がった優先
 *				度内でサブ優先度に関係なく最高優先順位になること
 *	  (H-9) ini_mtxで優先度上限ミューテックスを初期化したことによるロッ
 *			ク解除したタスクの優先順位の制御（ロックしたタスクが待ち状
 *			態でタスクの優先度順の待ち行列につながれている場合。対象タ
 *			スクの優先度が，サブ優先度を使用すると設定されている場合）
 *		(H-9-1) ロック解除したタスクの現在優先度が下がり，待ち行列の中
 *				での順序が，サブ優先度に関係なく，下がった優先度内で最
 *				後になること［NGKI2017］［NGKI2018］
 *		(H-9-2) ロック解除したタスクの現在優先度がそのままで，優先度上
 *				昇状態でなくなったために，待ち行列の中での順序が，サブ
 *				優先度に関係なく，同優先度内で最後になること［NGKI2017］
 *				［NGKI2018］
 *		(H-9-3) ロック解除したタスクの現在優先度がそのままで，優先度上
 *				昇状態が続くために，待ち行列の中での順序が，サブ優先度
 *				に関係なく，変わらないこと
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
 *	MTX3:  ミューテックス（TA_CEILING属性，上限は中優先度）
 *	MTX4:  ミューテックス（TA_CEILING属性，上限は高優先度）
 *
 * 【凡例】
 *
 *	「中：[0]TASK2」という記述の中の[0]は，TASK2のサブ優先度が0（最高
 *	値）であることを示す。また，[M]はサブ優先度がUINT_MAX（最低値）で
 *	あることを，[B]は現在優先度が上昇中でサブ優先度が0（最高値）とみな
 *	されていることを示す。
 *
 *	最低優先度タスク（TASK7）は，常に実行できる状態であるため，システ
 *	ムの状態に記載しない。
 *
 * 【テストシーケンス】
 *
 *	== TASK1（優先度：高）==
 *	1:	rot_rdq(HIGH_PRIORITY)						... (B-1)
 *		rot_rdq(MID_PRIORITY) -> E_ILUSE			... (B-1)
 *		rot_rdq(LOW_PRIORITY)						... (B-1)
 *
 *	2:	act_tsk(TASK5)
 *		slp_tsk()
 *	//		低：TASK5
 *	== TASK5-1（優先度：低）==
 *	3:	dis_dsp()
 *		act_tsk(TASK2)
 *	//		中：[M]TASK2，低：TASK5
 *		chg_spr(TASK2, 0)
 *	//		中：[0]TASK2，低：TASK5
 *		ploc_mtx(MTX1)								... (H-1-1)
 *	//		中：[B]TASK5→[0]TASK2，MTX1：TASK5
 *		ena_dsp()
 *	4:	ext_tsk()
 *	//		中：[0]TASK2
 *
 *	== TASK2-1（優先度：中）==
 *	5:	act_tsk(TASK3)
 *	//		中：[0]TASK2→[M]TASK3
 *		slp_tsk()
 *	//		中：[M]TASK3
 *	== TASK3-1（優先度：中）==
 *	6:	dis_dsp()
 *		wup_tsk(TASK2)
 *	//		中：[0]TASK2→[M]TASK3
 *		ploc_mtx(MTX1)								... (H-1-2)
 *	//		中：[B]TASK3→[0]TASK2，MTX1：TASK3
 *		ena_dsp()
 *		slp_tsk()
 *	//		中：[0]TASK2，MTX1：TASK3
 *
 *	== TASK2-1（続き）==
 *	7:	wup_tsk(TASK3)
 *	//		中：[0]TASK2→[B]TASK3，MTX1：TASK3
 *		chg_spr(TASK4, 0)
 *		act_tsk(TASK4)
 *	//		中：[0]TASK2→[B]TASK3→[0]TASK4，MTX1：TASK3
 *		ploc_mtx(MTX2)								... (H-1-3)
 *	//		中：[0]TASK2→[B]TASK3→[0]TASK4，MTX1：TASK3，MTX2：TASK3
 *		ext_tsk()
 *	//		中：[B]TASK3→[0]TASK4，MTX1：TASK3，MTX2：TASK3
 *	== TASK3-1（続き）==
 *	8:	ext_tsk()
 *	//		中：[0]TASK4
 *
 *	== TASK4-1（優先度：中）==
 *	9:	chg_spr(TASK4, 20)
 *	//		中：[20]TASK4
 *		act_tsk(TASK2)
 *	//		中：[0]TASK2→[20]TASK4
 *	== TASK2-2（優先度：中）==
 *	10:	chg_spr(TASK2, 10)
 *	//		中：[10]TASK2→[20]TASK4
 *		loc_mtx(MTX1)
 *	//		中：[B]TASK2→[20]TASK4，MTX1：TASK2
 *		slp_tsk()
 *	//		中：[20]TASK4，MTX1：TASK2
 *	== TASK4-1（続き）==
 *	11:	act_tsk(TASK3)
 *	//		中：[20]TASK4→[M]TASK3，MTX1：TASK2
 *		loc_mtx(MTX1)
 *	//		中：[M]TASK3，MTX1：TASK2→TASK4
 *	== TASK3-2（優先度：中）==
 *	12:	wup_tsk(TASK2)
 *	//		中：[B]TASK2→[M]TASK3，MTX1：TASK2→TASK4
 *	== TASK2-2（続き）==
 *	13:	chg_spr(TASK3, 0)
 *	//		中：[B]TASK2→[0]TASK3，MTX1：TASK2→TASK4
 *		unl_mtx(MTX1)								... (H-3-2)
 *	//		中：[0]TASK3→[B]TASK4→[10]TASK2，MTX1：TASK4
 *	== TASK3-2（続き）==
 *	14:	ext_tsk()
 *	//		中：[B]TASK4→[10]TASK2，MTX1：TASK4
 *	== TASK4-1（続き）==
 *	15:	ext_tsk()
 *	//		中：[10]TASK2
 *
 *	== TASK2-2（続き）==
 *	16:	loc_mtx(MTX1)
 *	//		中：[B]TASK2，MTX1：TASK2
 *		act_tsk(TASK5)
 *	//		中：[B]TASK2，低：TASK5，MTX1：TASK2
 *		slp_tsk()
 *	//		低：TASK5，MTX1：TASK2
 *	== TASK5-2（優先度：低）==
 *	17:	loc_mtx(MTX1)
 *	//		MTX1：TASK2→TASK5
 *	== TASK7（優先度：最低）==
 *	18:	wup_tsk(TASK2)
 *	//		中：[B]TASK2，MTX1：TASK2→TASK5
 *	== TASK2-2（続き）==
 *	19:	act_tsk(TASK3)
 *	//		中：[B]TASK2→[0]TASK3，MTX1：TASK2→TASK5
 *		unl_mtx(MTX1)								... (H-3-1)
 *	//		中：[0]TASK3→[B]TASK5→[10]TASK2，MTX1：TASK5
 *	== TASK3-3（優先度：中）==
 *	20:	ext_tsk()
 *	//		中：[B]TASK5→[10]TASK2，MTX1：TASK5
 *	== TASK5-2（続き）==
 *	21:	ext_tsk()
 *	//		中：[10]TASK2
 *
 *	== TASK2-2（続き）==
 *	22:	wup_tsk(TASK1)
 *	//		高：TASK1，中：[10]TASK2
 *	== TASK1（続き）==
 *	23:	loc_mtx(MTX4)
 *	//		高：TASK1，中：[10]TASK2，MTX4：TASK1
 *		slp_tsk()
 *	//		中：[10]TASK2，MTX4：TASK1
 *	== TASK2-2（続き）==
 *	24:	loc_mtx(MTX4)
 *	//		MTX4：TASK1→TASK2
 *	== TASK7（続き）==
 *	25:	wup_tsk(TASK1)
 *	//		高：TASK1，MTX4：TASK1→TASK2
 *	== TASK1（続き）==
 *	26:	unl_mtx(MTX4)								... (H-4-1)
 *	//		高：TASK1→TASK2，MTX4：TASK2
 *
 *	27:	act_tsk(TASK3)
 *	//		高：TASK1→TASK2，中：[0]TASK3，MTX4：TASK2
 *		chg_spr(TASK4, 10)
 *		act_tsk(TASK4)
 *	//		高：TASK1→TASK2，中：[0]TASK3→[10]TASK4，MTX4：TASK2
 *		rot_rdq(HIGH_PRIORITY)
 *	//		高：TASK2→TASK1，中：[0]TASK3→[10]TASK4，MTX4：TASK2
 *	== TASK2-2（続き）==
 *	28:	unl_mtx(MTX4)								... (H-5-1)
 *	//		高：TASK1，中：[0]TASK3→[10]TASK2→[10]TASK4
 *	== TASK1（続き）==
 *	29:	slp_tsk()
 *	//		中：[0]TASK3→[10]TASK2→[10]TASK4
 *	== TASK3-4（優先度：中）==
 *	30:	ext_tsk()
 *	//		中：[10]TASK2→[10]TASK4
 *	== TASK2-2（続き）==
 *	31:	ext_tsk()
 *	//		中：[10]TASK4
 *
 *	== TASK4-2（優先度：中）==
 *	32:	act_tsk(TASK2)
 *	//		中：[10]TASK4→[10]TASK2
 *		chg_spr(TASK4, 10)
 *	//		中：[10]TASK2→[10]TASK4
 *	== TASK2-3（優先度：中）==
 *	33:	loc_mtx(MTX4)
 *	//		高：TASK2，中：[10]TASK4，MTX4：TASK2
 *		unl_mtx(MTX4)								... (H-5-2)
 *	//		中：[10]TASK2→[10]TASK4
 *	34:	ext_tsk()
 *	//		中：[10]TASK4
 *
 *	== TASK4-2（続き）==
 *	35:	act_tsk(TASK2)
 *	//		中：[10]TASK4→[10]TASK2
 *		chg_spr(TASK4, 10)
 *	//		中：[10]TASK2→[10]TASK4
 *	== TASK2-4（優先度：中）==
 *	36:	loc_mtx(MTX1)
 *	//		中：[B]TASK2→[10]TASK4，MTX1：TASK2
 *		act_tsk(TASK3)
 *	//		中：[B]TASK2→[0]TASK3→[10]TASK4，MTX1：TASK2
 *		loc_mtx(MTX2)
 *	//		中：[B]TASK2→[0]TASK3→[10]TASK4，MTX1：TASK2，MTX2：TASK2
 *		unl_mtx(MTX2)								... (H-5-5)
 *	//		中：[B]TASK2→[0]TASK3→[10]TASK4，MTX1：TASK2
 *		get_nth(MID_PRIORITY, 0U, &tskid)
 *		assert(tskid == TASK2)
 *		get_nth(MID_PRIORITY, 1U, &tskid)
 *		assert(tskid == TASK3)
 *		get_nth(MID_PRIORITY, 2U, &tskid)
 *		assert(tskid == TASK4)
 *	37:	unl_mtx(MTX1)								... (H-5-3)
 *	//		中：[0]TASK3→[10]TASK2→[10]TASK4
 *	== TASK3-5（優先度：中）==
 *	38:	ext_tsk()
 *	//		中：[10]TASK2→[10]TASK4
 *
 *	== TASK2-4（続き）
 *	39:	loc_mtx(MTX1)
 *	//		中：[B]TASK2→[10]TASK4，MTX1：TASK2
 *		unl_mtx(MTX1)								... (H-5-4)
 *	//		中：[10]TASK2→[10]TASK4
 *	40:	ext_tsk()
 *	//		中：[10]TASK4
 *
 *	== TASK4-2（続き）==
 *	41:	act_tsk(TASK5)
 *	//		中：[10]TASK4，低：TASK5
 *		ext_tsk()
 *	//		低：TASK5
 *	== TASK5-3（優先度：低）==
 *	42:	act_tsk(TASK6)
 *	//		低：TASK5→TASK6
 *		chg_spr(TASK6, 10)
 *		loc_mtx(MTX1)
 *	//		中：[B]TASK5，低：TASK6，MTX1：TASK5
 *		unl_mtx(MTX1)								... (H-6-1)
 *	//		低：TASK5→TASK6，MTX1：TASK5
 *	43:	ext_tsk()
 *	//		低：TASK6
 *	== TASK6-1（優先度：低）==
 *	44:	ext_tsk()
 *	== TASK7（続き）==
 *	45:	act_tsk(TASK2)
 *	//		中：[10]TASK2
 *
 *	== TASK2-5（優先度：中）==
 *	46:	loc_mtx(MTX4)
 *	//		高：TASK2，MTX4：TASK2
 *		wup_tsk(TASK1)
 *	//		高：TASK2→TASK1，MTX4：TASK2
 *		act_tsk(TASK3)
 *	//		高：TASK2→TASK1，中：[0]TASK3，MTX4：TASK2
 *		act_tsk(TASK4)
 *	//		高：TASK2→TASK1，中：[0]TASK3→[10]TASK4，MTX4：TASK2
 *		ini_mtx(MTX4)								... (H-7-1)
 *	//		高：TASK1，中：[0]TASK3→[10]TASK2→[10]TASK4
 *	== TASK1（続き）==
 *	47:	slp_tsk()
 *	//		中：[0]TASK3→[10]TASK2→[10]TASK4
 *	== TASK3-6（優先度：中）==
 *	48:	ext_tsk()
 *	//		中：[10]TASK2→[10]TASK4
 *	== TASK2-5（続き）==
 *	49:	loc_mtx(MTX4)
 *	//		高：TASK2，中：[10]TASK4，MTX4：TASK2
 *		wup_tsk(TASK1)
 *	//		高：TASK2→TASK1，中：[10]TASK4，MTX4：TASK2
 *		rot_rdq(HIGH_PRIORITY)
 *	//		高：TASK1→TASK2，中：[10]TASK4，MTX4：TASK2
 *	== TASK1（続き）==
 *	50:	ini_mtx(MTX4)								... (H-7-2)
 *	//		高：TASK1，中：[10]TASK2→[10]TASK4
 *	51:	slp_tsk()
 *	//		中：[10]TASK2→[10]TASK4
 *	== TASK2-5（続き）==
 *	52:	ext_tsk()
 *	//		中：[10]TASK4
 *
 *	== TASK4-3（優先度：中）==
 *	53:	act_tsk(TASK2)
 *	//		中：[10]TASK4→[10]TASK2
 *		chg_spr(TASK4, 10)
 *	//		中：[10]TASK2→[10]TASK4
 *	== TASK2-6（優先度：中）==
 *	54:	loc_mtx(MTX1)
 *	//		中：[B]TASK2→[10]TASK4，MTX1：TASK2
 *		act_tsk(TASK3)
 *	//		中：[B]TASK2→[0]TASK3→[10]TASK4，MTX1：TASK2
 *		loc_mtx(MTX2)
 *	//		中：[B]TASK2→[0]TASK3→[10]TASK4，MTX1：TASK2，MTX2：TASK2
 *		ini_mtx(MTX2)								... (H-7-5)
 *	//		中：[B]TASK2→[0]TASK3→[10]TASK4，MTX1：TASK2
 *		get_nth(MID_PRIORITY, 0U, &tskid)
 *		assert(tskid == TASK2)
 *		get_nth(MID_PRIORITY, 1U, &tskid)
 *		assert(tskid == TASK3)
 *		get_nth(MID_PRIORITY, 2U, &tskid)
 *		assert(tskid == TASK4)
 *	55:	ini_mtx(MTX1)								... (H-7-3)
 *	//		中：[0]TASK3→[10]TASK2→[10]TASK4
 *	== TASK3-7（優先度：中）==
 *	56:	ext_tsk()
 *	//		中：[10]TASK2→[10]TASK4
 *
 *	== TASK2-6（続き）
 *	57:	loc_mtx(MTX1)
 *	//		中：[B]TASK2→[10]TASK4，MTX1：TASK2
 *		act_tsk(TASK3)
 *	//		中：[B]TASK2→[0]TASK3→[10]TASK4，MTX1：TASK2
 *		wup_tsk(TASK1)
 *	//		高：TASK1，中：[B]TASK2→[0]TASK3→[10]TASK4，MTX1：TASK2
 *	== TASK1（続き）==
 *	58:	ini_mtx(MTX1)								... (H-7-4)
 *	//		高：TASK1，中：[0]TASK3→[10]TASK2→[10]TASK4
 *		slp_tsk()
 *	//		中：[0]TASK3→[10]TASK2→[10]TASK4
 *	== TASK3-8（優先度：中）==
 *	59:	ext_tsk()
 *	//		中：[10]TASK2→[10]TASK4
 *	== TASK2-6（続き）
 *	60:	ext_tsk()
 *	//		中：[10]TASK4
 *
 *	== TASK4-3（続き）==
 *	61:	act_tsk(TASK5)
 *	//		中：[10]TASK4，低：TASK5
 *		ext_tsk()
 *	//		低：TASK5
 *	== TASK5-4（優先度：低）==
 *	62:	act_tsk(TASK6)
 *	//		低：TASK5→TASK6
 *		loc_mtx(MTX1)
 *	//		中：[B]TASK5，低：TASK6，MTX1：TASK5
 *		wup_tsk(TASK1)
 *	//		高：TASK1，中：[B]TASK5，低：TASK6，MTX1：TASK5
 *	== TASK1（続き）==
 *	63:	ini_mtx(MTX1)								... (H-8-1)
 *	//		高：TASK1，低：TASK5→TASK6，MTX1：TASK5
 *		slp_tsk()
 *	//		低：TASK5→TASK6，MTX1：TASK5
 *	== TASK5-4（続き）==
 *	64:	ext_tsk()
 *	//		低：TASK6
 *	== TASK6-2（優先度：低）==
 *	65:	ext_tsk()
 *
 *	== TASK7（続き）==
 *	66:	act_tsk(TASK4)
 *	//		中：[10]TASK4
 *	== TASK4-4（優先度：中）==
 *	67:	loc_mtx(MTX1)
 *	//		中：[10]TASK4，MTX1：TASK4
 *		act_tsk(TASK2)
 *	//		中：[10]TASK4→[10]TASK2，MTX1：TASK4
 *		chg_spr(TASK3, 20)
 *		act_tsk(TASK3)
 *	//		中：[10]TASK4→[10]TASK2→[20]TASK3，MTX1：TASK4
 *		slp_tsk()
 *	//		中：[10]TASK2→[20]TASK3，MTX1：TASK4
 *	== TASK2-7（優先度：中）==
 *	68:	loc_mtx(MTX4)
 *	//		高：TASK2，中：[20]TASK3，MTX1：TASK4，MTX4：TASK2
 *		loc_mtx(MTX1)
 *	//		中：[20]TASK3，MTX1：TASK4→TASK2，MTX4：TASK2
 *	== TASK3-9（優先度：中）==
 *	69:	loc_mtx(MTX1)
 *	//		MTX1：TASK4→TASK2→TASK3，MTX4：TASK2
 *	== TASK7（続き）==
 *	70:	wup_tsk(TASK1)
 *	//		高：TASK1，MTX1：TASK4→TASK2→TASK3，MTX4：TASK2
 *	== TASK1（続き）==
 *	71:	ini_mtx(MTX4)								... (H-9-1)
 *	//		高：TASK1，MTX1：TASK4→TASK3→TASK2
 *		wup_tsk(TASK4)
 *	//		高：TASK1，中：TASK4，MTX1：TASK4→TASK3→TASK2
 *		slp_tsk()
 *	//		中：TASK4，MTX1：TASK4→TASK3→TASK2
 *	== TASK4-4（続き）==
 *	72:	unl_mtx(MTX1)
 *	//		中：[10]TASK4→[20]TASK3，MTX1：TASK3→TASK2
 *		ext_tsk()
 *	//		中：[20]TASK3，MTX1：TASK3→TASK2
 *	== TASK3-9（続き）==
 *	73:	unl_mtx(MTX1)
 *	//		中：[10]TASK2→[20]TASK3，MTX1：TASK2
 *	== TASK2-7（続き）==
 *	74:	ext_tsk()
 *	//		中：[20]TASK3
 *
 *	== TASK3-9（続き）==
 *	75:	act_tsk(TASK4)
 *	//		中：[10]TASK4→[20]TASK3
 *	== TASK4-5（優先度：中）==
 *	76:	loc_mtx(MTX1)
 *	//		中：[10]TASK4→[20]TASK3，MTX1：TASK4
 *		act_tsk(TASK2)
 *	//		中：[10]TASK4→[10]TASK2→[20]TASK3，MTX1：TASK4
 *		slp_tsk()
 *	//		中：[10]TASK2→[20]TASK3，MTX1：TASK4
 *	== TASK2-8（優先度：中）==
 *	77:	loc_mtx(MTX2)
 *	//		中：[B]TASK2→[20]TASK3，MTX1：TASK4，MTX2：TASK2
 *		loc_mtx(MTX3)
 *	//		中：[B]TASK2→[20]TASK3，MTX1：TASK4，MTX2：TASK2，MTX3：TASK2
 *		loc_mtx(MTX1)
 *	//		中：[20]TASK3，MTX1：TASK4→TASK2，MTX2：TASK2，MTX3：TASK2
 *	== TASK3-9（続き）==
 *		loc_mtx(MTX1)
 *	//		MTX1：TASK4→TASK2→TASK3，MTX2：TASK2，MTX3：TASK2
 *	== TASK7（続き）==
 *	78:	wup_tsk(TASK1)
 *	//		高：TASK1，MTX1：TASK4→TASK2→TASK3，MTX2：TASK2，MTX3：TASK2
 *	== TASK1（続き）==
 *	79:	ini_mtx(MTX3)								... (H-9-3)
 *	//		高：TASK1，MTX1：TASK4→TASK2→TASK3，MTX2：TASK2
 *		ref_mtx(MTX1, &rmtx)
 *		assert(rmtx.htskid == TASK4)
 *		assert(rmtx.wtskid == TASK2)
 *	80:	ini_mtx(MTX2)								... (H-9-2)
 *	//		高：TASK1，MTX1：TASK4→TASK3→TASK2
 *		wup_tsk(TASK4)
 *	//		高：TASK1，中：TASK4，MTX1：TASK4→TASK3→TASK2
 *		slp_tsk()
 *	//		中：[10]TASK4，MTX1：TASK4→TASK3→TASK2
 *	== TASK4-5（続き）==
 *	81:	unl_mtx(MTX1)
 *	//		中：[10]TASK4→[20]TASK3，MTX1：TASK3→TASK2
 *		ext_tsk()
 *	//		中：[20]TASK3，MTX1：TASK3→TASK2
 *	== TASK3-9（続き）==
 *	82:	unl_mtx(MTX1)
 *	//		中：[10]TASK2→[20]TASK3，MTX1：TASK2
 *	== TASK2-8（続き）==
 *	83:	ext_tsk()
 *	//		中：[20]TASK3
 *	== TASK3-9（続き）==
 *	84:	END
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
	ercd = rot_rdq(HIGH_PRIORITY);
	check_ercd(ercd, E_OK);

	ercd = rot_rdq(MID_PRIORITY);
	check_ercd(ercd, E_ILUSE);

	ercd = rot_rdq(LOW_PRIORITY);
	check_ercd(ercd, E_OK);

	check_point(2);
	ercd = act_tsk(TASK5);
	check_ercd(ercd, E_OK);

	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(23);
	ercd = loc_mtx(MTX4);
	check_ercd(ercd, E_OK);

	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(26);
	ercd = unl_mtx(MTX4);
	check_ercd(ercd, E_OK);

	check_point(27);
	ercd = act_tsk(TASK3);
	check_ercd(ercd, E_OK);

	ercd = chg_spr(TASK4, 10);
	check_ercd(ercd, E_OK);

	ercd = act_tsk(TASK4);
	check_ercd(ercd, E_OK);

	ercd = rot_rdq(HIGH_PRIORITY);
	check_ercd(ercd, E_OK);

	check_point(29);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(47);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(50);
	ercd = ini_mtx(MTX4);
	check_ercd(ercd, E_OK);

	check_point(51);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(58);
	ercd = ini_mtx(MTX1);
	check_ercd(ercd, E_OK);

	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(63);
	ercd = ini_mtx(MTX1);
	check_ercd(ercd, E_OK);

	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(71);
	ercd = ini_mtx(MTX4);
	check_ercd(ercd, E_OK);

	ercd = wup_tsk(TASK4);
	check_ercd(ercd, E_OK);

	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(79);
	ercd = ini_mtx(MTX3);
	check_ercd(ercd, E_OK);

	ercd = ref_mtx(MTX1, &rmtx);
	check_ercd(ercd, E_OK);

	check_assert(rmtx.htskid == TASK4);

	check_assert(rmtx.wtskid == TASK2);

	check_point(80);
	ercd = ini_mtx(MTX2);
	check_ercd(ercd, E_OK);

	ercd = wup_tsk(TASK4);
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
	ID		tskid;

	switch (++task2_count) {
	case 1:
		check_point(5);
		ercd = act_tsk(TASK3);
		check_ercd(ercd, E_OK);

		ercd = slp_tsk();
		check_ercd(ercd, E_OK);

		check_point(7);
		ercd = wup_tsk(TASK3);
		check_ercd(ercd, E_OK);

		ercd = chg_spr(TASK4, 0);
		check_ercd(ercd, E_OK);

		ercd = act_tsk(TASK4);
		check_ercd(ercd, E_OK);

		ercd = ploc_mtx(MTX2);
		check_ercd(ercd, E_OK);

		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 2:
		check_point(10);
		ercd = chg_spr(TASK2, 10);
		check_ercd(ercd, E_OK);

		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_OK);

		ercd = slp_tsk();
		check_ercd(ercd, E_OK);

		check_point(13);
		ercd = chg_spr(TASK3, 0);
		check_ercd(ercd, E_OK);

		ercd = unl_mtx(MTX1);
		check_ercd(ercd, E_OK);

		check_point(16);
		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_OK);

		ercd = act_tsk(TASK5);
		check_ercd(ercd, E_OK);

		ercd = slp_tsk();
		check_ercd(ercd, E_OK);

		check_point(19);
		ercd = act_tsk(TASK3);
		check_ercd(ercd, E_OK);

		ercd = unl_mtx(MTX1);
		check_ercd(ercd, E_OK);

		check_point(22);
		ercd = wup_tsk(TASK1);
		check_ercd(ercd, E_OK);

		check_point(24);
		ercd = loc_mtx(MTX4);
		check_ercd(ercd, E_OK);

		check_point(28);
		ercd = unl_mtx(MTX4);
		check_ercd(ercd, E_OK);

		check_point(31);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 3:
		check_point(33);
		ercd = loc_mtx(MTX4);
		check_ercd(ercd, E_OK);

		ercd = unl_mtx(MTX4);
		check_ercd(ercd, E_OK);

		check_point(34);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 4:
		check_point(36);
		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_OK);

		ercd = act_tsk(TASK3);
		check_ercd(ercd, E_OK);

		ercd = loc_mtx(MTX2);
		check_ercd(ercd, E_OK);

		ercd = unl_mtx(MTX2);
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

		check_point(37);
		ercd = unl_mtx(MTX1);
		check_ercd(ercd, E_OK);

		check_point(39);
		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_OK);

		ercd = unl_mtx(MTX1);
		check_ercd(ercd, E_OK);

		check_point(40);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 5:
		check_point(46);
		ercd = loc_mtx(MTX4);
		check_ercd(ercd, E_OK);

		ercd = wup_tsk(TASK1);
		check_ercd(ercd, E_OK);

		ercd = act_tsk(TASK3);
		check_ercd(ercd, E_OK);

		ercd = act_tsk(TASK4);
		check_ercd(ercd, E_OK);

		ercd = ini_mtx(MTX4);
		check_ercd(ercd, E_OK);

		check_point(49);
		ercd = loc_mtx(MTX4);
		check_ercd(ercd, E_OK);

		ercd = wup_tsk(TASK1);
		check_ercd(ercd, E_OK);

		ercd = rot_rdq(HIGH_PRIORITY);
		check_ercd(ercd, E_OK);

		check_point(52);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 6:
		check_point(54);
		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_OK);

		ercd = act_tsk(TASK3);
		check_ercd(ercd, E_OK);

		ercd = loc_mtx(MTX2);
		check_ercd(ercd, E_OK);

		ercd = ini_mtx(MTX2);
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

		check_point(55);
		ercd = ini_mtx(MTX1);
		check_ercd(ercd, E_OK);

		check_point(57);
		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_OK);

		ercd = act_tsk(TASK3);
		check_ercd(ercd, E_OK);

		ercd = wup_tsk(TASK1);
		check_ercd(ercd, E_OK);

		check_point(60);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 7:
		check_point(68);
		ercd = loc_mtx(MTX4);
		check_ercd(ercd, E_OK);

		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_OK);

		check_point(74);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 8:
		check_point(77);
		ercd = loc_mtx(MTX2);
		check_ercd(ercd, E_OK);

		ercd = loc_mtx(MTX3);
		check_ercd(ercd, E_OK);

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

static uint_t	task3_count = 0;

void
task3(EXINF exinf)
{
	ER_UINT	ercd;

	switch (++task3_count) {
	case 1:
		check_point(6);
		ercd = dis_dsp();
		check_ercd(ercd, E_OK);

		ercd = wup_tsk(TASK2);
		check_ercd(ercd, E_OK);

		ercd = ploc_mtx(MTX1);
		check_ercd(ercd, E_OK);

		ercd = ena_dsp();
		check_ercd(ercd, E_OK);

		ercd = slp_tsk();
		check_ercd(ercd, E_OK);

		check_point(8);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 2:
		check_point(12);
		ercd = wup_tsk(TASK2);
		check_ercd(ercd, E_OK);

		check_point(14);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 3:
		check_point(20);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 4:
		check_point(30);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 5:
		check_point(38);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 6:
		check_point(48);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 7:
		check_point(56);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 8:
		check_point(59);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 9:
		check_point(69);
		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_OK);

		check_point(73);
		ercd = unl_mtx(MTX1);
		check_ercd(ercd, E_OK);

		check_point(75);
		ercd = act_tsk(TASK4);
		check_ercd(ercd, E_OK);

		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_OK);

		check_point(82);
		ercd = unl_mtx(MTX1);
		check_ercd(ercd, E_OK);

		check_finish(84);
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
		check_point(9);
		ercd = chg_spr(TASK4, 20);
		check_ercd(ercd, E_OK);

		ercd = act_tsk(TASK2);
		check_ercd(ercd, E_OK);

		check_point(11);
		ercd = act_tsk(TASK3);
		check_ercd(ercd, E_OK);

		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_OK);

		check_point(15);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 2:
		check_point(32);
		ercd = act_tsk(TASK2);
		check_ercd(ercd, E_OK);

		ercd = chg_spr(TASK4, 10);
		check_ercd(ercd, E_OK);

		check_point(35);
		ercd = act_tsk(TASK2);
		check_ercd(ercd, E_OK);

		ercd = chg_spr(TASK4, 10);
		check_ercd(ercd, E_OK);

		check_point(41);
		ercd = act_tsk(TASK5);
		check_ercd(ercd, E_OK);

		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 3:
		check_point(53);
		ercd = act_tsk(TASK2);
		check_ercd(ercd, E_OK);

		ercd = chg_spr(TASK4, 10);
		check_ercd(ercd, E_OK);

		check_point(61);
		ercd = act_tsk(TASK5);
		check_ercd(ercd, E_OK);

		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 4:
		check_point(67);
		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_OK);

		ercd = act_tsk(TASK2);
		check_ercd(ercd, E_OK);

		ercd = chg_spr(TASK3, 20);
		check_ercd(ercd, E_OK);

		ercd = act_tsk(TASK3);
		check_ercd(ercd, E_OK);

		ercd = slp_tsk();
		check_ercd(ercd, E_OK);

		check_point(72);
		ercd = unl_mtx(MTX1);
		check_ercd(ercd, E_OK);

		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 5:
		check_point(76);
		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_OK);

		ercd = act_tsk(TASK2);
		check_ercd(ercd, E_OK);

		ercd = slp_tsk();
		check_ercd(ercd, E_OK);

		check_point(81);
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

static uint_t	task5_count = 0;

void
task5(EXINF exinf)
{
	ER_UINT	ercd;

	switch (++task5_count) {
	case 1:
		check_point(3);
		ercd = dis_dsp();
		check_ercd(ercd, E_OK);

		ercd = act_tsk(TASK2);
		check_ercd(ercd, E_OK);

		ercd = chg_spr(TASK2, 0);
		check_ercd(ercd, E_OK);

		ercd = ploc_mtx(MTX1);
		check_ercd(ercd, E_OK);

		ercd = ena_dsp();
		check_ercd(ercd, E_OK);

		check_point(4);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 2:
		check_point(17);
		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_OK);

		check_point(21);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 3:
		check_point(42);
		ercd = act_tsk(TASK6);
		check_ercd(ercd, E_OK);

		ercd = chg_spr(TASK6, 10);
		check_ercd(ercd, E_OK);

		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_OK);

		ercd = unl_mtx(MTX1);
		check_ercd(ercd, E_OK);

		check_point(43);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 4:
		check_point(62);
		ercd = act_tsk(TASK6);
		check_ercd(ercd, E_OK);

		ercd = loc_mtx(MTX1);
		check_ercd(ercd, E_OK);

		ercd = wup_tsk(TASK1);
		check_ercd(ercd, E_OK);

		check_point(64);
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
		check_point(44);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 2:
		check_point(65);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

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

	check_point(18);
	ercd = wup_tsk(TASK2);
	check_ercd(ercd, E_OK);

	check_point(25);
	ercd = wup_tsk(TASK1);
	check_ercd(ercd, E_OK);

	check_point(45);
	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	check_point(66);
	ercd = act_tsk(TASK4);
	check_ercd(ercd, E_OK);

	check_point(70);
	ercd = wup_tsk(TASK1);
	check_ercd(ercd, E_OK);

	check_point(78);
	ercd = wup_tsk(TASK1);
	check_ercd(ercd, E_OK);

	check_assert(false);
}
