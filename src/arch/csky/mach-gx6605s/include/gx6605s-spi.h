/* SPDX-License-Identifier: MIT */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#ifndef __GX6605S_SPI_H__
#define __GX6605S_SPI_H__

#include <gx6605s.h>

enum gx6605s_spi_registers {
    GX6605S_SPI_CTRL        = 0x00,
    GX6605S_SPI_STAT        = 0x04,
    GX6605S_SPI_TX_FIFO     = 0x08,
    GX6605S_SPI_RX_FIFO     = 0x0c,
};

/********************************************************************************************/
/*      Mnemonic                        value           meaning/usage                       */

#define GX6605S_SPI_CTRL_NO_INTER_DELAY BIT(21)
#define GX6605S_SPI_CTRL_LSB            BIT(20)
#define GX6605S_SPI_CTRL_POS_RX         BIT(19)
#define GX6605S_SPI_CTRL_CS_EN          BIT(17)
#define GX6605S_SPI_CTRL_CS_FORCE       BIT(16)
#define GX6605S_SPI_CTRL_ICNT_MASK      BIT_RANGE(15, 14)
#define GX6605S_SPI_CTRL_ICNT_4         BIT_SHIFT(14, 3)
#define GX6605S_SPI_CTRL_ICNT_3         BIT_SHIFT(14, 2)
#define GX6605S_SPI_CTRL_ICNT_2         BIT_SHIFT(14, 1)
#define GX6605S_SPI_CTRL_SPIE           BIT(13)
#define GX6605S_SPI_CTRL_SPGO           BIT(12)
#define GX6605S_SPI_CTRL_CS_SEL         BIT(10)
#define GX6605S_SPI_CTRL_CPOL_H         BIT(9)
#define GX6605S_SPI_CTRL_CPHA_H         BIT(8)
#define GX6605S_SPI_CTRL_CSDL           BIT_RANGE(7, 5)
#define GX6605S_SPI_CTRL_SPBR           BIT_RANGE(4, 0)

#define GX6605S_SPI_STAT_BUSY           BIT(1)
#define GX6605S_SPI_STAT_OPE_RDY        BIT(0)

#endif  /* __GX6605S_SPI_H__ */
