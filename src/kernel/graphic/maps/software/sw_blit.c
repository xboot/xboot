/*
 * kernel/graphic/maps/software/sw_blit.c
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

#include <graphic/maps/software.h>

extern void sw_blit_replace(struct surface_t * dst, struct rect_t * dst_rect, struct surface_t * src, struct rect_t * src_rect);
extern void sw_blit_alpha(struct surface_t * dst, struct rect_t * dst_rect, struct surface_t * src, struct rect_t * src_rect);

bool_t map_software_blit(struct surface_t * dst, struct rect_t * dst_rect,
		struct surface_t * src, struct rect_t * src_rect, enum blend_mode mode)
{
	struct rect_t dr, sr;
	struct rect_t * clip;
	s32_t x, y, w, h;
	s32_t maxw, maxh;
	s32_t dx, dy;

	if(!dst || !src)
		return FALSE;

	if(!dst_rect)
	{
		dr.x = 0;
		dr.y = 0;
		dr.w = dst->w;
		dr.h = dst->h;
	}
	else
	{
		dr.x = dst_rect->x;
		dr.y = dst_rect->y;
		dr.w = dst_rect->w;
		dr.h = dst_rect->h;
	}

	/*
	 * Clip the source rectangle to the source surface
	 */
	if (src_rect)
	{
		x = src_rect->x;
		w = src_rect->w;
		if (x < 0)
		{
			w += x;
			dr.x -= x;
			x = 0;
		}
		maxw = src->w - x;
		if (maxw < w)
			w = maxw;

		y = src_rect->y;
		h = src_rect->h;
		if (y < 0)
		{
			h += y;
			dr.y -= y;
			y = 0;
		}
		maxh = src->h - y;
		if (maxh < h)
			h = maxh;
	}
	else
	{
		x = 0;
		y = 0;
		w = src->w;
		h = src->h;
	}

	/*
	 * Clip the destination rectangle against the clip rectangle
	 */
	{
		clip = &dst->clip;

		dx = clip->x - dr.x;
		if (dx > 0)
		{
			w -= dx;
			dr.x += dx;
			x += dx;
		}
		dx = dr.x + w - clip->x - clip->w;
		if (dx > 0)
			w -= dx;

		dy = clip->y - dr.y;
		if (dy > 0)
		{
			h -= dy;
			dr.y += dy;
			y += dy;
		}
		dy = dr.y + h - clip->y - clip->h;
		if (dy > 0)
			h -= dy;
	}

	if (w > 0 && h > 0)
	{
		sr.x = x;
		sr.y = y;
		sr.w = dr.w = w;
		sr.h = dr.h = h;

		if(mode == BLEND_MODE_REPLACE)
		{
			sw_blit_replace(dst, &dr, src, &sr);
		}
		else if(mode == BLEND_MODE_ALPHA)
		{
			sw_blit_alpha(dst, &dr, src, &sr);
		}
		else
		{
			return FALSE;
		}
	}

	return TRUE;
}
