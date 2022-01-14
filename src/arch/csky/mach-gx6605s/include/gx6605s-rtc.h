/* SPDX-License-Identifier: MIT */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#ifndef __GX6605S_RTC_H__
#define __GX6605S_RTC_H__

#include <gx6605s.h>

enum gx6605s_rtc_regsiters {
    GX6605S_RTC_CTL             = 0x00,
    GX6605S_RTC_INT             = 0x04,
    GX6605S_RTC_CLK             = 0x0c,
    GX6605S_RTC_ALM             = 0x10,

    GX6605S_RTC_ALM1_US         = 0x14,
    GX6605S_RTC_ALM1_MS         = 0x18,
    GX6605S_RTC_ALM1_SEC        = 0x1c,
    GX6605S_RTC_ALM1_MIN        = 0x20,
    GX6605S_RTC_ALM1_HOUR       = 0x24,
    GX6605S_RTC_ALM1_WEEK       = 0x28,
    GX6605S_RTC_ALM1_DAY        = 0x2c,
    GX6605S_RTC_ALM1_MONTH      = 0x30,
    GX6605S_RTC_ALM1_YEAR       = 0x34,

    GX6605S_RTC_ALM2_US         = 0x3c,
    GX6605S_RTC_ALM2_MS         = 0x40,
    GX6605S_RTC_ALM2_SEC        = 0x44,
    GX6605S_RTC_ALM2_MIN        = 0x48,
    GX6605S_RTC_ALM2_HOUR       = 0x4c,
    GX6605S_RTC_ALM2_WEEK       = 0x50,
    GX6605S_RTC_ALM2_DAY        = 0x54,
    GX6605S_RTC_ALM2_MONTH      = 0x58,
    GX6605S_RTC_ALM2_YEAR       = 0x5c,

    GX6605S_RTC_US              = 0x64,
    GX6605S_RTC_MS              = 0x68,
    GX6605S_RTC_SEC             = 0x6c,
    GX6605S_RTC_MIN             = 0x70,
    GX6605S_RTC_HOUR            = 0x74,
    GX6605S_RTC_WEEK            = 0x78,
    GX6605S_RTC_DAY             = 0x7c,
    GX6605S_RTC_MONTH           = 0x80,
    GX6605S_RTC_YEAR            = 0x84,
};

/************************************************************************************/
/*      Mnemonic                    value             meaning/usage                 */

#define GX6605S_RTC_CTL_ALM2        BIT(3)
#define GX6605S_RTC_CTL_ALM1        BIT(2)
#define GX6605S_RTC_CTL_CLOCK       BIT(1)
#define GX6605S_RTC_CTL_TIME        BIT(0)

#define GX6605S_RTC_INT_ALM2        BIT(2)
#define GX6605S_RTC_INT_ALM1        BIT(1)
#define GX6605S_RTC_INT_EN          BIT(0)

#define GX6605S_RTC_LATCH(fin)      ((fin) / 100000)

#endif  /* __GX6605S_RTC_H__ */
