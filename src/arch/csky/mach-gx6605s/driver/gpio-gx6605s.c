/* SPDX-License-Identifier: MIT */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <xboot.h>
#include <gpio/gpio.h>
#include <gx6605s-gctl.h>
#include <gx6605s-gpio.h>

struct gx6605s_device {
    virtual_addr_t base;
    uint64_t rate;
    char *parent;
    int channel;
};

static const uint8_t gx6605s_pinmux[21][3] = {
    {  0,  44, 255}, {  1,  45, 255}, {  2,  46, 255},
    {  3,  47, 255}, {  4,  48, 255}, {  5,  32,  64},
    {  6,  33,  65}, {  7,  34,  66}, {  8,  35,  67},
    {  9,  36,  68}, { 10,  37,  69}, { 11,  38, 255},
    { 12,  39, 255}, { 13,  40, 255}, { 14, 255, 255},
    { 15, 255, 255}, { 16,  41, 255}, { 17,  42, 255},
    { 18, 255, 255}, { 19, 255, 255}, { 20,  43, 255},
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

static void gpio_gx6605s_set_cfg(struct gpiochip_t *chip, int offset, int cfg)
{
    uint32_t val;
    int count;

    if (offset >= chip->ngpio)
        return;

    for (count = 0; count < 3; ++count) {
        virtual_addr_t reg;
        unsigned int shift;

        if ((shift = gx6605s_pinmux[offset][count]) == 255)
            break;

        reg = GX6605S_PINMUX_PORTA + ((shift / 32) * 4);
        val = read32(GCTL_BASE + reg);
        val &= ~(0x01 << (shift % 32));
        val |= ((cfg >> count) & 0x01) << (shift % 32);
        write32(GCTL_BASE + reg, val);
    }
}

static int gpio_gx6605s_get_cfg(struct gpiochip_t * chip, int offset)
{
    uint32_t val;
    int count, fun = 0;

    for (count = 0; count < 3; ++count) {
        virtual_addr_t reg;
        unsigned int shift;

        if ((shift = gx6605s_pinmux[offset][count]) == 255)
            break;

        reg = GX6605S_PINMUX_PORTA + ((shift / 32) * 4);
        val = read32(GCTL_BASE + reg);
        val = (val >> (shift % 32)) & 0x01;
        fun |= val << count;
    }

    return fun;
}

static void gpio_gx6605s_set_pull(struct gpiochip_t *chip, int offset, enum gpio_pull_t pull)
{
}

static enum gpio_pull_t gpio_gx6605s_get_pull(struct gpiochip_t *chip, int offset)
{
    return GPIO_PULL_NONE;
}

static void gpio_gx6605s_set_drv(struct gpiochip_t *chip, int offset, enum gpio_drv_t drv)
{
}

static enum gpio_drv_t gpio_gx6605s_get_drv(struct gpiochip_t *chip, int offset)
{
    return GPIO_DRV_WEAK;
}

static void gpio_gx6605s_set_rate(struct gpiochip_t *chip, int offset, enum gpio_rate_t rate)
{
}

static enum gpio_rate_t gpio_gx6605s_get_rate(struct gpiochip_t *chip, int offset)
{
    return GPIO_RATE_SLOW;
}

static void gpio_gx6605s_set_dir(struct gpiochip_t *chip, int offset, enum gpio_direction_t dir)
{
    struct gx6605s_device *gdev = chip->priv;
    uint32_t val;

    if (offset >= chip->ngpio)
        return;

    val = gx6605s_read(gdev, GX6605S_GPIO_DIROUT);
    val &= ~(0x01 << offset);
    val |= (dir & 0x01) << offset;
    gx6605s_write(gdev, GX6605S_GPIO_DIROUT, val);
}

static enum gpio_direction_t gpio_gx6605s_get_dir(struct gpiochip_t *chip, int offset)
{
    struct gx6605s_device *gdev = chip->priv;
    uint32_t val;

    if (offset >= chip->ngpio)
        return GPIO_DIRECTION_INPUT;

    val = gx6605s_read(gdev, GX6605S_GPIO_DIROUT);
    val = (val >> offset) & 0x01;
    return val ? GPIO_DIRECTION_OUTPUT : GPIO_DIRECTION_INPUT;
}

static void gpio_gx6605s_set_value(struct gpiochip_t *chip, int offset, int value)
{
    struct gx6605s_device *gdev = chip->priv;
    uint32_t val;

    if (offset >= chip->ngpio)
        return;

    val = gx6605s_read(gdev, GX6605S_GPIO_DAT);
    val &= ~(0x01 << offset);
    val |= (value & 0x01) << offset;
    gx6605s_write(gdev, GX6605S_GPIO_DAT, val);
}

static int gpio_gx6605s_get_value(struct gpiochip_t *chip, int offset)
{
    struct gx6605s_device *gdev = chip->priv;
    uint32_t val;

    if (offset >= chip->ngpio)
        return 0;

    val = gx6605s_read(gdev, GX6605S_GPIO_DAT);
    val = (val >> offset) & 0x01;
    return val;
}

static int gpio_gx6605s_to_irq(struct gpiochip_t *chip, int offset)
{
    return 0;
}

static struct device_t *gpio_gx6605s_probe(struct driver_t *drv, struct dtnode_t *node)
{
    struct gx6605s_device *gdev;
    struct device_t *dev;
    struct gpiochip_t *chip;
    int offset, ngpio;

    offset = dt_read_int(node, "gpio-base", -1);
    ngpio = dt_read_int(node, "gpio-count", -1);
    if ((offset < 0) || (ngpio <= 0))
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

    chip->name = alloc_device_name(dt_read_name(node), -1);
    chip->base = offset;
    chip->ngpio = ngpio;
    chip->set_cfg = gpio_gx6605s_set_cfg;
    chip->get_cfg = gpio_gx6605s_get_cfg;
    chip->set_pull = gpio_gx6605s_set_pull;
    chip->get_pull = gpio_gx6605s_get_pull;
    chip->set_drv = gpio_gx6605s_set_drv;
    chip->get_drv = gpio_gx6605s_get_drv;
    chip->set_rate = gpio_gx6605s_set_rate;
    chip->get_rate = gpio_gx6605s_get_rate;
    chip->set_dir = gpio_gx6605s_set_dir;
    chip->get_dir = gpio_gx6605s_get_dir;
    chip->set_value = gpio_gx6605s_set_value;
    chip->get_value = gpio_gx6605s_get_value;
    chip->to_irq = gpio_gx6605s_to_irq;
    chip->priv = gdev;

    if (!(dev = register_gpiochip(chip, drv))) {
        free_device_name(chip->name);
        free(chip->priv);
        free(chip);
        return NULL;
    }

    return dev;
}

static void gpio_gx6605s_remove(struct device_t *dev)
{
    struct gpiochip_t *chip = dev->priv;

    if (chip) {
        unregister_gpiochip(chip);
        free_device_name(chip->name);
        free(chip->priv);
        free(chip);
    }
}

static void gpio_gx6605s_suspend(struct device_t *dev) {}
static void gpio_gx6605s_resume(struct device_t *dev) {}

static struct driver_t gpio_gx6605s_pll = {
    .name = "gpio-gx6605s",
    .probe = gpio_gx6605s_probe,
    .remove = gpio_gx6605s_remove,
    .suspend = gpio_gx6605s_suspend,
    .resume = gpio_gx6605s_resume,
};

static __init void gpio_gx6605s_driver_init(void)
{
    register_driver(&gpio_gx6605s_pll);
}

static __exit void gpio_gx6605s_driver_exit(void)
{
    unregister_driver(&gpio_gx6605s_pll);
}

driver_initcall(gpio_gx6605s_driver_init);
driver_exitcall(gpio_gx6605s_driver_exit);
