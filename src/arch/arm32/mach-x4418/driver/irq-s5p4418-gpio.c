/*
 * driver/irq-s5p4418-gpio.c
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

struct irq_s5p4418_gpio_pdata_t
{
	virtual_addr_t virt;
	int base;
	int nirq;
	int parent;
};

static void irq_s5p4418_gpio_enable(struct irqchip_t * chip, int offset)
{
	struct irq_s5p4418_gpio_pdata_t * pdat = (struct irq_s5p4418_gpio_pdata_t *)chip->priv;
	write32(pdat->virt + GPIO_INTENB, (read32(pdat->virt + GPIO_INTENB) | (0x1 << offset)));
}

static void irq_s5p4418_gpio_disable(struct irqchip_t * chip, int offset)
{
	struct irq_s5p4418_gpio_pdata_t * pdat = (struct irq_s5p4418_gpio_pdata_t *)chip->priv;
	write32(pdat->virt + GPIO_INTENB, (read32(pdat->virt + GPIO_INTENB) & ~(0x1 << offset)));
}

static void irq_s5p4418_gpio_settype(struct irqchip_t * chip, int offset, enum irq_type_t type)
{
	struct irq_s5p4418_gpio_pdata_t * pdat = (struct irq_s5p4418_gpio_pdata_t *)chip->priv;
	u32_t val, cfg = 0x0;

	switch(type)
	{
	case IRQ_TYPE_NONE:
		break;
	case IRQ_TYPE_LEVEL_LOW:
		cfg = 0x0;
		break;
	case IRQ_TYPE_LEVEL_HIGH:
		cfg = 0x1;
		break;
	case IRQ_TYPE_EDGE_FALLING:
		cfg = 0x2;
		break;
	case IRQ_TYPE_EDGE_RISING:
		cfg = 0x3;
		break;
	case IRQ_TYPE_EDGE_BOTH:
		cfg = 0x4;
		break;
	default:
		return;
	}

	if(type != IRQ_TYPE_NONE)
	{
		if(offset < 16)
		{
			val = read32(pdat->virt +  + GPIO_DETMODE0);
			val &= ~(0x3 << (offset << 0x1));
			val |= (cfg & 0x3) << (offset << 0x1);
			write32(pdat->virt +  + GPIO_DETMODE0, val);
		}
		else if(offset < 32)
		{
			val = read32(pdat->virt +  + GPIO_DETMODE1);
			val &= ~(0x3 << ((offset - 16) << 0x1));
			val |= (cfg & 0x3) << ((offset - 16) << 0x1);
			write32(pdat->virt +  + GPIO_DETMODE1, val);
		}

		val = read32(pdat->virt +  + GPIO_DETMODEEX);
		if(cfg & 0x4)
			val |= 0x1 << offset;
		else
			val &= ~(0x1 << offset);
		write32(pdat->virt +  + GPIO_DETMODEEX, val);

		val = read32(pdat->virt +  + GPIO_DETENB);
		val |= 0x1 << offset;
		write32(pdat->virt +  + GPIO_DETENB, val);
	}
	else
	{
		val = read32(pdat->virt +  + GPIO_DETENB);
		val &= ~(0x1 << offset);
		write32(pdat->virt +  + GPIO_DETENB, val);
	}
}

static void irq_s5p4418_gpio_dispatch(struct irqchip_t * chip)
{
	struct irq_s5p4418_gpio_pdata_t * pdat = (struct irq_s5p4418_gpio_pdata_t *)chip->priv;
	u32_t det = read32(pdat->virt + GPIO_DET);

	if(det != 0)
	{
		u32_t offset = __ffs(det);
		if((offset >= 0) && (offset < chip->nirq))
		{
			(chip->handler[offset].func)(chip->handler[offset].data);
			write32(pdat->virt + GPIO_DET, (0x1 << offset));
		}
	}
}

static struct device_t * irq_s5p4418_gpio_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct irq_s5p4418_gpio_pdata_t * pdat;
	struct irqchip_t * chip;
	struct device_t * dev;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	int base = dt_read_int(n, "interrupt-base", -1);
	int nirq = dt_read_int(n, "interrupt-count", -1);
	int parent = dt_read_int(n, "interrupt-parent", -1);

	if((base < 0) || (nirq <= 0) || !irq_is_valid(parent))
		return NULL;

	pdat = malloc(sizeof(struct irq_s5p4418_gpio_pdata_t));
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
	chip->enable = irq_s5p4418_gpio_enable;
	chip->disable = irq_s5p4418_gpio_disable;
	chip->settype = irq_s5p4418_gpio_settype;
	chip->dispatch = irq_s5p4418_gpio_dispatch;
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

static void irq_s5p4418_gpio_remove(struct device_t * dev)
{
	struct irqchip_t * chip = (struct irqchip_t *)dev->priv;
	struct irq_s5p4418_gpio_pdata_t * pdat = (struct irq_s5p4418_gpio_pdata_t *)chip->priv;

	if(chip)
	{
		unregister_sub_irqchip(pdat->parent, chip);
		free_device_name(chip->name);
		free(chip->handler);
		free(chip->priv);
		free(chip);
	}
}

static void irq_s5p4418_gpio_suspend(struct device_t * dev)
{
}

static void irq_s5p4418_gpio_resume(struct device_t * dev)
{
}

static struct driver_t irq_s5p4418_gpio = {
	.name		= "irq-s5p4418-gpio",
	.probe		= irq_s5p4418_gpio_probe,
	.remove		= irq_s5p4418_gpio_remove,
	.suspend	= irq_s5p4418_gpio_suspend,
	.resume		= irq_s5p4418_gpio_resume,
};

static __init void irq_s5p4418_gpio_driver_init(void)
{
	register_driver(&irq_s5p4418_gpio);
}

static __exit void irq_s5p4418_gpio_driver_exit(void)
{
	unregister_driver(&irq_s5p4418_gpio);
}

driver_initcall(irq_s5p4418_gpio_driver_init);
driver_exitcall(irq_s5p4418_gpio_driver_exit);
