/*
 * drivers/fb/fbblit.c
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
#include <fb/fb.h>
#include <fb/bitmap.h>
#include <fb/fbcolor.h>
#include <fb/fbpixel.h>
#include <fb/fbblit.h>

/*
 * generic replacing blitter that works for every supported mode
 */
static void bitmap_blit_replace_generic(struct bitmap * dst, struct bitmap * src, x_u32 x, x_u32 y, x_u32 w, x_u32 h, x_u32 ox, x_u32 oy)
{
	x_u8 r, g, b, a;
	x_u32 sc, dc;
	x_u32 i, j;

	for(j = 0; j < h; j++)
	{
		for(i = 0; i < w; i++)
		{
			sc = bitmap_get_pixel(src, i + ox, j + oy);
			bitmap_unmap_color(src, sc, &r, &g, &b, &a);
			dc = bitmap_map_color(dst, r, g, b, a);
			bitmap_set_pixel(dst, x + i, y + j, dc);
		}
	}
}

/*
 * generic blending blitter that works for every supported mode
 */
static void bitmap_blit_blend_generic(struct bitmap * dst, struct bitmap * src, x_u32 x, x_u32 y, x_u32 w, x_u32 h, x_u32 ox, x_u32 oy)
{
	x_u8 sr, sg, sb, sa;
	x_u8 dr, dg, db, da;
	x_u32 sc, dc;
	x_u32 i, j;

	for(j = 0; j < h; j++)
	{
		for(i = 0; i < w; i++)
		{
			sc = bitmap_get_pixel(src, i + ox, j + oy);
			bitmap_unmap_color(src, sc, &sr, &sg, &sb, &sa);

			if(sa == 0)
				continue;

			if(sa == 255)
			{
				dc = bitmap_map_color(dst, sr, sg, sb, sa);
				bitmap_set_pixel(dst, x + i, y + j, dc);
				continue;
			}

			dc = bitmap_get_pixel(dst, x + i, y + j);
			bitmap_unmap_color(dst, dc, &dr, &dg, &db, &da);

			dr = (((sr * sa) + (dr * (255 - sa))) / 255);
			dg = (((sg * sa) + (dg * (255 - sa))) / 255);
			db = (((sb * sa) + (db * (255 - sa))) / 255);
			da = sa;

			dc = bitmap_map_color(dst, dr, dg, db, da);
			bitmap_set_pixel(dst, x + i, y + j, dc);
		}
	}
}

/*
 * common bitmap blitter
 */
void common_bitmap_blit(struct bitmap * dst, struct bitmap * src, enum blit_mode mode, x_u32 x, x_u32 y, x_u32 w, x_u32 h, x_u32 ox, x_u32 oy)
{
	if(mode == BLIT_MODE_REPLACE)
	{
		bitmap_blit_replace_generic(dst, src, x, y, w, h, ox, oy);
	}

	else if(mode == BLIT_MODE_BLEND)
	{
		bitmap_blit_blend_generic(dst, src, x, y, w, h, ox, oy);
	}
}
