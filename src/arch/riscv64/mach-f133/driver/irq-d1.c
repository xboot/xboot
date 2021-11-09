/*
 * driver/irq-d1.c
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
#include <riscv64.h>
#include <interrupt/interrupt.h>

#define PLIC_PRIO(x)		(0x0000 + (x) * 4)
#define PLIC_IP(x)			(0x1000 + (x) * 4)
#define PLIC_MIE(x)			(0x2000 + (x) * 4)
#define PLIC_SIE(x)			(0x2080 + (x) * 4)
#define PLIC_CTRL			(0x1ffffc)
#define PLIC_MTH			(0x200000)
#define PLIC_MCLAIM			(0x200004)
#define PLIC_STH			(0x201000)
#define PLIC_SCLAIM			(0x201004)

struct irq_d1_pdata_t
{
	virtual_addr_t virt;
	int base;
	int nirq;
};

static void irq_d1_enable(struct irqchip_t * chip, int offset)
{
	struct irq_d1_pdata_t * pdat = (struct irq_d1_pdata_t *)chip->priv;
	int irq = chip->base + offset;
	u32_t val = read32(pdat->virt + PLIC_MIE(irq >> 5));
	val |= (1 << (irq & 0x1f));
	write32(pdat->virt + PLIC_MIE(irq >> 5), val);
}

static void irq_d1_disable(struct irqchip_t * chip, int offset)
{
	struct irq_d1_pdata_t * pdat = (struct irq_d1_pdata_t *)chip->priv;
	int irq = chip->base + offset;
	u32_t val = read32(pdat->virt + PLIC_MIE(irq >> 5));
	val &= ~(1 << (irq & 0x1f));
	write32(pdat->virt + PLIC_MIE(irq >> 5), val);
}

static void irq_d1_settype(struct irqchip_t * chip, int offset, enum irq_type_t type)
{
}

static void irq_d1_dispatch(struct irqchip_t * chip)
{
	struct irq_d1_pdata_t * pdat = (struct irq_d1_pdata_t *)chip->priv;
	u32_t irq = read32(pdat->virt + PLIC_MCLAIM);
	int offset = irq + chip->base;

	if((offset > 0) && (offset < chip->nirq))
	{
		(chip->handler[offset].func)(chip->handler[offset].data);
		write32(pdat->virt + PLIC_MCLAIM, offset);
	}
}

static void plic_init(struct irq_d1_pdata_t * pdat)
{
	u32_t val;
	int i;

	/*
	 * Disable all interrupts
	 */
	for(i = 0; i < pdat->nirq; i++)
	{
		val = read32(pdat->virt + PLIC_MIE(i >> 5));
		val &= ~(1 << (i & 0x1f));
		write32(pdat->virt + PLIC_MIE(i >> 5), val);
	}
	/*
	 * Set all source priorities to one
	 */
	for(i = 0; i < pdat->nirq; i++)
	{
		write32(pdat->virt + PLIC_PRIO(i), 1);
	}
	/*
	 * Clear pending bits
	 */
	for(i = 0; i < pdat->nirq; i++)
	{
		write32(pdat->virt + PLIC_MCLAIM, i);
	}
}

static struct device_t * irq_d1_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct irq_d1_pdata_t * pdat;
	struct irqchip_t * chip;
	struct device_t * dev;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	int base = dt_read_int(n, "interrupt-base", -1);
	int nirq = dt_read_int(n, "interrupt-count", -1);

	if((base < 0) || (nirq <= 0))
		return NULL;

	pdat = malloc(sizeof(struct irq_d1_pdata_t));
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

	chip->name = alloc_device_name(dt_read_name(n), -1);
	chip->base = pdat->base;
	chip->nirq = pdat->nirq;
	chip->handler = malloc(sizeof(struct irq_handler_t) * pdat->nirq);
	chip->enable = irq_d1_enable;
	chip->disable = irq_d1_disable;
	chip->settype = irq_d1_settype;
	chip->dispatch = irq_d1_dispatch;
	chip->priv = pdat;

	plic_init(pdat);
	csr_set(mie, MIE_MEIE);
	csr_set(mstatus, MSTATUS_MIE);

	if(!(dev = register_irqchip(chip, drv)))
	{
		free_device_name(chip->name);
		free(chip->handler);
		free(chip->priv);
		free(chip);
		return NULL;
	}
	return dev;
}

static void irq_d1_remove(struct device_t * dev)
{
	struct irqchip_t * chip = (struct irqchip_t *)dev->priv;

	if(chip)
	{
		unregister_irqchip(chip);
		free_device_name(chip->name);
		free(chip->handler);
		free(chip->priv);
		free(chip);
	}
}

static void irq_d1_suspend(struct device_t * dev)
{
}

static void irq_d1_resume(struct device_t * dev)
{
}

static struct driver_t irq_d1 = {
	.name		= "irq-d1",
	.probe		= irq_d1_probe,
	.remove		= irq_d1_remove,
	.suspend	= irq_d1_suspend,
	.resume		= irq_d1_resume,
};

static __init void irq_d1_driver_init(void)
{
	register_driver(&irq_d1);
}

static __exit void irq_d1_driver_exit(void)
{
	unregister_driver(&irq_d1);
}

driver_initcall(irq_d1_driver_init);
driver_exitcall(irq_d1_driver_exit);
