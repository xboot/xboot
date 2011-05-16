/*
 * drivers/fb/fbcolor.c
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
#include <byteorder.h>
#include <fb/fb.h>
#include <fb/bitmap.h>
#include <fb/graphic.h>
#include <fb/fbcolor.h>

/*
 * map bitmap color
 */
u32_t bitmap_map_color(struct bitmap * bitmap, u8_t r, u8_t g, u8_t b, u8_t a)
{
	struct bitmap_info * info = &(bitmap->info);
	u32_t value;

	if(info->fmt == BITMAP_FORMAT_MONOCHROME)
	{
		if( (r == info->fg_r) && (g == info->fg_g) && (b == info->fg_b) && (a == info->fg_a) )
			return 1;
		else
			return 0;
	}
	else
	{
		r >>= 8 - info->red_mask_size;
		g >>= 8 - info->green_mask_size;
		b >>= 8 - info->blue_mask_size;
		a >>= 8 - info->alpha_mask_size;

		value = r << info->red_field_pos;
		value |= g << info->green_field_pos;
		value |= b << info->blue_field_pos;
		value |= a << info->alpha_field_pos;

		return value;
	}
}

/*
 * unmap bitmap color
 */
void bitmap_unmap_color(struct bitmap * bitmap, u32_t c, u8_t * r, u8_t * g, u8_t * b, u8_t * a)
{
	struct bitmap_info * info = &(bitmap->info);
	u32_t tmp;

	if(info->fmt == BITMAP_FORMAT_MONOCHROME)
	{
		if(c & 0x1)
		{
			*r = info->fg_r;
			*g = info->fg_g;
			*b = info->fg_b;
			*a = info->fg_a;
		}
		else
		{
			*r = info->bg_r;
			*g = info->bg_g;
			*b = info->bg_b;
			*a = info->bg_a;
		}
	}
	else
	{
		/* get red component */
		tmp = c >> info->red_field_pos;
		tmp &= (1 << info->red_mask_size) - 1;
		tmp <<= 8 - info->red_mask_size;
		tmp |= (1 << (8 - info->red_mask_size)) - 1;
		*r = tmp & 0xff;

		/* get green component */
		tmp = c >> info->green_field_pos;
		tmp &= (1 << info->green_mask_size) - 1;
		tmp <<= 8 - info->green_mask_size;
		tmp |= (1 << (8 - info->green_mask_size)) - 1;
		*g = tmp & 0xff;

		/* get blue component */
		tmp = c >> info->blue_field_pos;
		tmp &= (1 << info->blue_mask_size) - 1;
		tmp <<= 8 - info->blue_mask_size;
		tmp |= (1 << (8 - info->blue_mask_size)) - 1;
		*b = tmp & 0xff;

		/* get alpha component */
		if(info->alpha_mask_size > 0)
		{
			tmp = c >> info->alpha_field_pos;
			tmp &= (1 << info->alpha_mask_size) - 1;
			tmp <<= 8 - info->alpha_mask_size;
			tmp |= (1 << (8 - info->alpha_mask_size)) - 1;
		}
		else
		{
			tmp = 255;
		}
		*a = tmp & 0xff;
	}
}
