/*
 *  TOPPERS/ASP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Advanced Standard Profile Kernel
 *
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2005-2015 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: core_kernel_impl_v6m.h 1799 2023-04-01 00:50:30Z ertl-komori $
 */

/*
 *  割込み処理モデル（ARMv6-M用）
 *
 *  このインクルードファイルは，core_kernel_impl.h（または，そこから
 *  インクルードされるファイル）のみからインクルードされる．他のファイル
 *  から直接インクルードしてはならない．
 */

#ifndef TOPPERS_CORE_KERNEL_IMPL_V6M_H
#define TOPPERS_CORE_KERNEL_IMPL_V6M_H

#define TBITW_IPRI 2

/*
 *  割込み優先度マスクの外部表現と内部表現の変換
 *
 *  アセンブリ言語のソースファイルからインクルードする場合のために，
 *  CASTを使用
 *                  Highest              Lowest
 *   外部表現 : -(1 << TBITW_IPRI)  ～       0
 *   内部表現 :          0          ～  (1 << TBITW_IPRI)
 */
#define EXT_IPM(iipm) (CAST(PRI, iipm - (1 << TBITW_IPRI)))    /* 内部表現を外部表現に */
#define INT_IPM(ipm)  (CAST(uint8_t, ipm + (1 << TBITW_IPRI))) /* 外部表現を内部表現に */

/*
 *  割込み優先度マスクをNVICの優先度に変換
 */
#define INT_NVIC_PRI(ipm) (((1 << TBITW_IPRI) - CAST(uint8_t, -(ipm)))  << (8 - TBITW_IPRI))

/*
 *  TIPM_ENAALL（割込み優先度マスク全解除）の内部表現
 */
#define IIPM_ENAALL INT_IPM(0)

#define IIPM_LOCK INT_IPM(TMIN_INTPRI)

#define INTNO_2_IRQBIT(n) (1 << (n - 16))

#ifndef TOPPERS_MACRO_ONLY

/*
 *  割込み優先度マスク実現のための変数
 */
extern volatile bool_t lock_flag;
extern uint32_t iipm_enable_masks[];
extern uint32_t *current_iipm_enable_mask;

/*
 *  割込み優先度マスク実現のための定数（kernel_cfg.c）
 */
extern const uint32_t iipm_enable_irq_tbl[];

/*
 *  CPUロック状態への移行
 */
Inline void lock_cpu(void)
{
    sil_wrw_mem((void *)NVIC_CLRENA0, 0xFFFFFFFF);
    sil_wrw_mem((void *)NVIC_SETENA0, iipm_enable_masks[IIPM_LOCK]);
    /* 
     * クリティカルセクションに入る前に全てのメモリアクセスが
     * 終了していることを保証する．
     */
    data_synchronization_barrier();
    lock_flag = 1;
    /* クリティカルセクションの前後でメモリが書き換わる可能性がある */
    ARM_MEMORY_CHANGED;
}

#define lock_cpu_dsp lock_cpu

/*
 *  CPUロック状態の解除
 */
Inline void unlock_cpu(void)
{
    /* クリティカルセクションの前後でメモリが書き換わる可能性がある */
    ARM_MEMORY_CHANGED;
    lock_flag = 0;
    /* 
     * クリティカルセクションに入る前に全てのメモリアクセスが
     * 終了していることを保証する．
     */
    data_synchronization_barrier();
    sil_wrw_mem((void *)NVIC_SETENA0, *current_iipm_enable_mask);
}

Inline void unlock_cpu_dsp(void)
{
    /* クリティカルセクションの前後でメモリが書き換わる可能性がある */
    ARM_MEMORY_CHANGED;
    lock_flag = 0;
    /* 
     * クリティカルセクションに入る前に全てのメモリアクセスが
     * 終了していることを保証する．
     */
    data_synchronization_barrier();
    sil_wrw_mem((void *)NVIC_SETENA0, iipm_enable_masks[IIPM_ENAALL]);
}

/*
 *  CPUロック状態の参照
 */
Inline bool_t sense_lock(void)
{
    return lock_flag;
}

/*
 *  chg_ipmで有効な割込み優先度の範囲の判定
 *
 *  TMIN_INTPRIの値によらず，chg_ipmでは，-(1 << TBITW_IPRI)～TIPM_ENAALL（＝0）
 *  の範囲に設定できることとする（ターゲット定義の拡張）．
 *  割込み優先度のビット幅(TBITW_IPRI)が 2 の場合は，-4 ～ 0 が指定可能である．
 *   
 */
#define VALID_INTPRI_CHGIPM(intpri) \
                ((-((1 << TBITW_IPRI) - 1) <= (intpri) && (intpri) <= TIPM_ENAALL))

