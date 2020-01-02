/*
 * driver/irq-s5l8930-gpio.c
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

#define GPIO_INTBLK		(0xc00)
#define GPIO_INTDIS(x)	(0x800 + (x) * 4)
#define GPIO_INTEN(x)	(0x840 + (x) * 4)
#define GPIO_INTSTS(x)	(0x880 + (x) * 4)

struct irq_s5l8930_gpio_pdata_t
{
	virtual_addr_t virt;
	int base;
	int nirq;
	int parent;
};

static void irq_s5l8930_gpio_enable(struct irqchip_t * chip, int offset)
{
	struct irq_s5l8930_gpio_pdata_t * pdat = (struct irq_s5l8930_gpio_pdata_t *)chip->priv;
	int bank = offset / 32;
	int field = offset % 32;
	write32(pdat->virt + GPIO_INTEN(bank), 1 << field);
}

static void irq_s5l8930_gpio_disable(struct irqchip_t * chip, int offset)
{
	struct irq_s5l8930_gpio_pdata_t * pdat = (struct irq_s5l8930_gpio_pdata_t *)chip->priv;
	int bank = offset / 32;
	int field = offset % 32;
	write32(pdat->virt + GPIO_INTDIS(bank), 1 << field);
}

static void irq_s5l8930_gpio_settype(struct irqchip_t * chip, int offset, enum irq_type_t type)
{
	struct irq_s5l8930_gpio_pdata_t * pdat = (struct irq_s5l8930_gpio_pdata_t *)chip->priv;
	u32_t val, cfg = 0x0;

	switch(type)
	{
	case IRQ_TYPE_NONE:
		return;
	case IRQ_TYPE_LEVEL_LOW:
		cfg = 0x3;
		break;
	case IRQ_TYPE_LEVEL_HIGH:
		cfg = 0x2;
		break;
	case IRQ_TYPE_EDGE_FALLING:
		cfg = 0x5;
		break;
	case IRQ_TYPE_EDGE_RISING:
		cfg = 0x4;
		break;
	case IRQ_TYPE_EDGE_BOTH:
		cfg = 0x6;
		break;
	default:
		return;
	}
	val = read32(pdat->virt + offset * 4);
	val |= (0x1 << 9);
	write32(pdat->virt + offset * 4, (val & ~(0x7 << 1)) | (cfg << 1));
}

static void irq_s5l8930_gpio_dispatch(struct irqchip_t * chip)
{
	struct irq_s5l8930_gpio_pdata_t * pdat = (struct irq_s5l8930_gpio_pdata_t *)chip->priv;
	int bank, field, offset;
	u32_t blk, sts;

	if((blk = read32(pdat->virt + GPIO_INTBLK)) == 0)
		return;

	bank = __ffs(blk);
	if((sts = read32(pdat->virt + GPIO_INTSTS(bank))) != 0)
	{
		field = __ffs(sts);
		offset = (bank * 32) + field;
		if((offset >= 0) && (offset < chip->nirq))
		{
			(chip->handler[offset].func)(chip->handler[offset].data);
			write32(pdat->virt + GPIO_INTSTS(bank), 1 << field);
		}
	}
}

static struct device_t * irq_s5l8930_gpio_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct irq_s5l8930_gpio_pdata_t * pdat;
	struct irqchip_t * chip;
	struct device_t * dev;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	int base = dt_read_int(n, "interrupt-base", -1);
	int nirq = dt_read_int(n, "interrupt-count", -1);
	int parent = dt_read_int(n, "interrupt-parent", -1);

	if((base < 0) || (nirq <= 0) || !irq_is_valid(parent))
		return NULL;

	pdat = malloc(sizeof(struct irq_s5l8930_gpio_pdata_t));
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

	chip->name = alloc_device_name(dt_read_name(n), -1);
	chip->base = pdat->base;
	chip->nirq = pdat->nirq;
	chip->handler = malloc(sizeof(struct irq_handler_t) * pdat->nirq);
	chip->enable = irq_s5l8930_gpio_enable;
	chip->disable = irq_s5l8930_gpio_disable;
	chip->settype = irq_s5l8930_gpio_settype;
	chip->dispatch = irq_s5l8930_gpio_dispatch;
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

static void irq_s5l8930_gpio_remove(struct device_t * dev)
{
	struct irqchip_t * chip = (struct irqchip_t *)dev->priv;
	struct irq_s5l8930_gpio_pdata_t * pdat = (struct irq_s5l8930_gpio_pdata_t *)chip->priv;

	if(chip)
	{
		unregister_sub_irqchip(pdat->parent, chip);
		free_device_name(chip->name);
		free(chip->handler);
		free(chip->priv);
		free(chip);
	}
}

static void irq_s5l8930_gpio_suspend(struct device_t * dev)
{
}

static void irq_s5l8930_gpio_resume(struct device_t * dev)
{
}

static struct driver_t irq_s5l8930_gpio = {
	.name		= "irq-s5l8930-gpio",
	.probe		= irq_s5l8930_gpio_probe,
	.remove		= irq_s5l8930_gpio_remove,
	.suspend	= irq_s5l8930_gpio_suspend,
	.resume		= irq_s5l8930_gpio_resume,
};

static __init void irq_s5l8930_gpio_driver_init(void)
{
	register_driver(&irq_s5l8930_gpio);
}

static __exit void irq_s5l8930_gpio_driver_exit(void)
{
	unregister_driver(&irq_s5l8930_gpio);
}

driver_initcall(irq_s5l8930_gpio_driver_init);
driver_exitcall(irq_s5l8930_gpio_driver_exit);
