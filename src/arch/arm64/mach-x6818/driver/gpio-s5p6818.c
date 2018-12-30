/*
 * driver/gpio-s5p6818.c
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

enum {
	GPIO_OUT						= 0x00,
	GPIO_OUTENB						= 0x04,
	GPIO_DETMODE0					= 0x08,
	GPIO_DETMODE1					= 0x0C,
	GPIO_INTENB						= 0x10,
	GPIO_DET						= 0x14,
	GPIO_PAD						= 0x18,
	GPIO_ALTFN0						= 0x20,
	GPIO_ALTFN1						= 0x24,
	GPIO_DETMODEEX					= 0x28,
	GPIO_DETENB						= 0x3C,
	GPIO_SLEW						= 0x40,
	GPIO_SLEW_DISABLE_DEFAULT		= 0x44,
	GPIO_DRV1						= 0x48,
	GPIO_DRV1_DISABLE_DEFAULT		= 0x4C,
	GPIO_DRV0						= 0x50,
	GPIO_DRV0_DISABLE_DEFAULT		= 0x54,
	GPIO_PULLSEL					= 0x58,
	GPIO_PULLSEL_DISABLE_DEFAULT	= 0x5C,
	GPIO_PULLENB					= 0x60,
	GPIO_PULLENB_DISABLE_DEFAULT	= 0x64,
};

struct gpio_s5p6818_pdata_t
{
	virtual_addr_t virt;
	int base;
	int ngpio;
	int oirq;
};

static void gpio_s5p6818_set_cfg(struct gpiochip_t * chip, int offset, int cfg)
{
	struct gpio_s5p6818_pdata_t * pdat = (struct gpio_s5p6818_pdata_t *)chip->priv;
	u32_t val;

	if(offset >= chip->ngpio)
		return;

	if(offset < 16)
	{
		offset = offset << 0x1;
		val = read32(pdat->virt + GPIO_ALTFN0);
		val &= ~(0x3 << offset);
		val |= cfg << offset;
		write32(pdat->virt + GPIO_ALTFN0, val);
	}
	else if(offset < 32)
	{
		offset = (offset - 16) << 0x1;
		val = read32(pdat->virt + GPIO_ALTFN1);
		val &= ~(0x3 << offset);
		val |= cfg << offset;
		write32(pdat->virt + GPIO_ALTFN1, val);
	}
}

static int gpio_s5p6818_get_cfg(struct gpiochip_t * chip, int offset)
{
	struct gpio_s5p6818_pdata_t * pdat = (struct gpio_s5p6818_pdata_t *)chip->priv;
	u32_t val;

	if(offset >= chip->ngpio)
		return 0;

	if(offset < 16)
	{
		offset = offset << 0x1;
		val = read32(pdat->virt + GPIO_ALTFN0);
		return ((val >> offset) & 0x3);
	}
	else if(offset < 32)
	{
		offset = (offset - 16) << 0x1;
		val = read32(pdat->virt + GPIO_ALTFN1);
		return ((val >> offset) & 0x3);
	}

	return 0;
}

static void gpio_s5p6818_set_pull(struct gpiochip_t * chip, int offset, enum gpio_pull_t pull)
{
	struct gpio_s5p6818_pdata_t * pdat = (struct gpio_s5p6818_pdata_t *)chip->priv;
	u32_t val;

	if(offset >= chip->ngpio)
		return;

	switch(pull)
	{
	case GPIO_PULL_UP:
		val = read32(pdat->virt + GPIO_PULLSEL);
		val |= 1 << offset;
		write32(pdat->virt + GPIO_PULLSEL, val);
		val = read32(pdat->virt + GPIO_PULLENB);
		val |= 1 << offset;
		write32(pdat->virt + GPIO_PULLENB, val);
		break;

	case GPIO_PULL_DOWN:
		val = read32(pdat->virt + GPIO_PULLSEL);
		val &= ~(1 << offset);
		write32(pdat->virt + GPIO_PULLSEL, val);
		val = read32(pdat->virt + GPIO_PULLENB);
		val |= 1 << offset;
		write32(pdat->virt + GPIO_PULLENB, val);
		break;

	case GPIO_PULL_NONE:
		val = read32(pdat->virt + GPIO_PULLENB);
		val &= ~(1 << offset);
		write32(pdat->virt + GPIO_PULLENB, val);
		break;

	default:
		break;
	}

	val = read32(pdat->virt + GPIO_PULLSEL_DISABLE_DEFAULT);
	val |= 1 << offset;
	write32(pdat->virt + GPIO_PULLSEL_DISABLE_DEFAULT, val);
	val = read32(pdat->virt + GPIO_PULLENB_DISABLE_DEFAULT);
	val |= 1 << offset;
	write32(pdat->virt + GPIO_PULLENB_DISABLE_DEFAULT, val);
}

static enum gpio_pull_t gpio_s5p6818_get_pull(struct gpiochip_t * chip, int offset)
{
	struct gpio_s5p6818_pdata_t * pdat = (struct gpio_s5p6818_pdata_t *)chip->priv;
	u32_t val;

	if(offset >= chip->ngpio)
		return GPIO_PULL_NONE;

	val = read32(pdat->virt + GPIO_PULLENB);
	if(!((val >> offset) & 0x1))
	{
		val = read32(pdat->virt + GPIO_PULLSEL);
		if((val >> offset) & 0x1)
			return GPIO_PULL_UP;
		else
			return GPIO_PULL_DOWN;
	}
	return GPIO_PULL_NONE;
}

static void gpio_s5p6818_set_drv(struct gpiochip_t * chip, int offset, enum gpio_drv_t drv)
{
	struct gpio_s5p6818_pdata_t * pdat = (struct gpio_s5p6818_pdata_t *)chip->priv;
	u32_t val;

	if(offset >= chip->ngpio)
		return;

	switch(drv)
	{
	case GPIO_DRV_WEAK:
		val = read32(pdat->virt + GPIO_DRV0);
		val &= ~(1 << offset);
		write32(pdat->virt + GPIO_DRV0, val);
		val = read32(pdat->virt + GPIO_DRV1);
		val &= ~(1 << offset);
		write32(pdat->virt + GPIO_DRV1, val);
		break;

	case GPIO_DRV_WEAKER:
		val = read32(pdat->virt + GPIO_DRV0);
		val &= ~(1 << offset);
		write32(pdat->virt + GPIO_DRV0, val);
		val = read32(pdat->virt + GPIO_DRV1);
		val |= 1 << offset;
		write32(pdat->virt + GPIO_DRV1, val);
		break;

	case GPIO_DRV_STRONGER:
		val = read32(pdat->virt + GPIO_DRV0);
		val |= 1 << offset;
		write32(pdat->virt + GPIO_DRV0, val);
		val = read32(pdat->virt + GPIO_DRV1);
		val &= ~(1 << offset);
		write32(pdat->virt + GPIO_DRV1, val);
		break;

	case GPIO_DRV_STRONG:
		val = read32(pdat->virt + GPIO_DRV0);
		val |= 1 << offset;
		write32(pdat->virt + GPIO_DRV0, val);
		val = read32(pdat->virt + GPIO_DRV1);
		val |= 1 << offset;
		write32(pdat->virt + GPIO_DRV1, val);
		break;

	default:
		break;
	}

	val = read32(pdat->virt + GPIO_DRV0_DISABLE_DEFAULT);
	val |= 1 << offset;
	write32(pdat->virt + GPIO_DRV0_DISABLE_DEFAULT, val);
	val = read32(pdat->virt + GPIO_DRV1_DISABLE_DEFAULT);
	val |= 1 << offset;
	write32(pdat->virt + GPIO_DRV1_DISABLE_DEFAULT, val);
}

static enum gpio_drv_t gpio_s5p6818_get_drv(struct gpiochip_t * chip, int offset)
{
	struct gpio_s5p6818_pdata_t * pdat = (struct gpio_s5p6818_pdata_t *)chip->priv;
	u32_t val, d;

	if(offset >= chip->ngpio)
		return GPIO_DRV_WEAK;

	val = read32(pdat->virt + GPIO_DRV0);
	d = (val >> offset) & 0x1;
	val = read32(pdat->virt + GPIO_DRV1);
	d |= ((val >> offset) & 0x1) << 1;

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

static void gpio_s5p6818_set_rate(struct gpiochip_t * chip, int offset, enum gpio_rate_t rate)
{
	struct gpio_s5p6818_pdata_t * pdat = (struct gpio_s5p6818_pdata_t *)chip->priv;
	u32_t val;

	if(offset >= chip->ngpio)
		return;

	switch(rate)
	{
	case GPIO_RATE_SLOW:
		val = read32(pdat->virt + GPIO_SLEW);
		val |= 1 << offset;
		write32(pdat->virt + GPIO_SLEW, val);
		break;

	case GPIO_RATE_FAST:
		val = read32(pdat->virt + GPIO_SLEW);
		val &= ~(1 << offset);
		write32(pdat->virt + GPIO_SLEW, val);
		break;

	default:
		break;
	}

	val = read32(pdat->virt + GPIO_SLEW_DISABLE_DEFAULT);
	val |= 1 << offset;
	write32(pdat->virt + GPIO_SLEW_DISABLE_DEFAULT, val);
}

static enum gpio_rate_t gpio_s5p6818_get_rate(struct gpiochip_t * chip, int offset)
{
	struct gpio_s5p6818_pdata_t * pdat = (struct gpio_s5p6818_pdata_t *)chip->priv;
	u32_t val;

	if(offset >= chip->ngpio)
		return GPIO_RATE_SLOW;

	val = read32(pdat->virt + GPIO_SLEW);
	if((val >> offset) & 0x1)
		return GPIO_RATE_SLOW;
	else
		return GPIO_RATE_FAST;
}

static void gpio_s5p6818_set_dir(struct gpiochip_t * chip, int offset, enum gpio_direction_t dir)
{
	struct gpio_s5p6818_pdata_t * pdat = (struct gpio_s5p6818_pdata_t *)chip->priv;
	u32_t val;

	if(offset >= chip->ngpio)
		return;

	switch(dir)
	{
	case GPIO_DIRECTION_INPUT:
		val = read32(pdat->virt + GPIO_OUTENB);
		val &= ~(0x1 << offset);
		write32(pdat->virt + GPIO_OUTENB, val);
		break;

	case GPIO_DIRECTION_OUTPUT:
		val = read32(pdat->virt + GPIO_OUTENB);
		val |= 0x1 << offset;
		write32(pdat->virt + GPIO_OUTENB, val);
		break;

	default:
		break;
	}
}

static enum gpio_direction_t gpio_s5p6818_get_dir(struct gpiochip_t * chip, int offset)
{
	struct gpio_s5p6818_pdata_t * pdat = (struct gpio_s5p6818_pdata_t *)chip->priv;
	u32_t val, d;

	if(offset >= chip->ngpio)
		return GPIO_DIRECTION_INPUT;

	val = read32(pdat->virt + GPIO_OUTENB);
	d = (val >> offset) & 0x1;
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

static void gpio_s5p6818_set_value(struct gpiochip_t * chip, int offset, int value)
{
	struct gpio_s5p6818_pdata_t * pdat = (struct gpio_s5p6818_pdata_t *)chip->priv;
	u32_t val;

	if(offset >= chip->ngpio)
		return;

	val = read32(pdat->virt + GPIO_OUT);
	val &= ~(1 << offset);
	val |= (!!value) << offset;
	write32(pdat->virt + GPIO_OUT, val);
}

static int gpio_s5p6818_get_value(struct gpiochip_t * chip, int offset)
{
	struct gpio_s5p6818_pdata_t * pdat = (struct gpio_s5p6818_pdata_t *)chip->priv;
	u32_t val;

	if(offset >= chip->ngpio)
		return 0;

	val = read32(pdat->virt + GPIO_PAD);
	return !!(val & (1 << offset));
}

static int gpio_s5p6818_to_irq(struct gpiochip_t * chip, int offset)
{
	struct gpio_s5p6818_pdata_t * pdat = (struct gpio_s5p6818_pdata_t *)chip->priv;

	if((offset >= chip->ngpio) || (pdat->oirq < 0))
		return -1;
	return pdat->oirq + offset;
}

static struct device_t * gpio_s5p6818_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct gpio_s5p6818_pdata_t * pdat;
	struct gpiochip_t * chip;
	struct device_t * dev;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	int base = dt_read_int(n, "gpio-base", -1);
	int ngpio = dt_read_int(n, "gpio-count", -1);

	if((base < 0) || (ngpio <= 0))
		return NULL;

	pdat = malloc(sizeof(struct gpio_s5p6818_pdata_t));
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
	chip->set_cfg = gpio_s5p6818_set_cfg;
	chip->get_cfg = gpio_s5p6818_get_cfg;
	chip->set_pull = gpio_s5p6818_set_pull;
	chip->get_pull = gpio_s5p6818_get_pull;
	chip->set_drv = gpio_s5p6818_set_drv;
	chip->get_drv = gpio_s5p6818_get_drv;
	chip->set_rate = gpio_s5p6818_set_rate;
	chip->get_rate = gpio_s5p6818_get_rate;
	chip->set_dir = gpio_s5p6818_set_dir;
	chip->get_dir = gpio_s5p6818_get_dir;
	chip->set_value = gpio_s5p6818_set_value;
	chip->get_value = gpio_s5p6818_get_value;
	chip->to_irq = gpio_s5p6818_to_irq;
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

static void gpio_s5p6818_remove(struct device_t * dev)
{
	struct gpiochip_t * chip = (struct gpiochip_t *)dev->priv;

	if(chip && unregister_gpiochip(chip))
	{
		free_device_name(chip->name);
		free(chip->priv);
		free(chip);
	}
}

static void gpio_s5p6818_suspend(struct device_t * dev)
{
}

static void gpio_s5p6818_resume(struct device_t * dev)
{
}

static struct driver_t gpio_s5p6818 = {
	.name		= "gpio-s5p6818",
	.probe		= gpio_s5p6818_probe,
	.remove		= gpio_s5p6818_remove,
	.suspend	= gpio_s5p6818_suspend,
	.resume		= gpio_s5p6818_resume,
};

static __init void gpio_s5p6818_driver_init(void)
{
	register_driver(&gpio_s5p6818);
}

static __exit void gpio_s5p6818_driver_exit(void)
{
	unregister_driver(&gpio_s5p6818);
}

driver_initcall(gpio_s5p6818_driver_init);
driver_exitcall(gpio_s5p6818_driver_exit);
