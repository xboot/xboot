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
#include <fb/graphic.h>

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

/*
 * map bitmap color
 */
x_u32 map_bitmap_color(struct bitmap * bitmap, x_u8 r, x_u8 g, x_u8 b, x_u8 a)
{
	struct bitmap_info * info = &(bitmap->info);
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
 * unmap bitmap color
 */
void unmap_bitmap_color(struct bitmap * bitmap, x_u32 c, x_u8 * r, x_u8 * g, x_u8 * b, x_u8 * a)
{
	struct bitmap_info * info = &(bitmap->info);
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
}

/*
 * get bitmap's pointer at coordinate of x, y
 */
x_u8 * get_bitmap_pointer(struct bitmap * bitmap, x_u32 x, x_u32 y)
{
	x_u8 * p;

	switch(bitmap->info.bpp)
	{
	case 32:
		p = bitmap->data + y * bitmap->info.pitch + x * 4;
		break;

	case 24:
		p = bitmap->data + y * bitmap->info.pitch + x * 3;
		break;

	case 16:
	case 15:
		p = bitmap->data + y * bitmap->info.pitch + x * 2;
		break;

	case 8:
		p = bitmap->data + y * bitmap->info.pitch + x;
		break;

	default:
		return 0;
	}

	return p;
}

/*
 * get bitmap's pixel at coordinate of x, y
 */
x_u32 get_bitmap_pixel(struct bitmap * bitmap, x_u32 x, x_u32 y)
{
	x_u32 c;
	x_u8 * p;

	switch(bitmap->info.bpp)
	{
	case 32:
		p = bitmap->data + y * bitmap->info.pitch + x * 4;
		c = cpu_to_le32( *((x_u32 *)p) );
		break;

	case 24:
		p = bitmap->data + y * bitmap->info.pitch + x * 3;
		c = p[0] | (p[1] << 8) | (p[2] << 16);
		break;

	case 16:
	case 15:
		p = bitmap->data + y * bitmap->info.pitch + x * 2;
		c = cpu_to_le16( *((x_u16 *)p) );
		break;

	case 8:
		p = bitmap->data + y * bitmap->info.pitch + x;
		c = *((x_u8 *)p);
		break;

	default:
		return 0;
	}

	return c;
}

/*
 * set bitmap's pixel at coordinate of x, y
 */
void set_bitmap_pixel(struct bitmap * bitmap, x_u32 x, x_u32 y, x_u32 c)
{
	x_u8 * p;

	switch(bitmap->info.bpp)
	{
	case 32:
		p = bitmap->data + y * bitmap->info.pitch + x * 4;
		*((x_u32 *)p) = cpu_to_le32(c);
		break;

	case 24:
		p = bitmap->data + y * bitmap->info.pitch + x * 3;
		p[0] = c & 0xff;
		p[1] = (c >> 8) & 0xff;
		p[2] = (c >> 16) & 0xff;
		break;

	case 16:
	case 15:
		p = bitmap->data + y * bitmap->info.pitch + x * 2;
		*((x_u16 *)p) = cpu_to_le16(c & 0xffff);
		break;

	case 8:
		p = bitmap->data + y * bitmap->info.pitch + x;
		*((x_u8 *)p) = c & 0xff;
		break;

	default:
		break;
	}
}

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
	switch(bitmap->info.bpp)
	{
	case 32:
		bitmap_fill_rect_direct32(bitmap, c, x, y, w, h);
		break;

	case 24:
		bitmap_fill_rect_direct24(bitmap, c, x, y, w, h);
		break;

	case 16:
	case 15:
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
