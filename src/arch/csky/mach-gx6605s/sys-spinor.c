/* SPDX-License-Identifier: MIT */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <xboot.h>
#include <gx6605s-spi.h>

static void __startup spi_sel(int sel)
{
    uint32_t val = read32(SPI_BASE + GX6605S_SPI_CTRL);

    if (sel) {
        val &= ~GX6605S_SPI_CTRL_CS_SEL;
        val |= GX6605S_SPI_CTRL_CS_EN | GX6605S_SPI_CTRL_CS_FORCE;
    } else {
        val &= ~GX6605S_SPI_CTRL_CS_SEL;
        val &= ~(GX6605S_SPI_CTRL_CS_EN | GX6605S_SPI_CTRL_CS_FORCE);
    }

    write32(SPI_BASE + GX6605S_SPI_CTRL, val);
}

static void __startup spi_transmit(uint8_t *txbuf, uint8_t *rxbuf, uint32_t len)
{
    uint32_t xfer, count, val;

    for (; (xfer = min(len, 4U)); len -= xfer) {
        /* set transmit len */
        val = read32(SPI_BASE + GX6605S_SPI_CTRL);
        val &= ~GX6605S_SPI_CTRL_ICNT_MASK;
        val |= ((xfer - 1) << 14) & GX6605S_SPI_CTRL_ICNT_MASK;
        write32(SPI_BASE + GX6605S_SPI_CTRL, val);

        for (val = 0, count = xfer; count; --count) {
            if (txbuf)
                val |= *txbuf++ << ((count - 1) * 8);
            else
                val |= 0xff << ((count - 1) * 8);
        }

        /* start to transfer data */
        write32(SPI_BASE + GX6605S_SPI_TX_FIFO, val);
        val = read32(SPI_BASE + GX6605S_SPI_CTRL);
        val |= GX6605S_SPI_CTRL_SPGO;
        write32(SPI_BASE + GX6605S_SPI_CTRL, val);

        while (!(read32(SPI_BASE + GX6605S_SPI_STAT) & GX6605S_SPI_STAT_OPE_RDY));

        val = read32(SPI_BASE + GX6605S_SPI_CTRL);
        val &= ~GX6605S_SPI_CTRL_SPGO;
        write32(SPI_BASE + GX6605S_SPI_CTRL, val);
        val = read32(SPI_BASE + GX6605S_SPI_RX_FIFO);

        for (count = xfer; rxbuf && count; --count)
            *rxbuf++ = val >> ((count - 1) * 8);
    }
}

void __startup sys_spinor_init(void)
{
    spi_sel(FALSE);
    write32(SPI_BASE + GX6605S_SPI_CTRL, 0x8420c000);
    tim_mdelay(10);
}

void __startup sys_spinor_read(uint8_t *buff, size_t addr, uint32_t len)
{
    uint8_t cmd[5] = {
        [0] = 0x0b,
        [1] = addr >> 16,
        [2] = addr >> 8,
        [3] = addr >> 0,
        [4] = 0x00,
    };

    spi_sel(TRUE);
    spi_transmit(cmd, NULL, ARRAY_SIZE(cmd));
    spi_transmit(NULL, buff, len);
    spi_sel(FALSE);
}
