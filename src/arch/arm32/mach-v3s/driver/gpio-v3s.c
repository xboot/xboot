/*
 * driver/gpio-v3s.c
 *
 * Copyright(c) 2007-2017 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <xboot.h>
#include <gpio/gpio.h>

enum {
	GPIO_CFG0	= 0x00,
	GPIO_CFG1	= 0x04,
	GPIO_CFG2	= 0x08,
	GPIO_CFG3	= 0x0c,
	GPIO_DAT	= 0x10,
	GPIO_DRV0	= 0x14,
	GPIO_DRV1	= 0x18,
	GPIO_PUL0	= 0x1c,
	GPIO_PUL1	= 0x20,
};

struct gpio_v3s_pdata_t
{
	virtual_addr_t virt;
	int base;
	int ngpio;
	int oirq;
};

static void gpio_v3s_set_cfg(struct gpiochip_t * chip, int offset, int cfg)
{
	struct gpio_v3s_pdata_t * pdat = (struct gpio_v3s_pdata_t *)chip->priv;
	virtual_addr_t addr;
	u32_t val;

	if(offset >= chip->ngpio)
		return;

	addr = pdat->virt + GPIO_CFG0 + ((offset >> 3) << 2);
	val = read32(addr);
	val &= ~(0xf << ((offset & 0x7) << 2));
	val |= ((cfg & 0x7) << ((offset & 0x7) << 2));
	write32(addr, val);
}

static int gpio_v3s_get_cfg(struct gpiochip_t * chip, int offset)
{
	struct gpio_v3s_pdata_t * pdat = (struct gpio_v3s_pdata_t *)chip->priv;
	virtual_addr_t addr;
	u32_t val;

	if(offset >= chip->ngpio)
		return 0;

	addr = pdat->virt + GPIO_CFG0 + ((offset >> 3) << 2);
	val = (read32(addr) >> ((offset & 0x7) << 2)) & 0x7;
	return val;
}

static void gpio_v3s_set_pull(struct gpiochip_t * chip, int offset, enum gpio_pull_t pull)
{
	struct gpio_v3s_pdata_t * pdat = (struct gpio_v3s_pdata_t *)chip->priv;
	virtual_addr_t addr;
	u32_t val, v;

	if(offset >= chip->ngpio)
		return;

	switch(pull)
	{
	case GPIO_PULL_UP:
		v = 0x1;
		break;

	case GPIO_PULL_DOWN:
		v = 0x2;
		break;

	case GPIO_PULL_NONE:
		v = 0x0;
		break;

	default:
		v = 0x0;
		break;
	}

	addr = pdat->virt + GPIO_PUL0 + ((offset >> 4) << 2);
	val = read32(addr);
	val &= ~(v << ((offset & 0xf) << 1));
	val |= (v << ((offset & 0xf) << 1));
	write32(addr, val);
}

static enum gpio_pull_t gpio_v3s_get_pull(struct gpiochip_t * chip, int offset)
{
	struct gpio_v3s_pdata_t * pdat = (struct gpio_v3s_pdata_t *)chip->priv;
	virtual_addr_t addr;
	u32_t v = 0;

	if(offset >= chip->ngpio)
		return GPIO_PULL_NONE;

	addr = pdat->virt + GPIO_PUL0 + ((offset >> 4) << 2);
	v = (read32(addr) >> ((offset & 0xf) << 1)) & 0x3;

	switch(v)
	{
	case 0:
		return GPIO_PULL_NONE;
	case 1:
		return GPIO_PULL_UP;
	case 2:
		return GPIO_PULL_DOWN;
	default:
		break;
	}
	return GPIO_PULL_NONE;
}

static void gpio_v3s_set_drv(struct gpiochip_t * chip, int offset, enum gpio_drv_t drv)
{
	struct gpio_v3s_pdata_t * pdat = (struct gpio_v3s_pdata_t *)chip->priv;
	virtual_addr_t addr;
	u32_t val, v;

	if(offset >= chip->ngpio)
		return;

	switch(drv)
	{
	case GPIO_DRV_WEAK:
		v = 0x0;
		break;

	case GPIO_DRV_WEAKER:
		v = 0x1;
		break;

	case GPIO_DRV_STRONGER:
		v = 0x2;
		break;

	case GPIO_DRV_STRONG:
		v = 0x3;
		break;

	default:
		v = 0x0;
		break;
	}

	addr = pdat->virt + GPIO_DRV0 + ((offset >> 4) << 2);
	val = read32(addr);
	val &= ~(v << ((offset & 0xf) << 1));
	val |= (v << ((offset & 0xf) << 1));
	write32(addr, val);
}

static enum gpio_drv_t gpio_v3s_get_drv(struct gpiochip_t * chip, int offset)
{
	struct gpio_v3s_pdata_t * pdat = (struct gpio_v3s_pdata_t *)chip->priv;
	virtual_addr_t addr;
	u32_t v = 0;

	if(offset >= chip->ngpio)
		return GPIO_DRV_WEAK;

	addr = pdat->virt + GPIO_DRV0 + ((offset >> 4) << 2);
	v = (read32(addr) >> ((offset & 0xf) << 1)) & 0x3;

	switch(v)
	{
	case 0:
		return GPIO_DRV_WEAK;
	case 1:
		return GPIO_DRV_WEAKER;
	case 2:
		return GPIO_DRV_STRONGER;
	case 3:
		return GPIO_DRV_STRONG;
	default:
		break;
	}
	return GPIO_DRV_WEAK;
}

static void gpio_v3s_set_rate(struct gpiochip_t * chip, int offset, enum gpio_rate_t rate)
{
}

static enum gpio_rate_t gpio_v3s_get_rate(struct gpiochip_t * chip, int offset)
{
	return GPIO_RATE_SLOW;
}

static void gpio_v3s_set_dir(struct gpiochip_t * chip, int offset, enum gpio_direction_t dir)
{
	if(offset >= chip->ngpio)
		return;

	switch(dir)
	{
	case GPIO_DIRECTION_INPUT:
		gpio_v3s_set_cfg(chip, offset, 0);
		break;

	case GPIO_DIRECTION_OUTPUT:
		gpio_v3s_set_cfg(chip, offset, 1);
		break;

	default:
		break;
	}
}

static enum gpio_direction_t gpio_v3s_get_dir(struct gpiochip_t * chip, int offset)
{
	if(offset >= chip->ngpio)
		return GPIO_DIRECTION_INPUT;

	switch(gpio_v3s_get_cfg(chip, offset))
	{
	case 0:
		return GPIO_DIRECTION_INPUT;
	case 1:
		return GPIO_DIRECTION_OUTPUT;
	default:
		break;
	}
	return GPIO_DIRECTION_INPUT;
}

static void gpio_v3s_set_value(struct gpiochip_t * chip, int offset, int value)
{
	struct gpio_v3s_pdata_t * pdat = (struct gpio_v3s_pdata_t *)chip->priv;
	u32_t val;

	if(offset >= chip->ngpio)
		return;

	val = read32(pdat->virt + GPIO_DAT);
	val &= ~(1 << offset);
	val |= (!!value) << offset;
	write32(pdat->virt + GPIO_DAT, val);
}

static int gpio_v3s_get_value(struct gpiochip_t * chip, int offset)
{
	struct gpio_v3s_pdata_t * pdat = (struct gpio_v3s_pdata_t *)chip->priv;
	u32_t val;

	if(offset >= chip->ngpio)
		return 0;

	val = read32(pdat->virt + GPIO_DAT);
	return !!(val & (1 << offset));
}

static int gpio_v3s_to_irq(struct gpiochip_t * chip, int offset)
{
	struct gpio_v3s_pdata_t * pdat = (struct gpio_v3s_pdata_t *)chip->priv;

	if((offset >= chip->ngpio) || (pdat->oirq < 0))
		return -1;
	return pdat->oirq + offset;
}

static struct device_t * gpio_v3s_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct gpio_v3s_pdata_t * pdat;
	struct gpiochip_t * chip;
	struct device_t * dev;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	int base = dt_read_int(n, "gpio-base", -1);
	int ngpio = dt_read_int(n, "gpio-count", -1);

	if((base < 0) || (ngpio <= 0))
		return NULL;

	pdat = malloc(sizeof(struct gpio_v3s_pdata_t));
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
	chip->set_cfg = gpio_v3s_set_cfg;
	chip->get_cfg = gpio_v3s_get_cfg;
	chip->set_pull = gpio_v3s_set_pull;
	chip->get_pull = gpio_v3s_get_pull;
	chip->set_drv = gpio_v3s_set_drv;
	chip->get_drv = gpio_v3s_get_drv;
	chip->set_rate = gpio_v3s_set_rate;
	chip->get_rate = gpio_v3s_get_rate;
	chip->set_dir = gpio_v3s_set_dir;
	chip->get_dir = gpio_v3s_get_dir;
	chip->set_value = gpio_v3s_set_value;
	chip->get_value = gpio_v3s_get_value;
	chip->to_irq = gpio_v3s_to_irq;
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

static void gpio_v3s_remove(struct device_t * dev)
{
	struct gpiochip_t * chip = (struct gpiochip_t *)dev->priv;

	if(chip && unregister_gpiochip(chip))
	{
		free_device_name(chip->name);
		free(chip->priv);
		free(chip);
	}
}

static void gpio_v3s_suspend(struct device_t * dev)
{
}

static void gpio_v3s_resume(struct device_t * dev)
{
}

static struct driver_t gpio_v3s = {
	.name		= "gpio-v3s",
	.probe		= gpio_v3s_probe,
	.remove		= gpio_v3s_remove,
	.suspend	= gpio_v3s_suspend,
	.resume		= gpio_v3s_resume,
};

static __init void gpio_v3s_driver_init(void)
{
	register_driver(&gpio_v3s);
}

static __exit void gpio_v3s_driver_exit(void)
{
	unregister_driver(&gpio_v3s);
}

driver_initcall(gpio_v3s_driver_init);
driver_exitcall(gpio_v3s_driver_exit);
