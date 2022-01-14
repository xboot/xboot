/* SPDX-License-Identifier: MIT */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <xboot.h>
#include <clk/clk.h>
#include <gpio/gpio.h>
#include <uart/uart.h>
#include <gx6605s-gctl.h>
#include <gx6605s-uart.h>

struct gx6605s_device {
    virtual_addr_t base;
    int baud;
    char *clk;
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

static ssize_t uart_gx6605s_get(struct uart_t *uart, int *baud, int *data, int *parity, int *stop)
{
    struct gx6605s_device *gdev = uart->priv;

    *baud = gdev->baud;
    *data = 8;
    *parity = 0;
    *stop = 1;

    return TRUE;
}

static ssize_t uart_gx6605s_set(struct uart_t *uart, int baud, int data, int parity, int stop)
{
    struct gx6605s_device *gdev = uart->priv;
    uint64_t clkdiv, apb;

    if (data != 8 || parity != 0 || stop != 1)
        return FALSE;

    apb = clk_get_rate(gdev->clk);
    clkdiv = apb / ((16 * baud) / 100);
    if (clkdiv % 100 > 50)
        clkdiv = clkdiv / 100;
    else
        clkdiv = clkdiv / 100 - 1;

    gdev->baud = baud;
    gx6605s_write(gdev, GX6605S_UART_CLK, clkdiv);

    return TRUE;
}

static ssize_t uart_gx6605s_read(struct uart_t *uart, u8_t *buf, size_t count)
{
    struct gx6605s_device *gdev = uart->priv;
    size_t xfer;

    for (xfer = 0; xfer <= count; ++xfer) {
        if (!(gx6605s_read(gdev, GX6605S_UART_STA) & GX6605S_UART_STA_DR))
            break;
        *buf++ = gx6605s_read(gdev, GX6605S_UART_DAT) & 0xff;
    }

    return xfer;
}

static ssize_t uart_gx6605s_write(struct uart_t *uart, const u8_t *buf, size_t count)
{
    struct gx6605s_device *gdev = uart->priv;
    size_t xfer;

    for (xfer = 0; xfer < count; ++xfer) {
        while (!(gx6605s_read(gdev, GX6605S_UART_STA) & GX6605S_UART_STA_THRE));
        gx6605s_write(gdev, GX6605S_UART_DAT, *buf++);
    }

    return xfer;
}

static struct device_t *uart_gx6605s_probe(struct driver_t *drv, struct dtnode_t *node)
{
    struct gx6605s_device *gdev;
    struct device_t *dev;
    struct uart_t *uart;
    uint32_t baud;
    int txd, txdcfg;
    int rxd, rxdcfg;
    char *clk;

    clk = dt_read_string(node, "clock-name", NULL);
    if (!search_clk(clk))
        return NULL;

    gdev = malloc(sizeof(*gdev));
    if (!gdev)
        return NULL;

    uart = malloc(sizeof(struct uart_t));
    if (!uart) {
        free(gdev);
        return NULL;
    }

    gdev->base = phys_to_virt(dt_read_address(node));
    gdev->clk = strdup(clk);

    uart->name = alloc_device_name(dt_read_name(node), -1);
    uart->set = uart_gx6605s_set;
    uart->get = uart_gx6605s_get;
    uart->read = uart_gx6605s_read;
    uart->write = uart_gx6605s_write;
    uart->priv = gdev;

    txd = dt_read_int(node, "txd-gpio", -1);
    txdcfg = dt_read_int(node, "txd-gpio-config", -1);
    rxd = dt_read_int(node, "rxd-gpio", -1);
    rxdcfg = dt_read_int(node, "rxd-gpio-config", -1);

    if (txd >= 0 && txdcfg >= 0)
        gpio_set_cfg(txd, txdcfg);

    if (rxd >= 0 && rxdcfg >= 0)
        gpio_set_cfg(rxd, rxdcfg);

    baud = dt_read_int(node, "baud-rates", 115200);
    uart_gx6605s_set(uart, baud, 8, 0, 1);
    gx6605s_write(gdev, GX6605S_UART_CTL, 0x600);

    if (!(dev = register_uart(uart, drv))) {
        free(gdev->clk);
        free_device_name(uart->name);
        free(uart->priv);
        free(uart);
        return NULL;
    }

    return dev;
}

static void uart_gx6605s_remove(struct device_t *dev)
{
    struct uart_t *uart = dev->priv;
    struct gx6605s_device *gdev = uart->priv;

    if (uart) {
        unregister_uart(uart);
        clk_disable(gdev->clk);
        free(gdev->clk);
        free_device_name(uart->name);
        free(uart->priv);
        free(uart);
    }
}

static void uart_gx6605s_suspend(struct device_t *dev) {}
static void uart_gx6605s_resume(struct device_t *dev) {}

static struct driver_t uart_gx6605s = {
    .name = "uart-gx6605s",
    .probe = uart_gx6605s_probe,
    .remove = uart_gx6605s_remove,
    .suspend = uart_gx6605s_suspend,
    .resume = uart_gx6605s_resume,
};

static __init void uart_gx6605s_driver_init(void)
{
    register_driver(&uart_gx6605s);
}

static __exit void uart_gx6605s_driver_exit(void)
{
    unregister_driver(&uart_gx6605s);
}

driver_initcall(uart_gx6605s_driver_init);
driver_exitcall(uart_gx6605s_driver_exit);
