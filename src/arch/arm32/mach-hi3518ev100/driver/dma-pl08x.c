/*
 * driver/dma-pl08x.c
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

enum
{
	PL08X_INT_STATUS			= 0x00,
	PL08X_TC_STATUS				= 0x04,
	PL08X_TC_CLEAR				= 0x08,
	PL08X_ERR_STATUS			= 0x0c,
	PL08X_ERR_CLEAR				= 0x10,
	PL08X_RAW_TC_STATUS			= 0x14,
	PL08X_RAW_ERR_STATUS		= 0x18,
	PL08X_EN_CHAN				= 0x1c,
	PL08X_SOFT_BREQ				= 0x20,
	PL08X_SOFT_SREQ				= 0x24,
	PL08X_SOFT_LBREQ			= 0x28,
	PL08X_SOFT_LSREQ			= 0x2c,
	PL08X_CFG					= 0x30,
	PL08X_SYNC					= 0x34,
};

#define PL08X_CH_SRC(x)			((0x100 + (x * 0x20)))
#define PL08X_CH_DST(x)			((0x104 + (x * 0x20)))
#define PL08X_CH_LLI(x)			((0x108 + (x * 0x20)))
#define PL08X_CH_CTL(x)			((0x10c + (x * 0x20)))
#define PL08X_CH_CFG(x)			((0x110 + (x * 0x20)))

#define PL08X_CCTL_ITC			(1 << 31)
#define PL08X_CCTL_PROT_CACHE	(1 << 30)
#define PL08X_CCTL_PROT_BUFF	(1 << 29)
#define PL08X_CCTL_PROT_SYS		(1 << 28)
#define PL08X_CCTL_DI			(1 << 27)
#define PL08X_CCTL_SI			(1 << 26)
#define PL08X_CCTL_DWIDTH(x)	(((x) & 0x7) << 21)
#define PL08X_CCTL_SWIDTH(x)	(((x) & 0x7) << 18)
#define PL08X_CCTL_DBSIZE(x)	(((x) & 0x7) << 15)
#define PL08X_CCTL_SBSIZE(x)	(((x) & 0x7) << 12)
#define PL08X_CCTL_TSIZE(x)		(((x) & 0xfff) << 0)

#define PL08X_WIDTH_8			(0x0)
#define PL08X_WIDTH_16			(0x1)
#define PL08X_WIDTH_32			(0x2)

#define PL08X_BSIZE_1			(0x0)
#define PL08X_BSIZE_4			(0x1)
#define PL08X_BSIZE_8			(0x2)
#define PL08X_BSIZE_16			(0x3)
#define PL08X_BSIZE_32			(0x4)
#define PL08X_BSIZE_64			(0x5)
#define PL08X_BSIZE_128			(0x6)
#define PL08X_BSIZE_256			(0x7)

#define PL08X_CCFG_HALT			(1 << 18)
#define PL08X_CCFG_ACTIVE		(1 << 17)
#define PL08X_CCFG_LOCK			(1 << 16)
#define PL08X_CCFG_ITC			(1 << 15)
#define PL08X_CCFG_IE			(1 << 14)
#define PL08X_CCFG_FLOW(x)		(((x) & 0x7) << 11)
#define PL08X_CCFG_DST(x)		(((x) & 0xf) << 6)
#define PL08X_CCFG_SRC(x)		(((x) & 0xf) << 1)
#define PL08X_CCFG_EN			(1 << 0)

#define PL08X_FLOW_MEM2MEM		(0x0)
#define PL08X_FLOW_MEM2PER		(0x1)
#define PL08X_FLOW_PER2MEM		(0x2)
#define PL08X_FLOW_SRC2DST		(0x3)
#define PL08X_FLOW_SRC2DST_DST	(0x4)
#define PL08X_FLOW_MEM2PER_PER	(0x5)
#define PL08X_FLOW_PER2MEM_PER	(0x6)
#define PL08X_FLOW_SRC2DST_SRC	(0x7)

struct dma_pl08x_pdata_t
{
	virtual_addr_t virt;
	char * clk;
	int irq;
	int reset;
	int base;
	int ndma;
	int tsize;
};

static u32_t get_pl08x_ctl(struct dma_channel_t * ch, int tsize)
{
	u32_t ctl;
	int size;

	size = min(tsize, ch->size - ch->len) / (1 << DMA_G_SRC_WIDTH(ch->flag));
	ctl = PL08X_CCTL_ITC | PL08X_CCTL_TSIZE(size);
	switch(DMA_G_SRC_INC(ch->flag))
	{
	case DMA_INCREASE:
		ctl |= PL08X_CCTL_SI;
		break;
	case DMA_CONSTANT:
	default:
		break;
	}
	switch(DMA_G_DST_INC(ch->flag))
	{
	case DMA_INCREASE:
		ctl |= PL08X_CCTL_DI;
		break;
	case DMA_CONSTANT:
	default:
		break;
	}
	switch(DMA_G_SRC_WIDTH(ch->flag))
	{
	case DMA_WIDTH_8BIT:
		ctl |= PL08X_CCTL_SWIDTH(PL08X_WIDTH_8);
		break;
	case DMA_WIDTH_16BIT:
		ctl |= PL08X_CCTL_SWIDTH(PL08X_WIDTH_16);
		break;
	case DMA_WIDTH_32BIT:
		ctl |= PL08X_CCTL_SWIDTH(PL08X_WIDTH_32);
		break;
	case DMA_WIDTH_64BIT:
	default:
		break;
	}
	switch(DMA_G_DST_WIDTH(ch->flag))
	{
	case DMA_WIDTH_8BIT:
		ctl |= PL08X_CCTL_DWIDTH(PL08X_WIDTH_8);
		break;
	case DMA_WIDTH_16BIT:
		ctl |= PL08X_CCTL_DWIDTH(PL08X_WIDTH_16);
		break;
	case DMA_WIDTH_32BIT:
		ctl |= PL08X_CCTL_DWIDTH(PL08X_WIDTH_32);
		break;
	case DMA_WIDTH_64BIT:
	default:
		break;
	}
	switch(DMA_G_SRC_BURST(ch->flag))
	{
	case DMA_BURST_SIZE_1:
		ctl |= PL08X_CCTL_SBSIZE(PL08X_BSIZE_1);
		break;
	case DMA_BURST_SIZE_4:
		ctl |= PL08X_CCTL_SBSIZE(PL08X_BSIZE_4);
		break;
	case DMA_BURST_SIZE_8:
		ctl |= PL08X_CCTL_SBSIZE(PL08X_BSIZE_8);
		break;
	case DMA_BURST_SIZE_16:
		ctl |= PL08X_CCTL_SBSIZE(PL08X_BSIZE_16);
		break;
	case DMA_BURST_SIZE_32:
		ctl |= PL08X_CCTL_SBSIZE(PL08X_BSIZE_32);
		break;
	case DMA_BURST_SIZE_64:
		ctl |= PL08X_CCTL_SBSIZE(PL08X_BSIZE_64);
		break;
	case DMA_BURST_SIZE_128:
		ctl |= PL08X_CCTL_SBSIZE(PL08X_BSIZE_128);
		break;
	case DMA_BURST_SIZE_256:
		ctl |= PL08X_CCTL_SBSIZE(PL08X_BSIZE_256);
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
	switch(DMA_G_DST_BURST(ch->flag))
	{
	case DMA_BURST_SIZE_1:
		ctl |= PL08X_CCTL_DBSIZE(PL08X_BSIZE_1);
		break;
	case DMA_BURST_SIZE_4:
		ctl |= PL08X_CCTL_DBSIZE(PL08X_BSIZE_4);
		break;
	case DMA_BURST_SIZE_8:
		ctl |= PL08X_CCTL_DBSIZE(PL08X_BSIZE_8);
		break;
	case DMA_BURST_SIZE_16:
		ctl |= PL08X_CCTL_DBSIZE(PL08X_BSIZE_16);
		break;
	case DMA_BURST_SIZE_32:
		ctl |= PL08X_CCTL_DBSIZE(PL08X_BSIZE_32);
		break;
	case DMA_BURST_SIZE_64:
		ctl |= PL08X_CCTL_DBSIZE(PL08X_BSIZE_64);
		break;
	case DMA_BURST_SIZE_128:
		ctl |= PL08X_CCTL_DBSIZE(PL08X_BSIZE_128);
		break;
	case DMA_BURST_SIZE_256:
		ctl |= PL08X_CCTL_DBSIZE(PL08X_BSIZE_256);
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

static u32_t get_pl08x_cfg(struct dma_channel_t * ch)
{
	u32_t cfg;

	cfg = PL08X_CCFG_ITC | PL08X_CCFG_IE | PL08X_CCFG_DST(DMA_G_DST_PORT(ch->flag)) | PL08X_CCFG_SRC(DMA_G_SRC_PORT(ch->flag));
	switch(DMA_G_TYPE(ch->flag))
	{
	case DMA_TYPE_MEMTOMEM:
		cfg |= PL08X_CCFG_FLOW(PL08X_FLOW_MEM2MEM);
		break;
	case DMA_TYPE_MEMTODEV:
		cfg |= PL08X_CCFG_FLOW(PL08X_FLOW_MEM2PER);
		break;
	case DMA_TYPE_DEVTOMEM:
		cfg |= PL08X_CCFG_FLOW(PL08X_FLOW_PER2MEM);
		break;
	case DMA_TYPE_DEVTODEV:
		cfg |= PL08X_CCFG_FLOW(PL08X_FLOW_SRC2DST);
		break;
	default:
		break;
	}
	return cfg;
}

static void dma_pl08x_start(struct dmachip_t * chip, int offset)
{
	struct dma_pl08x_pdata_t * pdat = (struct dma_pl08x_pdata_t *)chip->priv;
	struct dma_channel_t * ch;

	if(offset >= chip->ndma)
		return;

	ch = &chip->channel[offset];
	while(read32(pdat->virt + PL08X_EN_CHAN) & (1 << offset));
	while(read32(pdat->virt + PL08X_CH_CFG(offset)) & (PL08X_CCFG_ACTIVE | PL08X_CCFG_EN));
	smp_mb();
	write32(pdat->virt + PL08X_CH_SRC(offset), (u32_t)(ch->src + ch->len));
	write32(pdat->virt + PL08X_CH_DST(offset), (u32_t)(ch->dst + ch->len));
	write32(pdat->virt + PL08X_CH_LLI(offset), 0);
	write32(pdat->virt + PL08X_CH_CTL(offset), get_pl08x_ctl(ch, pdat->tsize));
	write32(pdat->virt + PL08X_CH_CFG(offset), get_pl08x_cfg(ch));
	write32(pdat->virt + PL08X_CH_CFG(offset), read32(pdat->virt + PL08X_CH_CFG(offset)) | PL08X_CCFG_EN);
	write32(pdat->virt + PL08X_CFG, read32(pdat->virt + PL08X_CFG) | (1 << 0));
	smp_mb();
}

static void dma_pl08x_stop(struct dmachip_t * chip, int offset)
{
	struct dma_pl08x_pdata_t * pdat = (struct dma_pl08x_pdata_t *)chip->priv;
	u32_t val;

	if(offset >= chip->ndma)
		return;

	val = read32(pdat->virt + PL08X_CH_CFG(offset));
	val |= PL08X_CCFG_HALT;
	write32(pdat->virt + PL08X_CH_CFG(offset), val);

	do {
		val = read32(pdat->virt + PL08X_CH_CFG(offset));
	} while(val & PL08X_CCFG_ACTIVE);

	smp_mb();
	val = read32(pdat->virt + PL08X_CH_CFG(offset));
	val &= ~PL08X_CCFG_EN;
	write32(pdat->virt + PL08X_CH_CFG(offset), val);
	smp_mb();
}

static int dma_pl08x_busying(struct dmachip_t * chip, int offset)
{
	struct dma_pl08x_pdata_t * pdat = (struct dma_pl08x_pdata_t *)chip->priv;

	if(offset < chip->ndma)
	{
		if(read32(pdat->virt + PL08X_EN_CHAN) & (1 << offset))
			return 1;
		if((read32(pdat->virt + PL08X_CH_CFG(offset)) & (PL08X_CCFG_ACTIVE | PL08X_CCFG_EN)))
			return 1;
	}
	return 0;
}

static void dma_pl08x_interrupt(void * data)
{
	struct dmachip_t * chip = (struct dmachip_t *)data;
	struct dma_pl08x_pdata_t * pdat = (struct dma_pl08x_pdata_t *)chip->priv;
	struct dma_channel_t * ch;
	u32_t err, tc;
	int i;

	err = read32(pdat->virt + PL08X_ERR_STATUS);
	if(err)
		write32(pdat->virt + PL08X_ERR_CLEAR, err);

	tc = read32(pdat->virt + PL08X_TC_STATUS);
	if(tc)
		write32(pdat->virt + PL08X_TC_CLEAR, tc);

	if(err || tc)
	{
		for(i = 0; i < pdat->ndma; i++)
		{
			if(((1 << i) & err) || ((1 << i) & tc))
			{
				ch = &chip->channel[i];
				ch->len += pdat->tsize;
				if(ch->len < ch->size)
				{
					while(read32(pdat->virt + PL08X_EN_CHAN) & (1 << i));
					while(read32(pdat->virt + PL08X_CH_CFG(i)) & (PL08X_CCFG_ACTIVE | PL08X_CCFG_EN));
					smp_mb();
					write32(pdat->virt + PL08X_CH_SRC(i), (u32_t)(ch->src + ch->len));
					write32(pdat->virt + PL08X_CH_DST(i), (u32_t)(ch->dst + ch->len));
					write32(pdat->virt + PL08X_CH_LLI(i), 0);
					write32(pdat->virt + PL08X_CH_CTL(i), get_pl08x_ctl(ch, pdat->tsize));
					write32(pdat->virt + PL08X_CH_CFG(i), get_pl08x_cfg(ch));
					write32(pdat->virt + PL08X_CH_CFG(i), read32(pdat->virt + PL08X_CH_CFG(i)) | PL08X_CCFG_EN);
					write32(pdat->virt + PL08X_CFG, read32(pdat->virt + PL08X_CFG) | (1 << 0));
					smp_mb();
				}
				else
				{
					while(read32(pdat->virt + PL08X_EN_CHAN) & (1 << i));
					while(read32(pdat->virt + PL08X_CH_CFG(i)) & (PL08X_CCFG_ACTIVE | PL08X_CCFG_EN));
					smp_mb();
					write32(pdat->virt + PL08X_CH_CFG(i), read32(pdat->virt + PL08X_CH_CFG(i)) & ~PL08X_CCFG_EN);
					smp_mb();
					if(ch->complete)
						ch->complete(ch->data);
				}
			}
		}
	}
}

static struct device_t * dma_pl08x_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct dma_pl08x_pdata_t * pdat;
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

	if(((id >> 12) & 0xff) != 0x41 || ((id & 0xfff) != 0x080 && (id & 0xfff) != 0x081))
		return NULL;

	if(!search_clk(clk))
		return NULL;

	if(!irq_is_valid(irq))
		return NULL;

	if((base < 0) || (ndma <= 0))
		return NULL;

	pdat = malloc(sizeof(struct dma_pl08x_pdata_t));
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
	pdat->tsize = 1024;

	chip->name = alloc_device_name(dt_read_name(n), -1);
	chip->base = pdat->base;
	chip->ndma = pdat->ndma;
	chip->channel = malloc(sizeof(struct dma_channel_t) * pdat->ndma);
	chip->start = dma_pl08x_start;
	chip->stop = dma_pl08x_stop;
	chip->busying = dma_pl08x_busying;
	chip->priv = pdat;

	clk_enable(pdat->clk);
	if(pdat->reset >= 0)
		reset_deassert(pdat->reset);
	request_irq(pdat->irq, dma_pl08x_interrupt, IRQ_TYPE_NONE, chip);
	write32(pdat->virt + PL08X_CFG, 0x0);
	write32(pdat->virt + PL08X_TC_CLEAR, (1 << pdat->ndma) - 1);
	write32(pdat->virt + PL08X_ERR_CLEAR, (1 << pdat->ndma) - 1);
	write32(pdat->virt + PL08X_SYNC, 0x0);
	for(i = 0; i < pdat->ndma; i++)
	{
		write32(pdat->virt + PL08X_CH_SRC(i), 0);
		write32(pdat->virt + PL08X_CH_DST(i), 0);
		write32(pdat->virt + PL08X_CH_LLI(i), 0);
		write32(pdat->virt + PL08X_CH_CTL(i), 0);
		write32(pdat->virt + PL08X_CH_CFG(i), 0);
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

static void dma_pl08x_remove(struct device_t * dev)
{
	struct dmachip_t * chip = (struct dmachip_t *)dev->priv;
	struct dma_pl08x_pdata_t * pdat = (struct dma_pl08x_pdata_t *)chip->priv;

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

static void dma_pl08x_suspend(struct device_t * dev)
{
}

static void dma_pl08x_resume(struct device_t * dev)
{
}

static struct driver_t dma_pl08x = {
	.name		= "dma-pl08x",
	.probe		= dma_pl08x_probe,
	.remove		= dma_pl08x_remove,
	.suspend	= dma_pl08x_suspend,
	.resume		= dma_pl08x_resume,
};

static __init void dma_pl08x_driver_init(void)
{
	register_driver(&dma_pl08x);
}

static __exit void dma_pl08x_driver_exit(void)
{
	unregister_driver(&dma_pl08x);
}

driver_initcall(dma_pl08x_driver_init);
driver_exitcall(dma_pl08x_driver_exit);
