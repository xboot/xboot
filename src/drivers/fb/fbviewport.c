/*
 * drivers/fb/fbviewport.c
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
#include <fb/graphic.h>
#include <fb/fbviewport.h>

/*
 * set bitmap viewport
 */
x_bool bitmap_set_viewport(struct bitmap * bitmap, struct rect * rect)
{
	struct rect r, x;

	if(!bitmap || !rect)
		return FALSE;

	x.left = 0;
	x.top = 0;
	x.right = bitmap->info.width;
	x.bottom = bitmap->info.height;

	if(rect_intersect(&r, &x, rect) == FALSE)
		return FALSE;

	bitmap->viewport.left = r.left;
	bitmap->viewport.top = r.top;
	bitmap->viewport.right = r.right;
	bitmap->viewport.bottom = r.bottom;

	return TRUE;
}

/*
 * get bitmap viewport
 */
x_bool bitmap_get_viewport(struct bitmap * bitmap, struct rect * rect)
{
	if(!bitmap || !rect)
		return FALSE;

	rect->left = bitmap->viewport.left;
	rect->top = bitmap->viewport.top;
	rect->right = bitmap->viewport.right;
	rect->bottom = bitmap->viewport.bottom;

	return TRUE;
}
