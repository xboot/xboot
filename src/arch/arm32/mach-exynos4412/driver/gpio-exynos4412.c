/*
 * driver/gpio-exynos4412.c
 *
 * Copyright(c) 2007-2021 Jianjun Jiang <8192542@qq.com>
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

enum {
	GPIO_CON 	= 0x00,
	GPIO_DAT 	= 0x04,
	GPIO_PUD 	= 0x08,
	GPIO_DRV 	= 0x0C,
	GPIO_CONPDN	= 0x10,
	GPIO_PUDPDN	= 0x14,
};

struct gpio_exynos4412_pdata_t
{
	virtual_addr_t virt;
	int base;
	int ngpio;
	int oirq;
};

static void gpio_exynos4412_set_cfg(struct gpiochip_t * chip, int offset, int cfg)
{
	struct gpio_exynos4412_pdata_t * pdat = (struct gpio_exynos4412_pdata_t *)chip->priv;
	u32_t val;

	if(offset >= chip->ngpio)
		return;

	offset <<= 0x2;
	val = read32(pdat->virt + GPIO_CON);
	val &= ~(0xf << offset);
	val |= cfg << offset;
	write32(pdat->virt + GPIO_CON, val);
}

static int gpio_exynos4412_get_cfg(struct gpiochip_t * chip, int offset)
{
	struct gpio_exynos4412_pdata_t * pdat = (struct gpio_exynos4412_pdata_t *)chip->priv;
	u32_t val;

	if(offset >= chip->ngpio)
		return 0;

	offset <<= 0x2;
	val = read32(pdat->virt + GPIO_CON);
	return ((val >> offset) & 0xf);
}

static void gpio_exynos4412_set_pull(struct gpiochip_t * chip, int offset, enum gpio_pull_t pull)
{
	struct gpio_exynos4412_pdata_t * pdat = (struct gpio_exynos4412_pdata_t *)chip->priv;
	u32_t val, p;

	if(offset >= chip->ngpio)
		return;

	switch(pull)
	{
	case GPIO_PULL_UP:
		p = 0x2;
		break;
	case GPIO_PULL_DOWN:
		p = 0x1;
		break;
	case GPIO_PULL_NONE:
		p = 0x0;
		break;
	default:
		p = 0x0;
		break;
	}

	offset <<= 0x1;
	val = read32(pdat->virt + GPIO_PUD);
	val &= ~(0x3 << offset);
	val |= p << offset;
	write32(pdat->virt + GPIO_PUD, val);
}

static enum gpio_pull_t gpio_exynos4412_get_pull(struct gpiochip_t * chip, int offset)
{
	struct gpio_exynos4412_pdata_t * pdat = (struct gpio_exynos4412_pdata_t *)chip->priv;
	u32_t val, p;

	if(offset >= chip->ngpio)
		return GPIO_PULL_NONE;

	offset <<= 0x1;
	val = read32(pdat->virt + GPIO_PUD);
	p = (val >> offset) & 0x3;
	switch(p)
	{
	case 0x0:
		return GPIO_PULL_NONE;
	case 0x1:
		return GPIO_PULL_DOWN;
	case 0x2:
		return GPIO_PULL_UP;
	default:
		break;
	}
	return GPIO_PULL_NONE;
}

static void gpio_exynos4412_set_drv(struct gpiochip_t * chip, int offset, enum gpio_drv_t drv)
{
	struct gpio_exynos4412_pdata_t * pdat = (struct gpio_exynos4412_pdata_t *)chip->priv;
	u32_t val, d;

	if(offset >= chip->ngpio)
		return;

	switch(drv)
	{
	case GPIO_DRV_WEAK:
		d = 0x0;
		break;
	case GPIO_DRV_WEAKER:
		d = 0x1;
		break;
	case GPIO_DRV_STRONGER:
		d = 0x2;
		break;
	case GPIO_DRV_STRONG:
		d = 0x3;
		break;
	default:
		d = 0x0;
		break;
	}

	offset <<= 0x1;
	val = read32(pdat->virt + GPIO_DRV);
	val &= ~(0x3 << offset);
	val |= d << offset;
	write32(pdat->virt + GPIO_DRV, val);
}

static enum gpio_drv_t gpio_exynos4412_get_drv(struct gpiochip_t * chip, int offset)
{
	struct gpio_exynos4412_pdata_t * pdat = (struct gpio_exynos4412_pdata_t *)chip->priv;
	u32_t val, d;

	if(offset >= chip->ngpio)
		return GPIO_DRV_WEAK;

	offset <<= 0x1;
	val = read32(pdat->virt + GPIO_DRV);
	d = (val >> offset) & 0x3;
	switch(d)
	{
	case 0x0:
		return GPIO_DRV_WEAK;
	case 0x1:
		return GPIO_DRV_WEAKER;
	case 0x2:
		return GPIO_DRV_STRONGER;
	case 0x3:
		return GPIO_DRV_STRONG;
	default:
		break;
	}
	return GPIO_DRV_WEAK;
}

static void gpio_exynos4412_set_rate(struct gpiochip_t * chip, int offset, enum gpio_rate_t rate)
{
}

static enum gpio_rate_t gpio_exynos4412_get_rate(struct gpiochip_t * chip, int offset)
{
	return GPIO_RATE_SLOW;
}

static void gpio_exynos4412_set_dir(struct gpiochip_t * chip, int offset, enum gpio_direction_t dir)
{
	struct gpio_exynos4412_pdata_t * pdat = (struct gpio_exynos4412_pdata_t *)chip->priv;
	u32_t val;

	if(offset >= chip->ngpio)
		return;

	switch(dir)
	{
	case GPIO_DIRECTION_INPUT:
		offset <<= 0x2;
		val = read32(pdat->virt + GPIO_CON);
		val &= ~(0xf << offset);
		write32(pdat->virt + GPIO_CON, val);
		break;

	case GPIO_DIRECTION_OUTPUT:
		offset <<= 0x2;
		val = read32(pdat->virt + GPIO_CON);
		val &= ~(0xf << offset);
		val |= 0x1 << offset;
		write32(pdat->virt + GPIO_CON, val);
		break;

	default:
		break;
	}
}

static enum gpio_direction_t gpio_exynos4412_get_dir(struct gpiochip_t * chip, int offset)
{
	struct gpio_exynos4412_pdata_t * pdat = (struct gpio_exynos4412_pdata_t *)chip->priv;
	u32_t val, d;

	if(offset >= chip->ngpio)
		return GPIO_DIRECTION_INPUT;

	offset <<= 0x2;
	val = read32(pdat->virt + GPIO_CON);
	d = (val >> offset) & 0x4;
	switch(d)
	{
	case 0x0:
		return GPIO_DIRECTION_INPUT;
	case 0x1:
		return GPIO_DIRECTION_OUTPUT;
	default:
		break;
	}
	return GPIO_DIRECTION_INPUT;
}

static void gpio_exynos4412_set_value(struct gpiochip_t * chip, int offset, int value)
{
	struct gpio_exynos4412_pdata_t * pdat = (struct gpio_exynos4412_pdata_t *)chip->priv;
	u32_t val;

	if(offset >= chip->ngpio)
		return;

	val = read32(pdat->virt + GPIO_DAT);
	val &= ~(1 << offset);
	val |= (!!value) << offset;
	write32(pdat->virt + GPIO_DAT, val);
}

static int gpio_exynos4412_get_value(struct gpiochip_t * chip, int offset)
{
	struct gpio_exynos4412_pdata_t * pdat = (struct gpio_exynos4412_pdata_t *)chip->priv;
	u32_t val;

	if(offset >= chip->ngpio)
		return 0;

	val = read32(pdat->virt + GPIO_DAT);
	return !!(val & (1 << offset));
}

static int gpio_exynos4412_to_irq(struct gpiochip_t * chip, int offset)
{
	struct gpio_exynos4412_pdata_t * pdat = (struct gpio_exynos4412_pdata_t *)chip->priv;

	if((offset >= chip->ngpio) || (pdat->oirq < 0))
		return -1;
	return pdat->oirq + offset;
}

static struct device_t * gpio_exynos4412_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct gpio_exynos4412_pdata_t * pdat;
	struct gpiochip_t * chip;
	struct device_t * dev;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	int base = dt_read_int(n, "gpio-base", -1);
	int ngpio = dt_read_int(n, "gpio-count", -1);

	if((base < 0) || (ngpio <= 0))
		return NULL;

	pdat = malloc(sizeof(struct gpio_exynos4412_pdata_t));
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
	chip->set_cfg = gpio_exynos4412_set_cfg;
	chip->get_cfg = gpio_exynos4412_get_cfg;
	chip->set_pull = gpio_exynos4412_set_pull;
	chip->get_pull = gpio_exynos4412_get_pull;
	chip->set_drv = gpio_exynos4412_set_drv;
	chip->get_drv = gpio_exynos4412_get_drv;
	chip->set_rate = gpio_exynos4412_set_rate;
	chip->get_rate = gpio_exynos4412_get_rate;
	chip->set_dir = gpio_exynos4412_set_dir;
	chip->get_dir = gpio_exynos4412_get_dir;
	chip->set_value = gpio_exynos4412_set_value;
	chip->get_value = gpio_exynos4412_get_value;
	chip->to_irq = gpio_exynos4412_to_irq;
	chip->priv = pdat;

	if(!(dev = register_gpiochip(chip, drv)))
	{
		free_device_name(chip->name);
		free(chip->priv);
		free(chip);
		return NULL;
	}
	return dev;
}

static void gpio_exynos4412_remove(struct device_t * dev)
{
	struct gpiochip_t * chip = (struct gpiochip_t *)dev->priv;

	if(chip)
	{
		unregister_gpiochip(chip);
		free_device_name(chip->name);
		free(chip->priv);
		free(chip);
	}
}

static void gpio_exynos4412_suspend(struct device_t * dev)
{
}

static void gpio_exynos4412_resume(struct device_t * dev)
{
}

static struct driver_t gpio_exynos4412 = {
	.name		= "gpio-exynos4412",
	.probe		= gpio_exynos4412_probe,
	.remove		= gpio_exynos4412_remove,
	.suspend	= gpio_exynos4412_suspend,
	.resume		= gpio_exynos4412_resume,
};

static __init void gpio_exynos4412_driver_init(void)
{
	register_driver(&gpio_exynos4412);
}

static __exit void gpio_exynos4412_driver_exit(void)
{
	unregister_driver(&gpio_exynos4412);
}

driver_initcall(gpio_exynos4412_driver_init);
driver_exitcall(gpio_exynos4412_driver_exit);
