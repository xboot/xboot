/*
 * driver/dma-t113.c
 *
 * Copyright(c) 2007-2022 Jianjun Jiang <8192542@qq.com>
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
#include <clk/clk.h>
#include <reset/reset.h>
#include <interrupt/interrupt.h>
#include <dma/dma.h>
#include <t113-dma.h>

enum {
	DMA_IRQ_EN0				= 0x00,
	DMA_IRQ_EN1				= 0x04,
	DMA_IRQ_PEND0			= 0x10,
	DMA_IRQ_PEND1			= 0x14,
	DMA_AUTO_GATE			= 0x28,
	DMA_STATUS				= 0x30,
};

#define DMA_CH_EN(x)		(0x100 + ((x) << 6))
#define DMA_CH_PAUSE(x)		(0x104 + ((x) << 6))
#define DMA_CH_DST(x)		(0x108 + ((x) << 6))
#define DMA_CH_CFG(x)		(0x10c + ((x) << 6))
#define DMA_CH_CUR_SRC(x)	(0x110 + ((x) << 6))
#define DMA_CH_CUR_DST(x)	(0x114 + ((x) << 6))
#define DMA_CH_BCNT_LEFT(x)	(0x118 + ((x) << 6))
#define DMA_CH_PARA(x)		(0x11c + ((x) << 6))
#define DMA_CH_MODE(x)		(0x128 + ((x) << 6))
#define DMA_CH_FDESC(x)		(0x12c + ((x) << 6))
#define DMA_CH_PKGNUM(x)	(0x130 + ((x) << 6))

struct dma_t113_desc_t {
	u32_t config;
	u32_t src;
	u32_t dst;
	u32_t count;
	u32_t para;
	u32_t link;
	u32_t reserved[2];
};

struct dma_t113_pdata_t
{
	virtual_addr_t virt;
	char * clk;
	int irq;
	int reset;
	int base;
	int ndma;
	struct dma_t113_desc_t * desc;
};

static u32_t get_t113_config(struct dma_channel_t * ch)
{
	u32_t cfg = 0;

	cfg |= (DMA_G_SRC_PORT(ch->flag) & 0x3f) << 0;
	switch(DMA_G_SRC_BURST(ch->flag))
	{
	case DMA_BURST_SIZE_1:
		cfg |= (0x0 << 6);
		break;
	case DMA_BURST_SIZE_4:
		cfg |= (0x1 << 6);
		break;
	case DMA_BURST_SIZE_8:
		cfg |= (0x2 << 6);
		break;
	case DMA_BURST_SIZE_16:
		cfg |= (0x3 << 6);
		break;
	case DMA_BURST_SIZE_32:
	case DMA_BURST_SIZE_64:
	case DMA_BURST_SIZE_128:
	case DMA_BURST_SIZE_256:
	case DMA_BURST_SIZE_512:
	case DMA_BURST_SIZE_1024:
	case DMA_BURST_SIZE_2048:
	case DMA_BURST_SIZE_4096:
	case DMA_BURST_SIZE_8192:
	case DMA_BURST_SIZE_16384:
	case DMA_BURST_SIZE_32768:
	case DMA_BURST_SIZE_65536:
	default:
		break;
	}
	switch(DMA_G_SRC_INC(ch->flag))
	{
	case DMA_INCREASE:
		cfg |= (0x0 << 8);
		break;
	case DMA_CONSTANT:
		cfg |= (0x1 << 8);
		break;
	default:
		break;
	}
	switch(DMA_G_SRC_WIDTH(ch->flag))
	{
	case DMA_WIDTH_8BIT:
		cfg |= (0x0 << 9);
		break;
	case DMA_WIDTH_16BIT:
		cfg |= (0x1 << 9);
		break;
	case DMA_WIDTH_32BIT:
		cfg |= (0x2 << 9);
		break;
	case DMA_WIDTH_64BIT:
		cfg |= (0x3 << 9);
		break;
	default:
		break;
	}
	cfg |= (DMA_G_DST_PORT(ch->flag) & 0x3f) << 16;
	switch(DMA_G_DST_BURST(ch->flag))
	{
	case DMA_BURST_SIZE_1:
		cfg |= (0x0 << 22);
		break;
	case DMA_BURST_SIZE_4:
		cfg |= (0x1 << 22);
		break;
	case DMA_BURST_SIZE_8:
		cfg |= (0x2 << 22);
		break;
	case DMA_BURST_SIZE_16:
		cfg |= (0x3 << 22);
		break;
	case DMA_BURST_SIZE_32:
	case DMA_BURST_SIZE_64:
	case DMA_BURST_SIZE_128:
	case DMA_BURST_SIZE_256:
	case DMA_BURST_SIZE_512:
	case DMA_BURST_SIZE_1024:
	case DMA_BURST_SIZE_2048:
	case DMA_BURST_SIZE_4096:
	case DMA_BURST_SIZE_8192:
	case DMA_BURST_SIZE_16384:
	case DMA_BURST_SIZE_32768:
	case DMA_BURST_SIZE_65536:
	default:
		break;
	}
	switch(DMA_G_DST_INC(ch->flag))
	{
	case DMA_INCREASE:
		cfg |= (0x0 << 24);
		break;
	case DMA_CONSTANT:
		cfg |= (0x1 << 24);
		break;
	default:
		break;
	}
	switch(DMA_G_DST_WIDTH(ch->flag))
	{
	case DMA_WIDTH_8BIT:
		cfg |= (0x0 << 25);
		break;
	case DMA_WIDTH_16BIT:
		cfg |= (0x1 << 25);
		break;
	case DMA_WIDTH_32BIT:
		cfg |= (0x2 << 25);
		break;
	case DMA_WIDTH_64BIT:
		cfg |= (0x3 << 25);
		break;
	default:
		break;
	}
	switch(DMA_G_TYPE(ch->flag))
	{
	case DMA_TYPE_MEMTOMEM:
		if(((u32_t)ch->src >= 0x00020000) && ((u32_t)ch->src < 0x00059000))
			cfg = (cfg & ~(0x3f << 0)) | (T113_DMA_PORT_SRAM << 0);
		else if(((u32_t)ch->src >= 0x40000000) && ((u32_t)ch->src < 0xc0000000))
			cfg = (cfg & ~(0x3f << 0)) | (T113_DMA_PORT_DRAM << 0);
		if(((u32_t)ch->dst >= 0x00020000) && ((u32_t)ch->dst < 0x00059000))
			cfg = (cfg & ~(0x3f << 16)) | (T113_DMA_PORT_SRAM << 16);
		else if(((u32_t)ch->dst >= 0x40000000) && ((u32_t)ch->dst < 0xc0000000))
			cfg = (cfg & ~(0x3f << 16)) | (T113_DMA_PORT_DRAM << 16);
		break;
	case DMA_TYPE_MEMTODEV:
		if(((u32_t)ch->src >= 0x00020000) && ((u32_t)ch->src < 0x00059000))
			cfg = (cfg & ~(0x3f << 0)) | (T113_DMA_PORT_SRAM << 0);
		else if(((u32_t)ch->src >= 0x40000000) && ((u32_t)ch->src < 0xc0000000))
			cfg = (cfg & ~(0x3f << 0)) | (T113_DMA_PORT_DRAM << 0);
		break;
	case DMA_TYPE_DEVTOMEM:
		if(((u32_t)ch->dst >= 0x00020000) && ((u32_t)ch->dst < 0x00059000))
			cfg = (cfg & ~(0x3f << 16)) | (T113_DMA_PORT_SRAM << 16);
		else if(((u32_t)ch->dst >= 0x40000000) && ((u32_t)ch->dst < 0xc0000000))
			cfg = (cfg & ~(0x3f << 16)) | (T113_DMA_PORT_DRAM << 16);
		break;
	case DMA_TYPE_DEVTODEV:
	default:
		break;
	}
	return cfg;
}

static u32_t get_t113_para(struct dma_channel_t * ch)
{
	u32_t para = 0;

	para |= (8 & 0xff) << 0;
	para |= (4 & 0xff) << 8;
	return para;
}

static void dma_t113_start(struct dmachip_t * chip, int offset)
{
	struct dma_t113_pdata_t * pdat = (struct dma_t113_pdata_t *)chip->priv;
	struct dma_t113_desc_t * desc;
	struct dma_channel_t * ch;

	if(offset >= chip->ndma)
		return;

	desc = &pdat->desc[offset];
	ch = &chip->channel[offset];
	desc->config = get_t113_config(ch);
	desc->src = (u32_t)(ch->src);
	desc->dst = (u32_t)(ch->dst);
	desc->count = (u32_t)(ch->size);
	desc->para = get_t113_para(ch);
	desc->link = 0xfffff800;
	smp_mb();
	write32(pdat->virt + DMA_CH_DST(offset), (u32_t)desc);
	smp_mb();
	write32(pdat->virt + DMA_CH_EN(offset), 1);
	write32(pdat->virt + DMA_CH_PAUSE(offset), 0);
	smp_mb();
}

static void dma_t113_stop(struct dmachip_t * chip, int offset)
{
	struct dma_t113_pdata_t * pdat = (struct dma_t113_pdata_t *)chip->priv;

	if(offset >= chip->ndma)
		return;
	write32(pdat->virt + DMA_CH_EN(offset), 0);
}

static int dma_t113_busying(struct dmachip_t * chip, int offset)
{
	struct dma_t113_pdata_t * pdat = (struct dma_t113_pdata_t *)chip->priv;

	if(offset < chip->ndma)
	{
		u32_t val = read32(pdat->virt + DMA_STATUS);
		if(val & (1 << offset))
			return 1;
	}
	return 0;
}

static void dma_t113_interrupt(void * data)
{
	struct dmachip_t * chip = (struct dmachip_t *)data;
	struct dma_t113_pdata_t * pdat = (struct dma_t113_pdata_t *)chip->priv;
	struct dma_channel_t * ch;
	u32_t pending;
	int i;

	pending = read32(pdat->virt + DMA_IRQ_PEND0);
	if(pending)
	{
		write32(pdat->virt + DMA_IRQ_PEND0, pending);
		for(i = 0; i < min(8, pdat->ndma); i++)
		{
			if(pending & (0x3 << (i << 2)))
			{
				ch = &chip->channel[i];
				if(pending & (0x1 << (i << 2)))
				{
					if(ch->half)
						ch->half(ch->data);
				}
				if(pending & (0x2 << (i << 2)))
				{
					if(ch->finish)
						ch->finish(ch->data);
				}
			}
		}
	}

	pending = read32(pdat->virt + DMA_IRQ_PEND1);
	if(pending)
	{
		write32(pdat->virt + DMA_IRQ_PEND1, pending);
		for(i = 8; i < min(16, pdat->ndma); i++)
		{
			if(pending & (0x3 << ((i - 8) << 2)))
			{
				ch = &chip->channel[i];
				if(pending & (0x1 << ((i - 8) << 2)))
				{
					if(ch->half)
						ch->half(ch->data);
				}
				if(pending & (0x2 << ((i - 8) << 2)))
				{
					if(ch->finish)
						ch->finish(ch->data);
				}
			}
		}
	}
}

static struct device_t * dma_t113_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct dma_t113_pdata_t * pdat;
	struct dmachip_t * chip;
	struct device_t * dev;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	char * clk = dt_read_string(n, "clock-name", NULL);
	int irq = dt_read_int(n, "interrupt", -1);
	int base = dt_read_int(n, "dma-base", -1);
	int ndma = dt_read_int(n, "dma-count", -1);
	int i;

	if(!search_clk(clk))
		return NULL;

	if(!irq_is_valid(irq))
		return NULL;

	if((base < 0) || (ndma <= 0))
		return NULL;

	pdat = malloc(sizeof(struct dma_t113_pdata_t));
	if(!pdat)
		return NULL;

	chip = malloc(sizeof(struct dmachip_t));
	if(!chip)
	{
		free(pdat);
		return NULL;
	}

	pdat->virt = virt;
	pdat->clk = strdup(clk);
	pdat->reset = dt_read_int(n, "reset", -1);
	pdat->irq = irq;
	pdat->base = base;
	pdat->ndma = ndma;
	pdat->desc = dma_alloc_coherent(sizeof(struct dma_t113_desc_t) * ndma);

	chip->name = alloc_device_name(dt_read_name(n), -1);
	chip->base = pdat->base;
	chip->ndma = pdat->ndma;
	chip->channel = malloc(sizeof(struct dma_channel_t) * pdat->ndma);
	chip->start = dma_t113_start;
	chip->stop = dma_t113_stop;
	chip->busying = dma_t113_busying;
	chip->priv = pdat;

	clk_enable(pdat->clk);
	if(pdat->reset >= 0)
	{
		reset_assert(pdat->reset);
		udelay(1);
		reset_deassert(pdat->reset);
		udelay(1);
	}
	request_irq(pdat->irq, dma_t113_interrupt, IRQ_TYPE_NONE, chip);
	write32(pdat->virt + DMA_IRQ_EN0, 0x33333333);
	write32(pdat->virt + DMA_IRQ_EN1, 0x33333333);
	write32(pdat->virt + DMA_IRQ_PEND0, 0x77777777);
	write32(pdat->virt + DMA_IRQ_PEND1, 0x77777777);
	write32(pdat->virt + DMA_AUTO_GATE, 0);
	for(i = 0; i < pdat->ndma; i++)
	{
		write32(pdat->virt + DMA_CH_EN(i), 0x0);
		write32(pdat->virt + DMA_CH_PAUSE(i), 0x1);
		write32(pdat->virt + DMA_CH_DST(i), 0xfffff800);
		write32(pdat->virt + DMA_CH_MODE(i), 0x0);
	}

	if(!(dev = register_dmachip(chip, drv)))
	{
		clk_disable(pdat->clk);
		free_irq(pdat->irq);
		free(pdat->clk);
		dma_free_coherent(pdat->desc);
		free_device_name(chip->name);
		free(chip->channel);
		free(chip->priv);
		free(chip);
		return NULL;
	}
	return dev;
}

static void dma_t113_remove(struct device_t * dev)
{
	struct dmachip_t * chip = (struct dmachip_t *)dev->priv;
	struct dma_t113_pdata_t * pdat = (struct dma_t113_pdata_t *)chip->priv;

	if(chip)
	{
		unregister_dmachip(chip);
		clk_disable(pdat->clk);
		free_irq(pdat->irq);
		free(pdat->clk);
		dma_free_coherent(pdat->desc);
		free_device_name(chip->name);
		free(chip->channel);
		free(chip->priv);
		free(chip);
	}
}

static void dma_t113_suspend(struct device_t * dev)
{
}

static void dma_t113_resume(struct device_t * dev)
{
}

static struct driver_t dma_t113 = {
	.name		= "dma-t113",
	.probe		= dma_t113_probe,
	.remove		= dma_t113_remove,
	.suspend	= dma_t113_suspend,
	.resume		= dma_t113_resume,
};

static __init void dma_t113_driver_init(void)
{
	register_driver(&dma_t113);
}

static __exit void dma_t113_driver_exit(void)
{
	unregister_driver(&dma_t113);
}

driver_initcall(dma_t113_driver_init);
driver_exitcall(dma_t113_driver_exit);
