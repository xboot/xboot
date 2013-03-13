/*
 * kernel/graphic/pixel.c
 *
 * Copyright(c) 2007-2013 jianjun jiang <jerryjianjun@gmail.com>
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

#include <graphic/pixel.h>

enum pixel_format get_pixel_format(struct pixel_info * info)
{
	if(info->bits_per_pixel == 32)
	{
		if((info->red_mask_size == 8)
			&& (info->green_mask_size == 8)
			&& (info->blue_mask_size == 8)
			&& (info->alpha_mask_size == 8)
			&& (info->red_field_pos == 0)
			&& (info->green_field_pos == 8)
			&& (info->blue_field_pos == 16)
			&& (info->alpha_field_pos == 24))
		{
			return PIXEL_FORMAT_ABGR_8888;
		}
		else if((info->red_mask_size == 16)
			&& (info->green_mask_size == 8)
			&& (info->blue_mask_size == 8)
			&& (info->alpha_mask_size == 8)
			&& (info->red_field_pos == 0)
			&& (info->green_field_pos == 8)
			&& (info->blue_field_pos == 0)
			&& (info->alpha_field_pos == 24))
		{
			return PIXEL_FORMAT_ARGB_8888;
		}
		else if((info->red_mask_size == 16)
			&& (info->green_mask_size == 8)
			&& (info->blue_mask_size == 8)
			&& (info->alpha_mask_size == 8)
			&& (info->red_field_pos == 8)
			&& (info->green_field_pos == 16)
			&& (info->blue_field_pos == 24)
			&& (info->alpha_field_pos == 0))
		{
			return PIXEL_FORMAT_BGRA_8888;
		}
		else if((info->red_mask_size == 16)
			&& (info->green_mask_size == 8)
			&& (info->blue_mask_size == 8)
			&& (info->alpha_mask_size == 8)
			&& (info->red_field_pos == 24)
			&& (info->green_field_pos == 16)
			&& (info->blue_field_pos == 8)
			&& (info->alpha_field_pos == 0))
		{
			return PIXEL_FORMAT_RGBA_8888;
		}
	}
	else if(info->bits_per_pixel == 24)
	{
		if((info->red_mask_size == 8)
			&& (info->green_mask_size == 8)
			&& (info->blue_mask_size == 8)
			&& (info->alpha_mask_size == 0)
			&& (info->red_field_pos == 0)
			&& (info->green_field_pos == 8)
			&& (info->blue_field_pos == 16)
			&& (info->alpha_field_pos == 0))
		{
			return PIXEL_FORMAT_BGR_888;
		}
		else if((info->red_mask_size == 16)
			&& (info->green_mask_size == 8)
			&& (info->blue_mask_size == 8)
			&& (info->alpha_mask_size == 0)
			&& (info->red_field_pos == 0)
			&& (info->green_field_pos == 8)
			&& (info->blue_field_pos == 0)
			&& (info->alpha_field_pos == 0))
		{
			return PIXEL_FORMAT_RGB_888;
		}
	}
	else if(info->bits_per_pixel == 16)
	{
		if((info->red_mask_size == 5)
			&& (info->green_mask_size == 6)
			&& (info->blue_mask_size == 5)
			&& (info->alpha_mask_size == 0)
			&& (info->red_field_pos == 0)
			&& (info->green_field_pos == 5)
			&& (info->blue_field_pos == 11)
			&& (info->alpha_field_pos == 0))
		{
			return PIXEL_FORMAT_BGR_565;
		}
		else if((info->red_mask_size == 5)
			&& (info->green_mask_size == 6)
			&& (info->blue_mask_size == 5)
			&& (info->alpha_mask_size == 0)
			&& (info->red_field_pos == 11)
			&& (info->green_field_pos == 5)
			&& (info->blue_field_pos == 0)
			&& (info->alpha_field_pos == 0))
		{
			return PIXEL_FORMAT_RGB_565;
		}
	}
	else if(info->bits_per_pixel == 8)
	{
		if((info->red_mask_size == 2)
			&& (info->green_mask_size == 3)
			&& (info->blue_mask_size == 3)
			&& (info->alpha_mask_size == 0)
			&& (info->red_field_pos == 0)
			&& (info->green_field_pos == 2)
			&& (info->blue_field_pos == 5)
			&& (info->alpha_field_pos == 0))
		{
			return PIXEL_FORMAT_BGR_332;
		}
		else if((info->red_mask_size == 3)
			&& (info->green_mask_size == 3)
			&& (info->blue_mask_size == 2)
			&& (info->alpha_mask_size == 0)
			&& (info->red_field_pos == 5)
			&& (info->green_field_pos == 2)
			&& (info->blue_field_pos == 0)
			&& (info->alpha_field_pos == 0))
		{
			return PIXEL_FORMAT_RGB_332;
		}
	}

	return PIXEL_FORMAT_GENERIC;
}

void set_pixel_info(struct pixel_info * info, enum pixel_format fmt)
{
	switch(fmt)
	{
	case PIXEL_FORMAT_GENERIC:
		break;

	case PIXEL_FORMAT_ABGR_8888:
		info->bits_per_pixel	= 32;
		info->bytes_per_pixel 	= 4;
        info->red_mask_size		= 8;
        info->green_mask_size	= 8;
        info->blue_mask_size	= 8;
        info->alpha_mask_size	= 8;
        info->red_field_pos		= 0;
        info->green_field_pos	= 8;
        info->blue_field_pos	= 16;
        info->alpha_field_pos	= 24;
		break;

	case PIXEL_FORMAT_ARGB_8888:
		info->bits_per_pixel	= 32;
		info->bytes_per_pixel 	= 4;
        info->red_mask_size		= 8;
        info->green_mask_size	= 8;
        info->blue_mask_size	= 8;
        info->alpha_mask_size	= 8;
        info->red_field_pos		= 16;
        info->green_field_pos	= 8;
        info->blue_field_pos	= 0;
        info->alpha_field_pos	= 24;
		break;

	case PIXEL_FORMAT_BGRA_8888:
		info->bits_per_pixel	= 32;
		info->bytes_per_pixel 	= 4;
        info->red_mask_size		= 8;
        info->green_mask_size	= 8;
        info->blue_mask_size	= 8;
        info->alpha_mask_size	= 8;
        info->red_field_pos		= 8;
        info->green_field_pos	= 16;
        info->blue_field_pos	= 24;
        info->alpha_field_pos	= 0;
		break;

	case PIXEL_FORMAT_RGBA_8888:
		info->bits_per_pixel	= 32;
		info->bytes_per_pixel 	= 4;
        info->red_mask_size		= 8;
        info->green_mask_size	= 8;
        info->blue_mask_size	= 8;
        info->alpha_mask_size	= 8;
        info->red_field_pos		= 24;
        info->green_field_pos	= 16;
        info->blue_field_pos	= 8;
        info->alpha_field_pos	= 0;
		break;

	case PIXEL_FORMAT_BGR_888:
		info->bits_per_pixel	= 24;
		info->bytes_per_pixel 	= 3;
        info->red_mask_size		= 8;
        info->green_mask_size	= 8;
        info->blue_mask_size	= 8;
        info->alpha_mask_size	= 0;
        info->red_field_pos		= 0;
        info->green_field_pos	= 8;
        info->blue_field_pos	= 16;
        info->alpha_field_pos	= 0;
		break;

	case PIXEL_FORMAT_RGB_888:
		info->bits_per_pixel	= 24;
		info->bytes_per_pixel 	= 3;
        info->red_mask_size		= 8;
        info->green_mask_size	= 8;
        info->blue_mask_size	= 8;
        info->alpha_mask_size	= 0;
        info->red_field_pos		= 16;
        info->green_field_pos	= 8;
        info->blue_field_pos	= 0;
        info->alpha_field_pos	= 0;
		break;

	case PIXEL_FORMAT_BGR_565:
		info->bits_per_pixel	= 16;
		info->bytes_per_pixel 	= 2;
        info->red_mask_size		= 5;
        info->green_mask_size	= 6;
        info->blue_mask_size	= 5;
        info->alpha_mask_size	= 0;
        info->red_field_pos		= 0;
        info->green_field_pos	= 5;
        info->blue_field_pos	= 11;
        info->alpha_field_pos	= 0;
		break;

	case PIXEL_FORMAT_RGB_565:
		info->bits_per_pixel	= 16;
		info->bytes_per_pixel 	= 2;
        info->red_mask_size		= 5;
        info->green_mask_size	= 6;
        info->blue_mask_size	= 5;
        info->alpha_mask_size	= 0;
        info->red_field_pos		= 11;
        info->green_field_pos	= 5;
        info->blue_field_pos	= 0;
        info->alpha_field_pos	= 0;
		break;

	case PIXEL_FORMAT_BGR_332:
		info->bits_per_pixel	= 8;
		info->bytes_per_pixel 	= 1;
        info->red_mask_size		= 2;
        info->green_mask_size	= 3;
        info->blue_mask_size	= 3;
        info->alpha_mask_size	= 0;
        info->red_field_pos		= 0;
        info->green_field_pos	= 2;
        info->blue_field_pos	= 5;
        info->alpha_field_pos	= 0;
        break;

	case PIXEL_FORMAT_RGB_332:
		info->bits_per_pixel	= 8;
		info->bytes_per_pixel 	= 1;
        info->red_mask_size		= 3;
        info->green_mask_size	= 3;
        info->blue_mask_size	= 2;
        info->alpha_mask_size	= 0;
        info->red_field_pos		= 5;
        info->green_field_pos	= 2;
        info->blue_field_pos	= 0;
        info->alpha_field_pos	= 0;
		break;

	default:
		break;
	}

	info->fmt = fmt;
}

u32_t map_pixel_color(struct pixel_info * info, struct color_t * col)
{
	u8_t r = col->r;
	u8_t g = col->g;
	u8_t b = col->b;
	u8_t a = col->a;
	u32_t c;

	r >>= 8 - info->red_mask_size;
	g >>= 8 - info->green_mask_size;
	b >>= 8 - info->blue_mask_size;
	a >>= 8 - info->alpha_mask_size;

	c = r << info->red_field_pos;
	c |= g << info->green_field_pos;
	c |= b << info->blue_field_pos;
	c |= a << info->alpha_field_pos;

	return c;
}

void unmap_pixel_color(struct pixel_info * info, u32_t c, struct color_t * col)
{
	u32_t t;

	/* red */
	t = c >> info->red_field_pos;
	t &= (1 << info->red_mask_size) - 1;
	t <<= 8 - info->red_mask_size;
	t |= (1 << (8 - info->red_mask_size)) - 1;
	col->r = t & 0xff;

	/* green */
	t = c >> info->green_field_pos;
	t &= (1 << info->green_mask_size) - 1;
	t <<= 8 - info->green_mask_size;
	t |= (1 << (8 - info->green_mask_size)) - 1;
	col->g = t & 0xff;

	/* blue */
	t = c >> info->blue_field_pos;
	t &= (1 << info->blue_mask_size) - 1;
	t <<= 8 - info->blue_mask_size;
	t |= (1 << (8 - info->blue_mask_size)) - 1;
	col->b = t & 0xff;

	/* alpha */
	if (info->alpha_mask_size > 0)
	{
		t = c >> info->alpha_field_pos;
		t &= (1 << info->alpha_mask_size) - 1;
		t <<= 8 - info->alpha_mask_size;
		t |= (1 << (8 - info->alpha_mask_size)) - 1;
	}
	else
	{
		t = 0xff;
	}
	col->a = t & 0xff;
}
