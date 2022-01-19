/* SPDX-License-Identifier: MIT */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <xboot.h>
#include <spi/spi.h>
#include <gx6605s-spi.h>

struct gx6605s_device {
    virtual_addr_t base;
};

static inline uint32_t
gx6605s_read(struct gx6605s_device *gdev, unsigned int reg)
{
    return read32(gdev->base + reg);
}

static inline void
gx6605s_write(struct gx6605s_device *gdev, unsigned int reg, uint32_t val)
{
    write32(gdev->base + reg, val);
}

static void spi_gx6605s_select(struct spi_t *spi, int cs)
{
    struct gx6605s_device *gdev = spi->priv;
    uint32_t val;

    val = gx6605s_read(gdev, GX6605S_SPI_CTRL);
    val &= ~GX6605S_SPI_CTRL_CS_SEL;
    val |= GX6605S_SPI_CTRL_CS_EN | GX6605S_SPI_CTRL_CS_FORCE;
    gx6605s_write(gdev, GX6605S_SPI_CTRL, val);
}

static void spi_gx6605s_deselect(struct spi_t *spi, int cs)
{
    struct gx6605s_device *gdev = spi->priv;
    uint32_t val;

    val = gx6605s_read(gdev, GX6605S_SPI_CTRL);
    val &= ~GX6605S_SPI_CTRL_CS_SEL;
    val &= ~(GX6605S_SPI_CTRL_CS_EN | GX6605S_SPI_CTRL_CS_FORCE);
    gx6605s_write(gdev, GX6605S_SPI_CTRL, val);
}

static int spi_gx6605s_transfer(struct spi_t *spi, struct spi_msg_t *msg)
{
    struct gx6605s_device *gdev = spi->priv;
    uint32_t xfer, count, val;
    int len = msg->len * msg->bits / 8;
    uint8_t *txbuf = msg->txbuf;
    uint8_t *rxbuf = msg->rxbuf;

    for (; (xfer = min(len, 4)); len -= xfer) {
        /* set transmit len */
        val = gx6605s_read(gdev, GX6605S_SPI_CTRL);
        val &= ~GX6605S_SPI_CTRL_ICNT_MASK;
        val |= ((xfer - 1) << 14) & GX6605S_SPI_CTRL_ICNT_MASK;
        gx6605s_write(gdev, GX6605S_SPI_CTRL, val);

        for (val = 0, count = xfer; count; --count) {
            if (txbuf)
                val |= *txbuf++ << ((count - 1) * 8);
            else
                val |= 0xff << ((count - 1) * 8);
        }

        /* start to transfer data */
        gx6605s_write(gdev, GX6605S_SPI_TX_FIFO, val);
        val = gx6605s_read(gdev, GX6605S_SPI_CTRL);
        val |= GX6605S_SPI_CTRL_SPGO;
        gx6605s_write(gdev, GX6605S_SPI_CTRL, val);

        while(!(gx6605s_read(gdev, GX6605S_SPI_STAT) & GX6605S_SPI_STAT_OPE_RDY));

        val = gx6605s_read(gdev, GX6605S_SPI_CTRL);
        val &= ~GX6605S_SPI_CTRL_SPGO;
        gx6605s_write(gdev, GX6605S_SPI_CTRL, val);
        val = gx6605s_read(gdev, GX6605S_SPI_RX_FIFO);

        for (count = xfer; rxbuf && count; --count)
            *rxbuf++ = val >> ((count - 1) * 8);
    }

    return msg->len;
}

static struct device_t *spi_gx6605s_probe(struct driver_t *drv, struct dtnode_t *node)
{
    struct gx6605s_device *gdev;
    struct device_t *dev;
    struct spi_t *spi;

    gdev = malloc(sizeof(*gdev));
    if (!gdev)
        return NULL;

    spi = malloc(sizeof(*spi));
    if (!spi) {
        free(gdev);
        return NULL;
    }

    gdev->base = phys_to_virt(dt_read_address(node));

    spi->name = alloc_device_name(dt_read_name(node), -1);
    spi->type = SPI_TYPE_SINGLE;
    spi->transfer = spi_gx6605s_transfer;
    spi->select = spi_gx6605s_select;
    spi->deselect = spi_gx6605s_deselect;
    spi->priv = gdev;

    gx6605s_write(gdev, GX6605S_SPI_CTRL, 0x8420c000);

    if (!(dev = register_spi(spi, drv))) {
        free_device_name(spi->name);
        free(spi->priv);
        free(spi);
        return NULL;
    }

    return dev;
}

static void spi_gx6605s_remove(struct device_t * dev)
{
    struct spi_t *spi = dev->priv;

    if (spi) {
        unregister_spi(spi);
        free_device_name(spi->name);
        free(spi->priv);
        free(spi);
    }
}

static void spi_gx6605s_suspend(struct device_t *dev) {}
static void spi_gx6605s_resume(struct device_t *dev) {}

static struct driver_t spi_gx6605s = {
    .name = "spi-gx6605s",
    .probe = spi_gx6605s_probe,
    .remove = spi_gx6605s_remove,
    .suspend = spi_gx6605s_suspend,
    .resume = spi_gx6605s_resume,
};

static __init void spi_gx6605s_driver_init(void)
{
    register_driver(&spi_gx6605s);
}

static __exit void spi_gx6605s_driver_exit(void)
{
    unregister_driver(&spi_gx6605s);
}

driver_initcall(spi_gx6605s_driver_init);
driver_exitcall(spi_gx6605s_driver_exit);