/*
 * （モデル上の）割込み優先度マスクの設定
 */
Inline void t_set_ipm(PRI intpri)
{
    current_iipm_enable_mask = &iipm_enable_masks[INT_IPM(intpri)];
    /* 一旦全割込み禁止 */
    sil_wrw_mem((void *)NVIC_CLRENA0, 0xFFFFFFFF);
    sil_wrw_mem((void *)NVIC_SETENA0, *current_iipm_enable_mask);
}

/*
 * （モデル上の）割込み優先度マスクの参照
 */
Inline PRI t_get_ipm(void)
{
    return EXT_IPM(((uintptr_t)current_iipm_enable_mask - (uintptr_t)iipm_enable_masks) / sizeof(iipm_enable_masks[0]));
}

/*
 *  割込み要求禁止フラグ
 */

/*
 *  割込み属性が設定されているかを判別するための変数（kernel_cfg.c）
 */
extern const uint32_t bitpat_cfgint[];

/*
 *  割込み属性の設定のチェック
 */
Inline bool_t check_intno_cfg(INTNO intno)
{
    if ((bitpat_cfgint[intno >> 5] & (1 << (intno & 0x1F))) == 0) {
        return false;
    }
    return true;
}

/*
 *  割込み要求禁止フラグのセット
 * 
 */
Inline void disable_int(INTNO intno)
{
    if (intno == IRQNO_SYSTICK) {
        sil_andw((uint32_t *)SYSTIC_CONTROL_STATUS, ~(SYSTIC_TICINT));
    } else {
        const uint32_t irqbit = INTNO_2_IRQBIT(intno);
        sil_wrw_mem((uint32_t *)NVIC_CLRENA0, irqbit);
        for (int iipm = 0; iipm <= 1 << TBITW_IPRI; ++iipm) {
            iipm_enable_masks[iipm] &= ~(irqbit);
        }
    }
}

/*
 *  割込み要求禁止フラグの解除
 *
 */
Inline void enable_int(INTNO intno)
{
    if (intno == IRQNO_SYSTICK) {
        sil_orw((uint32_t *)SYSTIC_CONTROL_STATUS, SYSTIC_TICINT);
    } else {
        const uint32_t irqbit = INTNO_2_IRQBIT(intno);
        for (int iipm = 0; iipm <= 1 << TBITW_IPRI; ++iipm) {
            if (iipm_enable_irq_tbl[iipm] & irqbit) {
                iipm_enable_masks[iipm] |= irqbit;
            }
        }
        /* 指定された割り込みがカーネル管理外である場合のため，NVICの設定を更新する． */
        sil_wrw_mem((uint32_t *)NVIC_SETENA0, iipm_enable_masks[IIPM_LOCK]);
    }
}

/*
 *  割込み要求がクリアできる割込み番号の範囲の判定
 */
#define VALID_INTNO_CLRINT(intno) \
                (IRQNO_SYSTICK <= (intno) && (intno) <= TMAX_INTNO)

/*
 *  割込み要求がクリアできる状態か？
 */
Inline bool_t check_intno_clear(INTNO intno)
{
    return true;
}

/*
 *  割込み要求のクリア
 */
Inline void clear_int(INTNO intno)
{
    if (intno == IRQNO_SYSTICK) {
        sil_wrw_mem((uint32_t *)NVIC_ICSR, NVIC_PENDSTCLR);
    } else {
        sil_wrw_mem((uint32_t *)NVIC_ICPR0, INTNO_2_IRQBIT(intno));
    }
}

/*
 *  割込みが要求できる状態か？
 */
Inline bool_t check_intno_raise(INTNO intno)
{
    return true;
}

/*
 *  割込みの要求
 */
Inline void raise_int(INTNO intno)
{
    if (intno == IRQNO_SYSTICK) {
        sil_wrw_mem((uint32_t *)NVIC_ICSR, NVIC_PENDSTSET);
    } else {
        sil_wrw_mem((uint32_t *)NVIC_ISPR0, INTNO_2_IRQBIT(intno));
    }
}

/*
 *  割込み要求のチェック
 */
Inline bool_t probe_int(INTNO intno)
{
    if (intno == IRQNO_SYSTICK) {
        return (sil_rew_mem((uint32_t *)NVIC_ICSR) & NVIC_PENDSTSET) ? true : false;
    } else {
        return (sil_rew_mem((uint32_t *)NVIC_ISPR0) & INTNO_2_IRQBIT(intno)) ? true : false;
    }
}

#endif /* TOPPERS_MACRO_ONLY */
#endif /* TOPPERS_CORE_KERNEL_IMPL_V6M_H */
