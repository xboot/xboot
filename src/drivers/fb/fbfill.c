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

#include <xboot.h>
#include <types.h>
#include <string.h>
#include <malloc.h>
#include <byteorder.h>
#include <fb/fb.h>
#include <fb/bitmap.h>
#include <fb/fbpixel.h>
#include <fb/fbfill.h>

/*
 * generic filler that works for every supported mode
 */
static void bitmap_fill_rect_generic(struct bitmap * bitmap, u32_t c, u32_t x, u32_t y, u32_t w, u32_t h)
{
	u32_t i, j;

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
static void bitmap_fill_rect_direct32(struct bitmap * bitmap, u32_t c, u32_t x, u32_t y, u32_t w, u32_t h)
{
	u8_t * p, * q;
	u32_t * t;
	u32_t len, skip;
	u32_t i;

	c = cpu_to_le32(c);
	len = bitmap->info.bytes_per_pixel * w;
	skip = bitmap->info.pitch;
	t = (u32_t *)bitmap_get_pointer(bitmap, x, y);
	p = q = (u8_t *)t;

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
static void bitmap_fill_rect_direct24(struct bitmap * bitmap, u32_t c, u32_t x, u32_t y, u32_t w, u32_t h)
{
	u8_t * p, * q;
	u8_t * t;
	u32_t len, skip;
	u32_t i;
	u8_t fill0 = (u8_t)((c >> 0) & 0xff);
	u8_t fill1 = (u8_t)((c >> 8) & 0xff);
	u8_t fill2 = (u8_t)((c >> 16) & 0xff);

	len = bitmap->info.bytes_per_pixel * w;
	skip = bitmap->info.pitch;
	t = (u8_t *)bitmap_get_pointer(bitmap, x, y);
	p = q = (u8_t *)t;

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
static void bitmap_fill_rect_direct16(struct bitmap * bitmap, u32_t c, u32_t x, u32_t y, u32_t w, u32_t h)
{
	u8_t * p, * q;
	u8_t * t;
	u32_t len, skip;
	u32_t i;
	u8_t fill0 = (u8_t)((c >> 0) & 0xff);
	u8_t fill1 = (u8_t)((c >> 8) & 0xff);

	len = bitmap->info.bytes_per_pixel * w;
	skip = bitmap->info.pitch;
	t = (u8_t *)bitmap_get_pointer(bitmap, x, y);
	p = q = (u8_t *)t;

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
static void bitmap_fill_rect_direct8(struct bitmap * bitmap, u32_t c, u32_t x, u32_t y, u32_t w, u32_t h)
{
	u8_t * p, * q;
	u8_t * t;
	u32_t len, skip;
	u32_t i;
	u8_t fill = (u8_t)(c & 0xff);

	len = bitmap->info.bytes_per_pixel * w;
	skip = bitmap->info.pitch;
	t = (u8_t *)bitmap_get_pointer(bitmap, x, y);
	p = q = (u8_t *)t;

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
void common_bitmap_fill_rect(struct bitmap * bitmap, u32_t c, u32_t x, u32_t y, u32_t w, u32_t h)
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
