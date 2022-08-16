/*
 * driver/gpio-rv1106.c
 *
 * Copyright(c) 2007-2022 Jianjun Jiang <8192542@qq.com>
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
	GPIO_SWPORT_DR_L	= 0x00,
	GPIO_SWPORT_DR_H	= 0x04,
	GPIO_SWPORT_DDR_L	= 0x08,
	GPIO_SWPORT_DDR_H	= 0x0c,
	GPIO_INT_EN_L		= 0x10,
	GPIO_INT_EN_H		= 0x14,
	GPIO_INT_MASK_L		= 0x18,
	GPIO_INT_MASK_H		= 0x1c,
	GPIO_INT_TYPE_L		= 0x20,
	GPIO_INT_TYPE_H		= 0x24,
	GPIO_INT_POLARITY_L	= 0x28,
	GPIO_INT_POLARITY_H	= 0x2c,
	GPIO_INT_BOTHEDGE_L	= 0x30,
	GPIO_INT_BOTHEDGE_H	= 0x34,
	GPIO_DEBOUNCE_L		= 0x38,
	GPIO_DEBOUNCE_H		= 0x3c,
	GPIO_DBCLK_DIV_EN_L	= 0x40,
	GPIO_DBCLK_DIV_EN_H	= 0x44,
	GPIO_DBCLK_DIV_CON	= 0x48,
	GPIO_INT_STATUS		= 0x50,
	GPIO_INT_RAWSTATUS	= 0x58,
	GPIO_PORT_EOI_L		= 0x60,
	GPIO_PORT_EOI_H		= 0x64,
	GPIO_EXT_PORT		= 0x70,
	GPIO_VER_ID			= 0x78,
};

struct gpio_rv1106_pdata_t
{
	virtual_addr_t virt;
	virtual_addr_t ioc;
	virtual_addr_t pmuioc;
	int base;
	int ngpio;
	int oirq;
};

static inline void gpio_write32(virtual_addr_t reg, u32_t val)
{
	write32(reg, (val & 0x0000ffff) | 0xffff0000);
	write32(reg + 0x4, (val >> 16) | 0xffff0000);
}

static inline u32_t gpio_read32(virtual_addr_t reg)
{
	return (read32(reg + 0x4) << 16) | (read32(reg) & 0x0000ffff);
}

static void gpio_rv1106_set_cfg(struct gpiochip_t * chip, int offset, int cfg)
{
	struct gpio_rv1106_pdata_t * pdat = (struct gpio_rv1106_pdata_t *)chip->priv;
	virtual_addr_t addr = pdat->pmuioc;
	u32_t val;

	if(offset >= chip->ngpio)
		return;

	if(pdat->base < 32)
	{
		if(offset < 8)
			addr = pdat->pmuioc + 0x00000000;
		else
			return;
	}
	else if(pdat->base < 64)
	{
		if(offset < 8)
			addr = pdat->ioc + 0x00000000;
		else if(offset < 16)
			addr = pdat->ioc + 0x00000008;
		else if(offset < 24)
			addr = pdat->ioc + 0x00000010;
		else if(offset < 32)
			addr = pdat->ioc + 0x00000018;
		else
			return;
	}
	else if(pdat->base < 96)
	{
		if(offset < 8)
			addr = pdat->ioc + 0x00010020;
		else if(offset < 16)
			addr = pdat->ioc + 0x00010028;
		else
			return;
	}
	else if(pdat->base < 128)
	{
		if(offset < 8)
			addr = pdat->ioc + 0x00020040;
		else if(offset < 16)
			addr = pdat->ioc + 0x00020048;
		else if(offset < 24)
			addr = pdat->ioc + 0x00020050;
		else if(offset < 32)
			addr = pdat->ioc + 0x00020058;
		else
			return;
	}
	else if(pdat->base < 160)
	{
		if(offset < 8)
			addr = pdat->ioc + 0x00030000;
		else if(offset < 16)
			addr = pdat->ioc + 0x00030008;
		else
			return;
	}
	else
		return;
	if(offset & 0x4)
		addr += 0x4;
	val = (((0xf << 16) | (cfg & 0xf)) << ((offset & 0x3) << 2));
	write32(addr, val);
}

static int gpio_rv1106_get_cfg(struct gpiochip_t * chip, int offset)
{
	struct gpio_rv1106_pdata_t * pdat = (struct gpio_rv1106_pdata_t *)chip->priv;
	virtual_addr_t addr = pdat->pmuioc;

	if(offset >= chip->ngpio)
		return 0;

	if(pdat->base < 32)
	{
		if(offset < 8)
			addr = pdat->pmuioc + 0x00000000;
		else
			return 0;
	}
	else if(pdat->base < 64)
	{
		if(offset < 8)
			addr = pdat->ioc + 0x00000000;
		else if(offset < 16)
			addr = pdat->ioc + 0x00000008;
		else if(offset < 24)
			addr = pdat->ioc + 0x00000010;
		else if(offset < 32)
			addr = pdat->ioc + 0x00000018;
		else
			return 0;
	}
	else if(pdat->base < 96)
	{
		if(offset < 8)
			addr = pdat->ioc + 0x00010020;
		else if(offset < 16)
			addr = pdat->ioc + 0x00010028;
		else
			return 0;
	}
	else if(pdat->base < 128)
	{
		if(offset < 8)
			addr = pdat->ioc + 0x00020040;
		else if(offset < 16)
			addr = pdat->ioc + 0x00020048;
		else if(offset < 24)
			addr = pdat->ioc + 0x00020050;
		else if(offset < 32)
			addr = pdat->ioc + 0x00020058;
		else
			return 0;
	}
	else if(pdat->base < 160)
	{
		if(offset < 8)
			addr = pdat->ioc + 0x00030000;
		else if(offset < 16)
			addr = pdat->ioc + 0x00030008;
		else
			return 0;
	}
	else
		return 0;
	if(offset & 0x4)
		addr += 0x4;
	return (read32(addr) >> ((offset & 0x3) << 2)) & 0xf;
}

static void gpio_rv1106_set_pull(struct gpiochip_t * chip, int offset, enum gpio_pull_t pull)
{
	struct gpio_rv1106_pdata_t * pdat = (struct gpio_rv1106_pdata_t *)chip->priv;
	virtual_addr_t addr;
	u32_t val, v = 0;

	if(offset >= chip->ngpio)
		return;

	if(pdat->base < 32)
		addr = pdat->pmuioc + 0x00000038 + ((offset >> 3) << 2);
	else if(pdat->base < 64)
		addr = pdat->ioc + 0x000001c0 + ((offset >> 3) << 2);
	else if(pdat->base < 96)
		addr = pdat->ioc + 0x000101d0 + ((offset >> 3) << 2);
	else if(pdat->base < 128)
		addr = pdat->ioc + 0x000201e0 + ((offset >> 3) << 2);
	else if(pdat->base < 160)
		addr = pdat->ioc + 0x00030070 + ((offset >> 3) << 2);
	else
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
		break;
	}
	val = (((0x3 << 16) | (v & 0x3)) << ((offset & 0x7) << 1));
	write32(addr, val);
}

static enum gpio_pull_t gpio_rv1106_get_pull(struct gpiochip_t * chip, int offset)
{
	struct gpio_rv1106_pdata_t * pdat = (struct gpio_rv1106_pdata_t *)chip->priv;
	virtual_addr_t addr;
	u32_t v = 0;

	if(offset >= chip->ngpio)
		return GPIO_PULL_NONE;

	if(pdat->base < 32)
		addr = pdat->pmuioc + 0x00000038 + ((offset >> 3) << 2);
	else if(pdat->base < 64)
		addr = pdat->ioc + 0x000001c0 + ((offset >> 3) << 2);
	else if(pdat->base < 96)
		addr = pdat->ioc + 0x000101d0 + ((offset >> 3) << 2);
	else if(pdat->base < 128)
		addr = pdat->ioc + 0x000201e0 + ((offset >> 3) << 2);
	else if(pdat->base < 160)
		addr = pdat->ioc + 0x00030070 + ((offset >> 3) << 2);
	else
		return GPIO_PULL_NONE;
	v = (read32(addr) >> ((offset & 0x7) << 1)) & 0x3;
	switch(v)
	{
	case 0x0:
		return GPIO_PULL_NONE;
	case 0x1:
		return GPIO_PULL_UP;
	case 0x2:
		return GPIO_PULL_DOWN;
	default:
		break;
	}
	return GPIO_PULL_NONE;
}

static void gpio_rv1106_set_drv(struct gpiochip_t * chip, int offset, enum gpio_drv_t drv)
{
	struct gpio_rv1106_pdata_t * pdat = (struct gpio_rv1106_pdata_t *)chip->priv;
	virtual_addr_t addr;
	u32_t val, v = 0;

	if(offset >= chip->ngpio)
		return;

	if(pdat->base < 32)
		addr = pdat->pmuioc + 0x00000010 + ((offset >> 1) << 2);
	else if(pdat->base < 64)
		addr = pdat->ioc + 0x00000080 + ((offset >> 1) << 2);
	else if(pdat->base < 96)
		addr = pdat->ioc + 0x000100c0 + ((offset >> 1) << 2);
	else if(pdat->base < 128)
		addr = pdat->ioc + 0x00020100 + ((offset >> 1) << 2);
	else if(pdat->base < 160)
		addr = pdat->ioc + 0x00030020 + ((offset >> 1) << 2);
	else
		return;
	switch(drv)
	{
	case GPIO_DRV_WEAK:
		v = 0x0;
		break;
	case GPIO_DRV_WEAKER:
		v = 0x5;
		break;
	case GPIO_DRV_STRONGER:
		v = 0xa;
		break;
	case GPIO_DRV_STRONG:
		v = 0xf;
		break;
	default:
		break;
	}
	val = (((0xff << 16) | (v & 0xff)) << ((offset & 0x1) << 3));
	write32(addr, val);
}

static enum gpio_drv_t gpio_rv1106_get_drv(struct gpiochip_t * chip, int offset)
{
	struct gpio_rv1106_pdata_t * pdat = (struct gpio_rv1106_pdata_t *)chip->priv;
	virtual_addr_t addr;
	u32_t v = 0;

	if(offset >= chip->ngpio)
		return GPIO_DRV_WEAK;

	if(pdat->base < 32)
		addr = pdat->pmuioc + 0x00000010 + ((offset >> 1) << 2);
	else if(pdat->base < 64)
		addr = pdat->ioc + 0x00000080 + ((offset >> 1) << 2);
	else if(pdat->base < 96)
		addr = pdat->ioc + 0x000100c0 + ((offset >> 1) << 2);
	else if(pdat->base < 128)
		addr = pdat->ioc + 0x00020100 + ((offset >> 1) << 2);
	else if(pdat->base < 160)
		addr = pdat->ioc + 0x00030020 + ((offset >> 1) << 2);
	else
		return GPIO_DRV_WEAK;
	v = (read32(addr) >> ((offset & 0x1) << 3)) & 0xff;
	switch(v)
	{
	case 0x0 ... 0x3:
		return GPIO_DRV_WEAK;
	case 0x4 ... 0x7:
		return GPIO_DRV_WEAKER;
	case 0x8 ... 0xb:
		return GPIO_DRV_STRONGER;
	case 0xc ... 0xf:
		return GPIO_DRV_STRONG;
	default:
		break;
	}
	return GPIO_DRV_WEAK;
}

static void gpio_rv1106_set_rate(struct gpiochip_t * chip, int offset, enum gpio_rate_t rate)
{
}

static enum gpio_rate_t gpio_rv1106_get_rate(struct gpiochip_t * chip, int offset)
{
	return GPIO_RATE_SLOW;
}

static void gpio_rv1106_set_dir(struct gpiochip_t * chip, int offset, enum gpio_direction_t dir)
{
	struct gpio_rv1106_pdata_t * pdat = (struct gpio_rv1106_pdata_t *)chip->priv;
	u32_t val;

	if(offset >= chip->ngpio)
		return;

	switch(dir)
	{
	case GPIO_DIRECTION_INPUT:
		val = gpio_read32(pdat->virt + GPIO_SWPORT_DDR_L);
		val &= ~(1 << offset);
		gpio_write32(pdat->virt + GPIO_SWPORT_DDR_L, val);
		break;

	case GPIO_DIRECTION_OUTPUT:
		val = gpio_read32(pdat->virt + GPIO_SWPORT_DDR_L);
		val |= 1 << offset;
		gpio_write32(pdat->virt + GPIO_SWPORT_DDR_L, val);
		break;

	default:
		break;
	}
}

static enum gpio_direction_t gpio_rv1106_get_dir(struct gpiochip_t * chip, int offset)
{
	struct gpio_rv1106_pdata_t * pdat = (struct gpio_rv1106_pdata_t *)chip->priv;
	u32_t val;

	if(offset >= chip->ngpio)
		return GPIO_DIRECTION_INPUT;

	val = gpio_read32(pdat->virt + GPIO_SWPORT_DDR_L);
	if((val & (1 << offset)))
		return GPIO_DIRECTION_OUTPUT;
	return GPIO_DIRECTION_INPUT;
}

static void gpio_rv1106_set_value(struct gpiochip_t * chip, int offset, int value)
{
	struct gpio_rv1106_pdata_t * pdat = (struct gpio_rv1106_pdata_t *)chip->priv;
	u32_t val;

	if(offset >= chip->ngpio)
		return;

	val = gpio_read32(pdat->virt + GPIO_SWPORT_DR_L);
	if(value)
		val |= (1 << offset);
	else
		val &= ~(1 << offset);
	gpio_write32(pdat->virt + GPIO_SWPORT_DR_L, val);
}

static int gpio_rv1106_get_value(struct gpiochip_t * chip, int offset)
{
	struct gpio_rv1106_pdata_t * pdat = (struct gpio_rv1106_pdata_t *)chip->priv;
	u32_t val;

	if(offset >= chip->ngpio)
		return 0;

	val = gpio_read32(pdat->virt + GPIO_EXT_PORT);
	return (val & (1 << offset)) ? 1 : 0;
}

static int gpio_rv1106_to_irq(struct gpiochip_t * chip, int offset)
{
	struct gpio_rv1106_pdata_t * pdat = (struct gpio_rv1106_pdata_t *)chip->priv;

	if((offset >= chip->ngpio) || (pdat->oirq < 0))
		return -1;
	return pdat->oirq + offset;
}

static struct device_t * gpio_rv1106_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct gpio_rv1106_pdata_t * pdat;
	struct gpiochip_t * chip;
	struct device_t * dev;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	int base = dt_read_int(n, "gpio-base", -1);
	int ngpio = dt_read_int(n, "gpio-count", -1);

	if((base < 0) || (ngpio <= 0))
		return NULL;

	pdat = malloc(sizeof(struct gpio_rv1106_pdata_t));
	if(!pdat)
		return NULL;

	chip = malloc(sizeof(struct gpiochip_t));
	if(!chip)
	{
		free(pdat);
		return NULL;
	}

	pdat->virt = virt;
	pdat->ioc = phys_to_virt(0xff538000);
	pdat->pmuioc = phys_to_virt(0xff388000);
	pdat->base = base;
	pdat->ngpio = ngpio;
	pdat->oirq = dt_read_int(n, "interrupt-offset", -1);

	chip->name = alloc_device_name(dt_read_name(n), -1);
	chip->base = pdat->base;
	chip->ngpio = pdat->ngpio;
	chip->set_cfg = gpio_rv1106_set_cfg;
	chip->get_cfg = gpio_rv1106_get_cfg;
	chip->set_pull = gpio_rv1106_set_pull;
	chip->get_pull = gpio_rv1106_get_pull;
	chip->set_drv = gpio_rv1106_set_drv;
	chip->get_drv = gpio_rv1106_get_drv;
	chip->set_rate = gpio_rv1106_set_rate;
	chip->get_rate = gpio_rv1106_get_rate;
	chip->set_dir = gpio_rv1106_set_dir;
	chip->get_dir = gpio_rv1106_get_dir;
	chip->set_value = gpio_rv1106_set_value;
	chip->get_value = gpio_rv1106_get_value;
	chip->to_irq = gpio_rv1106_to_irq;
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

static void gpio_rv1106_remove(struct device_t * dev)
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

static void gpio_rv1106_suspend(struct device_t * dev)
{
}

static void gpio_rv1106_resume(struct device_t * dev)
{
}

static struct driver_t gpio_rv1106 = {
	.name		= "gpio-rv1106",
	.probe		= gpio_rv1106_probe,
	.remove		= gpio_rv1106_remove,
	.suspend	= gpio_rv1106_suspend,
	.resume		= gpio_rv1106_resume,
};

static __init void gpio_rv1106_driver_init(void)
{
	register_driver(&gpio_rv1106);
}

static __exit void gpio_rv1106_driver_exit(void)
{
	unregister_driver(&gpio_rv1106);
}

driver_initcall(gpio_rv1106_driver_init);
driver_exitcall(gpio_rv1106_driver_exit);
