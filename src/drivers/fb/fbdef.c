/*
 * drivers/fb/fbdefault.c
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
#include <fb/bitmap.h>
#include <fb/fb.h>
#include <fb/fbdef.h>

/*
 * default map color function
 */
x_u32 fb_default_map_color(struct fb * fb, x_u8 r, x_u8 g, x_u8 b, x_u8 a)
{
	struct bitmap_info * info = &(fb->info->bitmap.info);
	x_u32 value;

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

/*
 * default unmap color function
 */
x_bool fb_default_unmap_color(struct fb * fb, x_u32 c, x_u8 * r, x_u8 * g, x_u8 * b, x_u8 * a)
{
	struct bitmap_info * info = &(fb->info->bitmap.info);
	x_u32 tmp;

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

	return TRUE;
}
