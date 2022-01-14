/* SPDX-License-Identifier: MIT */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#ifndef __GX6605S_UART_H__
#define __GX6605S_UART_H__

#include <gx6605s.h>

enum gx6605s_uart_regsiters {
    GX6605S_UART_CTL    = 0x00,
    GX6605S_UART_STA    = 0x04,
    GX6605S_UART_DAT    = 0x08,
    GX6605S_UART_CLK    = 0x0c,
    GX6605S_UART_CFG    = 0x10,
};

/********************************************************************************************/
/*      Mnemonic                    value           meaning/usage                           */

#define GX6605S_UART_STA_THRE       (1 << 7)    /* register empty */
#define GX6605S_UART_STA_DR         (1 << 5)    /* data ready */

#endif  /* __GX6605S_UART_H__ */

