/*
 * driver/irq-exynos4412.c
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
#include <exynos4412/reg-combiner.h>
#include <interrupt/interrupt.h>
#include <arm32.h>

enum {
	CPU_CTRL 			= 0x0000,
	CPU_PRIMASK 		= 0x0004,
	CPU_BINPOINT 		= 0x0008,
	CPU_INTACK 			= 0x000c,
	CPU_EOI 			= 0x0010,
	CPU_RUNNINGPRI 		= 0x0014,
	CPU_HIGHPRI 		= 0x0018,

	DIST_CTRL 			= 0x1000,
	DIST_CTR 			= 0x1004,
	DIST_ENABLE_SET 	= 0x1100,
	DIST_ENABLE_CLEAR 	= 0x1180,
	DIST_PENDING_SET 	= 0x1200,
	DIST_PENDING_CLEAR	= 0x1280,
	DIST_ACTIVE_BIT		= 0x1300,
	DIST_PRI			= 0x1400,
	DIST_TARGET			= 0x1800,
	DIST_CONFIG			= 0x1c00,
	DIST_SOFTINT		= 0x1f00,
};

struct irq_exynos4412_pdata_t
{
	virtual_addr_t virt;
	int base;
	int nirq;
};

static void irq_exynos4412_enable(struct irqchip_t * chip, int offset)
{
	struct irq_exynos4412_pdata_t * pdat = (struct irq_exynos4412_pdata_t *)chip->priv;
	int irq = chip->base + offset;
	write32(pdat->virt + DIST_ENABLE_SET + (irq / 32) * 4, 1 << (irq % 32));
}

static void irq_exynos4412_disable(struct irqchip_t * chip, int offset)
{
	struct irq_exynos4412_pdata_t * pdat = (struct irq_exynos4412_pdata_t *)chip->priv;
	int irq = chip->base + offset;
	write32(pdat->virt + DIST_ENABLE_CLEAR + (irq / 32) * 4, 1 << (irq % 32));
}

static void irq_exynos4412_settype(struct irqchip_t * chip, int offset, enum irq_type_t type)
{
}

static void irq_exynos4412_dispatch(struct irqchip_t * chip)
{
	struct irq_exynos4412_pdata_t * pdat = (struct irq_exynos4412_pdata_t *)chip->priv;
	int irq = read32(pdat->virt + CPU_INTACK) & 0x3ff;
	int offset = irq - chip->base;

	if((offset >= 0) && (offset < chip->nirq))
	{
		(chip->handler[offset].func)(chip->handler[offset].data);
		write32(pdat->virt + CPU_EOI, irq);
	}
}

static void exynos4412_dist_init(virtual_addr_t virt)
{
	u32_t gic_irqs;
	u32_t cpumask;
	int i;

	write32(virt + DIST_CTRL, 0x0);

	/*
	 * Find out how many interrupts are supported.
	 * The GIC only supports up to 1020 interrupt sources.
	 */
	gic_irqs = read32(virt + DIST_CTR) & 0x1f;
	gic_irqs = (gic_irqs + 1) * 32;
	if(gic_irqs > 1020)
		gic_irqs = 1020;

	/*
	 * Set all global interrupts to this CPU only.
	 */
	cpumask = 1 << smp_processor_id();
	cpumask |= cpumask << 8;
	cpumask |= cpumask << 16;
	for(i = 32; i < gic_irqs; i += 4)
		write32(virt + DIST_TARGET + i * 4 / 4, cpumask);

	/*
	 * Set all global interrupts to be level triggered, active low.
	 */
	for(i = 32; i < gic_irqs; i += 16)
		write32(virt + DIST_CONFIG + i * 4 / 16, 0);

	/*
	 * Set priority on all global interrupts.
	 */
	for(i = 32; i < gic_irqs; i += 4)
		write32(virt + DIST_PRI + i * 4 / 4, 0xa0a0a0a0);

	/*
	 * Disable all interrupts, leave the SGI and PPI alone
	 * as these enables are banked registers.
	 */
	for(i = 32; i < gic_irqs; i += 32)
		write32(virt + DIST_ENABLE_CLEAR + i * 4 / 32, 0xffffffff);

	write32(virt + DIST_CTRL, 0x1);
}

static void exynos4412_cpu_init(virtual_addr_t virt)
{
	int i;

	/*
	 * Deal with the banked SGI and PPI interrupts - enable all
	 * SGI interrupts, ensure all PPI interrupts are disabled.
	 */
	write32(virt + DIST_ENABLE_CLEAR, 0xffff0000);
	write32(virt + DIST_ENABLE_SET, 0x0000ffff);

	/*
	 * Set priority on SGI and PPI interrupts
	 */
	for(i = 0; i < 32; i += 4)
		write32(virt + DIST_PRI + i * 4 / 4, 0xa0a0a0a0);

	write32(virt + CPU_PRIMASK, 0xf0);
	write32(virt + CPU_CTRL, 0x1);
}

static void exynos4412_combiner_init(virtual_addr_t virt)
{
	int i;

	for(i = 0; i < 5; i++)
		write32(virt + COMBINER_ENABLE_CLEAR + i * 0x10, 0xffffffff);
}

static struct device_t * irq_exynos4412_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct irq_exynos4412_pdata_t * pdat;
	struct irqchip_t * chip;
	struct device_t * dev;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	int base = dt_read_int(n, "interrupt-base", -1);
	int nirq = dt_read_int(n, "interrupt-count", -1);

	if((base < 0) || (nirq <= 0))
		return NULL;

	pdat = malloc(sizeof(struct irq_exynos4412_pdata_t));
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
	chip->enable = irq_exynos4412_enable;
	chip->disable = irq_exynos4412_disable;
	chip->settype = irq_exynos4412_settype;
	chip->dispatch = irq_exynos4412_dispatch;
	chip->priv = pdat;

	exynos4412_dist_init(pdat->virt);
	exynos4412_cpu_init(pdat->virt);
	exynos4412_combiner_init(phys_to_virt(EXYNOS4412_COMBINER_BASE));
	arm32_interrupt_enable();

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

static void irq_exynos4412_remove(struct device_t * dev)
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

static void irq_exynos4412_suspend(struct device_t * dev)
{
}

static void irq_exynos4412_resume(struct device_t * dev)
{
}

static struct driver_t irq_exynos4412 = {
	.name		= "irq-exynos4412",
	.probe		= irq_exynos4412_probe,
	.remove		= irq_exynos4412_remove,
	.suspend	= irq_exynos4412_suspend,
	.resume		= irq_exynos4412_resume,
};

static __init void irq_exynos4412_driver_init(void)
{
	register_driver(&irq_exynos4412);
}

static __exit void irq_exynos4412_driver_exit(void)
{
	unregister_driver(&irq_exynos4412);
}

driver_initcall(irq_exynos4412_driver_init);
driver_exitcall(irq_exynos4412_driver_exit);
