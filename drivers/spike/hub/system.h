// SPDX-License-Identifier: MIT
/*
 * API for the hub built-in status light.
 *
 * Copyright (c) 2022 Embedded and Real-Time Systems Laboratory,
 *                    Graduate School of Information Science, Nagoya Univ., JAPAN
 */

/**
 * \file    spike/hub/light.h
 * \brief   API for the hub built-in status light.
 * \author  Shu Yoshifumi
 */

/**
 * \addtogroup  Hub Hub
 * @{
 */


/**
 * \~English
 * \defgroup  System System
 * \brief     API for the entire hub system.
 * @{
 *
 * \~Japanese
 * \defgroup  System システム
 * \brief     ハブ全体のシステム向けAPI．
 * @{
 */


#ifndef _HUB_SYSTEM_H_
#define _HUB_SYSTEM_H_

#include <stdint.h>

/**
 * \~English
 * \brief       Shut down the system.
 * \details     Equivalent to exit(0). Do not call in CPU locked state.
 *
 * \~Japanese
 * \brief       システムをシャットダウンさせる．
 * \details     標準ライブラリexit(0)と同じ．CPUロック状態から呼び出してはいけない．
 */
void hub_system_shutdown(void);

#endif // _HUB_SYSTEM_H_

/**
 * @} // End of group System
 */

/**
 * @} // End of group Hub
 */
