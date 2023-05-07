/*
 * driver/g2d-d1.c
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
#include <clk/clk.h>
#include <reset/reset.h>
#include <dma/dma.h>
#include <g2d/g2d.h>
#include <d1/reg-g2d.h>

struct g2d_d1_pdata_t {
	virtual_addr_t virt;
	char * clk;
	int reset;
	struct mutex_t m;
};

static inline int dcmp(double a, double b)
{
	return (fabs(a - b) < 0.000001);
}

static inline u32_t lo32(void * addr)
{
	return (u32_t)((unsigned long)addr);
}

static inline u32_t hi32(void * addr)
{
	if(sizeof(addr) == sizeof(u32_t))
		return 0;
	return (u32_t)(((u64_t)(unsigned long)addr) >> 32);
}

static inline void d1_g2d_init(struct g2d_d1_pdata_t * pdat)
{
	struct g2d_top_t * g2d_top = (struct g2d_top_t *)(pdat->virt + D1_G2D_TOP);

	g2d_top->G2D_SCLK_GATE = 0x3;
	g2d_top->G2D_HCLK_GATE = 0x3;
	g2d_top->G2D_SCLK_DIV = (0x0 << 4) | (0x0 << 0);
	g2d_top->G2D_AHB_RESET = 0x0;
	g2d_top->G2D_AHB_RESET = 0x3;
}

static inline void d1_g2d_mixer_reset(struct g2d_d1_pdata_t * pdat)
{
	struct g2d_top_t * g2d_top = (struct g2d_top_t *)(pdat->virt + D1_G2D_TOP);

	g2d_top->G2D_AHB_RESET = g2d_top->G2D_AHB_RESET & ~(0x1 << 0);
	g2d_top->G2D_AHB_RESET = g2d_top->G2D_AHB_RESET | (0x1 << 0);
}

static inline void d1_g2d_rot_reset(struct g2d_d1_pdata_t * pdat)
{
	struct g2d_top_t * g2d_top = (struct g2d_top_t *)(pdat->virt + D1_G2D_TOP);

	g2d_top->G2D_AHB_RESET = g2d_top->G2D_AHB_RESET & ~(0x1 << 1);
	g2d_top->G2D_AHB_RESET = g2d_top->G2D_AHB_RESET | (0x1 << 1);
}

static inline void d1_g2d_mixer_start(struct g2d_d1_pdata_t * pdat)
{
	struct g2d_mixer_t * g2d_mixer = (struct g2d_mixer_t *)(pdat->virt + D1_G2D_MIXER);

	g2d_mixer->G2D_MIXER_INT = 0x10;
	g2d_mixer->G2D_MIXER_CTL |= (0x1 << 31);
}

static inline int d1_g2d_wait(struct g2d_d1_pdata_t * pdat, int timeout)
{
	struct g2d_mixer_t * g2d_mixer = (struct g2d_mixer_t *)(pdat->virt + D1_G2D_MIXER);
	struct g2d_rot_t * g2d_rot = (struct g2d_rot_t *)(pdat->virt + D1_G2D_ROT);
	ktime_t t = ktime_add_ms(ktime_get(), timeout);

	do {
		if(g2d_mixer->G2D_MIXER_INT & (0x1 << 0))
		{
			g2d_mixer->G2D_MIXER_INT = 0x1 << 0;
			d1_g2d_mixer_reset(pdat);
			return 1;
		}
		else if(g2d_rot->ROT_INT & (0x1 << 0))
		{
			g2d_rot->ROT_INT = 0x1 << 0;
			d1_g2d_rot_reset(pdat);
			return 1;
		}
	} while(ktime_before(ktime_get(), t));

	return 0;
}

static int d1_g2d_blit(struct g2d_d1_pdata_t * pdat, struct surface_t * s, struct surface_t * o, int x, int y, int w, int h)
{
	struct g2d_vsu_t * g2d_vsu = (struct g2d_vsu_t *)(pdat->virt + D1_G2D_VSU);
	struct g2d_rot_t * g2d_rot = (struct g2d_rot_t *)(pdat->virt + D1_G2D_ROT);
	struct g2d_bld_t * g2d_bld = (struct g2d_bld_t *)(pdat->virt + D1_G2D_BLD);
	struct g2d_wb_t * g2d_wb = (struct g2d_wb_t *)(pdat->virt + D1_G2D_WB);
	struct g2d_v0_t * g2d_v0 = (struct g2d_v0_t *)(pdat->virt + D1_G2D_V0);
	struct g2d_ui_t * g2d_ui0 = (struct g2d_ui_t *)(pdat->virt + D1_G2D_UI0);
	struct g2d_ui_t * g2d_ui1 = (struct g2d_ui_t *)(pdat->virt + D1_G2D_UI1);
	struct g2d_ui_t * g2d_ui2 = (struct g2d_ui_t *)(pdat->virt + D1_G2D_UI2);
	void * addr = s->pixels + y * s->stride + (x << 2);
	uint32_t hw = ((h - 1) << 16) | ((w - 1) << 0);
	int ret = 0;

	mutex_lock(&pdat->m);
	{
		g2d_vsu->VS_CTRL = 0x0;
		g2d_rot->ROT_CTL = 0x0;
		g2d_ui0->UI_ATTR = 0x0;
		g2d_ui1->UI_ATTR = 0x0;

		g2d_v0->V0_ATTR = (0xff << 24) | (0x1 << 17) | (0x0 << 8) | (0x0 << 1) | (0x1 << 0);
		g2d_v0->V0_MBSIZE = hw;
		g2d_v0->V0_SIZE = hw;
		g2d_v0->V0_COOR = 0;
		g2d_v0->V0_PITCH0 = s->stride;
		g2d_v0->V0_LADD0 = lo32(addr);
		g2d_v0->V0_PITCH1 = 0;
		g2d_v0->V0_LADD1 = 0;
		g2d_v0->V0_PITCH2 = 0;
		g2d_v0->V0_LADD2 = 0;
		g2d_v0->V0_HADD = hi32(addr);

		g2d_ui2->UI_ATTR = (0xff << 24) | (0x1 << 17) | (0x0 << 8) | (0x0 << 1) | (0x1 << 0);
		g2d_ui2->UI_MBSIZE = hw;
		g2d_ui2->UI_SIZE = hw;
		g2d_ui2->UI_COOR = 0;
		g2d_ui2->UI_PITCH = o->stride;
		g2d_ui2->UI_LADD = lo32(o->pixels);
		g2d_ui2->UI_HADD = hi32(o->pixels);

		g2d_bld->BLD_CH_ISIZE[0] = hw;
		g2d_bld->BLD_CH_OFFSET[0] = 0;
		g2d_bld->BLD_CH_ISIZE[1] = hw;
		g2d_bld->BLD_CH_OFFSET[1] = 0;
		g2d_bld->BLD_SIZE = hw;
		g2d_bld->BLD_EN_CTL = (0x1 << 9) | (0x1 << 8);
		g2d_bld->BLD_PREMUL_CTL = (0x1 << 1) | (0x1 << 0);
		g2d_bld->BLD_OUT_COLOR = (0x1 << 0);
		g2d_bld->BLD_KEY_CTL = 0x3;
		g2d_bld->ROP_CTL = 0xf0;
		g2d_bld->BLD_CTL = 0x03010301;

		g2d_wb->WB_ATTR = 0;
		g2d_wb->WB_SIZE = hw;
		g2d_wb->WB_PITCH0 = s->stride;
		g2d_wb->WB_LADD0 = lo32(addr);
		g2d_wb->WB_HADD0 = hi32(addr);
		g2d_wb->WB_PITCH1 = 0;
		g2d_wb->WB_LADD1 = 0;
		g2d_wb->WB_HADD1 = 0;
		g2d_wb->WB_PITCH2 = 0;
		g2d_wb->WB_LADD2 = 0;
		g2d_wb->WB_HADD2 = 0;

		dma_cache_sync(s->pixels, s->pixlen, DMA_TO_DEVICE);
		dma_cache_sync(o->pixels, o->pixlen, DMA_FROM_DEVICE);
		d1_g2d_mixer_start(pdat);
		ret = d1_g2d_wait(pdat, 100);
	}
	mutex_unlock(&pdat->m);

	return ret;
}

static int d1_g2d_fill(struct g2d_d1_pdata_t * pdat, struct surface_t * s, int x, int y, int w, int h, struct color_t * c)
{
	struct g2d_vsu_t * g2d_vsu = (struct g2d_vsu_t *)(pdat->virt + D1_G2D_VSU);
	struct g2d_rot_t * g2d_rot = (struct g2d_rot_t *)(pdat->virt + D1_G2D_ROT);
	struct g2d_bld_t * g2d_bld = (struct g2d_bld_t *)(pdat->virt + D1_G2D_BLD);
	struct g2d_wb_t * g2d_wb = (struct g2d_wb_t *)(pdat->virt + D1_G2D_WB);
	struct g2d_v0_t * g2d_v0 = (struct g2d_v0_t *)(pdat->virt + D1_G2D_V0);
	struct g2d_ui_t * g2d_ui0 = (struct g2d_ui_t *)(pdat->virt + D1_G2D_UI0);
	struct g2d_ui_t * g2d_ui1 = (struct g2d_ui_t *)(pdat->virt + D1_G2D_UI1);
	struct g2d_ui_t * g2d_ui2 = (struct g2d_ui_t *)(pdat->virt + D1_G2D_UI2);
	void * addr = s->pixels + y * s->stride + (x << 2);
	u32_t hw = ((h - 1) << 16) | ((w - 1) << 0);
	int ret = 0;

	mutex_lock(&pdat->m);
	{
		g2d_vsu->VS_CTRL = 0x0;
		g2d_rot->ROT_CTL = 0x0;
		g2d_v0->V0_ATTR = 0x0;
		g2d_ui0->UI_ATTR = 0x0;
		g2d_ui1->UI_ATTR = 0x0;
		g2d_ui2->UI_ATTR = 0x0;

		g2d_bld->BLD_CH_ISIZE[0] = hw;
		g2d_bld->BLD_CH_OFFSET[0] = 0x0;
		g2d_bld->BLD_SIZE = hw;
		g2d_bld->BLD_FILL_COLOR[0] = color_get_premult(c);
		g2d_bld->BLD_EN_CTL = (0x1 << 8) | (0x1 << 0);
		g2d_bld->BLD_PREMUL_CTL = (0x1 << 0);
		g2d_bld->BLD_OUT_COLOR = (0x1 << 0);
		g2d_bld->ROP_CTL = 0x0;
		g2d_bld->BLD_CTL = 0x03010301;

		g2d_wb->WB_ATTR = 0x0;
		g2d_wb->WB_SIZE = hw;
		g2d_wb->WB_PITCH0 = s->stride;
		g2d_wb->WB_LADD0 = lo32(addr);
		g2d_wb->WB_HADD0 = hi32(addr);
		g2d_wb->WB_PITCH1 = 0;
		g2d_wb->WB_LADD1 = 0;
		g2d_wb->WB_HADD1 = 0;
		g2d_wb->WB_PITCH2 = 0;
		g2d_wb->WB_LADD2 = 0;
		g2d_wb->WB_HADD2 = 0;

		dma_cache_sync(s->pixels, s->pixlen, DMA_FROM_DEVICE);
		d1_g2d_mixer_start(pdat);
		ret = d1_g2d_wait(pdat, 100);
	}
	mutex_unlock(&pdat->m);

	return ret;
}

static bool_t g2d_d1_blit(struct g2d_t * g2d, struct surface_t * s, struct region_t * clip, struct matrix_t * m, struct surface_t * o)
{
	struct g2d_d1_pdata_t * pdat = (struct g2d_d1_pdata_t *)g2d->priv;
	struct region_t r, region;

	region_init(&r, 0, 0, surface_get_width(s), surface_get_height(s));
	if(clip)
	{
		if(!region_intersect(&r, &r, clip))
			return TRUE;
	}
	matrix_transform_region(m, surface_get_width(o), surface_get_height(o), &region);
	if(!region_intersect(&r, &r, &region))
		return TRUE;

	if((r.w >= 2) && (r.h >= 2))
	{
		if(dcmp(m->b, 0) && dcmp(m->c, 0))
		{
			if(dcmp(m->a - m->d, 0))
			{
				if(dcmp(m->a, 1))		/* rotate 0 */
				{
					return d1_g2d_blit(pdat, s, o, r.x, r.y, r.w, r.h);
				}
				else if(dcmp(m->a, -1))	/* rotate 180 */
				{
					return FALSE;
				}
			}
			else if(dcmp(m->a + m->d, 0))
			{
				if(dcmp(m->a, 1))		/* rotate 0, v-flip | rotate 180, h-flip */
				{
					return FALSE;
				}
				else if(dcmp(m->a, -1))	/* rotate 0, h-flip | rotate 180, v-flip */
				{
					return FALSE;
				}
			}
		}
		else if(dcmp(m->a, 0) && dcmp(m->d, 0))
		{
			if(dcmp(m->b + m->c, 0))
			{
				if(dcmp(m->b, 1))		/* rotate 90 */
				{
					return FALSE;
				}
				else if(dcmp(m->b, -1))	/* rotate 270 */
				{
					return FALSE;
				}
			}
			else if(dcmp(m->b - m->c, 0))
			{
				if(dcmp(m->b, 1))		/* rotate 90, v-flip | rotate 270, h-flip */
				{
					return FALSE;
				}
				else if(dcmp(m->b, -1))	/* rotate 90, h-flip | rotate 270, v-flip */
				{
					return FALSE;
				}
			}
		}
	}
	return FALSE;
}

