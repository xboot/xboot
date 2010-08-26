/*
 * drivers/fb/fbsoft.c
 *
 * Copyright (c) 2007-2010  jianjun jiang <jerryjianjun@gmail.com>
 * website: http://xboot.org
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

#include <configs.h>
#include <default.h>
#include <types.h>
#include <xboot.h>
#include <malloc.h>
#include <gui/rect.h>
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
x_u32 fb_soft_map_color(struct fb * fb, x_u8 r, x_u8 g, x_u8 b, x_u8 a)
{
	return bitmap_map_color(&(fb->info->bitmap), r, g, b, a);
}

/*
 * default soft unmap color function
 */
void fb_soft_unmap_color(struct fb * fb, x_u32 c, x_u8 * r, x_u8 * g, x_u8 * b, x_u8 * a)
{
	bitmap_unmap_color(&(fb->info->bitmap), c, r, g, b, a);
}

/*
 * default soft fill rect function
 */
void fb_soft_fill_rect(struct fb * fb, x_u32 c, x_u32 x, x_u32 y, x_u32 w, x_u32 h)
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

	if(rect_intersect(&r, &a, & b) == FALSE)
		return;

	x = r.left;
	y = r.top;
	w = r.right - r.left;
	h = r.bottom - r.top;

	common_bitmap_fill_rect(p, c, x, y, w, h);
}

/*
 * default soft blit bitmap function
 */
void fb_soft_blit_bitmap(struct fb * fb, struct bitmap * bitmap, enum blit_mode mode, x_u32 x, x_u32 y, x_u32 w, x_u32 h, x_u32 ox, x_u32 oy)
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

	if(rect_intersect(&r, &a, & b) == FALSE)
		return;

	x = r.left;
	y = r.top;
	w = r.right - r.left;
	h = r.bottom - r.top;

	common_bitmap_blit(p, bitmap, mode, x, y, w, h, ox, oy);
}
