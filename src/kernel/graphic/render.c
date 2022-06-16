/*
 * kernel/graphic/render.c
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

void render_default_blit(struct surface_t * s, struct region_t * clip, struct matrix_t * m, struct surface_t * src)
{
	struct region_t r, region;
	struct matrix_t t;
	uint32_t * p;
	uint32_t * dp = surface_get_pixels(s);
	uint32_t * sp = surface_get_pixels(src);
	int ds = surface_get_stride(s) >> 2;
	int ss = surface_get_stride(src) >> 2;
	int sw = surface_get_width(src);
	int sh = surface_get_height(src);
	int x1, y1, x2, y2, stride;
	int x, y, ox, oy;
	double fx, fy, ofx, ofy;

	region_init(&r, 0, 0, surface_get_width(s), surface_get_height(s));
	if(clip)
	{
		if(!region_intersect(&r, &r, clip))
			return;
	}
	matrix_transform_region(m, sw, sh, &region);
	if(!region_intersect(&r, &r, &region))
		return;

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
}

void render_default_fill(struct surface_t * s, struct region_t * clip, struct matrix_t * m, int w, int h, struct color_t * c)
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
			return;
	}
	matrix_transform_region(m, w, h, &region);
	if(!region_intersect(&r, &r, &region))
		return;

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
}
