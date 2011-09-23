/*
 * drivers/fb/fbsoft.c
 *
 * Copyright (c) 2007-2010  jianjun jiang <jerryjianjun@gmail.com>
 * official site: http://xboot.org
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
#include <types.h>
#include <malloc.h>
#include <rect.h>
#include <fb/fb.h>
#include <fb/bitmap.h>
#include <fb/fbcolor.h>
#include <fb/fbfill.h>
#include <fb/fbblit.h>
#include <fb/graphic.h>
#include <fb/fbsoft.h>

/*
 * default soft map color function
 */
u32_t fb_soft_map_color(struct fb * fb, u8_t r, u8_t g, u8_t b, u8_t a)
{
	return bitmap_map_color(&(fb->info->bitmap), r, g, b, a);
}

/*
 * default soft unmap color function
 */
void fb_soft_unmap_color(struct fb * fb, u32_t c, u8_t * r, u8_t * g, u8_t * b, u8_t * a)
{
	bitmap_unmap_color(&(fb->info->bitmap), c, r, g, b, a);
}

/*
 * default soft fill rect function
 */
bool_t fb_soft_fill_rect(struct fb * fb, u32_t c, u32_t x, u32_t y, u32_t w, u32_t h)
{
	struct bitmap * p = &(fb->info->bitmap);
	struct rect r, a, b;

	a.left = x;
	a.top = y;
	a.right = x + w;
	a.bottom = y + h;

	b.left = p->viewport.left;
	b.top = p->viewport.top;
	b.right = p->viewport.right;
	b.bottom = p->viewport.bottom;

	if(rect_intersect_old(&r, &a, & b) == FALSE)
		return FALSE;

	x = r.left;
	y = r.top;
	w = r.right - r.left;
	h = r.bottom - r.top;

	common_bitmap_fill_rect(p, c, x, y, w, h);

	return TRUE;
}

/*
 * default soft blit bitmap function
 */
bool_t fb_soft_blit_bitmap(struct fb * fb, struct bitmap * bitmap, enum blit_mode mode, u32_t x, u32_t y, u32_t w, u32_t h, u32_t ox, u32_t oy)
{
	struct bitmap * p = &(fb->info->bitmap);
	struct rect r, a, b;

	a.left = x;
	a.top = y;
	a.right = x + w;
	a.bottom = y + h;

	b.left = p->viewport.left;
	b.top = p->viewport.top;
	b.right = p->viewport.right;
	b.bottom = p->viewport.bottom;

	if(rect_intersect_old(&r, &a, & b) == FALSE)
		return FALSE;

	x = r.left;
	y = r.top;
	w = r.right - r.left;
	h = r.bottom - r.top;

	common_bitmap_blit(p, bitmap, mode, x, y, w, h, ox, oy);

	return TRUE;
}
