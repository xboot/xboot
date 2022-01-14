/* SPDX-License-Identifier: MIT */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <xboot.h>
#include <clk/clk.h>
#include <gx6605s-gctl.h>

struct gx6605s_device {
    virtual_addr_t base;
    char *parent;
    int channel;
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

static void clk_gx6605s_divider_set_parent(struct clk_t *clk, const char *pname) {}
static const char *clk_gx6605s_divider_get_parent(struct clk_t *clk)
{
    struct gx6605s_device *gdev = clk->priv;
    return gdev->parent;
}

static void clk_gx6605s_divider_set_enable(struct clk_t *clk, bool_t enable) {}
static bool_t clk_gx6605s_divider_get_enable(struct clk_t *clk)
{
    return TRUE;
}

static void clk_gx6605s_divider_set_rate(struct clk_t *clk, uint64_t prate, uint64_t rate)
{
    struct gx6605s_device *gdev = clk->priv;
    virtual_addr_t reg;
    uint64_t clkdiv;

    if (!prate)
        return;

    /**
     * div = freq * 2^30 / fin
     * freq = fin * div / 2^30
     */

    clkdiv = rate / prate * 0x40000000;
    reg = GX6605S_DTO0_CONFIG + (4 * gdev->channel);

    gx6605s_write(gdev, reg, (1 << 31));
    gx6605s_write(gdev, reg, (0 << 31));
    gx6605s_write(gdev, reg, (1 << 31));
    gx6605s_write(gdev, reg, (1 << 31) | clkdiv);
    gx6605s_write(gdev, reg, (1 << 31) | (1 << 30)| clkdiv);
    gx6605s_write(gdev, reg, (1 << 31) | (1 << 30)| clkdiv);
    gx6605s_write(gdev, reg, (1 << 31) | clkdiv);
}

static uint64_t clk_gx6605s_divider_get_rate(struct clk_t *clk, uint64_t prate)
{
    struct gx6605s_device *gdev = clk->priv;
    virtual_addr_t reg;
    uint32_t val;

    /**
     * div = freq * 2^30 / fin
     * freq = fin * div / 2^30
     */

    reg = GX6605S_DTO0_CONFIG + (4 * gdev->channel);
    val = gx6605s_read(gdev, reg);

    return prate * (val & 0x3fffffff) / 0x40000000;
}

static struct device_t *clk_gx6605s_divider_probe(struct driver_t *drv, struct dtnode_t *node)
{
    struct gx6605s_device *gdev;
    struct device_t *dev;
    struct clk_t *clk;
    struct dtnode_t obj;
    char *parent, *name;
    int channel;

    name = dt_read_string(node, "name", NULL);
    parent = dt_read_string(node, "parent", NULL);
    channel = dt_read_int(node, "channel", -1);
    if (!name || !parent || channel < 0 || channel > 15)
        return NULL;

    if (!search_clk(parent) || search_clk(name))
        return NULL;

    gdev = malloc(sizeof(*gdev));
    if (!gdev)
        return NULL;

    clk = malloc(sizeof(struct clk_t));
    if (!clk) {
        free(gdev);
        return NULL;
    }

    gdev->base = phys_to_virt(dt_read_address(node));
    gdev->parent = strdup(parent);
    gdev->channel = channel;

    clk->name = strdup(name);
    clk->count = 0;
    clk->set_parent = clk_gx6605s_divider_set_parent;
    clk->get_parent = clk_gx6605s_divider_get_parent;
    clk->set_enable = clk_gx6605s_divider_set_enable;
    clk->get_enable = clk_gx6605s_divider_get_enable;
    clk->set_rate = clk_gx6605s_divider_set_rate;
    clk->get_rate = clk_gx6605s_divider_get_rate;
    clk->priv = gdev;

    if (!(dev = register_clk(clk, drv))) {
        free(gdev->parent);
        free(clk->name);
        free(clk->priv);
        free(clk);
        return NULL;
    }

    if (dt_read_object(node, "default", &obj)) {
        char *p, *c = clk->name;
        u64_t r;
        int e;

        if ((p = dt_read_string(&obj, "parent", NULL)) && search_clk(p))
            clk_set_parent(c, p);
        if ((r = (u64_t)dt_read_long(&obj, "rate", 0)) > 0)
            clk_set_rate(c, r);
        if ((e = dt_read_bool(&obj, "enable", -1)) != -1) {
            if (e > 0)
                clk_enable(c);
            else
                clk_disable(c);
        }
    }

    return dev;
}

static void clk_gx6605s_divider_remove(struct device_t *dev)
{
    struct clk_t *clk = dev->priv;
    struct gx6605s_device *pdat = clk->priv;

    if (clk) {
        unregister_clk(clk);
        free(pdat->parent);
        free(clk->name);
        free(clk->priv);
        free(clk);
    }
}

static void clk_gx6605s_divider_suspend(struct device_t *dev) {}
static void clk_gx6605s_divider_resume(struct device_t *dev) {}

static struct driver_t clk_gx6605s_divider = {
    .name = "clk-gx6605s-divider",
    .probe = clk_gx6605s_divider_probe,
    .remove = clk_gx6605s_divider_remove,
    .suspend = clk_gx6605s_divider_suspend,
    .resume = clk_gx6605s_divider_resume,
};

static __init void clk_gx6605s_pll_driver_init(void)
{
    register_driver(&clk_gx6605s_divider);
}

static __exit void clk_gx6605s_pll_driver_exit(void)
{
    unregister_driver(&clk_gx6605s_divider);
}

driver_initcall(clk_gx6605s_pll_driver_init);
driver_exitcall(clk_gx6605s_pll_driver_exit);
