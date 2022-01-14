/* SPDX-License-Identifier: MIT */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <xboot.h>
#include <clk/clk.h>
#include <clocksource/clocksource.h>
#include <gx6605s-tim.h>

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

static uint64_t cs_gx6605s_timer_read(struct clocksource_t *cs)
{
    struct gx6605s_device *gdev = cs->priv;
    return (uint64_t)gx6605s_read(gdev, GX6605S_TIM_VALUE);
}

static inline void cs_gx6605s_timer_hwinit(struct gx6605s_device *gdev)
{
    /* reset the timer at first */
    gx6605s_write(gdev, GX6605S_TIM_CONTRL, GX6605S_TIM_CONTRL_RST);
    gx6605s_write(gdev, GX6605S_TIM_CONTRL, 0);

    /* setup timer clock */
    gx6605s_write(gdev, GX6605S_TIM_CLKDIV, 0);
    gx6605s_write(gdev, GX6605S_TIM_RELOAD, 0);
    gx6605s_write(gdev, GX6605S_TIM_VALUE, 0);

    /* start the timer */
    gx6605s_write(gdev, GX6605S_TIM_CONFIG, GX6605S_TIM_CONFIG_EN);
    gx6605s_write(gdev, GX6605S_TIM_CONTRL, GX6605S_TIM_CONTRL_START);
}

static struct device_t *cs_gx6605s_timer_probe(struct driver_t *drv, struct dtnode_t *node)
{
    struct gx6605s_device *gdev;
    struct device_t *dev;
    struct clocksource_t *cs;
    char *clk;

    clk = dt_read_string(node, "clock-name", NULL);
    if (!search_clk(clk))
        return NULL;

    gdev = malloc(sizeof(*gdev));
    if (!gdev)
        return NULL;

    cs = malloc(sizeof(*cs));
    if (!cs) {
        free(gdev);
        return NULL;
    }

    gdev->base = phys_to_virt(dt_read_address(node));
    gdev->clk = strdup(clk);

    clocksource_calc_mult_shift(&cs->mult, &cs->shift, clk_get_rate(gdev->clk), 1000000000ULL, 10);
    cs->name = alloc_device_name(dt_read_name(node), -1);
    cs->mask = CLOCKSOURCE_MASK(32);
    cs->read = cs_gx6605s_timer_read;
    cs->priv = gdev;

    cs_gx6605s_timer_hwinit(gdev);

    if (!(dev = register_clocksource(cs, drv))) {
        free(gdev->clk);
        free_device_name(cs->name);
        free(cs->priv);
        free(cs);
        return NULL;
    }

    return dev;
}

static void cs_gx6605s_timer_remove(struct device_t *dev)
{
    struct clocksource_t *cs = dev->priv;
    struct gx6605s_device *gdev = cs->priv;

    if (cs) {
        unregister_clocksource(cs);
        free(gdev->clk);
        free_device_name(cs->name);
        free(cs->priv);
        free(cs);
    }
}

static void cs_gx6605s_timer_suspend(struct device_t *dev) {}
static void cs_gx6605s_timer_resume(struct device_t *dev) {}

static struct driver_t cs_gx6605s_timer = {
    .name = "cs-gx6605s-timer",
    .probe = cs_gx6605s_timer_probe,
    .remove = cs_gx6605s_timer_remove,
    .suspend = cs_gx6605s_timer_suspend,
    .resume = cs_gx6605s_timer_resume,
};

static __init void cs_gx6605s_timer_driver_init(void)
{
    register_driver(&cs_gx6605s_timer);
}

static __exit void cs_gx6605s_timer_driver_exit(void)
{
    unregister_driver(&cs_gx6605s_timer);
}

driver_initcall(cs_gx6605s_timer_driver_init);
driver_exitcall(cs_gx6605s_timer_driver_exit);
