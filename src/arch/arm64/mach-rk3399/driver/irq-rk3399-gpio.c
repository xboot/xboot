/*
 * driver/irq-rk3399-gpio.c
 *
 * Copyright(c) 2007-2020 Jianjun Jiang <8192542@qq.com>
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
#include <interrupt/interrupt.h>

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

struct irq_rk3399_gpio_pdata_t
{
	virtual_addr_t virt;
	int base;
	int nirq;
	int parent;
	u32_t both;
};

static void irq_rk3399_gpio_enable(struct irqchip_t * chip, int offset)
{
	struct irq_rk3399_gpio_pdata_t * pdat = (struct irq_rk3399_gpio_pdata_t *)chip->priv;

	write32(pdat->virt + GPIO_INTMASK, (read32(pdat->virt + GPIO_INTMASK) & ~(0x1 << offset)));
	write32(pdat->virt + GPIO_INTEN, (read32(pdat->virt + GPIO_INTEN) | (0x1 << offset)));
}

static void irq_rk3399_gpio_disable(struct irqchip_t * chip, int offset)
{
	struct irq_rk3399_gpio_pdata_t * pdat = (struct irq_rk3399_gpio_pdata_t *)chip->priv;

	write32(pdat->virt + GPIO_INTMASK, (read32(pdat->virt + GPIO_INTMASK) | (0x1 << offset)));
	write32(pdat->virt + GPIO_INTEN, (read32(pdat->virt + GPIO_INTEN) & ~(0x1 << offset)));
}

static void irq_rk3399_gpio_settype(struct irqchip_t * chip, int offset, enum irq_type_t type)
{
	struct irq_rk3399_gpio_pdata_t * pdat = (struct irq_rk3399_gpio_pdata_t *)chip->priv;

	switch(type)
	{
	case IRQ_TYPE_NONE:
		pdat->both &= ~(0x1 << offset);
		break;

	case IRQ_TYPE_LEVEL_LOW:
		pdat->both &= ~(0x1 << offset);
		write32(pdat->virt + GPIO_INTTYPE_LEVEL, (read32(pdat->virt + GPIO_INTTYPE_LEVEL) & ~(0x1 << offset)));
		write32(pdat->virt + GPIO_INT_POLARITY, (read32(pdat->virt + GPIO_INT_POLARITY) & ~(0x1 << offset)));
		break;

	case IRQ_TYPE_LEVEL_HIGH:
		pdat->both &= ~(0x1 << offset);
		write32(pdat->virt + GPIO_INTTYPE_LEVEL, (read32(pdat->virt + GPIO_INTTYPE_LEVEL) & ~(0x1 << offset)));
		write32(pdat->virt + GPIO_INT_POLARITY, (read32(pdat->virt + GPIO_INT_POLARITY) | (0x1 << offset)));
		break;

	case IRQ_TYPE_EDGE_FALLING:
		pdat->both &= ~(0x1 << offset);
		write32(pdat->virt + GPIO_INTTYPE_LEVEL, (read32(pdat->virt + GPIO_INTTYPE_LEVEL) | (0x1 << offset)));
		write32(pdat->virt + GPIO_INT_POLARITY, (read32(pdat->virt + GPIO_INT_POLARITY) & ~(0x1 << offset)));
		break;

	case IRQ_TYPE_EDGE_RISING:
		pdat->both &= ~(0x1 << offset);
		write32(pdat->virt + GPIO_INTTYPE_LEVEL, (read32(pdat->virt + GPIO_INTTYPE_LEVEL) | (0x1 << offset)));
		write32(pdat->virt + GPIO_INT_POLARITY, (read32(pdat->virt + GPIO_INT_POLARITY) | (0x1 << offset)));
		break;

	case IRQ_TYPE_EDGE_BOTH:
		pdat->both |= (0x1 << offset);
		write32(pdat->virt + GPIO_INTTYPE_LEVEL, (read32(pdat->virt + GPIO_INTTYPE_LEVEL) | (0x1 << offset)));
		if(read32(pdat->virt + GPIO_EXT_PORT) & (0x1 << offset))
			write32(pdat->virt + GPIO_INT_POLARITY, (read32(pdat->virt + GPIO_INT_POLARITY) & ~(0x1 << offset)));
		else
			write32(pdat->virt + GPIO_INT_POLARITY, (read32(pdat->virt + GPIO_INT_POLARITY) | (0x1 << offset)));
		break;

	default:
		break;
	}
}

static void irq_rk3399_gpio_dispatch(struct irqchip_t * chip)
{
	struct irq_rk3399_gpio_pdata_t * pdat = (struct irq_rk3399_gpio_pdata_t *)chip->priv;
	u32_t pend = read32(pdat->virt + GPIO_INT_STATUS);

	if(pend != 0)
	{
		u32_t offset = __ffs(pend);
		if((offset >= 0) && (offset < chip->nirq))
		{
			(chip->handler[offset].func)(chip->handler[offset].data);
			if(pdat->both & (0x1 << offset))
			{
				write32(pdat->virt + GPIO_INTTYPE_LEVEL, (read32(pdat->virt + GPIO_INTTYPE_LEVEL) | (0x1 << offset)));
				if(read32(pdat->virt + GPIO_EXT_PORT) & (0x1 << offset))
					write32(pdat->virt + GPIO_INT_POLARITY, (read32(pdat->virt + GPIO_INT_POLARITY) & ~(0x1 << offset)));
				else
					write32(pdat->virt + GPIO_INT_POLARITY, (read32(pdat->virt + GPIO_INT_POLARITY) | (0x1 << offset)));
			}
			write32(pdat->virt + GPIO_PORTS_EOI, (read32(pdat->virt + GPIO_PORTS_EOI) | (0x1 << offset)));
		}
	}
}

static struct device_t * irq_rk3399_gpio_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct irq_rk3399_gpio_pdata_t * pdat;
	struct irqchip_t * chip;
	struct device_t * dev;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	int base = dt_read_int(n, "interrupt-base", -1);
	int nirq = dt_read_int(n, "interrupt-count", -1);
	int parent = dt_read_int(n, "interrupt-parent", -1);

	if((base < 0) || (nirq <= 0) || !irq_is_valid(parent))
		return NULL;

	pdat = malloc(sizeof(struct irq_rk3399_gpio_pdata_t));
	if(!pdat)
		return NULL;

	chip = malloc(sizeof(struct irqchip_t));
	if(!chip)
	{
		free(pdat);
		return NULL;
	}

	pdat->virt = virt;
	pdat->base = base;
	pdat->nirq = nirq;
	pdat->parent = parent;
	pdat->both = 0;

	chip->name = alloc_device_name(dt_read_name(n), -1);
	chip->base = pdat->base;
	chip->nirq = pdat->nirq;
	chip->handler = malloc(sizeof(struct irq_handler_t) * pdat->nirq);
	chip->enable = irq_rk3399_gpio_enable;
	chip->disable = irq_rk3399_gpio_disable;
	chip->settype = irq_rk3399_gpio_settype;
	chip->dispatch = irq_rk3399_gpio_dispatch;
	chip->priv = pdat;

	if(!(dev = register_sub_irqchip(pdat->parent, chip, drv)))
	{
		free_device_name(chip->name);
		free(chip->handler);
		free(chip->priv);
		free(chip);
		return NULL;
	}
	return dev;
}

static void irq_rk3399_gpio_remove(struct device_t * dev)
{
	struct irqchip_t * chip = (struct irqchip_t *)dev->priv;
	struct irq_rk3399_gpio_pdata_t * pdat = (struct irq_rk3399_gpio_pdata_t *)chip->priv;

	if(chip)
	{
		unregister_sub_irqchip(pdat->parent, chip);
		free_device_name(chip->name);
		free(chip->handler);
		free(chip->priv);
		free(chip);
	}
}

static void irq_rk3399_gpio_suspend(struct device_t * dev)
{
}

static void irq_rk3399_gpio_resume(struct device_t * dev)
{
}

static struct driver_t irq_rk3399_gpio = {
	.name		= "irq-rk3399-gpio",
	.probe		= irq_rk3399_gpio_probe,
	.remove		= irq_rk3399_gpio_remove,
	.suspend	= irq_rk3399_gpio_suspend,
	.resume		= irq_rk3399_gpio_resume,
};

static __init void irq_rk3399_gpio_driver_init(void)
{
	register_driver(&irq_rk3399_gpio);
}

static __exit void irq_rk3399_gpio_driver_exit(void)
{
	unregister_driver(&irq_rk3399_gpio);
}

driver_initcall(irq_rk3399_gpio_driver_init);
driver_exitcall(irq_rk3399_gpio_driver_exit);
