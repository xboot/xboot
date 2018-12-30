/*
 * driver/irq-k210.c
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
#include <riscv64.h>
#include <interrupt/interrupt.h>
#include <k210/reg-gpiohs.h>
#include <k210-irq.h>

#define PLIC_SOURCE_PRIORITY(x)		(0x00000000 + ((x) * 0x4))
#define PLIC_TARGET_ENABLE(cpu)		(0x00002000 + ((cpu) * 0x80))
#define PLIC_TARGET_THRESHOLD(cpu)	(0x00200000 + ((cpu) * 0x1000))
#define PLIC_TARGET_COMPLETE(cpu)	(0x00200004 + ((cpu) * 0x1000))

struct irq_k210_pdata_t
{
	virtual_addr_t virtplic;
	virtual_addr_t virtgpiohs;
	int base;
	int nirq;
	int cpu;
	int ncpu;
	enum irq_type_t gpiohs[32];
};

static void irq_k210_enable(struct irqchip_t * chip, int offset)
{
	struct irq_k210_pdata_t * pdat = (struct irq_k210_pdata_t *)chip->priv;
	int irq = chip->base + offset;
	u32_t val = read32(pdat->virtplic + PLIC_TARGET_ENABLE(pdat->cpu) + ((irq >> 5) << 2));
	val |= (1 << (irq & 0x1f));
	write32(pdat->virtplic + PLIC_TARGET_ENABLE(pdat->cpu) + ((irq >> 5) << 2), val);
}

static void irq_k210_disable(struct irqchip_t * chip, int offset)
{
	struct irq_k210_pdata_t * pdat = (struct irq_k210_pdata_t *)chip->priv;
	int irq = chip->base + offset;
	u32_t val = read32(pdat->virtplic + PLIC_TARGET_ENABLE(pdat->cpu) + ((irq >> 5) << 2));
	val &= ~(1 << (irq & 0x1f));
	write32(pdat->virtplic + PLIC_TARGET_ENABLE(pdat->cpu) + ((irq >> 5) << 2), val);
}

static void irq_k210_settype(struct irqchip_t * chip, int offset, enum irq_type_t type)
{
	struct irq_k210_pdata_t * pdat = (struct irq_k210_pdata_t *)chip->priv;

	offset = chip->base + offset;
	if((offset >= K210_IRQ_GPIOHS0) && (offset <= K210_IRQ_GPIOHS31))
	{
		offset = offset - K210_IRQ_GPIOHS0;
		write32(pdat->virtgpiohs + GPIOHS_RISE_IE, (read32(pdat->virtgpiohs + GPIOHS_RISE_IE) & ~(0x1 << offset)));
		write32(pdat->virtgpiohs + GPIOHS_FALL_IE, (read32(pdat->virtgpiohs + GPIOHS_FALL_IE) & ~(0x1 << offset)));
		write32(pdat->virtgpiohs + GPIOHS_HIGH_IE, (read32(pdat->virtgpiohs + GPIOHS_HIGH_IE) & ~(0x1 << offset)));
		write32(pdat->virtgpiohs + GPIOHS_LOW_IE,  (read32(pdat->virtgpiohs + GPIOHS_LOW_IE)  & ~(0x1 << offset)));
		write32(pdat->virtgpiohs + GPIOHS_RISE_IP, (read32(pdat->virtgpiohs + GPIOHS_RISE_IP) | (0x1 << offset)));
		write32(pdat->virtgpiohs + GPIOHS_FALL_IP, (read32(pdat->virtgpiohs + GPIOHS_FALL_IP) | (0x1 << offset)));
		write32(pdat->virtgpiohs + GPIOHS_HIGH_IP, (read32(pdat->virtgpiohs + GPIOHS_HIGH_IP) | (0x1 << offset)));
		write32(pdat->virtgpiohs + GPIOHS_LOW_IP,  (read32(pdat->virtgpiohs + GPIOHS_LOW_IP)  | (0x1 << offset)));

		switch(type)
		{
		case IRQ_TYPE_NONE:
			pdat->gpiohs[offset] = IRQ_TYPE_NONE;
			break;
		case IRQ_TYPE_LEVEL_LOW:
			pdat->gpiohs[offset] = IRQ_TYPE_LEVEL_LOW;
			write32(pdat->virtgpiohs + GPIOHS_LOW_IE,  (read32(pdat->virtgpiohs + GPIOHS_LOW_IE)  | (0x1 << offset)));
			break;
		case IRQ_TYPE_LEVEL_HIGH:
			pdat->gpiohs[offset] = IRQ_TYPE_LEVEL_HIGH;
			write32(pdat->virtgpiohs + GPIOHS_HIGH_IE, (read32(pdat->virtgpiohs + GPIOHS_HIGH_IE) | (0x1 << offset)));
			break;
		case IRQ_TYPE_EDGE_FALLING:
			pdat->gpiohs[offset] = IRQ_TYPE_EDGE_FALLING;
			write32(pdat->virtgpiohs + GPIOHS_FALL_IE, (read32(pdat->virtgpiohs + GPIOHS_FALL_IE) | (0x1 << offset)));
			break;
		case IRQ_TYPE_EDGE_RISING:
			pdat->gpiohs[offset] = IRQ_TYPE_EDGE_RISING;
			write32(pdat->virtgpiohs + GPIOHS_RISE_IE, (read32(pdat->virtgpiohs + GPIOHS_RISE_IE) | (0x1 << offset)));
			break;
		case IRQ_TYPE_EDGE_BOTH:
			pdat->gpiohs[offset] = IRQ_TYPE_EDGE_BOTH;
			write32(pdat->virtgpiohs + GPIOHS_RISE_IE, (read32(pdat->virtgpiohs + GPIOHS_RISE_IE) | (0x1 << offset)));
			write32(pdat->virtgpiohs + GPIOHS_FALL_IE, (read32(pdat->virtgpiohs + GPIOHS_FALL_IE) | (0x1 << offset)));
			break;
		default:
			break;
		}
	}
}

static void irq_k210_dispatch(struct irqchip_t * chip)
{
	struct irq_k210_pdata_t * pdat = (struct irq_k210_pdata_t *)chip->priv;
	u32_t irq = read32(pdat->virtplic + PLIC_TARGET_COMPLETE(smp_processor_id()));
	int offset = irq + chip->base;

	if((offset > 0) && (offset < chip->nirq))
	{
		(chip->handler[offset].func)(chip->handler[offset].data);
		if((offset >= K210_IRQ_GPIOHS0) && (offset <= K210_IRQ_GPIOHS31))
		{
			offset = offset - K210_IRQ_GPIOHS0;
			switch(pdat->gpiohs[offset])
			{
			case IRQ_TYPE_NONE:
				break;
			case IRQ_TYPE_LEVEL_LOW:
				write32(pdat->virtgpiohs + GPIOHS_LOW_IP, (read32(pdat->virtgpiohs + GPIOHS_LOW_IP) | (0x1 << offset)));
				break;
			case IRQ_TYPE_LEVEL_HIGH:
				write32(pdat->virtgpiohs + GPIOHS_HIGH_IP, (read32(pdat->virtgpiohs + GPIOHS_HIGH_IP) | (0x1 << offset)));
				break;
			case IRQ_TYPE_EDGE_FALLING:
				write32(pdat->virtgpiohs + GPIOHS_FALL_IP, (read32(pdat->virtgpiohs + GPIOHS_FALL_IP) | (0x1 << offset)));
				break;
			case IRQ_TYPE_EDGE_RISING:
				write32(pdat->virtgpiohs + GPIOHS_RISE_IP, (read32(pdat->virtgpiohs + GPIOHS_RISE_IP) | (0x1 << offset)));
				break;
			case IRQ_TYPE_EDGE_BOTH:
				write32(pdat->virtgpiohs + GPIOHS_RISE_IP, (read32(pdat->virtgpiohs + GPIOHS_RISE_IP) | (0x1 << offset)));
				write32(pdat->virtgpiohs + GPIOHS_FALL_IP, (read32(pdat->virtgpiohs + GPIOHS_FALL_IP) | (0x1 << offset)));
				break;
			default:
				break;
			}
		}
		write32(pdat->virtplic + PLIC_TARGET_COMPLETE(smp_processor_id()), irq);
	}
}

static void plic_init(struct irq_k210_pdata_t * pdat)
{
	int cpu, irq;
	int i;

	/*
	 * Disable interrupts for all cpu cores
	 */
	for(cpu = 0; cpu < pdat->ncpu; cpu++)
	{
		for(i = 0; i < (pdat->nirq + 32) >> 5; i++)
			write32(pdat->virtplic + PLIC_TARGET_ENABLE(cpu) + (i << 2), 0);
	}

	/*
	 * Set all source priorities to one
	 */
	for(i = 0; i < pdat->nirq; i++)
	{
		write32(pdat->virtplic + PLIC_SOURCE_PRIORITY(i), 1);
	}

	/*
	 * Set target priority threshold for all cpu cores
	 */
	for(cpu = 0; cpu < pdat->ncpu; cpu++)
	{
		write32(pdat->virtplic + PLIC_TARGET_THRESHOLD(cpu), 0);
	}

	/*
	 * Clear pending bits for all cpu cores
	 */
	for(cpu = 0; cpu < pdat->ncpu; cpu++)
	{
		while((irq = read32(pdat->virtplic + PLIC_TARGET_COMPLETE(cpu))) > 0)
		{
			write32(pdat->virtplic + PLIC_TARGET_COMPLETE(cpu), irq);
		}
	}
}

