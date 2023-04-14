/*
 * driver/irq-rv1106-gpio.c
 *
 * Copyright(c) 2007-2023 Jianjun Jiang <8192542@qq.com>
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

struct irq_rv1106_gpio_pdata_t
{
	virtual_addr_t virt;
	int base;
	int nirq;
	int parent;
};

static void irq_rv1106_gpio_enable(struct irqchip_t * chip, int offset)
{
	struct irq_rv1106_gpio_pdata_t * pdat = (struct irq_rv1106_gpio_pdata_t *)chip->priv;

	write32(pdat->virt + GPIO_INT_MASK_L + (offset & 0xf0) ? 0x4 : 0x0, (0x00010000 << (offset & 0x0f)));
	write32(pdat->virt + GPIO_INT_EN_L + (offset & 0xf0) ? 0x4 : 0x0, (0x00010001 << (offset & 0x0f)));
}

static void irq_rv1106_gpio_disable(struct irqchip_t * chip, int offset)
{
	struct irq_rv1106_gpio_pdata_t * pdat = (struct irq_rv1106_gpio_pdata_t *)chip->priv;

	write32(pdat->virt + GPIO_INT_MASK_L + (offset & 0xf0) ? 0x4 : 0x0, (0x00010001 << (offset & 0x0f)));
	write32(pdat->virt + GPIO_INT_EN_L + (offset & 0xf0) ? 0x4 : 0x0, (0x00010000 << (offset & 0x0f)));
}

static void irq_rv1106_gpio_settype(struct irqchip_t * chip, int offset, enum irq_type_t type)
{
	struct irq_rv1106_gpio_pdata_t * pdat = (struct irq_rv1106_gpio_pdata_t *)chip->priv;

	switch(type)
	{
	case IRQ_TYPE_NONE:
		write32(pdat->virt + GPIO_INT_BOTHEDGE_L + (offset & 0xf0) ? 0x4 : 0x0, (0x00010000 << (offset & 0x0f)));
		break;

	case IRQ_TYPE_LEVEL_LOW:
		write32(pdat->virt + GPIO_INT_BOTHEDGE_L + (offset & 0xf0) ? 0x4 : 0x0, (0x00010000 << (offset & 0x0f)));
		write32(pdat->virt + GPIO_INT_TYPE_L + (offset & 0xf0) ? 0x4 : 0x0, (0x00010000 << (offset & 0x0f)));
		write32(pdat->virt + GPIO_INT_POLARITY_L + (offset & 0xf0) ? 0x4 : 0x0, (0x00010000 << (offset & 0x0f)));
		break;

	case IRQ_TYPE_LEVEL_HIGH:
		write32(pdat->virt + GPIO_INT_BOTHEDGE_L + (offset & 0xf0) ? 0x4 : 0x0, (0x00010000 << (offset & 0x0f)));
		write32(pdat->virt + GPIO_INT_TYPE_L + (offset & 0xf0) ? 0x4 : 0x0, (0x00010000 << (offset & 0x0f)));
		write32(pdat->virt + GPIO_INT_POLARITY_L + (offset & 0xf0) ? 0x4 : 0x0, (0x00010001 << (offset & 0x0f)));
		break;

	case IRQ_TYPE_EDGE_FALLING:
		write32(pdat->virt + GPIO_INT_BOTHEDGE_L + (offset & 0xf0) ? 0x4 : 0x0, (0x00010000 << (offset & 0x0f)));
		write32(pdat->virt + GPIO_INT_TYPE_L + (offset & 0xf0) ? 0x4 : 0x0, (0x00010001 << (offset & 0x0f)));
		write32(pdat->virt + GPIO_INT_POLARITY_L + (offset & 0xf0) ? 0x4 : 0x0, (0x00010000 << (offset & 0x0f)));
		break;

	case IRQ_TYPE_EDGE_RISING:
		write32(pdat->virt + GPIO_INT_BOTHEDGE_L + (offset & 0xf0) ? 0x4 : 0x0, (0x00010000 << (offset & 0x0f)));
		write32(pdat->virt + GPIO_INT_TYPE_L + (offset & 0xf0) ? 0x4 : 0x0, (0x00010001 << (offset & 0x0f)));
		write32(pdat->virt + GPIO_INT_POLARITY_L + (offset & 0xf0) ? 0x4 : 0x0, (0x00010001 << (offset & 0x0f)));
		break;

	case IRQ_TYPE_EDGE_BOTH:
		write32(pdat->virt + GPIO_INT_BOTHEDGE_L + (offset & 0xf0) ? 0x4 : 0x0, (0x00010001 << (offset & 0x0f)));
		break;

	default:
		break;
	}
}

static void irq_rv1106_gpio_dispatch(struct irqchip_t * chip)
{
	struct irq_rv1106_gpio_pdata_t * pdat = (struct irq_rv1106_gpio_pdata_t *)chip->priv;
	u32_t pend = read32(pdat->virt + GPIO_INT_STATUS);

	if(pend != 0)
	{
		u32_t offset = __ffs(pend);
		if((offset >= 0) && (offset < chip->nirq))
		{
			(chip->handler[offset].func)(chip->handler[offset].data);
			write32(pdat->virt + GPIO_PORT_EOI_L + (offset & 0xf0) ? 0x4 : 0x0, (0x00010001 << (offset & 0x0f)));
		}
	}
}

static struct device_t * irq_rv1106_gpio_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct irq_rv1106_gpio_pdata_t * pdat;
	struct irqchip_t * chip;
	struct device_t * dev;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	int base = dt_read_int(n, "interrupt-base", -1);
	int nirq = dt_read_int(n, "interrupt-count", -1);
	int parent = dt_read_int(n, "interrupt-parent", -1);

	if((base < 0) || (nirq <= 0) || !irq_is_valid(parent))
		return NULL;

	pdat = malloc(sizeof(struct irq_rv1106_gpio_pdata_t));
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
	chip->enable = irq_rv1106_gpio_enable;
	chip->disable = irq_rv1106_gpio_disable;
	chip->settype = irq_rv1106_gpio_settype;
	chip->dispatch = irq_rv1106_gpio_dispatch;
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

static void irq_rv1106_gpio_remove(struct device_t * dev)
{
	struct irqchip_t * chip = (struct irqchip_t *)dev->priv;
	struct irq_rv1106_gpio_pdata_t * pdat = (struct irq_rv1106_gpio_pdata_t *)chip->priv;

	if(chip)
	{
		unregister_sub_irqchip(pdat->parent, chip);
		free_device_name(chip->name);
		free(chip->handler);
		free(chip->priv);
		free(chip);
	}
}

static void irq_rv1106_gpio_suspend(struct device_t * dev)
{
}

static void irq_rv1106_gpio_resume(struct device_t * dev)
{
}

static struct driver_t irq_rv1106_gpio = {
	.name		= "irq-rv1106-gpio",
	.probe		= irq_rv1106_gpio_probe,
	.remove		= irq_rv1106_gpio_remove,
	.suspend	= irq_rv1106_gpio_suspend,
	.resume		= irq_rv1106_gpio_resume,
};

static __init void irq_rv1106_gpio_driver_init(void)
{
	register_driver(&irq_rv1106_gpio);
}

static __exit void irq_rv1106_gpio_driver_exit(void)
{
	unregister_driver(&irq_rv1106_gpio);
}

driver_initcall(irq_rv1106_gpio_driver_init);
driver_exitcall(irq_rv1106_gpio_driver_exit);
