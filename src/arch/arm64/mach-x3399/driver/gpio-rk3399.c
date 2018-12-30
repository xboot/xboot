/*
 * driver/gpio-rk3399.c
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
#include <rk3399/reg-pmu-grf.h>
#include <rk3399/reg-grf.h>

enum {
	GPIO_SWPORT_DR		= 0x00,
	GPIO_SWPORT_DDR		= 0x04,
	GPIO_INTEN			= 0x30,
	GPIO_INTMASK		= 0x34,
	GPIO_INTTYPE_LEVEL	= 0x38,
	GPIO_INT_POLARITY	= 0x3c,
	GPIO_INT_STATUS		= 0x40,
	GPIO_INT_RAWSTATUS	= 0x44,
	GPIO_DEBOUNCE		= 0x48,
	GPIO_PORTS_EOI		= 0x4c,
	GPIO_EXT_PORT		= 0x50,
	GPIO_LS_SYNC		= 0x60,
};

struct gpio_rk3399_pdata_t
{
	virtual_addr_t virt;
	virtual_addr_t pmugrf;
	virtual_addr_t grf;
	int base;
	int ngpio;
	int oirq;
};

static void gpio_rk3399_set_cfg(struct gpiochip_t * chip, int offset, int cfg)
{
	struct gpio_rk3399_pdata_t * pdat = (struct gpio_rk3399_pdata_t *)chip->priv;
	virtual_addr_t addr;
	u32_t val;

	if(offset >= chip->ngpio)
		return;

	if(pdat->base < 32)
	{
		addr = pdat->pmugrf + PMU_GRF_GPIO0A_IOMUX + ((offset >> 3) << 2);
		val = (((0x3 << 16) | (cfg & 0x3)) << ((offset & 0x7) << 1));
		write32(addr, val);
	}
	else if(pdat->base < 64)
	{
		addr = pdat->pmugrf + PMU_GRF_GPIO1A_IOMUX + ((offset >> 3) << 2);
		val = (((0x3 << 16) | (cfg & 0x3)) << ((offset & 0x7) << 1));
		write32(addr, val);
	}
	else if(pdat->base < 96)
	{
		addr = pdat->grf + (GRF_GPIO2A_IOMUX) + ((offset >> 3) << 2);
		val = (((0x3 << 16) | (cfg & 0x3)) << ((offset & 0x7) << 1));
		write32(addr, val);
	}
	else if(pdat->base < 128)
	{
		addr = pdat->grf + (GRF_GPIO3A_IOMUX) + ((offset >> 3) << 2);
		val = (((0x3 << 16) | (cfg & 0x3)) << ((offset & 0x7) << 1));
		write32(addr, val);
	}
	else if(pdat->base < 160)
	{
		addr = pdat->grf + (GRF_GPIO4A_IOMUX) + ((offset >> 3) << 2);
		val = (((0x3 << 16) | (cfg & 0x3)) << ((offset & 0x7) << 1));
		write32(addr, val);
	}
}

static int gpio_rk3399_get_cfg(struct gpiochip_t * chip, int offset)
{
	struct gpio_rk3399_pdata_t * pdat = (struct gpio_rk3399_pdata_t *)chip->priv;
	virtual_addr_t addr;
	u32_t val;

	if(offset >= chip->ngpio)
		return 0;

	if(pdat->base < 32)
	{
		addr = pdat->pmugrf + PMU_GRF_GPIO0A_IOMUX + ((offset >> 3) << 2);
		val = (read32(addr) >> ((offset & 0x7) << 1)) & 0x3;
		return val;
	}
	else if(pdat->base < 64)
	{
		addr = pdat->pmugrf + PMU_GRF_GPIO1A_IOMUX + ((offset >> 3) << 2);
		val = (read32(addr) >> ((offset & 0x7) << 1)) & 0x3;
		return val;
	}
	else if(pdat->base < 96)
	{
		addr = pdat->grf + GRF_GPIO2A_IOMUX + ((offset >> 3) << 2);
		val = (read32(addr) >> ((offset & 0x7) << 1)) & 0x3;
		return val;
	}
	else if(pdat->base < 128)
	{
		addr = pdat->grf + GRF_GPIO3A_IOMUX + ((offset >> 3) << 2);
		val = (read32(addr) >> ((offset & 0x7) << 1)) & 0x3;
		return val;
	}
	else if(pdat->base < 160)
	{
		addr = pdat->grf + GRF_GPIO4A_IOMUX + ((offset >> 3) << 2);
		val = (read32(addr) >> ((offset & 0x7) << 1)) & 0x3;
		return val;
	}

	return 0;
}

static void gpio_rk3399_set_pull(struct gpiochip_t * chip, int offset, enum gpio_pull_t pull)
{
	struct gpio_rk3399_pdata_t * pdat = (struct gpio_rk3399_pdata_t *)chip->priv;
	virtual_addr_t addr;
	u32_t val, v;

	if(offset >= chip->ngpio)
		return;

	if(pdat->base < 32)
	{
		if(offset < 16)
		{
			switch(pull)
			{
			case GPIO_PULL_UP:
				v = 0x3;
				break;
			case GPIO_PULL_DOWN:
				v = 0x1;
				break;
			case GPIO_PULL_NONE:
				v = 0x0;
				break;
			default:
				v = 0x0;
				break;
			}
			addr = pdat->pmugrf + (PMU_GRF_GPIO0A_P - 0x00) + ((pdat->base >> 5) * 0x10) + ((offset >> 3) << 2);
			val = (((0x3 << 16) | (v & 0x3)) << ((offset & 0x7) << 1));
			write32(addr, val);
		}
	}
	else if(pdat->base < 64)
	{
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
		addr = pdat->pmugrf + (PMU_GRF_GPIO0A_P - 0x00) + ((pdat->base >> 5) * 0x10) + ((offset >> 3) << 2);
		val = (((0x3 << 16) | (v & 0x3)) << ((offset & 0x7) << 1));
		write32(addr, val);
	}
	else if(pdat->base < 160)
	{
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
		addr = pdat->grf + (GRF_GPIO2A_P - 0x20) + ((pdat->base >> 5) * 0x10) + ((offset >> 3) << 2);
		val = (((0x3 << 16) | (v & 0x3)) << ((offset & 0x7) << 1));
		write32(addr, val);
	}
}

static enum gpio_pull_t gpio_rk3399_get_pull(struct gpiochip_t * chip, int offset)
{
	struct gpio_rk3399_pdata_t * pdat = (struct gpio_rk3399_pdata_t *)chip->priv;
	virtual_addr_t addr;
	u32_t v = 0;

	if(offset >= chip->ngpio)
		return GPIO_PULL_NONE;

	if(pdat->base < 32)
	{
		if(offset < 16)
		{
			addr = pdat->pmugrf + (PMU_GRF_GPIO0A_P - 0x00) + ((pdat->base >> 5) * 0x10) + ((offset >> 3) << 2);
			v = (read32(addr) >> ((offset & 0x7) << 1)) & 0x3;
			switch(v)
			{
			case 0x0:
				return GPIO_PULL_NONE;
			case 0x1:
				return GPIO_PULL_DOWN;
			case 0x2:
				return GPIO_PULL_NONE;
			case 0x3:
				return GPIO_PULL_UP;
			default:
				break;
			}
		}
	}
	else if(pdat->base < 64)
	{
		addr = pdat->pmugrf + (PMU_GRF_GPIO0A_P - 0x00) + ((pdat->base >> 5) * 0x10) + ((offset >> 3) << 2);
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
	}
	else if(pdat->base < 160)
	{
		addr = pdat->grf + (GRF_GPIO2A_P - 0x20) + ((pdat->base >> 5) * 0x10) + ((offset >> 3) << 2);
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
	}
	return GPIO_PULL_NONE;
}

static void gpio_rk3399_set_drv(struct gpiochip_t * chip, int offset, enum gpio_drv_t drv)
{
	struct gpio_rk3399_pdata_t * pdat = (struct gpio_rk3399_pdata_t *)chip->priv;
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

	if(pdat->base < 32)
	{
		if(offset < 16)
		{
			addr = pdat->pmugrf + PMU_GRF_GPIO0A_E + ((offset >> 3) << 2);
			val = (((0x3 << 16) | (v & 0x3)) << ((offset & 0x7) << 1));
			write32(addr, val);
		}
	}
	else if(pdat->base < 64)
	{
		addr = pdat->pmugrf + PMU_GRF_GPIO1A_E + ((offset >> 3) << 2);
		val = (((0x3 << 16) | (v & 0x3)) << ((offset & 0x7) << 1));
		write32(addr, val);
	}
	else if(pdat->base < 96)
	{
		addr = pdat->grf + GRF_GPIO2A_E + ((offset >> 3) << 2);
		val = (((0x3 << 16) | (v & 0x3)) << ((offset & 0x7) << 1));
		write32(addr, val);
	}
	else if(pdat->base < 128)
	{
		if(offset < 8)
		{
		}
		else if(offset < 16)
		{
		}
		else if(offset < 24)
		{
		}
		else if(offset < 32)
		{
			addr = pdat->grf + GRF_GPIO3D_E;
			val = (((0x3 << 16) | (v & 0x3)) << ((offset & 0x7) << 1));
			write32(addr, val);
		}
	}
	else if(pdat->base < 160)
	{
		if(offset < 8)
		{
			addr = pdat->grf + GRF_GPIO4A_E;
			val = (((0x3 << 16) | (v & 0x3)) << ((offset & 0x7) << 1));
			write32(addr, val);
		}
		else if(offset < 16)
		{
		}
		else if(offset < 24)
		{
			addr = pdat->grf + GRF_GPIO4C_E;
			val = (((0x3 << 16) | (v & 0x3)) << ((offset & 0x7) << 1));
			write32(addr, val);
		}
		else if(offset < 32)
		{
			addr = pdat->grf + GRF_GPIO4D_E;
			val = (((0x3 << 16) | (v & 0x3)) << ((offset & 0x7) << 1));
			write32(addr, val);
		}
	}
}

static enum gpio_drv_t gpio_rk3399_get_drv(struct gpiochip_t * chip, int offset)
{
	struct gpio_rk3399_pdata_t * pdat = (struct gpio_rk3399_pdata_t *)chip->priv;
	virtual_addr_t addr;
	u32_t v = 0;

	if(offset >= chip->ngpio)
		return GPIO_DRV_WEAK;

	if(pdat->base < 32)
	{
		if(offset < 16)
		{
			addr = pdat->pmugrf + PMU_GRF_GPIO0A_E + ((offset >> 3) << 2);
			v = (read32(addr) >> ((offset & 0x7) << 1)) & 0x3;
		}
	}
	else if(pdat->base < 64)
	{
		addr = pdat->pmugrf + PMU_GRF_GPIO1A_E + ((offset >> 3) << 2);
		v = (read32(addr) >> ((offset & 0x7) << 1)) & 0x3;
	}
	else if(pdat->base < 96)
	{
		addr = pdat->grf + GRF_GPIO2A_E + ((offset >> 3) << 2);
		v = (read32(addr) >> ((offset & 0x7) << 1)) & 0x3;
	}
	else if(pdat->base < 128)
	{
		if(offset < 8)
		{
		}
		else if(offset < 16)
		{
		}
		else if(offset < 24)
		{
		}
		else if(offset < 32)
		{
			addr = pdat->grf + GRF_GPIO3D_E;
			v = (read32(addr) >> ((offset & 0x7) << 1)) & 0x3;
		}
	}
	else if(pdat->base < 160)
	{
		if(offset < 8)
		{
			addr = pdat->grf + GRF_GPIO4A_E;
			v = (read32(addr) >> ((offset & 0x7) << 1)) & 0x3;
		}
		else if(offset < 16)
		{
		}
		else if(offset < 24)
		{
			addr = pdat->grf + GRF_GPIO4C_E;
			v = (read32(addr) >> ((offset & 0x7) << 1)) & 0x3;
		}
		else if(offset < 32)
		{
			addr = pdat->grf + GRF_GPIO4D_E;
			v = (read32(addr) >> ((offset & 0x7) << 1)) & 0x3;
		}
	}

	if(v == 0x0)
		return GPIO_DRV_WEAK;
	else if(v == 0x1)
		return GPIO_DRV_WEAKER;
	else if(v == 0x2)
		return GPIO_DRV_STRONGER;
	else if(v == 0x3)
		return GPIO_DRV_STRONG;
	return GPIO_DRV_WEAK;
}

static void gpio_rk3399_set_rate(struct gpiochip_t * chip, int offset, enum gpio_rate_t rate)
{
	struct gpio_rk3399_pdata_t * pdat = (struct gpio_rk3399_pdata_t *)chip->priv;
	virtual_addr_t addr;
	u32_t val, v;

	if(offset >= chip->ngpio)
		return;

	switch(rate)
	{
	case GPIO_RATE_SLOW:
		v = 0x0;
		break;
	case GPIO_RATE_FAST:
		v = 0x1;
		break;
	default:
		v = 0x0;
		break;
	}

	if(pdat->base < 32)
	{
		if(offset < 16)
		{
			addr = pdat->pmugrf + PMU_GRF_GPIO0L_SR + ((offset >> 4) << 2);
			val = (((0x1 << 16) | (v & 0x1)) << (offset & 0xf));
			write32(addr, val);
		}
	}
	else if(pdat->base < 64)
	{
		addr = pdat->pmugrf + PMU_GRF_GPIO1L_SR + ((offset >> 4) << 2);
		val = (((0x1 << 16) | (v & 0x1)) << (offset & 0xf));
		write32(addr, val);
	}
	else if(pdat->base < 96)
	{
		addr = pdat->grf + GRF_GPIO2A_SR + ((offset >> 3) << 2);
		val = (((0x1 << 16) | (v & 0x1)) << (offset & 0x7));
		write32(addr, val);
	}
	else if(pdat->base < 128)
	{
		if(offset >= 24)
		{
			addr = pdat->grf + (GRF_GPIO3D_SR - 0xc) + ((offset >> 3) << 2);
			val = (((0x1 << 16) | (v & 0x1)) << (offset & 0x7));
			write32(addr, val);
		}
	}
	else if(pdat->base < 160)
	{
		addr = pdat->grf + GRF_GPIO4A_SR + ((offset >> 3) << 2);
		val = (((0x1 << 16) | (v & 0x1)) << (offset & 0x7));
		write32(addr, val);
	}
}

static enum gpio_rate_t gpio_rk3399_get_rate(struct gpiochip_t * chip, int offset)
{
	struct gpio_rk3399_pdata_t * pdat = (struct gpio_rk3399_pdata_t *)chip->priv;
	virtual_addr_t addr;
	u32_t v = 0;

	if(offset >= chip->ngpio)
		return GPIO_RATE_SLOW;

	if(pdat->base < 32)
	{
		if(offset < 16)
		{
			addr = pdat->pmugrf + PMU_GRF_GPIO0L_SR + ((offset >> 4) << 2);
			v = (read32(addr) >> (offset & 0xf)) & 0x1;
		}
	}
	else if(pdat->base < 64)
	{
		addr = pdat->pmugrf + PMU_GRF_GPIO1L_SR + ((offset >> 4) << 2);
		v = (read32(addr) >> (offset & 0xf)) & 0x1;
	}
	else if(pdat->base < 96)
	{
		addr = pdat->grf + GRF_GPIO2A_SR + ((offset >> 3) << 2);
		v = (read32(addr) >> (offset & 0x7)) & 0x1;
	}
	else if(pdat->base < 128)
	{
		if(offset >= 24)
		{
			addr = pdat->grf + (GRF_GPIO3D_SR - 0xc) + ((offset >> 3) << 2);
			v = (read32(addr) >> (offset & 0x7)) & 0x1;
		}
	}
	else if(pdat->base < 160)
	{
		addr = pdat->grf + GRF_GPIO4A_SR + ((offset >> 3) << 2);
		v = (read32(addr) >> (offset & 0x7)) & 0x1;
	}

	if(v == 0x0)
		return GPIO_RATE_SLOW;
	else if(v == 0x1)
		return GPIO_RATE_FAST;
	return GPIO_RATE_SLOW;
}

static void gpio_rk3399_set_dir(struct gpiochip_t * chip, int offset, enum gpio_direction_t dir)
{
	struct gpio_rk3399_pdata_t * pdat = (struct gpio_rk3399_pdata_t *)chip->priv;
	u32_t val;

	if(offset >= chip->ngpio)
		return;

	switch(dir)
	{
	case GPIO_DIRECTION_INPUT:
		val = read32(pdat->virt + GPIO_SWPORT_DDR);
		val &= ~(1 << offset);
		write32(pdat->virt + GPIO_SWPORT_DDR, val);
		break;

	case GPIO_DIRECTION_OUTPUT:
		val = read32(pdat->virt + GPIO_SWPORT_DDR);
		val |= 1 << offset;
		write32(pdat->virt + GPIO_SWPORT_DDR, val);
		break;

	default:
		break;
	}
}

static enum gpio_direction_t gpio_rk3399_get_dir(struct gpiochip_t * chip, int offset)
{
	struct gpio_rk3399_pdata_t * pdat = (struct gpio_rk3399_pdata_t *)chip->priv;
	u32_t val;

	if(offset >= chip->ngpio)
		return GPIO_DIRECTION_INPUT;

	val = read32(pdat->virt + GPIO_SWPORT_DDR);
	if((val & (1 << offset)))
		return GPIO_DIRECTION_OUTPUT;
	return GPIO_DIRECTION_INPUT;
}

static void gpio_rk3399_set_value(struct gpiochip_t * chip, int offset, int value)
{
	struct gpio_rk3399_pdata_t * pdat = (struct gpio_rk3399_pdata_t *)chip->priv;
	u32_t val;

	if(offset >= chip->ngpio)
		return;

	val = read32(pdat->virt + GPIO_SWPORT_DR);
	val &= ~(1 << offset);
	val |= (!!value) << offset;
	write32(pdat->virt + GPIO_SWPORT_DR, val);
}

static int gpio_rk3399_get_value(struct gpiochip_t * chip, int offset)
{
	struct gpio_rk3399_pdata_t * pdat = (struct gpio_rk3399_pdata_t *)chip->priv;
	u32_t val;

	if(offset >= chip->ngpio)
		return 0;

	val = read32(pdat->virt + GPIO_EXT_PORT);
	return !!(val & (1 << offset));
}

static int gpio_rk3399_to_irq(struct gpiochip_t * chip, int offset)
{
	struct gpio_rk3399_pdata_t * pdat = (struct gpio_rk3399_pdata_t *)chip->priv;

	if((offset >= chip->ngpio) || (pdat->oirq < 0))
		return -1;
	return pdat->oirq + offset;
}

static struct device_t * gpio_rk3399_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct gpio_rk3399_pdata_t * pdat;
	struct gpiochip_t * chip;
	struct device_t * dev;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	int base = dt_read_int(n, "gpio-base", -1);
	int ngpio = dt_read_int(n, "gpio-count", -1);

	if((base < 0) || (ngpio <= 0))
		return NULL;

	pdat = malloc(sizeof(struct gpio_rk3399_pdata_t));
	if(!pdat)
		return NULL;

	chip = malloc(sizeof(struct gpiochip_t));
	if(!chip)
	{
		free(pdat);
		return NULL;
	}

	pdat->virt = virt;
	pdat->pmugrf = phys_to_virt(RK3399_PMU_GRF_BASE);
	pdat->grf = phys_to_virt(RK3399_GRF_BASE);
	pdat->base = base;
	pdat->ngpio = ngpio;
	pdat->oirq = dt_read_int(n, "interrupt-offset", -1);

	chip->name = alloc_device_name(dt_read_name(n), -1);
	chip->base = pdat->base;
	chip->ngpio = pdat->ngpio;
	chip->set_cfg = gpio_rk3399_set_cfg;
	chip->get_cfg = gpio_rk3399_get_cfg;
	chip->set_pull = gpio_rk3399_set_pull;
	chip->get_pull = gpio_rk3399_get_pull;
	chip->set_drv = gpio_rk3399_set_drv;
	chip->get_drv = gpio_rk3399_get_drv;
	chip->set_rate = gpio_rk3399_set_rate;
	chip->get_rate = gpio_rk3399_get_rate;
	chip->set_dir = gpio_rk3399_set_dir;
	chip->get_dir = gpio_rk3399_get_dir;
	chip->set_value = gpio_rk3399_set_value;
	chip->get_value = gpio_rk3399_get_value;
	chip->to_irq = gpio_rk3399_to_irq;
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

static void gpio_rk3399_remove(struct device_t * dev)
{
	struct gpiochip_t * chip = (struct gpiochip_t *)dev->priv;

	if(chip && unregister_gpiochip(chip))
	{
		free_device_name(chip->name);
		free(chip->priv);
		free(chip);
	}
}

static void gpio_rk3399_suspend(struct device_t * dev)
{
}

static void gpio_rk3399_resume(struct device_t * dev)
{
}

static struct driver_t gpio_rk3399 = {
	.name		= "gpio-rk3399",
	.probe		= gpio_rk3399_probe,
	.remove		= gpio_rk3399_remove,
	.suspend	= gpio_rk3399_suspend,
	.resume		= gpio_rk3399_resume,
};

static __init void gpio_rk3399_driver_init(void)
{
	register_driver(&gpio_rk3399);
}

static __exit void gpio_rk3399_driver_exit(void)
{
	unregister_driver(&gpio_rk3399);
}

driver_initcall(gpio_rk3399_driver_init);
driver_exitcall(gpio_rk3399_driver_exit);
