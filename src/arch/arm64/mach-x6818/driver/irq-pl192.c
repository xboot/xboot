/*
 * driver/irq-pl192.c
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
#include <interrupt/interrupt.h>
#include <arm64.h>

enum {
	VIC_IRQSTATUS		= 0x000,
	VIC_FIQSTATUS		= 0x004,
	VIC_RAWINTR			= 0x008,
	VIC_INTSELECT		= 0x00c,
	VIC_INTENABLE		= 0x010,
	VIC_INTENCLEAR		= 0x014,
	VIC_SOFTINT			= 0x018,
	VIC_SOFTINTCLEAR	= 0x01c,
	VIC_PROTECTION		= 0x020,
	VIC_SWPRIORITYMASK	= 0x024,
	VIC_PRIORITYDAISY	= 0x028,
	VIC_VECTADDR0		= 0x100,
	VIC_VECPRIORITY0	= 0x200,
	VIC_ADDRESS			= 0xf00,
};

struct irq_pl192_pdata_t
{
	virtual_addr_t virt;
	int base;
	int nirq;
};

static void irq_pl192_enable(struct irqchip_t * chip, int offset)
{
	struct irq_pl192_pdata_t * pdat = (struct irq_pl192_pdata_t *)chip->priv;
	write32(pdat->virt + VIC_INTENABLE, (read32(pdat->virt + VIC_INTENABLE) | (0x1 << offset)));
}

static void irq_pl192_disable(struct irqchip_t * chip, int offset)
{
	struct irq_pl192_pdata_t * pdat = (struct irq_pl192_pdata_t *)chip->priv;
	write32(pdat->virt + VIC_INTENCLEAR, (read32(pdat->virt + VIC_INTENCLEAR) | (0x1 << offset)));
}

static void irq_pl192_settype(struct irqchip_t * chip, int offset, enum irq_type_t type)
{
}

static void irq_pl192_dispatch(struct irqchip_t * chip)
{
	struct irq_pl192_pdata_t * pdat = (struct irq_pl192_pdata_t *)chip->priv;
	u32_t val = read32(pdat->virt + VIC_IRQSTATUS);

	if(val != 0)
	{
		u32_t offset = __ffs(val);
		if((offset >= 0) && (offset < chip->nirq))
		{
			(chip->handler[offset].func)(chip->handler[offset].data);
			write32(pdat->virt + VIC_SOFTINTCLEAR, 0x1 << offset);
			write32(pdat->virt + VIC_ADDRESS, 0);
		}
	}
}

static void pl192_ctrl_init(virtual_addr_t virt)
{
	extern void irq(void);
	int i;

	/*
	 * Select irq mode
	 */
	write32(virt + VIC_INTSELECT, 0x00000000);
	/*
	 * Disable all interrupts
	 */
	write32(virt + VIC_INTENABLE, 0x00000000);
	/*
	 * Clear all interrupts
	 */
	write32(virt + VIC_INTENCLEAR, 0xffffffff);
	/*
	 * Clear all irq status
	 */
	write32(virt + VIC_IRQSTATUS, 0x00000000);
	/*
	 * Clear all fiq status
	 */
	write32(virt + VIC_FIQSTATUS, 0x00000000);
	/*
	 * Clear all software interrupts
	 */
	write32(virt + VIC_SOFTINTCLEAR, 0xffffffff);
	/*
	 * Set vic address to zero
	 */
	write32(virt + VIC_ADDRESS, 0x00000000);

	for(i = 0; i < 32; i++)
		write32(virt + VIC_VECPRIORITY0 + 4 * i, 0xf);

//	for(i = 0; i < 32; i++)
//		write32(virt + VIC_VECTADDR0 + 4 * i, (u32_t)irq);
}

static struct device_t * irq_pl192_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct irq_pl192_pdata_t * pdat;
	struct irqchip_t * chip;
	struct device_t * dev;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	u32_t id = (((read32(virt + 0xfec) & 0xff) << 24) |
				((read32(virt + 0xfe8) & 0xff) << 16) |
				((read32(virt + 0xfe4) & 0xff) <<  8) |
				((read32(virt + 0xfe0) & 0xff) <<  0));
	int base = dt_read_int(n, "interrupt-base", -1);
	int nirq = dt_read_int(n, "interrupt-count", -1);

	if(((id >> 12) & 0xff) != 0x41 || (id & 0xfff) != 0x192)
		return NULL;

	if((base < 0) || (nirq <= 0))
		return NULL;

	pdat = malloc(sizeof(struct irq_pl192_pdata_t));
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
	chip->enable = irq_pl192_enable;
	chip->disable = irq_pl192_disable;
	chip->settype = irq_pl192_settype;
	chip->dispatch = irq_pl192_dispatch;
	chip->priv = pdat;

	pl192_ctrl_init(pdat->virt);
	arm64_interrupt_enable();

	if(!register_irqchip(&dev, chip))
	{
		free_device_name(chip->name);
		free(chip->handler);
		free(chip->priv);
		free(chip);
		return NULL;
	}
	dev->driver = drv;

	return dev;
}

static void irq_pl192_remove(struct device_t * dev)
{
	struct irqchip_t * chip = (struct irqchip_t *)dev->priv;

	if(chip && unregister_irqchip(chip))
	{
		free_device_name(chip->name);
		free(chip->handler);
		free(chip->priv);
		free(chip);
	}
}

static void irq_pl192_suspend(struct device_t * dev)
{
}

static void irq_pl192_resume(struct device_t * dev)
{
}

static struct driver_t irq_vic = {
	.name		= "irq-pl192",
	.probe		= irq_pl192_probe,
	.remove		= irq_pl192_remove,
	.suspend	= irq_pl192_suspend,
	.resume		= irq_pl192_resume,
};

static __init void irq_pl192_driver_init(void)
{
	register_driver(&irq_vic);
}

static __exit void irq_pl192_driver_exit(void)
{
	unregister_driver(&irq_vic);
}

driver_initcall(irq_pl192_driver_init);
driver_exitcall(irq_pl192_driver_exit);
