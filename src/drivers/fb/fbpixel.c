/*
 * drivers/fb/fbpixel.c
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
#include <fb/fbcolor.h>
#include <fb/fbfill.h>
#include <fb/fbblit.h>
#include <fb/graphic.h>
#include <fb/fbpixel.h>

/*
 * get bitmap's pointer at coordinate of x, y
 */
x_u8 * bitmap_get_pointer(struct bitmap * bitmap, x_u32 x, x_u32 y)
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

	case 1:
		return 0;

	default:
		return 0;
	}

	return p;
}

/*
 * get bitmap's pixel at coordinate of x, y
 */
x_u32 bitmap_get_pixel(struct bitmap * bitmap, x_u32 x, x_u32 y)
{
	x_u32 c;
	x_u8 * p;
	x_s32 pos;

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

	case 1:
		p = bitmap->data + y * bitmap->info.pitch + (x / 8);
		pos = 7 - x % 8;
		c = (*p >> pos) & 0x01;
		break;

	default:
		return 0;
	}

	return c;
}

/*
 * set bitmap's pixel at coordinate of x, y
 */
void bitmap_set_pixel(struct bitmap * bitmap, x_u32 x, x_u32 y, x_u32 c)
{
	x_u8 * p;
	x_s32 pos;

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

	case 1:
		p = bitmap->data + y * bitmap->info.pitch + (x / 8);
		pos = 7 - x % 8;
		*p = (*p & ~(1 << pos)) | ((c & 0x01) << pos);
		break;

	default:
		break;
	}
}
