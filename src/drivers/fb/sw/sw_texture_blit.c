/*
 * drivers/fb/sw/sw_texture_blit.c
 *
 * Copyright(c) 2007-2014 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
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

#include <fb/sw/sw.h>

void sw_render_blit_texture(struct render_t * render, struct rect_t * drect, struct texture_t * texture, struct rect_t * srect)
{
	struct rect_t dr, sr, clip;
	s32_t x, y, w, h;
	s32_t maxw, maxh;
	s32_t dx, dy;

	if(!render || !texture)
		return;

	clip.x = 0;
	clip.y = 0;
	clip.w = render->width;
	clip.h = render->height;

	if(!drect)
	{
		dr.x = 0;
		dr.y = 0;
		dr.w = render->width;
		dr.h = render->height;
	}
	else
	{
		dr.x = drect->x;
		dr.y = drect->y;
		dr.w = drect->w;
		dr.h = drect->h;
	}

	if (srect)
	{
		x = srect->x;
		w = srect->w;
		if (x < 0)
		{
			w += x;
			dr.x -= x;
			x = 0;
		}
		maxw = texture->width - x;
		if (maxw < w)
			w = maxw;

		y = srect->y;
		h = srect->h;
		if (y < 0)
		{
			h += y;
			dr.y -= y;
			y = 0;
		}
		maxh = texture->height - y;
		if (maxh < h)
			h = maxh;
	}
	else
	{
		x = 0;
		y = 0;
		w = texture->width;
		h = texture->height;
	}

	{
		dx = clip.x - dr.x;
		if (dx > 0)
		{
			w -= dx;
			dr.x += dx;
			x += dx;
		}
		dx = dr.x + w - clip.x - clip.w;
		if (dx > 0)
			w -= dx;

		dy = clip.y - dr.y;
		if (dy > 0)
		{
			h -= dy;
			dr.y += dy;
			y += dy;
		}
		dy = dr.y + h - clip.y - clip.h;
		if (dy > 0)
			h -= dy;
	}

	if (w > 0 && h > 0)
	{
		sr.x = x;
		sr.y = y;
		sr.w = dr.w = w;
		sr.h = dr.h = h;
		pixman_image_composite(PIXMAN_OP_SRC, texture->priv, NULL, render->priv, sr.x, sr.y, 0, 0, dr.x, dr.y, dr.w, dr.h);
	}
}
