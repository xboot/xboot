/*
 * kernel/graphic/effect.c
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
#include <graphic/surface.h>

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

void surface_effect_glass(struct surface_t * s, struct region_t * clip, int x, int y, int w, int h, int radius)
{
	struct region_t region, r;

	if(radius > 0)
	{
		region_init(&r, 0, 0, surface_get_width(s), surface_get_height(s));
		if(clip)
		{
			if(!region_intersect(&r, &r, clip))
				return;
		}
		region_init(&region, x, y, w, h);
		if(!region_intersect(&r, &r, &region))
			return;
		expblur(surface_get_pixels(s), surface_get_width(s), surface_get_height(s), r.x, r.y, r.w, r.h, radius);
	}
}

void surface_effect_shadow(struct surface_t * s, struct region_t * clip, int x, int y, int w, int h, int radius, struct color_t * c)
{
	struct region_t region, r;
	uint32_t * p, * q;
	int i, j;

	if(radius > 0)
	{
		region_init(&r, 0, 0, surface_get_width(s), surface_get_height(s));
		if(clip)
		{
			if(!region_intersect(&r, &r, clip))
				return;
		}
		int r2 = radius << 1;
		int r4 = radius << 2;
		region_init(&region, x - r2, y - r2, w + r4, h + r4);
		if(!region_intersect(&r, &r, &region))
			return;
		int tw = w + r4;
		int th = h + r4;
		int pixlen = th * (tw << 2);
		void * pixels = malloc(pixlen);
		if(pixels)
		{
			memset(pixels, 0, pixlen);
			int x1 = r2;
			int x2 = r2 + w;
			int y1 = r2;
			int y2 = r2 + h;
			uint32_t v = color_get_premult(c);
			q = (uint32_t *)pixels + y1 * tw + x1;
			for(j = y1; j < y2; j++, q += tw)
			{
				for(i = x1, p = q; i < x2; i++, p++)
					*p = v;
			}
			expblur(pixels, tw, th, 0, 0, tw, th, radius);
			p = (uint32_t *)s->pixels + r.y * s->width + r.x;
			q = (uint32_t *)pixels + (max(region.y, r.y) - region.y) * tw + (max(region.x, r.x) - region.x);
			for(j = 0; j < r.h; j++, p += s->width, q += tw)
			{
				for(i = 0; i < r.w; i++)
				{
					blend(p + i, q + i);
				}
			}
			free(pixels);
		}
	}
}

void surface_effect_gradient(struct surface_t * s, struct region_t * clip, int x, int y, int w, int h, struct color_t * lt, struct color_t * rt, struct color_t * rb, struct color_t * lb)
{
	struct region_t region, r;
	struct color_t cl, cr;
	unsigned char * p, * q;
	int stride;
	int x0, y0, x1, y1;
	int i, j, u, v, t;
	uint8_t da, dr, dg, db;
	uint8_t sa, sr, sg, sb;

	region_init(&r, 0, 0, surface_get_width(s), surface_get_height(s));
	if(clip)
	{
		if(!region_intersect(&r, &r, clip))
			return;
	}
	region_init(&region, x, y, w, h);
	if(!region_intersect(&r, &r, &region))
		return;
	stride = surface_get_stride(s);
	q = (unsigned char *)surface_get_pixels(s) + y * stride + (x << 2);
	x0 = r.x - x;
	y0 = r.y - y;
	x1 = x0 + r.w;
	y1 = y0 + r.h;
	for(j = 0; j < h; j++, q += stride)
	{
		if((j >= y0) && (j < y1))
		{
			if(h > 1)
				u = (j << 8) / (h - 1);
			else
				u = 0;
			v = 256 - u;
			cl.b = (lt->b * v + lb->b * u) >> 8;
			cl.g = (lt->g * v + lb->g * u) >> 8;
			cl.r = (lt->r * v + lb->r * u) >> 8;
			cl.a = (lt->a * v + lb->a * u) >> 8;
			cr.b = (rt->b * v + rb->b * u) >> 8;
			cr.g = (rt->g * v + rb->g * u) >> 8;
			cr.r = (rt->r * v + rb->r * u) >> 8;
			cr.a = (rt->a * v + rb->a * u) >> 8;
			for(i = 0, p = q; i < w; i++, p += 4)
			{
				if((i >= x0) && (i < x1))
				{
					if(w > 1)
						u = (i << 8) / (w - 1);
					else
						u = 0;
					v = 256 - u;
					sa = (cl.a * v + cr.a * u) >> 8;
					if(sa != 0)
					{
						if(sa == 255)
						{
							p[0] = (cl.b * v + cr.b * u) >> 8;
							p[1] = (cl.g * v + cr.g * u) >> 8;
							p[2] = (cl.r * v + cr.r * u) >> 8;
							p[3] = sa;
						}
						else
						{
							sr = idiv255(((cl.r * v + cr.r * u) >> 8) * sa);
							sg = idiv255(((cl.g * v + cr.g * u) >> 8) * sa);
							sb = idiv255(((cl.b * v + cr.b * u) >> 8) * sa);
							db = p[0];
							dg = p[1];
							dr = p[2];
							da = p[3];
							t = sa + (sa >> 8);
							p[3] = (((sa + da) << 8) - da * t) >> 8;
							p[2] = (((sr + dr) << 8) - dr * t) >> 8;
							p[1] = (((sg + dg) << 8) - dg * t) >> 8;
							p[0] = (((sb + db) << 8) - db * t) >> 8;
						}
					}
				}
			}
		}
	}
}

void surface_effect_checkerboard(struct surface_t * s, struct region_t * clip, int x, int y, int w, int h)
{
	struct region_t region, r;
	uint32_t * q, * p;
	int x1, y1, x2, y2;
	int i, j, l;

	region_init(&r, 0, 0, surface_get_width(s), surface_get_height(s));
	if(clip)
	{
		if(!region_intersect(&r, &r, clip))
			return;
	}
	region_init(&region, x, y, w, h);
	if(!region_intersect(&r, &r, &region))
		return;

	x1 = r.x;
	y1 = r.y;
	x2 = r.x + r.w;
	y2 = r.y + r.h;
	l = s->stride >> 2;
	q = (uint32_t *)s->pixels + y1 * l + x1;

	for(j = y1; j < y2; j++, q += l)
	{
		for(i = x1, p = q; i < x2; i++, p++)
		{
			if((i ^ j) & (1 << 3))
				*p = 0xffabb9bd;
			else
				*p = 0xff899598;
		}
	}
}
