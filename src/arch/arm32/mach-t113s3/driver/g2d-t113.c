/*
 * driver/g2d-t113.c
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
#include <g2d/g2d.h>

struct g2d_t113_pdata_t {
	virtual_addr_t virt;
	char * clk;
	int reset;
};

static void g2d_init(struct g2d_t * g2d)
{
	//g2d_bsp_open();
}

static inline void blend(uint32_t * d, uint32_t * s)
{
	uint32_t dv, sv = *s;
	uint8_t da, dr, dg, db;
	uint8_t sa, sr, sg, sb;
	uint8_t a, r, g, b;
	int t;

	sa = (sv >> 24) & 0xff;
	if(sa == 255)
	{
		*d = sv;
	}
	else if(sa != 0)
	{
		sr = (sv >> 16) & 0xff;
		sg = (sv >> 8) & 0xff;
		sb = (sv >> 0) & 0xff;
		dv = *d;
		da = (dv >> 24) & 0xff;
		dr = (dv >> 16) & 0xff;
		dg = (dv >> 8) & 0xff;
		db = (dv >> 0) & 0xff;
		t = sa + (sa >> 8);
		a = (((sa + da) << 8) - da * t) >> 8;
		r = (((sr + dr) << 8) - dr * t) >> 8;
		g = (((sg + dg) << 8) - dg * t) >> 8;
		b = (((sb + db) << 8) - db * t) >> 8;
		*d = (a << 24) | (r << 16) | (g << 8) | (b << 0);
	}
}

static inline bool_t sw_blit(struct surface_t * s, struct region_t * clip, struct matrix_t * m, struct surface_t * o)
{
	struct region_t r, region;
	struct matrix_t t;
	uint32_t * p;
	uint32_t * dp = surface_get_pixels(s);
	uint32_t * sp = surface_get_pixels(o);
	int ds = surface_get_stride(s) >> 2;
	int ss = surface_get_stride(o) >> 2;
	int sw = surface_get_width(o);
	int sh = surface_get_height(o);
	int x1, y1, x2, y2, stride;
	int x, y, ox, oy;
	double fx, fy, ofx, ofy;

	region_init(&r, 0, 0, surface_get_width(s), surface_get_height(s));
	if(clip)
	{
		if(!region_intersect(&r, &r, clip))
			return TRUE;
	}
	matrix_transform_region(m, sw, sh, &region);
	if(!region_intersect(&r, &r, &region))
		return TRUE;

	x1 = r.x;
	y1 = r.y;
	x2 = r.x + r.w;
	y2 = r.y + r.h;
	stride = ds - r.w;
	p = dp + y1 * ds + x1;
	fx = x1;
	fy = y1;
	memcpy(&t, m, sizeof(struct matrix_t));
	matrix_invert(&t);
	matrix_transform_point(&t, &fx, &fy);

	for(y = y1; y < y2; ++y, fx += t.c, fy += t.d)
	{
		ofx = fx;
		ofy = fy;
		for(x = x1; x < x2; ++x, ofx += t.a, ofy += t.b)
		{
			ox = (int)ofx;
			oy = (int)ofy;
			if(ox >= 0 && ox < sw && oy >= 0 && oy < sh)
			{
				blend(p, sp + oy * ss + ox);
			}
			p++;
		}
		p += stride;
	}
	return TRUE;
}

static inline bool_t sw_fill(struct surface_t * s, struct region_t * clip, struct matrix_t * m, int w, int h, struct color_t * c)
{
	struct region_t r, region;
	struct matrix_t t;
	uint32_t * p, v;
	int ds = surface_get_stride(s) >> 2;
	int x1, y1, x2, y2, stride;
	int x, y, ox, oy;
	double fx, fy, ofx, ofy;

	region_init(&r, 0, 0, surface_get_width(s), surface_get_height(s));
	if(clip)
	{
		if(!region_intersect(&r, &r, clip))
			return TRUE;
	}
	matrix_transform_region(m, w, h, &region);
	if(!region_intersect(&r, &r, &region))
		return TRUE;

	x1 = r.x;
	y1 = r.y;
	x2 = r.x + r.w;
	y2 = r.y + r.h;
	stride = ds - r.w;
	p = (uint32_t *)surface_get_pixels(s) + y1 * ds + x1;
	v = color_get_premult(c);
	fx = x1;
	fy = y1;
	memcpy(&t, m, sizeof(struct matrix_t));
	matrix_invert(&t);
	matrix_transform_point(&t, &fx, &fy);

	for(y = y1; y < y2; ++y, fx += t.c, fy += t.d)
	{
		ofx = fx;
		ofy = fy;
		for(x = x1; x < x2; ++x, ofx += t.a, ofy += t.b)
		{
			ox = (int)ofx;
			oy = (int)ofy;
			if(ox >= 0 && ox < w && oy >= 0 && oy < h)
				*p = v;
			p++;
		}
		p += stride;
	}
	return TRUE;
}

static bool_t g2d_t113_blit(struct g2d_t * g2d, struct surface_t * s, struct region_t * clip, struct matrix_t * m, struct surface_t * o)
{
	//struct g2d_t113_pdata_t * pdat = (struct g2d_t113_pdata_t *)g2d->priv;
	return sw_blit(s, clip, m, o);
}

static bool_t g2d_t113_fill(struct g2d_t * g2d, struct surface_t * s, struct region_t * clip, struct matrix_t * m, int w, int h, struct color_t * c)
{
	//struct g2d_t113_pdata_t * pdat = (struct g2d_t113_pdata_t *)g2d->priv;
	return sw_fill(s, clip, m, w, h, c);
}

static struct device_t * g2d_t113_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct g2d_t113_pdata_t * pdat;
	struct g2d_t * g2d;
	struct device_t * dev;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	char * clk = dt_read_string(n, "clock-name", NULL);

	if(!search_clk(clk))
		return NULL;

	pdat = malloc(sizeof(struct g2d_t113_pdata_t));
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

	g2d->name = alloc_device_name(dt_read_name(n), -1);
	g2d->blit = g2d_t113_blit;
	g2d->fill = g2d_t113_fill;
	g2d->priv = pdat;

	clk_enable(pdat->clk);
	if(pdat->reset >= 0)
		reset_deassert(pdat->reset);
	g2d_init(g2d);

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

static void g2d_t113_remove(struct device_t * dev)
{
	struct g2d_t * g2d = (struct g2d_t *)dev->priv;
	struct g2d_t113_pdata_t * pdat = (struct g2d_t113_pdata_t *)g2d->priv;

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

static void g2d_t113_suspend(struct device_t * dev)
{
}

static void g2d_t113_resume(struct device_t * dev)
{
}

static struct driver_t g2d_t113 = {
	.name		= "g2d-t113",
	.probe		= g2d_t113_probe,
	.remove		= g2d_t113_remove,
	.suspend	= g2d_t113_suspend,
	.resume		= g2d_t113_resume,
};

static __init void g2d_t113_driver_init(void)
{
	register_driver(&g2d_t113);
}

static __exit void g2d_t113_driver_exit(void)
{
	unregister_driver(&g2d_t113);
}

driver_initcall(g2d_t113_driver_init);
driver_exitcall(g2d_t113_driver_exit);
