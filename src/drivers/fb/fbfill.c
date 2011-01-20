/*
 * drivers/fb/fbfill.c
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

#include <configs.h>
#include <default.h>
#include <types.h>
#include <xboot.h>
#include <malloc.h>
#include <byteorder.h>
#include <fb/fb.h>
#include <fb/bitmap.h>
#include <fb/fbpixel.h>
#include <fb/fbfill.h>

/*
 * generic filler that works for every supported mode
 */
static void bitmap_fill_rect_generic(struct bitmap * bitmap, x_u32 c, x_u32 x, x_u32 y, x_u32 w, x_u32 h)
{
	x_u32 i, j;

	for(j = 0; j < h; j++)
	{
		for(i = 0; i < w; i++)
		{
			bitmap_set_pixel(bitmap, x + i, y + j, c);
		}
	}
}

/*
 * optimized filler for direct color 32 bit modes
 */
static void bitmap_fill_rect_direct32(struct bitmap * bitmap, x_u32 c, x_u32 x, x_u32 y, x_u32 w, x_u32 h)
{
	x_u8 * p, * q;
	x_u32 * t;
	x_u32 len, skip;
	x_u32 i;

	c = cpu_to_le32(c);
	len = bitmap->info.bytes_per_pixel * w;
	skip = bitmap->info.pitch - len + bitmap->info.pitch;
	t = (x_u32 *)bitmap_get_pointer(bitmap, x, y);
	p = q = (x_u8 *)t;

	for(i = 0; i < w; i++)
		*t++ = c;

	for(i = 1; i < h; i++)
	{
		q += skip;
		memcpy(q, p, len);
	}
}

/*
 * optimized filler for direct color 24 bit modes
 */
static void bitmap_fill_rect_direct24(struct bitmap * bitmap, x_u32 c, x_u32 x, x_u32 y, x_u32 w, x_u32 h)
{
	x_u8 * p, * q;
	x_u8 * t;
	x_u32 len, skip;
	x_u32 i;
	x_u8 fill0 = (x_u8)((c >> 0) & 0xff);
	x_u8 fill1 = (x_u8)((c >> 8) & 0xff);
	x_u8 fill2 = (x_u8)((c >> 16) & 0xff);

	len = bitmap->info.bytes_per_pixel * w;
	skip = bitmap->info.pitch - len + bitmap->info.pitch;
	t = (x_u8 *)bitmap_get_pointer(bitmap, x, y);
	p = q = (x_u8 *)t;

	for(i = 0; i < w; i++)
	{
		*t++ = fill0;
		*t++ = fill1;
		*t++ = fill2;
	}

	for(i = 1; i < h; i++)
	{
		q += skip;
		memcpy(q, p, len);
	}
}

/*
 * optimized filler for direct color 16 bit modes
 */
static void bitmap_fill_rect_direct16(struct bitmap * bitmap, x_u32 c, x_u32 x, x_u32 y, x_u32 w, x_u32 h)
{
	x_u8 * p, * q;
	x_u8 * t;
	x_u32 len, skip;
	x_u32 i;
	x_u8 fill0 = (x_u8)((c >> 0) & 0xff);
	x_u8 fill1 = (x_u8)((c >> 8) & 0xff);

	len = bitmap->info.bytes_per_pixel * w;
	skip = bitmap->info.pitch - len + bitmap->info.pitch;
	t = (x_u8 *)bitmap_get_pointer(bitmap, x, y);
	p = q = (x_u8 *)t;

	for(i = 0; i < w; i++)
	{
		*t++ = fill0;
		*t++ = fill1;
	}

	for(i = 1; i < h; i++)
	{
		q += skip;
		memcpy(q, p, len);
	}
}

/*
 * optimized filler for direct color 8 bit modes
 */
static void bitmap_fill_rect_direct8(struct bitmap * bitmap, x_u32 c, x_u32 x, x_u32 y, x_u32 w, x_u32 h)
{
	x_u8 * p, * q;
	x_u8 * t;
	x_u32 len, skip;
	x_u32 i;
	x_u8 fill = (x_u8)(c & 0xff);

	len = bitmap->info.bytes_per_pixel * w;
	skip = bitmap->info.pitch - len + bitmap->info.pitch;
	t = (x_u8 *)bitmap_get_pointer(bitmap, x, y);
	p = q = (x_u8 *)t;

	for(i = 0; i < w; i++)
	{
		*t++ = fill;
	}

	for(i = 1; i < h; i++)
	{
		q += skip;
		memcpy(q, p, len);
	}
}

/*
 * common file rect to bitmap
 */
void common_bitmap_fill_rect(struct bitmap * bitmap, x_u32 c, x_u32 x, x_u32 y, x_u32 w, x_u32 h)
{
	switch(bitmap->info.bpp)
	{
	case 32:
		bitmap_fill_rect_direct32(bitmap, c, x, y, w, h);
		break;

	case 24:
		bitmap_fill_rect_direct24(bitmap, c, x, y, w, h);
		break;

	case 16:
		bitmap_fill_rect_direct16(bitmap, c, x, y, w, h);
		break;

	case 8:
		bitmap_fill_rect_direct8(bitmap, c, x, y, w, h);
		break;

	default:
		bitmap_fill_rect_generic(bitmap, c, x, y, w, h);
		break;
	}
}
