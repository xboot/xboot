/* SPDX-License-Identifier: MIT */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <xboot.h>
#include <interrupt/interrupt.h>
#include <gx6605s-intc.h>

struct gx6605s_device {
    virtual_addr_t base;
    int offset, irqnum;
};

#define irq_source_val(count, magic) \
    (((count) << 24) | ((count) << 16) | ((count) << 8) | ((count) << 0) | (magic))

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

static void irq_gx6605s_enable(struct irqchip_t *chip, int offset)
{
    struct gx6605s_device *gdev = chip->priv;

    if (offset < 32)
        gx6605s_write(gdev, GX6605S_INTC_GATESET0, BIT(offset));
    else
        gx6605s_write(gdev, GX6605S_INTC_GATESET1, BIT(offset - 32));
}

static void irq_gx6605s_disable(struct irqchip_t *chip, int offset)
{
    struct gx6605s_device *gdev = chip->priv;

    if (offset < 8)
        gx6605s_write(gdev, GX6605S_INTC_GATECLR0, BIT(offset));
    else
        gx6605s_write(gdev, GX6605S_INTC_GATECLR1, BIT(offset - 32));
}

static void irq_gx6605s_settype(struct irqchip_t * chip, int offset, enum irq_type_t type)
{

}

static void irq_gx6605s_dispatch(struct irqchip_t *chip)
{
    struct gx6605s_device *gdev = chip->priv;
    unsigned int timeout, nr;
    uint32_t val;

    timeout = 33;
    while ((val = gx6605s_read(gdev, GX6605S_INTC_STAT1)) && --timeout) {
        nr = fls(val) + 32 - 1;
        chip->handler[nr].func(chip->handler[nr].data);
    } if (!timeout) {
        gx6605s_write(gdev, GX6605S_INTC_GATECLR1, val);
        LOG("interrupt 32-63 (%u) timeout, forced mask\r\n", nr);
    }

    timeout = 33;
    while ((val = gx6605s_read(gdev, GX6605S_INTC_STAT0)) && --timeout) {
        nr = fls(val) - 1;
        chip->handler[nr].func(chip->handler[nr].data);
    } if (!timeout) {
        gx6605s_write(gdev, GX6605S_INTC_GATECLR0, val);
        LOG("interrupt 0-31 (%u) timeout, forced mask\r\n", nr);
    }
}

static void irq_gx6605s_hwinit(struct gx6605s_device *gdev, uint32_t magic)
{
    unsigned int count;

    /* disable all interrupt gate */
    gx6605s_write(gdev, GX6605S_INTC_GATE0, 0);
    gx6605s_write(gdev, GX6605S_INTC_GATE1, 0);

    /* disable all mask, because we only use gate */
    gx6605s_write(gdev, GX6605S_INTC_MASK0, 0);
    gx6605s_write(gdev, GX6605S_INTC_MASK1, 0);

    /* mask all fiq, because we don't handle fiq */
    gx6605s_write(gdev, GX6605S_FIQC_MASK0, 0xffffffff);
    gx6605s_write(gdev, GX6605S_FIQC_MASK1, 0xffffffff);

    for (count = 0; count < GX6605S_INTC_SOURCE_NR; count += 4)
        gx6605s_write(gdev, GX6605S_INTC_SOURCE0 + count, irq_source_val(count, magic));
}

static struct device_t *irq_gx6605s_probe(struct driver_t *drv, struct dtnode_t *node)
{
    struct gx6605s_device *gdev;
    struct device_t *dev;
    struct irqchip_t *chip;
    int offset, irqnum;

    offset = dt_read_int(node, "interrupt-base", -1);
    irqnum = dt_read_int(node, "interrupt-count", -1);
    if ((offset < 0) || (irqnum <= 0))
        return NULL;

    gdev = malloc(sizeof(*gdev));
    if (!gdev)
        return NULL;

    chip = malloc(sizeof(*chip));
    if (!chip) {
        free(gdev);
        return NULL;
    }

    gdev->base = phys_to_virt(dt_read_address(node));
    gdev->offset = offset;
    gdev->irqnum = irqnum;

    chip->name = alloc_device_name(dt_read_name(node), -1);
    chip->base = gdev->offset;
    chip->nirq = gdev->irqnum;
    chip->handler = malloc(sizeof(struct irq_handler_t) * irqnum);
    chip->enable = irq_gx6605s_enable;
    chip->disable = irq_gx6605s_disable;
    chip->settype = irq_gx6605s_settype;
    chip->dispatch = irq_gx6605s_dispatch;
    chip->priv = gdev;

    irq_gx6605s_hwinit(gdev, 0x03020100);

    if (!(dev = register_irqchip(chip, drv))) {
        free_device_name(chip->name);
        free(chip->handler);
        free(chip->priv);
        free(chip);
        return NULL;
    }

    return dev;
}

static void irq_gx6605s_remove(struct device_t * dev)
{
    struct irqchip_t * chip = (struct irqchip_t *)dev->priv;

    if (chip) {
        unregister_irqchip(chip);
        free_device_name(chip->name);
        free(chip->handler);
        free(chip->priv);
        free(chip);
    }
}

static void irq_gx6605s_suspend(struct device_t *dev) {}
static void irq_gx6605s_resume(struct device_t *dev) {}

static struct driver_t irq_gx6605s = {
    .name = "irq-gx6605s",
    .probe = irq_gx6605s_probe,
    .remove = irq_gx6605s_remove,
    .suspend = irq_gx6605s_suspend,
    .resume = irq_gx6605s_resume,
};

static __init void irq_gx6605s_driver_init(void)
{
    register_driver(&irq_gx6605s);
}

static __exit void irq_gx6605s_driver_exit(void)
{
    unregister_driver(&irq_gx6605s);
}

driver_initcall(irq_gx6605s_driver_init);
driver_exitcall(irq_gx6605s_driver_exit);