static struct device_t * irq_k210_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct irq_k210_pdata_t * pdat;
	struct irqchip_t * chip;
	struct device_t * dev;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	int base = dt_read_int(n, "interrupt-base", -1);
	int nirq = dt_read_int(n, "interrupt-count", -1);

	if((base < 0) || (nirq <= 0))
		return NULL;

	pdat = malloc(sizeof(struct irq_k210_pdata_t));
	if(!pdat)
		return NULL;

	chip = malloc(sizeof(struct irqchip_t));
	if(!chip)
	{
		free(pdat);
		return NULL;
	}

	pdat->virtplic = virt;
	pdat->virtgpiohs = phys_to_virt(K210_GPIOHS_BASE);
	pdat->base = base;
	pdat->nirq = nirq;
	pdat->cpu = smp_processor_id();
	pdat->ncpu = dt_read_int(n, "cpu-count", 1);

	chip->name = alloc_device_name(dt_read_name(n), -1);
	chip->base = pdat->base;
	chip->nirq = pdat->nirq;
	chip->handler = malloc(sizeof(struct irq_handler_t) * pdat->nirq);
	chip->enable = irq_k210_enable;
	chip->disable = irq_k210_disable;
	chip->settype = irq_k210_settype;
	chip->dispatch = irq_k210_dispatch;
	chip->priv = pdat;

	plic_init(pdat);
	csr_set(mie, MIE_MEIE);
	csr_set(mstatus, MSTATUS_MIE);

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

static void irq_k210_remove(struct device_t * dev)
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

static void irq_k210_suspend(struct device_t * dev)
{
}

static void irq_k210_resume(struct device_t * dev)
{
}

static struct driver_t irq_k210 = {
	.name		= "irq-k210",
	.probe		= irq_k210_probe,
	.remove		= irq_k210_remove,
	.suspend	= irq_k210_suspend,
	.resume		= irq_k210_resume,
};

static __init void irq_k210_driver_init(void)
{
	register_driver(&irq_k210);
}

static __exit void irq_k210_driver_exit(void)
{
	unregister_driver(&irq_k210);
}

driver_initcall(irq_k210_driver_init);
driver_exitcall(irq_k210_driver_exit);
