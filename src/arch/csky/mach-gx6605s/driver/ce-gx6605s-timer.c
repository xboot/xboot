/* SPDX-License-Identifier: MIT */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <xboot.h>
#include <clk/clk.h>
#include <interrupt/interrupt.h>
#include <clockevent/clockevent.h>
#include <gx6605s-tim.h>

struct gx6605s_device {
    virtual_addr_t base;
    char *clk;
    int irq;
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

static void ce_gx6605s_timer_interrupt(void *data)
{
    struct clockevent_t *ce = data;
    struct gx6605s_device *gdev = ce->priv;
    uint32_t val;

    val = gx6605s_read(gdev, GX6605S_TIM_STATUS);
    val |= GX6605S_TIM_STATUS_CLR;
    gx6605s_write(gdev, GX6605S_TIM_STATUS, val);

    val = gx6605s_read(gdev, GX6605S_TIM_CONTRL);
    val &= ~GX6605S_TIM_CONTRL_START;
    gx6605s_write(gdev, GX6605S_TIM_CONTRL, val);

    ce->handler(ce, ce->data);
}

static bool_t ce_gx6605s_timer_next(struct clockevent_t *ce, uint64_t evt)
{
    struct gx6605s_device *gdev = ce->priv;
    uint32_t val;

    val = UINT32_MAX - evt + 1;
    gx6605s_write(gdev, GX6605S_TIM_RELOAD, val);
    gx6605s_write(gdev, GX6605S_TIM_VALUE, val);

    val = gx6605s_read(gdev, GX6605S_TIM_CONTRL);
    val |= GX6605S_TIM_CONTRL_START;
    gx6605s_write(gdev, GX6605S_TIM_CONTRL, val);

    return TRUE;
}

static inline void ce_gx6605s_timer_hwinit(struct gx6605s_device *gdev)
{
    /* reset the timer at first */
    gx6605s_write(gdev, GX6605S_TIM_CONTRL, GX6605S_TIM_CONTRL_RST);
    gx6605s_write(gdev, GX6605S_TIM_CONTRL, 0);

    gx6605s_write(gdev, GX6605S_TIM_CLKDIV, 0);
    gx6605s_write(gdev, GX6605S_TIM_RELOAD, 0);
    gx6605s_write(gdev, GX6605S_TIM_VALUE, 0);

    /* start the timer */
    gx6605s_write(gdev, GX6605S_TIM_CONFIG, GX6605S_TIM_CONFIG_IRQ_EN | GX6605S_TIM_CONFIG_EN);
    gx6605s_write(gdev, GX6605S_TIM_CONTRL, GX6605S_TIM_CONTRL_START);
}

static struct device_t *ce_gx6605s_timer_probe(struct driver_t *drv, struct dtnode_t *node)
{
    struct gx6605s_device *gdev;
    struct device_t *dev;
    struct clockevent_t *ce;
    char *clk;
    int irq;

    clk = dt_read_string(node, "clock-name", NULL);
    irq = dt_read_int(node, "interrupt", -1);
    if (!search_clk(clk) || !irq_is_valid(irq))
        return NULL;

    gdev = malloc(sizeof(*gdev));
    if (!gdev)
        return NULL;

    ce = malloc(sizeof(*ce));
    if (!ce) {
        free(gdev);
        return NULL;
    }

    gdev->base = phys_to_virt(dt_read_address(node));
    gdev->clk = strdup(clk);
    gdev->irq = irq;

    clockevent_calc_mult_shift(ce, clk_get_rate(gdev->clk), 10);
    ce->name = alloc_device_name(dt_read_name(node), -1);
    ce->min_delta_ns = clockevent_delta2ns(ce, 0x1);
    ce->max_delta_ns = clockevent_delta2ns(ce, 0xffffffff);
    ce->next = ce_gx6605s_timer_next;
    ce->priv = gdev;

    if (!request_irq(gdev->irq, ce_gx6605s_timer_interrupt, IRQ_TYPE_NONE, ce)) {
        clk_disable(gdev->clk);
        free(gdev->clk);
        free(ce->priv);
        free(ce);
        return NULL;
    }

    ce_gx6605s_timer_hwinit(gdev);

    if (!(dev = register_clockevent(ce, drv))) {
        clk_disable(gdev->clk);
        free_irq(gdev->irq);
        free(gdev->clk);
        free_device_name(ce->name);
        free(ce->priv);
        free(ce);
        return NULL;
    }

    return dev;
}

static void ce_gx6605s_timer_remove(struct device_t *dev)
{
    struct clockevent_t *ce = dev->priv;
    struct gx6605s_device *gdev = ce->priv;

    if (ce) {
        unregister_clockevent(ce);
        free_irq(gdev->irq);
        free(gdev->clk);
        free_device_name(ce->name);
        free(ce->priv);
        free(ce);
    }
}

static void ce_gx6605s_timer_suspend(struct device_t *dev) {}
static void ce_gx6605s_timer_resume(struct device_t *dev) {}

static struct driver_t ce_gx6605s_timer = {
    .name = "ce-gx6605s-timer",
    .probe = ce_gx6605s_timer_probe,
    .remove = ce_gx6605s_timer_remove,
    .suspend = ce_gx6605s_timer_suspend,
    .resume = ce_gx6605s_timer_resume,
};

static __init void ce_gx6605s_timer_driver_init(void)
{
    register_driver(&ce_gx6605s_timer);
}

static __exit void ce_gx6605s_timer_driver_exit(void)
{
    unregister_driver(&ce_gx6605s_timer);
}

driver_initcall(ce_gx6605s_timer_driver_init);
driver_exitcall(ce_gx6605s_timer_driver_exit);
