/*
 * driver/dma-f1c100s.c
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
#include <clk/clk.h>
#include <reset/reset.h>
#include <interrupt/interrupt.h>
#include <dma/dma.h>
#include <f1c100s-dma.h>

enum {
	DMA_INT_CTL			= 0x00,
	DMA_INT_STA			= 0x04,
	DMA_PTY_CFG			= 0x08,
};

#define NDMA_CH_CFG(x)	(0x100 + ((x) << 6))
#define NDMA_CH_SRC(x)	(0x104 + ((x) << 6))
#define NDMA_CH_DST(x)	(0x108 + ((x) << 6))
#define NDMA_CH_CNT(x)	(0x10c + ((x) << 6))
#define DDMA_CH_CFG(x)	(0x300 + ((x - 4) << 6))
#define DDMA_CH_SRC(x)	(0x304 + ((x - 4) << 6))
#define DDMA_CH_DST(x)	(0x308 + ((x - 4) << 6))
#define DDMA_CH_CNT(x)	(0x30c + ((x - 4) << 6))
#define DDMA_CH_PAR(x)	(0x318 + ((x - 4) << 6))
#define DDMA_CH_GEN(x)	(0x31c + ((x - 4) << 6))

struct dma_f1c100s_pdata_t
{
	virtual_addr_t virt;
	char * clk;
	int irq;
	int reset;
	int base;
	int ndma;
	int tsize[8];
};

static u32_t get_ndma_cfg(struct dma_channel_t * ch)
{
	u32_t cfg = (0 << 29) | (0 << 26) | (0 << 15);

	cfg |= (DMA_G_SRC_PORT(ch->flag) & 0x1f) << 0;
	switch(DMA_G_SRC_INC(ch->flag))
	{
	case DMA_INCREASE:
		cfg |= 0x0 << 5;
		break;
	case DMA_CONSTANT:
		cfg |= 0x1 << 5;
		break;
	default:
		break;
	}
	switch(DMA_G_SRC_BURST(ch->flag))
	{
	case DMA_BURST_SIZE_1:
		cfg |= 0x0 << 7;
		break;
	case DMA_BURST_SIZE_4:
		cfg |= 0x1 << 7;
		break;
	case DMA_BURST_SIZE_8:
	case DMA_BURST_SIZE_16:
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
	switch(DMA_G_SRC_WIDTH(ch->flag))
	{
	case DMA_WIDTH_8BIT:
		cfg |= 0x0 << 8;
		break;
	case DMA_WIDTH_16BIT:
		cfg |= 0x1 << 8;
		break;
	case DMA_WIDTH_32BIT:
		cfg |= 0x2 << 8;
		break;
	case DMA_WIDTH_64BIT:
	default:
		break;
	}
	cfg |= (DMA_G_DST_PORT(ch->flag) & 0x1f) << 16;
	switch(DMA_G_DST_INC(ch->flag))
	{
	case DMA_INCREASE:
		cfg |= 0x0 << 21;
		break;
	case DMA_CONSTANT:
		cfg |= 0x1 << 21;
		break;
	default:
		break;
	}
	switch(DMA_G_DST_BURST(ch->flag))
	{
	case DMA_BURST_SIZE_1:
		cfg |= 0x0 << 23;
		break;
	case DMA_BURST_SIZE_4:
		cfg |= 0x1 << 23;
		break;
	case DMA_BURST_SIZE_8:
	case DMA_BURST_SIZE_16:
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
	switch(DMA_G_DST_WIDTH(ch->flag))
	{
	case DMA_WIDTH_8BIT:
		cfg |= 0x0 << 24;
		break;
	case DMA_WIDTH_16BIT:
		cfg |= 0x1 << 24;
		break;
	case DMA_WIDTH_32BIT:
		cfg |= 0x2 << 24;
		break;
	case DMA_WIDTH_64BIT:
	default:
		break;
	}
	switch(DMA_G_TYPE(ch->flag))
	{
	case DMA_TYPE_MEMTOMEM:
		if(((u32_t)ch->src >= 0x00010000) && ((u32_t)ch->src < 0x0001a000))
			cfg = (cfg & ~(0x1f << 0)) | (F1C100S_NDMA_PORT_SRAM << 0);
		else if(((u32_t)ch->src >= 0x80000000) && ((u32_t)ch->src < 0x82000000))
			cfg = (cfg & ~(0x1f << 0)) | (F1C100S_NDMA_PORT_SDRAM << 0);
		if(((u32_t)ch->dst >= 0x00010000) && ((u32_t)ch->dst < 0x0001a000))
			cfg = (cfg & ~(0x1f << 16)) | (F1C100S_NDMA_PORT_SRAM << 16);
		else if(((u32_t)ch->dst >= 0x80000000) && ((u32_t)ch->dst < 0x82000000))
			cfg = (cfg & ~(0x1f << 16)) | (F1C100S_NDMA_PORT_SDRAM << 16);
		break;
	case DMA_TYPE_MEMTODEV:
		if(((u32_t)ch->src >= 0x00010000) && ((u32_t)ch->src < 0x0001a000))
			cfg = (cfg & ~(0x1f << 0)) | (F1C100S_NDMA_PORT_SRAM << 0);
		else if(((u32_t)ch->src >= 0x80000000) && ((u32_t)ch->src < 0x82000000))
			cfg = (cfg & ~(0x1f << 0)) | (F1C100S_NDMA_PORT_SDRAM << 0);
		break;
	case DMA_TYPE_DEVTOMEM:
		if(((u32_t)ch->dst >= 0x00010000) && ((u32_t)ch->dst < 0x0001a000))
			cfg = (cfg & ~(0x1f << 16)) | (F1C100S_NDMA_PORT_SRAM << 16);
		else if(((u32_t)ch->dst >= 0x80000000) && ((u32_t)ch->dst < 0x82000000))
			cfg = (cfg & ~(0x1f << 16)) | (F1C100S_NDMA_PORT_SDRAM << 16);
		break;
	case DMA_TYPE_DEVTODEV:
	default:
		break;
	}
	return cfg;
}

static u32_t get_ddma_cfg(struct dma_channel_t * ch)
{
	u32_t cfg = (0 << 29) | (0 << 28) | (0 << 26) | (0 << 15) | (0 << 10);

	cfg |= (DMA_G_SRC_PORT(ch->flag) & 0x1f) << 0;
	switch(DMA_G_SRC_INC(ch->flag))
	{
	case DMA_INCREASE:
		cfg |= 0x0 << 5;
		break;
	case DMA_CONSTANT:
		cfg |= 0x1 << 5;
		break;
	default:
		break;
	}
	switch(DMA_G_SRC_BURST(ch->flag))
	{
	case DMA_BURST_SIZE_1:
		cfg |= 0x0 << 7;
		break;
	case DMA_BURST_SIZE_4:
		cfg |= 0x1 << 7;
		break;
	case DMA_BURST_SIZE_8:
	case DMA_BURST_SIZE_16:
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
	switch(DMA_G_SRC_WIDTH(ch->flag))
	{
	case DMA_WIDTH_8BIT:
		cfg |= 0x0 << 8;
		break;
	case DMA_WIDTH_16BIT:
		cfg |= 0x1 << 8;
		break;
	case DMA_WIDTH_32BIT:
		cfg |= 0x2 << 8;
		break;
	case DMA_WIDTH_64BIT:
	default:
		break;
	}
	cfg |= (DMA_G_DST_PORT(ch->flag) & 0x1f) << 16;
	switch(DMA_G_DST_INC(ch->flag))
	{
	case DMA_INCREASE:
		cfg |= 0x0 << 21;
		break;
	case DMA_CONSTANT:
		cfg |= 0x1 << 21;
		break;
	default:
		break;
	}
	switch(DMA_G_DST_BURST(ch->flag))
	{
	case DMA_BURST_SIZE_1:
		cfg |= 0x0 << 23;
		break;
	case DMA_BURST_SIZE_4:
		cfg |= 0x1 << 23;
		break;
	case DMA_BURST_SIZE_8:
	case DMA_BURST_SIZE_16:
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
	switch(DMA_G_DST_WIDTH(ch->flag))
	{
	case DMA_WIDTH_8BIT:
		cfg |= 0x0 << 24;
		break;
	case DMA_WIDTH_16BIT:
		cfg |= 0x1 << 24;
		break;
	case DMA_WIDTH_32BIT:
		cfg |= 0x2 << 24;
		break;
	case DMA_WIDTH_64BIT:
	default:
		break;
	}
	switch(DMA_G_TYPE(ch->flag))
	{
	case DMA_TYPE_MEMTOMEM:
		if(((u32_t)ch->src >= 0x00010000) && ((u32_t)ch->src < 0x0001a000))
			cfg = (cfg & ~(0x1f << 0)) | (F1C100S_DDMA_PORT_SRAM << 0);
		else if(((u32_t)ch->src >= 0x80000000) && ((u32_t)ch->src < 0x82000000))
			cfg = (cfg & ~(0x1f << 0)) | (F1C100S_DDMA_PORT_SDRAM << 0);
		if(((u32_t)ch->dst >= 0x00010000) && ((u32_t)ch->dst < 0x0001a000))
			cfg = (cfg & ~(0x1f << 16)) | (F1C100S_DDMA_PORT_SRAM << 16);
		else if(((u32_t)ch->dst >= 0x80000000) && ((u32_t)ch->dst < 0x82000000))
			cfg = (cfg & ~(0x1f << 16)) | (F1C100S_DDMA_PORT_SDRAM << 16);
		break;
	case DMA_TYPE_MEMTODEV:
		if(((u32_t)ch->src >= 0x00010000) && ((u32_t)ch->src < 0x0001a000))
			cfg = (cfg & ~(0x1f << 0)) | (F1C100S_DDMA_PORT_SRAM << 0);
		else if(((u32_t)ch->src >= 0x80000000) && ((u32_t)ch->src < 0x82000000))
			cfg = (cfg & ~(0x1f << 0)) | (F1C100S_DDMA_PORT_SDRAM << 0);
		break;
	case DMA_TYPE_DEVTOMEM:
		if(((u32_t)ch->dst >= 0x00010000) && ((u32_t)ch->dst < 0x0001a000))
			cfg = (cfg & ~(0x1f << 16)) | (F1C100S_DDMA_PORT_SRAM << 16);
		else if(((u32_t)ch->dst >= 0x80000000) && ((u32_t)ch->dst < 0x82000000))
			cfg = (cfg & ~(0x1f << 16)) | (F1C100S_DDMA_PORT_SDRAM << 16);
		break;
	case DMA_TYPE_DEVTODEV:
	default:
		break;
	}
	return cfg;
}

static void dma_f1c100s_start(struct dmachip_t * chip, int offset)
{
	struct dma_f1c100s_pdata_t * pdat = (struct dma_f1c100s_pdata_t *)chip->priv;
	struct dma_channel_t * ch;

	if(offset >= chip->ndma)
		return;
	if(offset < 4)
	{
		ch = &chip->channel[offset];
		while(read32(pdat->virt + NDMA_CH_CFG(offset)) & (1 << 30));
		smp_mb();
		write32(pdat->virt + NDMA_CH_SRC(offset), (u32_t)(ch->src + ((DMA_G_SRC_INC(ch->flag) == DMA_INCREASE) ? ch->len : 0)));
		write32(pdat->virt + NDMA_CH_DST(offset), (u32_t)(ch->dst + ((DMA_G_DST_INC(ch->flag) == DMA_INCREASE) ? ch->len : 0)));
		pdat->tsize[offset] = min(0x20000, ch->size - ch->len);
		write32(pdat->virt + NDMA_CH_CNT(offset), pdat->tsize[offset]);
		write32(pdat->virt + NDMA_CH_CFG(offset), get_ndma_cfg(ch));
		write32(pdat->virt + DMA_INT_CTL, read32(pdat->virt + DMA_INT_CTL) | (0x3 << (offset << 1)));
		write32(pdat->virt + NDMA_CH_CFG(offset), read32(pdat->virt + NDMA_CH_CFG(offset)) | (1 << 31));
		smp_mb();
	}
	else
	{
		ch = &chip->channel[offset];
		while(read32(pdat->virt + DDMA_CH_CFG(offset)) & (1 << 30));
		smp_mb();
		write32(pdat->virt + DDMA_CH_SRC(offset), (u32_t)(ch->src + ((DMA_G_SRC_INC(ch->flag) == DMA_INCREASE) ? ch->len : 0)));
		write32(pdat->virt + DDMA_CH_DST(offset), (u32_t)(ch->dst + ((DMA_G_DST_INC(ch->flag) == DMA_INCREASE) ? ch->len : 0)));
		pdat->tsize[offset] = min(0x1000000, ch->size - ch->len);
		write32(pdat->virt + DDMA_CH_CNT(offset), pdat->tsize[offset]);
		write32(pdat->virt + DDMA_CH_CFG(offset), get_ddma_cfg(ch));
		write32(pdat->virt + DMA_INT_CTL, read32(pdat->virt + DMA_INT_CTL) | (0x30000 << ((offset - 4) << 1)));
		write32(pdat->virt + DDMA_CH_CFG(offset), read32(pdat->virt + DDMA_CH_CFG(offset)) | (1 << 31));
		smp_mb();
	}
}

static void dma_f1c100s_stop(struct dmachip_t * chip, int offset)
{
	struct dma_f1c100s_pdata_t * pdat = (struct dma_f1c100s_pdata_t *)chip->priv;
	u32_t val;

	if(offset >= chip->ndma)
		return;
	if(offset < 4)
	{
		smp_mb();
		write32(pdat->virt + DMA_INT_STA, 0x3 << (offset << 1));
		write32(pdat->virt + DMA_INT_CTL, read32(pdat->virt + DMA_INT_CTL) & ~(0x3 << (offset << 1)));
		val = read32(pdat->virt + NDMA_CH_CFG(offset));
		val &= ~(1 << 31);
		write32(pdat->virt + NDMA_CH_CFG(offset), val);
		smp_mb();
	}
	else
	{
		smp_mb();
		write32(pdat->virt + DMA_INT_STA, 0x30000 << ((offset - 4) << 1));
		write32(pdat->virt + DMA_INT_CTL, read32(pdat->virt + DMA_INT_CTL) & ~(0x30000 << ((offset - 4) << 1)));
		val = read32(pdat->virt + DDMA_CH_CFG(offset));
		val &= ~(1 << 31);
		write32(pdat->virt + DDMA_CH_CFG(offset), val);
		smp_mb();
	}
}

static int dma_f1c100s_busying(struct dmachip_t * chip, int offset)
{
	struct dma_f1c100s_pdata_t * pdat = (struct dma_f1c100s_pdata_t *)chip->priv;

	if(offset < chip->ndma)
	{
		if(offset < 4)
		{
			if((read32(pdat->virt + NDMA_CH_CFG(offset)) & (0x3 << 30)) || (read32(pdat->virt + DMA_INT_CTL) & (0x3 << (offset << 1))))
				return 1;
		}
		else
		{
			if((read32(pdat->virt + DDMA_CH_CFG(offset)) & (0x3 << 30)) || (read32(pdat->virt + DMA_INT_CTL) & (0x30000 << ((offset - 4) << 1))))
				return 1;
		}
	}
	return 0;
}

static void dma_f1c100s_interrupt(void * data)
{
	struct dmachip_t * chip = (struct dmachip_t *)data;
	struct dma_f1c100s_pdata_t * pdat = (struct dma_f1c100s_pdata_t *)chip->priv;
	struct dma_channel_t * ch;
	u32_t pending;
	int i;

	pending = read32(pdat->virt + DMA_INT_STA);
	if(pending)
	{
		write32(pdat->virt + DMA_INT_STA, pending);
		for(i = 0; i < pdat->ndma; i++)
		{
			if(i < 4)
			{
				if(pending & (0x3 << (i << 1)))
				{
					ch = &chip->channel[i];
					if(pending & (0x1 << (i << 1)))
					{
						if(ch->half)
							ch->half(ch->data);
					}
					if(pending & (0x2 << (i << 1)))
					{
						ch->len += pdat->tsize[i];
						if(ch->len < ch->size)
						{
							smp_mb();
							write32(pdat->virt + NDMA_CH_SRC(i), (u32_t)(ch->src + ((DMA_G_SRC_INC(ch->flag) == DMA_INCREASE) ? ch->len : 0)));
							write32(pdat->virt + NDMA_CH_DST(i), (u32_t)(ch->dst + ((DMA_G_DST_INC(ch->flag) == DMA_INCREASE) ? ch->len : 0)));
							pdat->tsize[i] = min(0x20000, ch->size - ch->len);
							write32(pdat->virt + NDMA_CH_CNT(i), pdat->tsize[i]);
							write32(pdat->virt + NDMA_CH_CFG(i), get_ndma_cfg(ch));
							write32(pdat->virt + DMA_INT_CTL, (read32(pdat->virt + DMA_INT_CTL) & ~(0x3 << (i << 1))) | (0x2 << (i << 1)));
							write32(pdat->virt + NDMA_CH_CFG(i), read32(pdat->virt + NDMA_CH_CFG(i)) | (1 << 31));
							smp_mb();
						}
						else
						{
							dma_f1c100s_stop(chip, i);
							if(ch->finish)
								ch->finish(ch->data);
						}
					}
				}
			}
			else
			{
				if(pending & (0x30000 << ((i - 4) << 1)))
				{
					ch = &chip->channel[i];
					if(pending & (0x10000 << ((i - 4) << 1)))
					{
						if(ch->half)
							ch->half(ch->data);
					}
					if(pending & (0x20000 << ((i - 4) << 1)))
					{
						ch->len += pdat->tsize[i];
						if(ch->len < ch->size)
						{
							smp_mb();
							write32(pdat->virt + DDMA_CH_SRC(i), (u32_t)(ch->src + ((DMA_G_SRC_INC(ch->flag) == DMA_INCREASE) ? ch->len : 0)));
							write32(pdat->virt + DDMA_CH_DST(i), (u32_t)(ch->dst + ((DMA_G_DST_INC(ch->flag) == DMA_INCREASE) ? ch->len : 0)));
							pdat->tsize[i] = min(0x1000000, ch->size - ch->len);
							write32(pdat->virt + DDMA_CH_CNT(i), pdat->tsize[i]);
							write32(pdat->virt + DDMA_CH_CFG(i), get_ddma_cfg(ch));
							write32(pdat->virt + DMA_INT_CTL, (read32(pdat->virt + DMA_INT_CTL) & ~(0x30000 << ((i - 4) << 1))) | (0x20000 << ((i - 4) << 1)));
							write32(pdat->virt + DDMA_CH_CFG(i), read32(pdat->virt + DDMA_CH_CFG(i)) | (1 << 31));
							smp_mb();
						}
						else
						{
							dma_f1c100s_stop(chip, i);
							if(ch->finish)
								ch->finish(ch->data);
						}
					}
				}
			}
		}
	}
}

static struct device_t * dma_f1c100s_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct dma_f1c100s_pdata_t * pdat;
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

	pdat = malloc(sizeof(struct dma_f1c100s_pdata_t));
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

	chip->name = alloc_device_name(dt_read_name(n), -1);
	chip->base = pdat->base;
	chip->ndma = pdat->ndma;
	chip->channel = malloc(sizeof(struct dma_channel_t) * pdat->ndma);
	chip->start = dma_f1c100s_start;
	chip->stop = dma_f1c100s_stop;
	chip->busying = dma_f1c100s_busying;
	chip->priv = pdat;

	clk_enable(pdat->clk);
	if(pdat->reset >= 0)
	{
		reset_assert(pdat->reset);
		udelay(1);
		reset_deassert(pdat->reset);
		udelay(1);
	}
	request_irq(pdat->irq, dma_f1c100s_interrupt, IRQ_TYPE_NONE, chip);
	write32(pdat->virt + DMA_INT_CTL, 0x00000000);
	write32(pdat->virt + DMA_INT_STA, 0x00ff00ff);
	write32(pdat->virt + DMA_PTY_CFG, 0x00000190);
	for(i = 0; i < pdat->ndma; i++)
	{
		if(i < 4)
		{
			write32(pdat->virt + NDMA_CH_CFG(i), 0x0);
			write32(pdat->virt + NDMA_CH_SRC(i), 0x0);
			write32(pdat->virt + NDMA_CH_DST(i), 0x0);
			write32(pdat->virt + NDMA_CH_CNT(i), 0x0);
		}
		else
		{
			write32(pdat->virt + DDMA_CH_CFG(i), 0x0);
			write32(pdat->virt + DDMA_CH_SRC(i), 0x0);
			write32(pdat->virt + DDMA_CH_DST(i), 0x0);
			write32(pdat->virt + DDMA_CH_CNT(i), 0x0);
			write32(pdat->virt + DDMA_CH_PAR(i), 0x0);
			write32(pdat->virt + DDMA_CH_GEN(i), 0x0);
		}
	}

	if(!(dev = register_dmachip(chip, drv)))
	{
		clk_disable(pdat->clk);
		free_irq(pdat->irq);
		free(pdat->clk);
		free_device_name(chip->name);
		free(chip->channel);
		free(chip->priv);
		free(chip);
		return NULL;
	}
	return dev;
}

static void dma_f1c100s_remove(struct device_t * dev)
{
	struct dmachip_t * chip = (struct dmachip_t *)dev->priv;
	struct dma_f1c100s_pdata_t * pdat = (struct dma_f1c100s_pdata_t *)chip->priv;

	if(chip)
	{
		unregister_dmachip(chip);
		clk_disable(pdat->clk);
		free_irq(pdat->irq);
		free(pdat->clk);
		free_device_name(chip->name);
		free(chip->channel);
		free(chip->priv);
		free(chip);
	}
}

static void dma_f1c100s_suspend(struct device_t * dev)
{
}

static void dma_f1c100s_resume(struct device_t * dev)
{
}

static struct driver_t dma_f1c100s = {
	.name		= "dma-f1c100s",
	.probe		= dma_f1c100s_probe,
	.remove		= dma_f1c100s_remove,
	.suspend	= dma_f1c100s_suspend,
	.resume		= dma_f1c100s_resume,
};

static __init void dma_f1c100s_driver_init(void)
{
	register_driver(&dma_f1c100s);
}

static __exit void dma_f1c100s_driver_exit(void)
{
	unregister_driver(&dma_f1c100s);
}

driver_initcall(dma_f1c100s_driver_init);
driver_exitcall(dma_f1c100s_driver_exit);
