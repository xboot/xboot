/* SPDX-License-Identifier: MIT */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <xboot.h>
#include <clk/clk.h>
#include <gpio/gpio.h>
#include <i2c/i2c.h>
#include <gx6605s-i2c.h>

struct gx6605s_device {
    virtual_addr_t base;
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

static bool_t i2c_gx6605s_wait_busy(struct gx6605s_device *gdev)
{
    int timeout = 500;
    uint32_t val;

    while (--timeout) {
        val = gx6605s_read(gdev, GX6605S_I2C_CTRL);
        if (val & (GX6605S_I2C_CTRL_DONE))
            break;
        usleep(100);
    }

    return !!timeout;
}

static bool_t i2c_gx6605s_wait_ack(struct gx6605s_device *gdev)
{
    int timeout = 500;
    uint32_t val;

    while (--timeout) {
        val = gx6605s_read(gdev, GX6605S_I2C_CTRL);
        if ((val & GX6605S_I2C_CTRL_DONE) && !(val & GX6605S_I2C_CTRL_WACK))
            break;
        usleep(100);
    }

    return !!timeout;
}

static bool_t i2c_gx6605s_start(struct gx6605s_device *gdev, uint32_t address)
{
    unsigned int retry = 3;

    while (--retry) {
        gx6605s_write(gdev, GX6605S_I2C_DATA, address);
        gx6605s_write(gdev, GX6605S_I2C_CTRL, GX6605S_I2C_CTRL_EN | GX6605S_I2C_CTRL_START);

        if (i2c_gx6605s_wait_ack(gdev))
            break;
    }

    return !!retry;
}

static bool_t i2c_gx6605s_stop(struct gx6605s_device *gdev)
{
    gx6605s_write(gdev, GX6605S_I2C_CTRL, GX6605S_I2C_CTRL_EN | GX6605S_I2C_CTRL_STOP);

    if (!i2c_gx6605s_wait_ack(gdev))
        return FALSE;

    gx6605s_write(gdev, GX6605S_I2C_CTRL, 0);
    return TRUE;
}

static bool_t i2c_gx6605s_read(struct gx6605s_device *gdev, unsigned int len, uint8_t *buff)
{
    uint32_t mask;

    /**
     * NOTE: here's one BUG for HW. ACK/NOACK must be sent before reading data,
     * or the 1st data is incorrect (device address).
     */

    mask = GX6605S_I2C_CTRL_EN | GX6605S_I2C_CTRL_RACK;

    while (len--) {
        gx6605s_write(gdev, GX6605S_I2C_CTRL, mask);

        if (!i2c_gx6605s_wait_busy(gdev))
            return FALSE;

        *buff++ = gx6605s_read(gdev, GX6605S_I2C_DATA);
        mask = GX6605S_I2C_CTRL_EN;
    }

    return TRUE;
}

static bool_t i2c_gx6605s_write(struct gx6605s_device *gdev, unsigned int len, uint8_t *buff)
{
    while (len--) {
        unsigned int retry = 3;

        while (--retry) {
            gx6605s_write(gdev, GX6605S_I2C_DATA, *buff++);
            gx6605s_write(gdev, GX6605S_I2C_CTRL, GX6605S_I2C_CTRL_EN);

            if (i2c_gx6605s_wait_ack(gdev))
                break;
        } if (!retry)
            return FALSE;
    }

    return TRUE;
}

static int i2c_gx6605s_xfer(struct i2c_t *i2c, struct i2c_msg_t *msgs, int num)
{
    struct gx6605s_device *gdev = i2c->priv;
    unsigned int count;
    uint32_t address;

    for (count = 0; count < num; ++count) {
        struct i2c_msg_t *msg = &msgs[count];

        if (!(msg->flags & I2C_M_NOSTART)) {
            if (msgs->flags & I2C_M_RD)
                address = (msgs->addr << 1) | 0x01;
            else
                address = (msgs->addr << 1);
            if (!i2c_gx6605s_start(gdev, address))
                goto exit;
        }

        if (msg->flags & I2C_M_RD) {
            if (!i2c_gx6605s_read(gdev, msg->len, msg->buf))
                goto exit;
        } else {
            if (!i2c_gx6605s_write(gdev, msg->len, msg->buf))
                goto exit;
        }

        if (!(msg->flags & I2C_M_REV_DIR_ADDR)) {
            if (!i2c_gx6605s_stop(gdev))
                return 0;
        }
    }

    return count;

exit:
    i2c_gx6605s_stop(gdev);
    return 0;
}

static struct device_t *i2c_gx6605s_probe(struct driver_t *drv, struct dtnode_t *node)
{
    struct gx6605s_device *gdev;
    struct device_t *dev;
    struct i2c_t *i2c;
    int sda, sdacfg;
    int scl, sclcfg;
    int baud;
    char *clk;
    uint32_t val;

    clk = dt_read_string(node, "clock-name", NULL);
    if (!search_clk(clk))
        return NULL;

    gdev = malloc(sizeof(*gdev));
    if (!gdev)
        return NULL;

    i2c = malloc(sizeof(*i2c));
    if (!i2c) {
        free(gdev);
        return NULL;
    }

    gdev->base = phys_to_virt(dt_read_address(node));
    gdev->clk = strdup(clk);

    i2c->name = alloc_device_name(dt_read_name(node), -1);
    i2c->xfer = i2c_gx6605s_xfer;
    i2c->priv = gdev;

    sda = dt_read_int(node, "sda-gpio", -1);
    sdacfg = dt_read_int(node, "sda-gpio-config", -1);
    scl = dt_read_int(node, "scl-gpio", -1);
    sclcfg = dt_read_int(node, "scl-gpio-config", -1);

    if (sda >= 0 && sdacfg >= 0)
        gpio_set_cfg(sda, sdacfg);

    if (scl >= 0 && sclcfg >= 0)
        gpio_set_cfg(scl, sclcfg);

    baud = dt_read_int(node, "clock-frequency", 400000);
    val = baud / clk_get_rate(gdev->clk) / 4 - 1;
    gx6605s_write(gdev, GX6605S_I2C_CLK, (val << 16) + val);
    gx6605s_write(gdev, GX6605S_I2C_CTRL, GX6605S_I2C_CTRL_EN);

    if (!(dev = register_i2c(i2c, drv))) {
        free_device_name(i2c->name);
        free(i2c->priv);
        free(i2c);
        return NULL;
    }

    return dev;
}

static void i2c_gx6605s_remove(struct device_t *dev)
{
    struct i2c_t *i2c = (struct i2c_t *)dev->priv;

    if (i2c) {
        unregister_i2c(i2c);
        free_device_name(i2c->name);
        free(i2c->priv);
        free(i2c);
    }
}


static void i2c_gx6605s_suspend(struct device_t *dev) {}
static void i2c_gx6605s_resume(struct device_t *dev) {}

static struct driver_t i2c_gx6605s = {
    .name = "i2c-gx6605s",
    .probe = i2c_gx6605s_probe,
    .remove = i2c_gx6605s_remove,
    .suspend = i2c_gx6605s_suspend,
    .resume = i2c_gx6605s_resume,
};

static __init void i2c_gx6605s_driver_init(void)
{
    register_driver(&i2c_gx6605s);
}

static __exit void i2c_gx6605s_driver_exit(void)
{
    unregister_driver(&i2c_gx6605s);
}

driver_initcall(i2c_gx6605s_driver_init);
driver_exitcall(i2c_gx6605s_driver_exit);
