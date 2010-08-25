/*
 * drivers/fb/fbfill.c
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
#include <fb/fb.h>
#include <fb/bitmap.h>
#include <fb/graphic.h>
#include <fb/fbfill.h>

/*
 * generic filler that works for every supported mode
 */
void bitmap_fill_rect_generic(struct bitmap * bitmap, x_u32 c, x_u32 x, x_u32 y, x_u32 w, x_u32 h)
{
	x_u32 i, j;

	for(j = 0; j < h; j++)
	{
		for(i = 0; i < w; i++)
		{
			set_bitmap_pixel(bitmap, x + i, y + j, c);
		}
	}
}
