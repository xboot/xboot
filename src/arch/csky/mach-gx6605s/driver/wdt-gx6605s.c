/* SPDX-License-Identifier: MIT */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <xboot.h>
#include <clk/clk.h>
#include <watchdog/watchdog.h>
#include <gx6605s-wdt.h>

#define GX6605S_WDT_FREQ    1000

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

static void gx6605s_wdt_start(struct gx6605s_device *gdev)
{
    uint32_t val;

    val = gx6605s_read(gdev, GX6605S_WDT_CTRL);
    val |= GX6605S_WDT_CTRL_ENABLE | GX6605S_WDT_CTRL_RESET;
    gx6605s_write(gdev, GX6605S_WDT_CTRL, val);
}

static void gx6605s_wdt_stop(struct gx6605s_device *gdev)
{
    uint32_t val;

    val = gx6605s_read(gdev, GX6605S_WDT_CTRL);
    val &= ~(GX6605S_WDT_CTRL_ENABLE | GX6605S_WDT_CTRL_RESET);
    gx6605s_write(gdev, GX6605S_WDT_CTRL, val);
}

static void gx6605s_wdt_feed(struct gx6605s_device *gdev)
{
    uint32_t val;

    val = gx6605s_read(gdev, GX6605S_WDT_WSR) & 0xffff0000;
    gx6605s_write(gdev, GX6605S_WDT_WSR, val | GX6605S_WDT_WSR_MASK1);
    gx6605s_write(gdev, GX6605S_WDT_WSR, val | GX6605S_WDT_WSR_MASK2);
}

static void wdg_gx6605s_set(struct watchdog_t *watchdog, int timeout)
{
    struct gx6605s_device *gdev = watchdog->priv;
    uint32_t val;

    gx6605s_wdt_feed(gdev);
    gx6605s_wdt_stop(gdev);

    if (timeout) {
        val = gx6605s_read(gdev, GX6605S_WDT_MATCH);
        val &= ~GX6605S_WDT_MATCH_TIMEOUT;
        val |= 0x10000 - (GX6605S_WDT_FREQ * timeout);
        gx6605s_write(gdev, GX6605S_WDT_MATCH, val);
        gx6605s_wdt_start(gdev);
    }
}

static int wdg_gx6605s_get(struct watchdog_t *watchdog)
{
    return 0;
}

static void gx6605s_wdt_hwinit(struct gx6605s_device *gdev)
{
    uint16_t div;

    /* clear wdt regsiter */
    gx6605s_write(gdev, GX6605S_WDT_CTRL, 0);
    gx6605s_write(gdev, GX6605S_WDT_MATCH, 0);
    gx6605s_write(gdev, GX6605S_WDT_COUNT, 0);
    gx6605s_write(gdev, GX6605S_WDT_WSR, 0);

    /* setup wdt clock */
    div = osc_freq / GX6605S_WDT_FREQ - 1;
    gx6605s_write(gdev, GX6605S_WDT_MATCH, div << 16);
}

static struct device_t *wdg_gx6605s_probe(struct driver_t *drv, struct dtnode_t *node)
{
    struct gx6605s_device *gdev;
    struct device_t *dev;
    struct watchdog_t *wdg;

    gdev = malloc(sizeof(*gdev));
    if (!gdev)
        return NULL;

    wdg = malloc(sizeof(*wdg));
    if (!wdg) {
        free(gdev);
        return NULL;
    }

    gdev->base = phys_to_virt(dt_read_address(node));

    wdg->name = alloc_device_name(dt_read_name(node), -1);
    wdg->set = wdg_gx6605s_set;
    wdg->get = wdg_gx6605s_get;
    wdg->priv = gdev;

    gx6605s_wdt_hwinit(gdev);

    if (!(dev = register_watchdog(wdg, drv))) {
        gx6605s_wdt_stop(gdev);
        free_device_name(wdg->name);
        free(wdg->priv);
        free(wdg);
        return NULL;
    }

    return dev;
}

static void wdg_gx6605s_remove(struct device_t *dev)
{
    struct watchdog_t *wdg = dev->priv;
    struct gx6605s_device *gdev = wdg->priv;

    if (wdg) {
        unregister_watchdog(wdg);
        gx6605s_wdt_stop(gdev);
        free_device_name(wdg->name);
        free(wdg->priv);
        free(wdg);
    }
}

static void wdg_gx6605s_suspend(struct device_t *dev) {}
static void wdg_gx6605s_resume(struct device_t *dev) {}

static struct driver_t wdg_gx6605s = {
    .name		= "wdg-gx6605s",
    .probe		= wdg_gx6605s_probe,
    .remove		= wdg_gx6605s_remove,
    .suspend	= wdg_gx6605s_suspend,
    .resume		= wdg_gx6605s_resume,
};

static __init void wdg_gx6605s_driver_init(void)
{
    register_driver(&wdg_gx6605s);
}

static __exit void wdg_gx6605s_driver_exit(void)
{
    unregister_driver(&wdg_gx6605s);
}

driver_initcall(wdg_gx6605s_driver_init);
driver_exitcall(wdg_gx6605s_driver_exit);