static bool_t g2d_d1_fill(struct g2d_t * g2d, struct surface_t * s, struct region_t * clip, struct matrix_t * m, int w, int h, struct color_t * c)
{
	struct g2d_d1_pdata_t * pdat = (struct g2d_d1_pdata_t *)g2d->priv;
	struct region_t r, region;

	region_init(&r, 0, 0, surface_get_width(s), surface_get_height(s));
	if(clip)
	{
		if(!region_intersect(&r, &r, clip))
			return TRUE;
	}
	matrix_transform_region(m, w, h, &region);
	if(!region_intersect(&r, &r, &region))
		return TRUE;

	if((r.w >= 2) && (r.h >= 2) && (c->a == 255))
	{
		if(dcmp(m->b, 0) && dcmp(m->c, 0))
		{
			if(dcmp(m->a - m->d, 0))
			{
				if(dcmp(m->a, 1))		/* rotate 0 */
				{
					return d1_g2d_fill(pdat, s, r.x, r.y, r.w, r.h, c);
				}
				else if(dcmp(m->a, -1))	/* rotate 180 */
				{
					return d1_g2d_fill(pdat, s, r.x, r.y, r.w, r.h, c);
				}
			}
			else if(dcmp(m->a + m->d, 0))
			{
				if(dcmp(m->a, 1))		/* rotate 0, v-flip | rotate 180, h-flip */
				{
					return d1_g2d_fill(pdat, s, r.x, r.y, r.w, r.h, c);
				}
				else if(dcmp(m->a, -1))	/* rotate 0, h-flip | rotate 180, v-flip */
				{
					return d1_g2d_fill(pdat, s, r.x, r.y, r.w, r.h, c);
				}
			}
		}
		else if(dcmp(m->a, 0) && dcmp(m->d, 0))
		{
			if(dcmp(m->b + m->c, 0))
			{
				if(dcmp(m->b, 1))		/* rotate 90 */
				{
					return d1_g2d_fill(pdat, s, r.x, r.y, r.w, r.h, c);
				}
				else if(dcmp(m->b, -1))	/* rotate 270 */
				{
					return d1_g2d_fill(pdat, s, r.x, r.y, r.w, r.h, c);
				}
			}
			else if(dcmp(m->b - m->c, 0))
			{
				if(dcmp(m->b, 1))		/* rotate 90, v-flip | rotate 270, h-flip */
				{
					return d1_g2d_fill(pdat, s, r.x, r.y, r.w, r.h, c);
				}
				else if(dcmp(m->b, -1))	/* rotate 90, h-flip | rotate 270, v-flip */
				{
					return d1_g2d_fill(pdat, s, r.x, r.y, r.w, r.h, c);
				}
			}
		}
	}
	return FALSE;
}

