/* SPDX-License-Identifier: MIT */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#ifndef __GX6605S_I2C_H_
#define __GX6605S_I2C_H_

#include <gx6605s.h>

enum gx6605s_i2c_registers {
    GX6605S_I2C_CTRL    = 0x00,
    GX6605S_I2C_DATA    = 0x04,
    GX6605S_I2C_CLK     = 0x08,
};

/************************************************************************************/
/*      Mnemonic                    value              meaning/usage                */

#define GX6605S_I2C_CTRL_INTEN      BIT(7)          /* Interrupt Enable */
#define GX6605S_I2C_CTRL_EN         BIT(6)          /* Module Enable */
#define GX6605S_I2C_CTRL_START      BIT(5)          /* Start Transfer */
#define GX6605S_I2C_CTRL_STOP       BIT(4)          /* Stop Transfer */
#define GX6605S_I2C_CTRL_DONE       BIT(3)          /* Operatin Finish */
#define GX6605S_I2C_CTRL_RACK       BIT(2)          /* Read Ack */
#define GX6605S_I2C_CTRL_WACK       BIT(1)          /* Write Ack */

/* BUG for hardware, if 4 division, it fail often */
#define GX6605S_I2C_CLKDIV(IN, HZ)  (((IN) / (HZ) / 4) - 1)

#endif  /* __GX6605S_I2C_H_ */