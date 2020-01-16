/*
 * driver/dma-pl081.c
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
#include <clk/clk.h>
#include <interrupt/interrupt.h>
#include <dma/dma.h>

enum
{
	PL081_INT_STATUS			= 0x00,
	PL081_TC_STATUS				= 0x04,
	PL081_TC_CLEAR				= 0x08,
	PL081_ERR_STATUS			= 0x0c,
	PL081_ERR_CLEAR				= 0x10,
	PL081_RAW_TC_STATUS			= 0x14,
	PL081_RAW_ERR_STATUS		= 0x18,
	PL081_EN_CHAN				= 0x1c,
	PL081_SOFT_BREQ				= 0x20,
	PL081_SOFT_SREQ				= 0x24,
	PL081_SOFT_LBREQ			= 0x28,
	PL081_SOFT_LSREQ			= 0x2c,
	PL081_CFG					= 0x30,
	PL081_SYNC					= 0x34,
};

#define PL081_CH_SRC(x)			((0x100 + (x * 0x20)))
#define PL081_CH_DST(x)			((0x104 + (x * 0x20)))
#define PL081_CH_LLI(x)			((0x108 + (x * 0x20)))
#define PL081_CH_CTL(x)			((0x10c + (x * 0x20)))
#define PL081_CH_CFG(x)			((0x110 + (x * 0x20)))

#define PL081_CCTL_ITC			(1 << 31)
#define PL081_CCTL_PROT_CACHE	(1 << 30)
#define PL081_CCTL_PROT_BUFF	(1 << 29)
#define PL081_CCTL_PROT_SYS		(1 << 28)
#define PL081_CCTL_DI			(1 << 27)
#define PL081_CCTL_SI			(1 << 26)
#define PL081_CCTL_DWIDTH(x)	(((x) & 0x7) << 21)
#define PL081_CCTL_SWIDTH(x)	(((x) & 0x7) << 18)
#define PL081_CCTL_DBSIZE(x)	(((x) & 0x7) << 15)
#define PL081_CCTL_SBSIZE(x)	(((x) & 0x7) << 12)
#define PL081_CCTL_TSIZE(x)		(((x) & 0xfff) << 0)

#define PL081_WIDTH_8			(0x0)
#define PL081_WIDTH_16			(0x1)
#define PL081_WIDTH_32			(0x2)

#define PL081_BSIZE_1			(0x0)
#define PL081_BSIZE_4			(0x1)
#define PL081_BSIZE_8			(0x2)
#define PL081_BSIZE_16			(0x3)
#define PL081_BSIZE_32			(0x4)
#define PL081_BSIZE_64			(0x5)
#define PL081_BSIZE_128			(0x6)
#define PL081_BSIZE_256			(0x7)

#define PL081_CCFG_HALT			(1 << 18)
#define PL081_CCFG_ACTIVE		(1 << 17)
#define PL081_CCFG_LOCK			(1 << 16)
#define PL081_CCFG_ITC			(1 << 15)
#define PL081_CCFG_IE			(1 << 14)
#define PL081_CCFG_FLOW(x)		(((x) & 0x7) << 11)
#define PL081_CCFG_DST(x)		(((x) & 0xf) << 6)
#define PL081_CCFG_SRC(x)		(((x) & 0xf) << 1)
#define PL081_CCFG_EN			(1 << 0)

#define PL081_FLOW_MEM2MEM		(0x0)
#define PL081_FLOW_MEM2PER		(0x1)
#define PL081_FLOW_PER2MEM		(0x2)
#define PL081_FLOW_SRC2DST		(0x3)
#define PL081_FLOW_SRC2DST_DST	(0x4)
#define PL081_FLOW_MEM2PER_PER	(0x5)
#define PL081_FLOW_PER2MEM_PER	(0x6)
#define PL081_FLOW_SRC2DST_SRC	(0x7)

struct dma_pl081_pdata_t
{
	virtual_addr_t virt;
	char * clk;
	int irq;
	int base;
	int ndma;
};

static u32_t get_pl081_ctl(struct dma_channel_t * ch)
{
	u32_t ctl;
	int size;

	size = min(DMA_G_TSIZE(ch->flag), ch->size - ch->len);
	ctl = PL081_CCTL_ITC | PL081_CCTL_TSIZE(size);
	switch(DMA_S_SRC_INC(ch->flag))
	{
	case DMA_INCREASE:
		ctl |= PL081_CCTL_SI;
		break;
	case DMA_CONSTANT:
	default:
		break;
	}
	switch(DMA_S_DST_INC(ch->flag))
	{
	case DMA_INCREASE:
		ctl |= PL081_CCTL_DI;
		break;
	case DMA_CONSTANT:
	default:
		break;
	}
	switch(DMA_S_SRC_WIDTH(ch->flag))
	{
	case DMA_WIDTH_1:
		ctl |= PL081_CCTL_SWIDTH(PL081_WIDTH_8);
		break;
	case DMA_WIDTH_2:
		ctl |= PL081_CCTL_SWIDTH(PL081_WIDTH_16);
		break;
	case DMA_WIDTH_4:
		ctl |= PL081_CCTL_SWIDTH(PL081_WIDTH_32);
		break;
	case DMA_WIDTH_8:
	default:
		break;
	}
	switch(DMA_S_DST_WIDTH(ch->flag))
	{
	case DMA_WIDTH_1:
		ctl |= PL081_CCTL_DWIDTH(PL081_WIDTH_8);
		break;
	case DMA_WIDTH_2:
		ctl |= PL081_CCTL_DWIDTH(PL081_WIDTH_16);
		break;
	case DMA_WIDTH_4:
		ctl |= PL081_CCTL_DWIDTH(PL081_WIDTH_32);
		break;
	case DMA_WIDTH_8:
	default:
		break;
	}
	switch(DMA_S_SRC_BURST(ch->flag))
	{
	case DMA_BURST_SIZE_1:
		ctl |= PL081_CCTL_SBSIZE(PL081_BSIZE_1);
		break;
	case DMA_BURST_SIZE_4:
		ctl |= PL081_CCTL_SBSIZE(PL081_BSIZE_4);
		break;
	case DMA_BURST_SIZE_8:
		ctl |= PL081_CCTL_SBSIZE(PL081_BSIZE_8);
		break;
	case DMA_BURST_SIZE_16:
		ctl |= PL081_CCTL_SBSIZE(PL081_BSIZE_16);
		break;
	case DMA_BURST_SIZE_32:
		ctl |= PL081_CCTL_SBSIZE(PL081_BSIZE_32);
		break;
	case DMA_BURST_SIZE_64:
		ctl |= PL081_CCTL_SBSIZE(PL081_BSIZE_64);
		break;
	case DMA_BURST_SIZE_128:
		ctl |= PL081_CCTL_SBSIZE(PL081_BSIZE_128);
		break;
	case DMA_BURST_SIZE_256:
		ctl |= PL081_CCTL_SBSIZE(PL081_BSIZE_256);
		break;
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
	switch(DMA_S_DST_BURST(ch->flag))
	{
	case DMA_BURST_SIZE_1:
		ctl |= PL081_CCTL_DBSIZE(PL081_BSIZE_1);
		break;
	case DMA_BURST_SIZE_4:
		ctl |= PL081_CCTL_DBSIZE(PL081_BSIZE_4);
		break;
	case DMA_BURST_SIZE_8:
		ctl |= PL081_CCTL_DBSIZE(PL081_BSIZE_8);
		break;
	case DMA_BURST_SIZE_16:
		ctl |= PL081_CCTL_DBSIZE(PL081_BSIZE_16);
		break;
	case DMA_BURST_SIZE_32:
		ctl |= PL081_CCTL_DBSIZE(PL081_BSIZE_32);
		break;
	case DMA_BURST_SIZE_64:
		ctl |= PL081_CCTL_DBSIZE(PL081_BSIZE_64);
		break;
	case DMA_BURST_SIZE_128:
		ctl |= PL081_CCTL_DBSIZE(PL081_BSIZE_128);
		break;
	case DMA_BURST_SIZE_256:
		ctl |= PL081_CCTL_DBSIZE(PL081_BSIZE_256);
		break;
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
	return ctl;
}

static u32_t get_pl081_cfg(struct dma_channel_t * ch)
{
	u32_t cfg;

	cfg = PL081_CCFG_ITC | PL081_CCFG_IE | PL081_CCFG_DST(0) | PL081_CCFG_SRC(0) | PL081_CCFG_EN;
	switch(DMA_G_TYPE(ch->flag))
	{
	case DMA_TYPE_MEMTOMEM:
		cfg |= PL081_CCFG_FLOW(PL081_FLOW_MEM2MEM);
		break;
	case DMA_TYPE_MEMTODEV:
		cfg |= PL081_CCFG_FLOW(PL081_FLOW_MEM2PER);
		break;
	case DMA_TYPE_DEVTOMEM:
		cfg |= PL081_CCFG_FLOW(PL081_FLOW_PER2MEM);
		break;
	case DMA_TYPE_DEVTODEV:
		cfg |= PL081_CCFG_FLOW(PL081_FLOW_SRC2DST);
		break;
	default:
		break;
	}
	return cfg;
}

static void dma_pl081_start(struct dmachip_t * chip, int offset)
{
	struct dma_pl081_pdata_t * pdat = (struct dma_pl081_pdata_t *)chip->priv;
	struct dma_channel_t * ch;

	if(offset >= chip->ndma)
		return;

	ch = &chip->channel[offset];
	write32(pdat->virt + PL081_CH_SRC(offset), (u32_t)(ch->src + ch->len));
	write32(pdat->virt + PL081_CH_DST(offset), (u32_t)(ch->dst + ch->len));
	write32(pdat->virt + PL081_CH_LLI(offset), 0);
	write32(pdat->virt + PL081_CH_CTL(offset), get_pl081_ctl(ch));
	write32(pdat->virt + PL081_CH_CFG(offset), get_pl081_cfg(ch));
	write32(pdat->virt + PL081_CFG, read32(pdat->virt + PL081_CFG) | (1 << 0));
}

static void dma_pl081_stop(struct dmachip_t * chip, int offset)
{
	struct dma_pl081_pdata_t * pdat = (struct dma_pl081_pdata_t *)chip->priv;

	if(offset >= chip->ndma)
		return;

	write32(pdat->virt + PL081_CH_SRC(offset), 0);
	write32(pdat->virt + PL081_CH_DST(offset), 0);
	write32(pdat->virt + PL081_CH_LLI(offset), 0);
	write32(pdat->virt + PL081_CH_CTL(offset), 0);
	write32(pdat->virt + PL081_CH_CFG(offset), 0);
}

static void dma_pl081_wait(struct dmachip_t * chip, int offset)
{
	struct dma_pl081_pdata_t * pdat = (struct dma_pl081_pdata_t *)chip->priv;
}

static void dma_pl081_interrupt(void * data)
{
	struct dmachip_t * chip = (struct dmachip_t *)data;
	struct dma_pl081_pdata_t * pdat = (struct dma_pl081_pdata_t *)chip->priv;
	struct dma_channel_t * ch;
	u32_t err, tc;
	int i;

	err = read32(pdat->virt + PL081_ERR_STATUS);
	if(err)
		write32(pdat->virt + PL081_ERR_CLEAR, err);

	tc = read32(pdat->virt + PL081_TC_STATUS);
	if(tc)
		write32(pdat->virt + PL081_TC_CLEAR, tc);

	if(err || tc)
	{
		for(i = 0; i < pdat->ndma; i++)
		{
			if(((1 << i) & err) || ((1 << i) & tc))
			{
				ch = &chip->channel[i];
				ch->len += DMA_G_TSIZE(ch->flag);
				if(ch->len < ch->size)
				{
					write32(pdat->virt + PL081_CH_SRC(i), (u32_t)(ch->src + ch->len));
					write32(pdat->virt + PL081_CH_DST(i), (u32_t)(ch->dst + ch->len));
					write32(pdat->virt + PL081_CH_LLI(i), 0);
					write32(pdat->virt + PL081_CH_CTL(i), get_pl081_ctl(ch));
					write32(pdat->virt + PL081_CH_CFG(i), get_pl081_cfg(ch));
					write32(pdat->virt + PL081_CFG, read32(pdat->virt + PL081_CFG) | (1 << 0));
				}
				else
				{
					write32(pdat->virt + PL081_CH_SRC(i), 0);
					write32(pdat->virt + PL081_CH_DST(i), 0);
					write32(pdat->virt + PL081_CH_LLI(i), 0);
					write32(pdat->virt + PL081_CH_CTL(i), 0);
					write32(pdat->virt + PL081_CH_CFG(i), 0);
					if(ch->complete)
						ch->complete(ch->data);
				}
			}
		}
	}
}

static struct device_t * dma_pl081_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct dma_pl081_pdata_t * pdat;
	struct dmachip_t * chip;
	struct device_t * dev;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	char * clk = dt_read_string(n, "clock-name", NULL);
	int irq = dt_read_int(n, "interrupt", -1);
	int base = dt_read_int(n, "dma-base", -1);
	int ndma = dt_read_int(n, "dma-count", -1);
	int i;
	u32_t id = (((read32(virt + 0xfec) & 0xff) << 24) |
				((read32(virt + 0xfe8) & 0xff) << 16) |
				((read32(virt + 0xfe4) & 0xff) <<  8) |
				((read32(virt + 0xfe0) & 0xff) <<  0));

	if(((id >> 12) & 0xff) != 0x41 || (id & 0xfff) != 0x081)
		return NULL;

	if(!search_clk(clk))
		return NULL;

	if(!irq_is_valid(irq))
		return NULL;

	if((base < 0) || (ndma <= 0))
		return NULL;

	pdat = malloc(sizeof(struct dma_pl081_pdata_t));
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
	pdat->irq = irq;
	pdat->base = base;
	pdat->ndma = ndma;

	chip->name = alloc_device_name(dt_read_name(n), -1);
	chip->base = pdat->base;
	chip->ndma = pdat->ndma;
	chip->channel = malloc(sizeof(struct dma_channel_t) * pdat->ndma);
	chip->start = dma_pl081_start;
	chip->stop = dma_pl081_stop;
	chip->wait = dma_pl081_wait;
	chip->priv = pdat;

	request_irq(pdat->irq, dma_pl081_interrupt, IRQ_TYPE_NONE, chip);
	clk_enable(pdat->clk);
	for(i = 0; i < pdat->ndma; i++)
	{
		write32(pdat->virt + PL081_CH_SRC(i), 0);
		write32(pdat->virt + PL081_CH_DST(i), 0);
		write32(pdat->virt + PL081_CH_LLI(i), 0);
		write32(pdat->virt + PL081_CH_CTL(i), 0);
		write32(pdat->virt + PL081_CH_CFG(i), 0);
	}
	write32(pdat->virt + PL081_TC_CLEAR, 0x3);
	write32(pdat->virt + PL081_ERR_CLEAR, 0x3);
	write32(pdat->virt + PL081_CFG, 0x0);

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

static void dma_pl081_remove(struct device_t * dev)
{
	struct dmachip_t * chip = (struct dmachip_t *)dev->priv;
	struct dma_pl081_pdata_t * pdat = (struct dma_pl081_pdata_t *)chip->priv;

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

static void dma_pl081_suspend(struct device_t * dev)
{
}

static void dma_pl081_resume(struct device_t * dev)
{
}

static struct driver_t dma_pl081 = {
	.name		= "dma-pl081",
	.probe		= dma_pl081_probe,
	.remove		= dma_pl081_remove,
	.suspend	= dma_pl081_suspend,
	.resume		= dma_pl081_resume,
};

static __init void dma_pl081_driver_init(void)
{
	register_driver(&dma_pl081);
}

static __exit void dma_pl081_driver_exit(void)
{
	unregister_driver(&dma_pl081);
}

driver_initcall(dma_pl081_driver_init);
driver_exitcall(dma_pl081_driver_exit);