static struct device_t * g2d_d1_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct g2d_d1_pdata_t * pdat;
	struct g2d_t * g2d;
	struct device_t * dev;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	char * clk = dt_read_string(n, "clock-name", NULL);

	if(!search_clk(clk))
		return NULL;

	pdat = malloc(sizeof(struct g2d_d1_pdata_t));
	if(!pdat)
		return NULL;

	g2d = malloc(sizeof(struct g2d_t));
	if(!g2d)
	{
		free(pdat);
		return NULL;
	}

	pdat->virt = virt;
	pdat->clk = strdup(clk);
	pdat->reset = dt_read_int(n, "reset", -1);
	mutex_init(&pdat->m);

	g2d->name = alloc_device_name(dt_read_name(n), -1);
	g2d->blit = g2d_d1_blit;
	g2d->fill = g2d_d1_fill;
	g2d->priv = pdat;

	clk_enable(pdat->clk);
	if(pdat->reset >= 0)
		reset_deassert(pdat->reset);
	d1_g2d_init(pdat);

	if(!(dev = register_g2d(g2d, drv)))
	{
		clk_disable(pdat->clk);
		free(pdat->clk);
		free_device_name(g2d->name);
		free(g2d->priv);
		free(g2d);
		return NULL;
	}
	return dev;
}

static void g2d_d1_remove(struct device_t * dev)
{
	struct g2d_t * g2d = (struct g2d_t *)dev->priv;
	struct g2d_d1_pdata_t * pdat = (struct g2d_d1_pdata_t *)g2d->priv;

	if(g2d)
	{
		unregister_g2d(g2d);
		clk_disable(pdat->clk);
		free(pdat->clk);
		free_device_name(g2d->name);
		free(g2d->priv);
		free(g2d);
	}
}

static void g2d_d1_suspend(struct device_t * dev)
{
}

static void g2d_d1_resume(struct device_t * dev)
{
}

static struct driver_t g2d_d1 = {
	.name		= "g2d-d1",
	.probe		= g2d_d1_probe,
	.remove		= g2d_d1_remove,
	.suspend	= g2d_d1_suspend,
	.resume		= g2d_d1_resume,
};

static __init void g2d_d1_driver_init(void)
{
	register_driver(&g2d_d1);
}

static __exit void g2d_d1_driver_exit(void)
{
	unregister_driver(&g2d_d1);
}

driver_initcall(g2d_d1_driver_init);
driver_exitcall(g2d_d1_driver_exit);
