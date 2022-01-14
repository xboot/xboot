/* SPDX-License-Identifier: MIT */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#ifndef __GX6605S_CIR_H__
#define __GX6605S_CIR_H__

#include <gx6605s.h>

enum gx6605s_cir_regsiters {
    GX6605S_CIR_CNTL    = 0x00,
    GX6605S_CIR_INT     = 0x04,
    GX6605S_CIR_FIFO    = 0x08,
    GX6605S_CIR_CLK     = 0x0c,
};

/************************************************************************************/
/*      Mnemonic                    value             meaning/usage                 */

#define GX6605S_CIR_CNTL_TOV        BIT_SHIFT(12, 20000)
#define GX6605S_CIR_CNTL_FIFOIL     BIT_RANGE(11, 6)
#define GX6605S_CIR_CNTL_POL        BIT(5)
#define GX6605S_CIR_CNTL_ENIR       BIT(3)
#define GX6605S_CIR_CNTL_FIFOTM     BIT(1)

#define GX6605S_INT_NUM             BIT_RANGE(8, 3)
#define GX6605S_INT_FIFOTS          BIT(1)

#endif  /* __GX6605S_CIR_H__ */
