/* SPDX-License-Identifier: MIT */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <xboot.h>
#include <clk/clk.h>
#include <rtc/rtc.h>
#include <gx6605s-rtc.h>

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

static bool_t rtc_gx6605s_settime(struct rtc_t *rtc, struct rtc_time_t *time)
{
    struct gx6605s_device *gdev = rtc->priv;

    gx6605s_write(gdev, GX6605S_RTC_SEC, bin2bcd(time->second));
    gx6605s_write(gdev, GX6605S_RTC_MIN, bin2bcd(time->minute));
    gx6605s_write(gdev, GX6605S_RTC_HOUR, bin2bcd(time->hour));
    gx6605s_write(gdev, GX6605S_RTC_DAY, bin2bcd(time->day));
    gx6605s_write(gdev, GX6605S_RTC_WEEK, bin2bcd(time->week));
    gx6605s_write(gdev, GX6605S_RTC_MONTH, bin2bcd(time->month));
    gx6605s_write(gdev, GX6605S_RTC_YEAR, bin2bcd(time->year));

    return TRUE;
}

static bool_t rtc_gx6605s_gettime(struct rtc_t *rtc, struct rtc_time_t *time)
{
    struct gx6605s_device *gdev = rtc->priv;

    time->second = bcd2bin(gx6605s_read(gdev, GX6605S_RTC_SEC));
    time->minute = bcd2bin(gx6605s_read(gdev, GX6605S_RTC_MIN));
    time->hour = bcd2bin(gx6605s_read(gdev, GX6605S_RTC_HOUR));
    time->day = bcd2bin(gx6605s_read(gdev, GX6605S_RTC_DAY));
    time->week = bcd2bin(gx6605s_read(gdev, GX6605S_RTC_WEEK));
    time->month = bcd2bin(gx6605s_read(gdev, GX6605S_RTC_MONTH));
    time->year = bcd2bin(gx6605s_read(gdev, GX6605S_RTC_YEAR));

    return TRUE;
}

static void rtc_gx6605s_hwinit(struct rtc_t *rtc)
{
    struct gx6605s_device *gdev = rtc->priv;
    struct rtc_time_t time = {};
    uint64_t freq;
    uint32_t val;

    freq = clk_get_rate(gdev->clk);
    gx6605s_write(gdev, GX6605S_RTC_CLK, GX6605S_RTC_LATCH(freq));

    val = gx6605s_read(gdev, GX6605S_RTC_CTL);
    val &= ~GX6605S_RTC_CTL_TIME;
    gx6605s_write(gdev, GX6605S_RTC_CTL, val);

    rtc_gx6605s_settime(rtc, &time);

    val |= GX6605S_RTC_CTL_TIME;
    gx6605s_write(gdev, GX6605S_RTC_CTL, val);
}

static struct device_t *rtc_gx6605s_probe(struct driver_t *drv, struct dtnode_t *node)
{
    struct gx6605s_device *gdev;
    struct device_t *dev;
    struct rtc_t *rtc;
    char *clk;

    clk = dt_read_string(node, "clock-name", NULL);
    if (!search_clk(clk))
        return NULL;

    gdev = malloc(sizeof(*gdev));
    if (!gdev)
        return NULL;

    rtc = malloc(sizeof(*rtc));
    if (!rtc) {
        free(gdev);
        return NULL;
    }

    gdev->base = phys_to_virt(dt_read_address(node));
    gdev->clk = clk;

    rtc->name = alloc_device_name(dt_read_name(node), -1);
    rtc->settime = rtc_gx6605s_settime;
    rtc->gettime = rtc_gx6605s_gettime;
    rtc->priv = gdev;

    rtc_gx6605s_hwinit(rtc);

    if (!(dev = register_rtc(rtc, drv))) {
        clk_disable(gdev->clk);
        free(gdev->clk);
        free_device_name(rtc->name);
        free(rtc->priv);
        free(rtc);
        return NULL;
    }

    return dev;
}

static void rtc_gx6605s_remove(struct device_t * dev)
{
    struct rtc_t *rtc = dev->priv;
    struct gx6605s_device *gdev = rtc->priv;

    if (rtc) {
        unregister_rtc(rtc);
        clk_disable(gdev->clk);
        free(gdev->clk);
        free_device_name(rtc->name);
        free(rtc->priv);
        free(rtc);
    }
}

static void rtc_gx6605s_suspend(struct device_t *dev) {}
static void rtc_gx6605s_resume(struct device_t *dev) {}

static struct driver_t rtc_gx6605s = {
    .name = "rtc-gx6605s",
    .probe = rtc_gx6605s_probe,
    .remove = rtc_gx6605s_remove,
    .suspend = rtc_gx6605s_suspend,
    .resume = rtc_gx6605s_resume,
};

static __init void rtc_gx6605s_driver_init(void)
{
    register_driver(&rtc_gx6605s);
}

static __exit void rtc_gx6605s_driver_exit(void)
{
    unregister_driver(&rtc_gx6605s);
}

driver_initcall(rtc_gx6605s_driver_init);
driver_exitcall(rtc_gx6605s_driver_exit);
