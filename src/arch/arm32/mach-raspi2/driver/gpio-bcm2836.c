/*
 * driver/gpio-bcm2836.c
 *
 * Copyright(c) 2007-2019 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <xboot.h>
#include <gpio/gpio.h>

#define GPIO_FSEL(x)	(0x00 + (x) * 4)
#define GPIO_SET(x)		(0x1c + (x) * 4)
#define GPIO_CLR(x)		(0x28 + (x) * 4)
#define GPIO_LEV(x)		(0x34 + (x) * 4)
#define GPIO_EDS(x)		(0x40 + (x) * 4)
#define GPIO_REN(x)		(0x4c + (x) * 4)
#define GPIO_FEN(x)		(0x58 + (x) * 4)
#define GPIO_HEN(x)		(0x64 + (x) * 4)
#define GPIO_LEN(x)		(0x70 + (x) * 4)
#define GPIO_ARE(x)		(0x7c + (x) * 4)
#define GPIO_AFE(x)		(0x88 + (x) * 4)
#define GPIO_UD(x)		(0x94 + (x) * 4)
#define GPIO_UDCLK(x)	(0x98 + (x) * 4)

enum {
	ALT_FUNC0 = 0x4,
	ALT_FUNC1 = 0x5,
	ALT_FUNC2 = 0x6,
	ALT_FUNC3 = 0x7,
	ALT_FUNC4 = 0x3,
	ALT_FUNC5 = 0x2,
	ALT_FUNC6_GPIO_OUTPUT = 0x1,
	ALT_FUNC7_GPIO_INPUT = 0x0,
};

struct gpio_bcm2836_pdata_t
{
	virtual_addr_t virt;
	int base;
	int ngpio;
	int oirq;
};

static void gpio_bcm2836_set_cfg(struct gpiochip_t * chip, int offset, int cfg)
{
	struct gpio_bcm2836_pdata_t * pdat = (struct gpio_bcm2836_pdata_t *)chip->priv;
	int bank = offset / 10;
	int field = (offset - 10 * bank) * 3;
	u32_t val;

	if(offset >= chip->ngpio)
		return;

	switch(cfg & 0x7)
	{
	case 0: cfg = ALT_FUNC0; break;
	case 1: cfg = ALT_FUNC1; break;
	case 2: cfg = ALT_FUNC2; break;
	case 3: cfg = ALT_FUNC3; break;
	case 4: cfg = ALT_FUNC4; break;
	case 5: cfg = ALT_FUNC5; break;
	case 6: cfg = ALT_FUNC6_GPIO_OUTPUT; break;
	case 7: cfg = ALT_FUNC7_GPIO_INPUT; break;
	default: break;
	}

	val = read32(pdat->virt + GPIO_FSEL(bank));
	val &= ~(0x7 << field);
	val |= cfg << field;
	write32(pdat->virt + GPIO_FSEL(bank), val);
}

static int gpio_bcm2836_get_cfg(struct gpiochip_t * chip, int offset)
{
	struct gpio_bcm2836_pdata_t * pdat = (struct gpio_bcm2836_pdata_t *)chip->priv;
	int bank = offset / 10;
	int field = (offset - 10 * bank) * 3;
	int cfg;
	u32_t val;

	val = read32(pdat->virt + GPIO_FSEL(bank));
	switch((val >> field) & 0x7)
	{
	case ALT_FUNC0: cfg = 0; break;
	case ALT_FUNC1: cfg = 1; break;
	case ALT_FUNC2: cfg = 2; break;
	case ALT_FUNC3: cfg = 3; break;
	case ALT_FUNC4: cfg = 4; break;
	case ALT_FUNC5: cfg = 5; break;
	case ALT_FUNC6_GPIO_OUTPUT: cfg = 6; break;
	case ALT_FUNC7_GPIO_INPUT: cfg = 7; break;
	default: break;
	}
	return cfg;
}

static void gpio_bcm2836_set_pull(struct gpiochip_t * chip, int offset, enum gpio_pull_t pull)
{
	struct gpio_bcm2836_pdata_t * pdat = (struct gpio_bcm2836_pdata_t *)chip->priv;
	int bank = offset / 32;
	int field = (offset - 32 * bank);

	if(offset >= chip->ngpio)
		return;

	switch(pull)
	{
	case GPIO_PULL_UP:
		write32(pdat->virt + GPIO_UD(0), 2);
		break;

	case GPIO_PULL_DOWN:
		write32(pdat->virt + GPIO_UD(0), 1);
		break;

	case GPIO_PULL_NONE:
		write32(pdat->virt + GPIO_UD(0), 0);
		break;

	default:
		return;
	}

	udelay(5);
	write32(pdat->virt + GPIO_UDCLK(bank), 1 << field);
	udelay(5);
	write32(pdat->virt + GPIO_UD(0), 0);
	write32(pdat->virt + GPIO_UDCLK(bank), 0 << field);
}

static enum gpio_pull_t gpio_bcm2836_get_pull(struct gpiochip_t * chip, int offset)
{
	if(offset >= chip->ngpio)
		return GPIO_PULL_NONE;
	return GPIO_PULL_NONE;
}

static void gpio_bcm2836_set_drv(struct gpiochip_t * chip, int offset, enum gpio_drv_t drv)
{
}

static enum gpio_drv_t gpio_bcm2836_get_drv(struct gpiochip_t * chip, int offset)
{
	return GPIO_DRV_WEAK;
}

static void gpio_bcm2836_set_rate(struct gpiochip_t * chip, int offset, enum gpio_rate_t rate)
{
}

static enum gpio_rate_t gpio_bcm2836_get_rate(struct gpiochip_t * chip, int offset)
{
	return GPIO_RATE_SLOW;
}

static void gpio_bcm2836_set_dir(struct gpiochip_t * chip, int offset, enum gpio_direction_t dir)
{
	if(offset >= chip->ngpio)
		return;

	switch(dir)
	{
	case GPIO_DIRECTION_INPUT:
		gpio_bcm2836_set_cfg(chip, offset, 7);
		break;

	case GPIO_DIRECTION_OUTPUT:
		gpio_bcm2836_set_cfg(chip, offset, 6);
		break;

	default:
		break;
	}
}

static enum gpio_direction_t gpio_bcm2836_get_dir(struct gpiochip_t * chip, int offset)
{
	if(offset >= chip->ngpio)
		return GPIO_DIRECTION_INPUT;

	switch(gpio_bcm2836_get_cfg(chip, offset))
	{
	case 6:
		return GPIO_DIRECTION_OUTPUT;
	case 7:
		return GPIO_DIRECTION_INPUT;
	default:
		break;
	}
	return GPIO_DIRECTION_INPUT;
}

static void gpio_bcm2836_set_value(struct gpiochip_t * chip, int offset, int value)
{
	struct gpio_bcm2836_pdata_t * pdat = (struct gpio_bcm2836_pdata_t *)chip->priv;
	int bank = offset / 32;
	int field = (offset - 32 * bank);

	if(offset >= chip->ngpio)
		return;

	if(value)
		write32(pdat->virt + GPIO_SET(bank), 1 << field);
	else
		write32(pdat->virt + GPIO_CLR(bank), 1 << field);
}

static int gpio_bcm2836_get_value(struct gpiochip_t * chip, int offset)
{
	struct gpio_bcm2836_pdata_t * pdat = (struct gpio_bcm2836_pdata_t *)chip->priv;
	int bank = offset / 32;
	int field = (offset - 32 * bank);
	u32_t lev;

	if(offset >= chip->ngpio)
		return 0;

	lev = read32(pdat->virt + GPIO_LEV(bank));
	return (lev & (1 << field)) ? 1 : 0;
}

static int gpio_bcm2836_to_irq(struct gpiochip_t * chip, int offset)
{
	struct gpio_bcm2836_pdata_t * pdat = (struct gpio_bcm2836_pdata_t *)chip->priv;

	if((offset >= chip->ngpio) || (pdat->oirq < 0))
		return -1;
	return pdat->oirq + offset;
}

static struct device_t * gpio_bcm2836_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct gpio_bcm2836_pdata_t * pdat;
	struct gpiochip_t * chip;
	struct device_t * dev;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	int base = dt_read_int(n, "gpio-base", -1);
	int ngpio = dt_read_int(n, "gpio-count", -1);

	if((base < 0) || (ngpio <= 0))
		return NULL;

	pdat = malloc(sizeof(struct gpio_bcm2836_pdata_t));
	if(!pdat)
		return NULL;

	chip = malloc(sizeof(struct gpiochip_t));
	if(!chip)
	{
		free(pdat);
		return NULL;
	}

	pdat->virt = virt;
	pdat->base = base;
	pdat->ngpio = ngpio;
	pdat->oirq = dt_read_int(n, "interrupt-offset", -1);

	chip->name = alloc_device_name(dt_read_name(n), -1);
	chip->base = pdat->base;
	chip->ngpio = pdat->ngpio;
	chip->set_cfg = gpio_bcm2836_set_cfg;
	chip->get_cfg = gpio_bcm2836_get_cfg;
	chip->set_pull = gpio_bcm2836_set_pull;
	chip->get_pull = gpio_bcm2836_get_pull;
	chip->set_drv = gpio_bcm2836_set_drv;
	chip->get_drv = gpio_bcm2836_get_drv;
	chip->set_rate = gpio_bcm2836_set_rate;
	chip->get_rate = gpio_bcm2836_get_rate;
	chip->set_dir = gpio_bcm2836_set_dir;
	chip->get_dir = gpio_bcm2836_get_dir;
	chip->set_value = gpio_bcm2836_set_value;
	chip->get_value = gpio_bcm2836_get_value;
	chip->to_irq = gpio_bcm2836_to_irq;
	chip->priv = pdat;

	if(!register_gpiochip(&dev, chip))
	{
		free_device_name(chip->name);
		free(chip->priv);
		free(chip);
		return NULL;
	}
	dev->driver = drv;

	return dev;
}

static void gpio_bcm2836_remove(struct device_t * dev)
{
	struct gpiochip_t * chip = (struct gpiochip_t *)dev->priv;

	if(chip && unregister_gpiochip(chip))
	{
		free_device_name(chip->name);
		free(chip->priv);
		free(chip);
	}
}

static void gpio_bcm2836_suspend(struct device_t * dev)
{
}

static void gpio_bcm2836_resume(struct device_t * dev)
{
}

static struct driver_t gpio_bcm2836 = {
	.name		= "gpio-bcm2836",
	.probe		= gpio_bcm2836_probe,
	.remove		= gpio_bcm2836_remove,
	.suspend	= gpio_bcm2836_suspend,
	.resume		= gpio_bcm2836_resume,
};

static __init void gpio_bcm2836_driver_init(void)
{
	register_driver(&gpio_bcm2836);
}

static __exit void gpio_bcm2836_driver_exit(void)
{
	unregister_driver(&gpio_bcm2836);
}

driver_initcall(gpio_bcm2836_driver_init);
driver_exitcall(gpio_bcm2836_driver_exit);
