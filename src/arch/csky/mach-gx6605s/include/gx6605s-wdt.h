/* SPDX-License-Identifier: MIT */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#ifndef __GX6605S_WDT_H__
#define __GX6605S_WDT_H__

#include <gx6605s.h>

enum gx6605s_wdt_regsiters {
    GX6605S_WDT_CTRL        = 0x00,
    GX6605S_WDT_MATCH       = 0x04,
    GX6605S_WDT_COUNT       = 0x08,
    GX6605S_WDT_WSR         = 0x0c,
};

/************************************************************************************/
/*      Mnemonic                    value             meaning/usage                 */

#define GX6605S_WDT_CTRL_RESET      BIT(1)
#define GX6605S_WDT_CTRL_ENABLE     BIT(0)

#define GX6605S_WDT_MATCH_CLKDIV    BIT_RANGE(31, 16)
#define GX6605S_WDT_MATCH_TIMEOUT   BIT_RANGE(15, 0)

#define GX6605S_WDT_WSR_MASK2       0xaaaa
#define GX6605S_WDT_WSR_MASK1       0x5555

#endif  /* __GX6605S_WDT_H__ */
