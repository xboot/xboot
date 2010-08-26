/*
 * kernel/gui/rect.c
 *
 * Copyright (c) 2007-2009  jianjun jiang <jerryjianjun@gmail.com>
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
#include <string.h>
#include <gui/rect.h>

x_bool rect_set(struct rect * rect, x_s32 left, x_s32 top, x_s32 right, x_s32 bottom)
{
	rect->left = left;
	rect->top = top;
	rect->right = right;
	rect->bottom = bottom;

	return TRUE;
}

x_bool rect_set_empty(struct rect * rect)
{
	rect->left = 0;
	rect->top = 0;
	rect->right = 0;
	rect->bottom = 0;

	return TRUE;
}

x_bool rect_copy(struct rect * dst, struct rect * src)
{
	dst->left = src->left;
	dst->top = src->top;
	dst->right = src->right;
	dst->bottom = src->bottom;

	return TRUE;
}

x_bool rect_is_empty(struct rect * rect)
{
	if( (rect->right <= rect->left) || (rect->bottom <= rect->top) )
	{
		return TRUE;
	}

	return FALSE;
}

x_bool rect_is_equal(struct rect * rect1, struct rect * rect2)
{
	if( (rect1->left == rect2->left)
		&& (rect1->top == rect2->top)
		&& (rect1->right == rect2->right)
		&& (rect1->bottom == rect2->bottom) )
	{
		return TRUE;
	}

	return FALSE;
}

x_bool rect_intersect(struct rect * rect, struct rect * src1, struct rect * src2)
{
	rect->left = MAX(src1->left, src2->left);
	rect->top = MAX(src1->top, src2->top);
	rect->right = MIN(src1->right, src2->right);
	rect->bottom = MIN(src1->bottom, src2->bottom);

	if( (rect->right <= rect->left) || (rect->bottom <= rect->top) )
	{
		rect->left = 0;
		rect->top = 0;
		rect->right = 0;
		rect->bottom = 0;

		return FALSE;
	}

	return TRUE;
}

x_bool rect_union(struct rect * rect, struct rect * src1, struct rect * src2)
{
	rect->left = MIN(src1->left, src2->left);
	rect->top = MIN(src1->top, src2->top);
	rect->right = MAX(src1->right, src2->right);
	rect->bottom = MAX(src1->bottom, src2->bottom);

	if( (rect->right <= rect->left) || (rect->bottom <= rect->top) )
	{
		rect->left = 0;
		rect->top = 0;
		rect->right = 0;
		rect->bottom = 0;

		return FALSE;
	}

	return TRUE;
}

x_bool rect_subtract(struct rect * rect, struct rect * src1, struct rect * src2)
{
	struct rect r;

	rect->left = 0;
	rect->top = 0;
	rect->right = 0;
	rect->bottom = 0;

	if(rect_intersect(&r, src1, src2))
	{
		if( (r.left == src1->left) && (r.top == src1->top) )
		{
			if(r.bottom == src1->bottom)
			{
				rect->left = r.right;
				rect->top = r.top;
				rect->right = src1->right;
				rect->bottom = r.bottom;
			}
			else if(r.right == src1->right)
			{
				rect->left = r.left;
				rect->top = r.bottom;
				rect->right = r.right;
				rect->bottom = src1->bottom;
			}
		}
		else if( (r.right == src1->right) && (r.bottom == src1->bottom) )
		{
			if(r.left == src1->left)
			{
				rect->left = r.left;
				rect->top =  src1->top;
				rect->right = r.right;
				rect->bottom = r.top;
			}
			else if(r.top == src1->top)
			{
				rect->left = src1->left;
				rect->top =  r.top;
				rect->right = r.left;
				rect->bottom = r.top;
			}
		}
	}

	if( (rect->right <= rect->left) || (rect->bottom <= rect->top) )
	{
		rect->left = 0;
		rect->top = 0;
		rect->right = 0;
		rect->bottom = 0;

		return FALSE;
	}

	return TRUE;
}

x_bool rect_offset(struct rect * rect, x_s32 dx, x_s32 dy)
{
	rect->left += dx;
	rect->right += dx;
	rect->top += dy;
	rect->bottom += dy;

	return TRUE;
}

x_bool rect_inflate(struct rect * rect, x_s32 dx, x_s32 dy)
{
	rect->left -= dx;
	rect->right += dx;
	rect->top -= dy;
	rect->bottom += dy;

	return TRUE;
}

x_bool rect_have_point(struct rect * rect, x_s32 x, x_s32 y)
{
	if( (x >= rect->left) && (x < rect->right) && (y >= rect->top) && (y < rect->bottom) )
	{
		return TRUE;
	}

	return FALSE;
}
