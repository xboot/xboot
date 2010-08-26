/*
 * drivers/fb/graphic.c
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
#include <byteorder.h>
#include <fb/bitmap.h>
#include <fb/fb.h>
#include <fb/fbfill.h>
#include <fb/fbblit.h>
#include <fb/graphic.h>

#if 0
/*
 * save bitmap's viewport
 */
void save_bitmap_viewport(struct bitmap * bitmap, struct rect * rect)
{
	rect->x = bitmap->viewport.x;
	rect->y = bitmap->viewport.y;
	rect->w = bitmap->viewport.w;
	rect->h = bitmap->viewport.h;
}

/*
 * restore bitmap's viewport
 */
void restore_bitmap_viewport(struct bitmap * bitmap, struct rect * rect)
{
	bitmap->viewport.x = rect->x;
	bitmap->viewport.y = rect->y;
	bitmap->viewport.w = rect->w;
	bitmap->viewport.h = rect->h;
}
#endif

/*
 * fill rect to bitmap
 */
void bitmap_fill_rect(struct bitmap * bitmap, x_u32 c, x_u32 x, x_u32 y, x_u32 w, x_u32 h)
{
/*	if((x >= bitmap->viewport.w))
		return;

	if((y >= bitmap->viewport.h))
		return;

	if((x + w) > bitmap->viewport.w)
		w = bitmap->viewport.w - x;
	if((y + h) > bitmap->viewport.h)
		h = bitmap->viewport.h - y;
*/

	common_bitmap_fill_rect(bitmap, c, x, y, w, h);
}

/*
 * bitmap blitter
 */
void bitmap_blit(struct bitmap * dst, struct bitmap * src, enum blit_mode mode, x_u32 x, x_u32 y, x_u32 w, x_u32 h, x_u32 ox, x_u32 oy)
{
	/*	if((x >= bitmap->viewport.w))
			return;

		if((y >= bitmap->viewport.h))
			return;

		if((x + w) > bitmap->viewport.w)
			w = bitmap->viewport.w - x;
		if((y + h) > bitmap->viewport.h)
			h = bitmap->viewport.h - y;
	*/

	common_bitmap_blit(dst, src, mode, x, y, w, h, ox, oy);
}
