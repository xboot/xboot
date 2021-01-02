/*
 * driver/irq-gic500.c
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
#include <interrupt/interrupt.h>
#include <arm64.h>

enum {
	GICD_CTLR 		= 0x0000,
	GICD_TYPER 		= 0x0004,
	GICD_IIDR 		= 0x0008,
	GICD_STATUSR 	= 0x0010,
	GICD_SETSPI_NSR = 0x0040,
	GICD_CLRSPI_NSR = 0x0048,
	GICD_SETSPI_SR 	= 0x0050,
	GICD_CLRSPI_SR 	= 0x0058,
	GICD_SEIR 		= 0x0068,
	GICD_IGROUPR 	= 0x0080,
	GICD_ISENABLER 	= 0x0100,
	GICD_ICENABLER 	= 0x0180,
	GICD_ISPENDR 	= 0x0200,
	GICD_ICPENDR 	= 0x0280,
	GICD_ISACTIVER 	= 0x0300,
	GICD_ICACTIVER 	= 0x0380,
	GICD_IPRIORITYR	= 0x0400,
	GICD_ITARGETSR 	= 0x0800,
	GICD_ICFGR 		= 0x0c00,
	GICD_IGRPMODR 	= 0x0d00,
	GICD_NSACR 		= 0x0e00,
	GICD_SGIR 		= 0x0f00,
	GICD_CPENDSGIR 	= 0x0f10,
	GICD_SPENDSGIR 	= 0x0f20,
	GICD_IROUTER 	= 0x6100,
	GICD_IDREGS 	= 0xffd0,

	GICR_CTLR 		= (0x100000 + 0x0000),
	GICR_IIDR 		= (0x100000 + 0x0004),
	GICR_TYPER 		= (0x100000 + 0x0008),
	GICR_STATUSR 	= (0x100000 + 0x0010),
	GICR_WAKER 		= (0x100000 + 0x0014),
	GICR_SETLPIR	= (0x100000 + 0x0040),
	GICR_CLRLPIR 	= (0x100000 + 0x0048),
	GICR_PROPBASER 	= (0x100000 + 0x0070),
	GICR_PENDBASER 	= (0x100000 + 0x0078),
	GICR_INVLPIR 	= (0x100000 + 0x00a0),
	GICR_INVALLR 	= (0x100000 + 0x00b0),
	GICR_SYNCR 		= (0x100000 + 0x00c0),
	GICR_IDREGS 	= (0x100000 + 0xffd0),

	GICR_IGROUPR0 	= (0x110000 + 0x0080),
	GICR_ISENABLER0	= (0x110000 + 0x0100),
	GICR_ICENABLER0 = (0x110000 + 0x0180),
	GICR_ISPENDR0 	= (0x110000 + 0x0200),
	GICR_ICPENDR0 	= (0x110000 + 0x0280),
	GICR_ISACTIVER0 = (0x110000 + 0x0300),
	GICR_ICACTIVER0 = (0x110000 + 0x0380),
	GICR_IPRIORITYR = (0x110000 + 0x0400),
	GICR_ICFGR0 	= (0x110000 + 0x0c00),
	GICR_ICFGR1 	= (0x110000 + 0x0c04),
	GICR_IGRPMODR0 	= (0x110000 + 0x0d00),
	GICR_NSACR 		= (0x110000 + 0x0e00),
};

struct irq_gic500_pdata_t
{
	virtual_addr_t virt;
	int base;
	int nirq;
};

static void irq_gic500_enable(struct irqchip_t * chip, int offset)
{
	struct irq_gic500_pdata_t * pdat = (struct irq_gic500_pdata_t *)chip->priv;
	int irq = chip->base + offset;

	if(irq < 32)
	{
		write32(pdat->virt + GICR_ISENABLER0 + (irq / 32) * 4, 1 << (irq % 32));
		while(read32(pdat->virt + GICR_CTLR) & (0x1 << 31));
	}
	else
	{
		write32(pdat->virt + GICD_ISENABLER + (irq / 32) * 4, 1 << (irq % 32));
		while(read32(pdat->virt + GICD_CTLR) & (0x1 << 31));
	}
}

static void irq_gic500_disable(struct irqchip_t * chip, int offset)
{
	struct irq_gic500_pdata_t * pdat = (struct irq_gic500_pdata_t *)chip->priv;
	int irq = chip->base + offset;

	if(irq < 32)
	{
		write32(pdat->virt + GICR_ICENABLER0 + (irq / 32) * 4, 1 << (irq % 32));
		while(read32(pdat->virt + GICR_CTLR) & (0x1 << 31));
	}
	else
	{
		write32(pdat->virt + GICD_ICENABLER + (irq / 32) * 4, 1 << (irq % 32));
		while(read32(pdat->virt + GICD_CTLR) & (0x1 << 31));
	}
}

static void irq_gic500_settype(struct irqchip_t * chip, int offset, enum irq_type_t type)
{
}

static void irq_gic500_dispatch(struct irqchip_t * chip)
{
	int irq = arm64_read_sysreg(S3_0_C12_C12_0) & 0x3ff;
	int offset = irq - chip->base;

	if((offset >= 0) && (offset < chip->nirq))
	{
		(chip->handler[offset].func)(chip->handler[offset].data);
		arm64_write_sysreg(S3_0_C12_C12_1, (uint64_t)irq);
		arm64_write_sysreg(S3_0_C12_C11_1, (uint64_t)irq);
	}
}

static u64_t gic_mpidr_to_affinity(unsigned long mpidr)
{
	u64_t aff;

	#define MPIDR_AFFINITY_LEVEL(mpidr, level) \
		((mpidr >> (((1 << level) >> 1) << 3)) & ((1 << (1 << 3)) - 1))
	aff = ((u64_t)MPIDR_AFFINITY_LEVEL(mpidr, 3) << 32 |
	       MPIDR_AFFINITY_LEVEL(mpidr, 2) << 16 |
	       MPIDR_AFFINITY_LEVEL(mpidr, 1) << 8  |
	       MPIDR_AFFINITY_LEVEL(mpidr, 0));
	return aff;
}

static void gic500_dist_init(virtual_addr_t virt)
{
	u32_t typer;
	u32_t irq_nr;
	u64_t affinity;
	int i;

	/*
	 * Disable the distributor
	 */
	write32(virt + GICD_CTLR, 0x0);
	while(read32(virt + GICD_CTLR) & (0x1 << 31));

	/*
	 * Find out how many interrupts are supported.
	 * The GIC only supports up to 1020 interrupt sources (SGI+PPI+SPI)
	 */
	typer = read32(virt + GICD_TYPER);
	irq_nr = ((typer & 0x1f) + 1) * 32;
	if(irq_nr > 1020)
		irq_nr = 1020;

	/*
	 * Configure SPIs as non-secure Group-1. This will only matter
	 * if the GIC only has a single security state. This will not
	 * do the right thing if the kernel is running in secure mode,
	 * but that's not the intended use case anyway.
	 */
	for(i = 32; i < irq_nr; i += 32)
		write32(virt + GICD_IGROUPR + i / 8, ~0);

	/*
	 * Set all global interrupts to be level triggered, active low.
	 */
	for(i = 32; i < irq_nr; i += 16)
		write32(virt + GICD_ICFGR + i / 4, 0x0);

	/*
	 * Set priority on all global interrupts.
	 */
	for(i = 32; i < irq_nr; i += 4)
		write32(virt + GICD_IPRIORITYR + i, 0xa0a0a0a0);

	/*
	 * Deactivate and disable all SPIs. Leave the PPI and SGIs
	 * alone as they are in the redistributor registers on GICv3.
	 */
	for(i = 32; i < irq_nr; i += 32)
	{
		write32(virt + GICD_ICACTIVER + i / 8, 0xffffffff);
		write32(virt + GICD_ICENABLER + i / 8, 0xffffffff);
	}
	while(read32(virt + GICD_CTLR) & (0x1 << 31));

	/*
	 * Enable distributor with ARE, Group1
	 */
	write32(virt + GICD_CTLR, (1 << 4) | (1 << 1) | (1 << 0));

	/*
	 * Set all global interrupts to the boot CPU only. ARE must be
	 * enabled.
	 */
	affinity = gic_mpidr_to_affinity(smp_processor_id());
	for(i = 32; i < irq_nr; i++)
		write64(virt + GICD_IROUTER + i * 8, affinity);
}

