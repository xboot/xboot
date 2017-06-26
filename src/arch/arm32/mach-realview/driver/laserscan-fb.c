/*
 * laserscan-fb.c
 *
 * Copyright(c) 2007-2017 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <xboot.h>
#include <fb/fb.h>
#include <laserscan/laserscan.h>

struct laserscan_fb_pdata_t {
	u8_t r, g, b, a;
	u32_t color;
	float x, y;
	float tx, ty;
	float a11, a12, a13;
	float a21, a22, a23;
	float a31, a32, a33;

	struct fb_t * fb;
	void * pixels;
	int width, height;
	int w2, h2;
	int bytes_per_pixel;
};

static inline void fb_projective_transformation(struct laserscan_fb_pdata_t * pdat, float x, float y, float * tx, float * ty)
{
	float w = pdat->a13 * x + pdat->a23 * y + pdat->a33;
	*tx = (pdat->a11 * x + pdat->a21 * y + pdat->a31) / w;
	*ty = (pdat->a12 * x + pdat->a22 * y + pdat->a32) / w;
}

static inline void fb_set_pixel(struct laserscan_fb_pdata_t * pdat, int x, int y)
{
	if((x >= 0) && (x < pdat->width) && (y >= 0) && (y < pdat->height))
		*((u32_t *)((u8_t *)pdat->pixels + (y * pdat->width + x) * pdat->bytes_per_pixel)) = pdat->color;
}

static inline void fb_line(struct laserscan_fb_pdata_t * pdat, int x1, int y1, int x2, int y2)
{
	int dx = x2 - x1;
	int dy = y2 - y1;
	int e, s;
	int sx = dx > 0;
	int sy = dy > 0;

	if(!sx)
		dx = -dx;
	if(!sy)
		dy = -dy;
	s = dx > dy;

	fb_set_pixel(pdat, x1, y1);
	if(s)
	{
		e = (dy << 1) - dx;
		while(x1 != x2)
		{
			if(e < 0)
			{
				sx ? x1++ : x1--;
				e += dy << 1;
				fb_set_pixel(pdat, x1, y1);
			}
			else
			{
				sy ? y1++ : y1--;
				e -= dx << 1;
			}
		}
	}
	else
	{
		e = (dx << 1) - dy;
		while(y1 != y2)
		{
			if(e < 0)
			{
				sy ? y1++ : y1--;
				e += dx << 1;
				fb_set_pixel(pdat, x1, y1);
			}
			else
			{
				sx ? x1++ : x1--;
				e -= dy << 1;
			}
		}
	}
}

static void laserscan_fb_perspective(struct laserscan_t * l, float x, float y)
{
	struct laserscan_fb_pdata_t * pdat = (struct laserscan_fb_pdata_t *)l->priv;
	pdat->a13 = x;
	pdat->a23 = y;
	fb_projective_transformation(pdat, pdat->x, pdat->y, &pdat->tx, &pdat->ty);
}

static void laserscan_fb_translate(struct laserscan_t * l, float x, float y)
{
	struct laserscan_fb_pdata_t * pdat = (struct laserscan_fb_pdata_t *)l->priv;
	pdat->a31 = x;
	pdat->a32 = y;
	fb_projective_transformation(pdat, pdat->x, pdat->y, &pdat->tx, &pdat->ty);
}

static void laserscan_fb_scale(struct laserscan_t * l, float x, float y)
{
	struct laserscan_fb_pdata_t * pdat = (struct laserscan_fb_pdata_t *)l->priv;
	pdat->a11 = x;
	pdat->a22 = y;
	fb_projective_transformation(pdat, pdat->x, pdat->y, &pdat->tx, &pdat->ty);
}

static void laserscan_fb_shear(struct laserscan_t * l, float x, float y)
{
	struct laserscan_fb_pdata_t * pdat = (struct laserscan_fb_pdata_t *)l->priv;
	pdat->a21 = x;
	pdat->a12 = y;
	fb_projective_transformation(pdat, pdat->x, pdat->y, &pdat->tx, &pdat->ty);
}

static void laserscan_fb_set_color(struct laserscan_t * l, u8_t r, u8_t g, u8_t b, u8_t a)
{
	struct laserscan_fb_pdata_t * pdat = (struct laserscan_fb_pdata_t *)l->priv;

	pdat->r = r;
	pdat->g = g;
	pdat->b = b;
	pdat->a = a;
	if(a == 0)
		pdat->color = 0xff000000;
	else
		pdat->color = (0xff << 24) | (r << 16) | (g << 8) | (b << 0);
}

static void laserscan_fb_get_color(struct laserscan_t * l, u8_t * r, u8_t * g, u8_t * b, u8_t * a)
{
	struct laserscan_fb_pdata_t * pdat = (struct laserscan_fb_pdata_t *)l->priv;

	if(r)
		*r = pdat->r;
	if(g)
		*g = pdat->g;
	if(b)
		*b = pdat->b;
	if(a)
		*a = pdat->a;
}

static void laserscan_fb_move_to(struct laserscan_t * l, float x, float y)
{
	struct laserscan_fb_pdata_t * pdat = (struct laserscan_fb_pdata_t *)l->priv;
	float tx, ty;
	int x0, y0;

	fb_projective_transformation(pdat, x, y, &tx, &ty);
	x0 = tx * pdat->w2 + pdat->w2;
	y0 = -ty * pdat->h2 + pdat->h2;
	fb_set_pixel(pdat, x0, y0);
	pdat->x = x;
	pdat->y = y;
	pdat->tx = tx;
	pdat->ty = ty;
}

static void laserscan_fb_rel_move_to(struct laserscan_t * l, float dx, float dy)
{
	struct laserscan_fb_pdata_t * pdat = (struct laserscan_fb_pdata_t *)l->priv;
	laserscan_fb_move_to(l, pdat->x + dx, pdat->y + dy);
}

static void laserscan_fb_line_to(struct laserscan_t * l, float x, float y)
{
	struct laserscan_fb_pdata_t * pdat = (struct laserscan_fb_pdata_t *)l->priv;
	float tx, ty;
	int x1, y1;
	int x2, y2;

	fb_projective_transformation(pdat, x, y, &tx, &ty);
	x1 = pdat->tx * pdat->w2 + pdat->w2;
	y1 = -pdat->ty * pdat->h2 + pdat->h2;
	x2 = tx * pdat->w2 + pdat->w2;
	y2 = -ty * pdat->h2 + pdat->h2;
	fb_line(pdat, x1, y1, x2, y2);
	pdat->x = x;
	pdat->y = y;
	pdat->tx = tx;
	pdat->ty = ty;
}

static void laserscan_fb_rel_line_to(struct laserscan_t * l, float dx, float dy)
{
	struct laserscan_fb_pdata_t * pdat = (struct laserscan_fb_pdata_t *)l->priv;
	laserscan_fb_line_to(l, pdat->x + dx, pdat->y + dy);
}

static void laserscan_fb_curve_to(struct laserscan_t * l, float x1, float y1, float x2, float y2, float x3, float y3)
{
}

static void laserscan_fb_rel_curve_to(struct laserscan_t * l, float dx1, float dy1, float dx2, float dy2, float dx3, float dy3)
{
}

static void laserscan_fb_arc(struct laserscan_t * l, float xc, float yc, float r, float a1, float a2)
{
}

static void laserscan_fb_arc_negative(struct laserscan_t * l, float xc, float yc, float r, float a1, float a2)
{
}

static void laserscan_fb_clear(struct laserscan_t * l)
{
	struct laserscan_fb_pdata_t * pdat = (struct laserscan_fb_pdata_t *)l->priv;
	mdelay(100);
	memset(pdat->pixels, 0, pdat->width * pdat->height * pdat->bytes_per_pixel);
}

static struct device_t * laserscan_fb_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct laserscan_fb_pdata_t * pdat;
	struct fb_t * fb;
	struct laserscan_t * l;
	struct device_t * dev;

	if(!(fb = search_fb(dt_read_string(n, "fb-name", NULL))))
		return NULL;

	pdat = malloc(sizeof(struct laserscan_fb_pdata_t));
	if(!pdat)
		return NULL;

	l = malloc(sizeof(struct laserscan_t));
	if(!l)
	{
		free(pdat);
		return NULL;
	}

	pdat->r = 0;
	pdat->g = 0;
	pdat->b = 0;
	pdat->a = 0;
	pdat->color = 0xff000000;
	pdat->x = 0;
	pdat->y = 0;
	pdat->tx = 0;
	pdat->ty = 0;
	pdat->a11 = 1; pdat->a12 = 0; pdat->a13 = 0;
	pdat->a21 = 0; pdat->a22 = 1; pdat->a23 = 0;
	pdat->a31 = 0; pdat->a32 = 0; pdat->a33 = 1;
	pdat->fb = fb;
	pdat->pixels = fb->alone->pixels;
	pdat->width = fb->width;
	pdat->height = fb->height;
	pdat->w2 = fb->width / 2;
	pdat->h2 = fb->height / 2;
	pdat->bytes_per_pixel = fb->bpp / 8;

	l->name = alloc_device_name(dt_read_name(n), dt_read_id(n));
	l->perspective = laserscan_fb_perspective;
	l->translate = laserscan_fb_translate;
	l->scale = laserscan_fb_scale;
	l->shear = laserscan_fb_shear;
	l->set_color = laserscan_fb_set_color;
	l->get_color = laserscan_fb_get_color;
	l->move_to = laserscan_fb_move_to;
	l->rel_move_to = laserscan_fb_rel_move_to;
	l->line_to = laserscan_fb_line_to;
	l->rel_line_to = laserscan_fb_rel_line_to;
	l->curve_to = laserscan_fb_curve_to;
	l->rel_curve_to = laserscan_fb_rel_curve_to;
	l->arc = laserscan_fb_arc;
	l->arc_negative = laserscan_fb_arc_negative;
	l->clear = laserscan_fb_clear,
	l->priv = pdat;

	if(!register_laserscan(&dev, l))
	{
		free_device_name(l->name);
		free(l->priv);
		free(l);
		return NULL;
	}
	dev->driver = drv;

	return dev;
}

static void laserscan_fb_remove(struct device_t * dev)
{
	struct laserscan_t * l = (struct laserscan_t *)dev->priv;

	if(l && unregister_laserscan(l))
	{
		free_device_name(l->name);
		free(l->priv);
		free(l);
	}
}

static void laserscan_fb_suspend(struct device_t * dev)
{
}

static void laserscan_fb_resume(struct device_t * dev)
{
}

static struct driver_t laserscan_fb = {
	.name		= "laserscan-fb",
	.probe		= laserscan_fb_probe,
	.remove		= laserscan_fb_remove,
	.suspend	= laserscan_fb_suspend,
	.resume		= laserscan_fb_resume,
};

static __init void laserscan_fb_driver_init(void)
{
	register_driver(&laserscan_fb);
}

static __exit void laserscan_fb_driver_exit(void)
{
	unregister_driver(&laserscan_fb);
}

driver_initcall(laserscan_fb_driver_init);
driver_exitcall(laserscan_fb_driver_exit);