static void gic500_cpu_init(virtual_addr_t virt)
{
	u32_t typer;
	u32_t irq_nr;
	u32_t val;
	int i;

	/*
	 * Find out how many interrupts are supported.
	 * The GIC only supports up to 1020 interrupt sources (SGI+PPI+SPI)
	 */
	typer = read32(virt + GICD_TYPER);
	irq_nr = ((typer & 0x1f) + 1) * 32;
	if(irq_nr > 1020)
		irq_nr = 1020;

	/*
	 * Wake up this CPU redistributor
	 */
	val = read32(virt + GICR_WAKER);
	val &= ~(1 << 1);
	write32(virt + GICR_WAKER, val);

	/*
	 * Configure SGIs / PPIs as non-secure Group-1
	 */
	write32(virt + GICR_IGROUPR0, ~0);

	/*
	 * Deal with the banked PPI and SGI interrupts - disable all
	 * PPI interrupts, ensure all SGI interrupts are enabled.
	 * Make sure everything is deactivated.
	 */
	write32(virt + GICR_ICACTIVER0, 0xffffffff);
	write32(virt + GICR_ICENABLER0, 0xffff0000);
	write32(virt + GICR_ISENABLER0, 0x0000ffff);

	/*
	 * Set priority on PPI and SGI interrupts
	 */
	for(i = 0; i < 32; i += 4)
		write32(virt + GICR_IPRIORITYR + i * 4 / 4, 0xa0a0a0a0);
	while(read32(virt + GICR_CTLR) & (0x1 << 31));
}

static struct device_t * irq_gic500_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct irq_gic500_pdata_t * pdat;
	struct irqchip_t * chip;
	struct device_t * dev;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	int base = dt_read_int(n, "interrupt-base", -1);
	int nirq = dt_read_int(n, "interrupt-count", -1);
	u32_t ver = (read32(virt + 0xffe8) >> 4) & 0x0f;

	if((base < 0) || (nirq <= 0) || (ver != 0x3))
		return NULL;

	pdat = malloc(sizeof(struct irq_gic500_pdata_t));
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
	chip->enable = irq_gic500_enable;
	chip->disable = irq_gic500_disable;
	chip->settype = irq_gic500_settype;
	chip->dispatch = irq_gic500_dispatch;
	chip->priv = pdat;

	gic500_dist_init(pdat->virt);
	gic500_cpu_init(pdat->virt);
	arm64_interrupt_enable();

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

static void irq_gic500_remove(struct device_t * dev)
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

static void irq_gic500_suspend(struct device_t * dev)
{
}

static void irq_gic500_resume(struct device_t * dev)
{
}

static struct driver_t irq_gic500 = {
	.name		= "irq-gic500",
	.probe		= irq_gic500_probe,
	.remove		= irq_gic500_remove,
	.suspend	= irq_gic500_suspend,
	.resume		= irq_gic500_resume,
};

static __init void irq_gic500_driver_init(void)
{
	register_driver(&irq_gic500);
}

static __exit void irq_gic500_driver_exit(void)
{
	unregister_driver(&irq_gic500);
}

driver_initcall(irq_gic500_driver_init);
driver_exitcall(irq_gic500_driver_exit);
