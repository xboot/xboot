/*
 * kernel/graphic/software/sw_zoom.c
 *
 * Copyright (c) 2007-2011  jianjun jiang <jerryjianjun@gmail.com>
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

#include <graphic/software.h>

static void software_zoom_4byte(struct surface_t * dst, struct surface_t * src, struct rect_t * rect)
{
	u32_t * dp, * sp;
	u8_t * tp, * op;
	s32_t dx, dy, dw, dh;
	s32_t pitch;
	s32_t tx, ty;
	s32_t x, y;

	dw = dst->w;
	dh = dst->h;
	pitch = src->pitch;

	tx = (rect->w << 16) / dw;
	ty = (rect->h << 16) / dh;

	dp = dst->pixels;
	op = (u8_t *)(src->pixels + rect->y * pitch + rect->x * src->info.bytes_per_pixel);
	y = 0;

	for(dy = 0; dy < dh; dy++)
	{
		tp = (u8_t *)(op + (y >> 16) * pitch);
		x = 0;
		for (dx = 0; dx < dw; dx++)
		{
			sp = (u32_t *)((u32_t *)tp + (x >> 16));
			*dp++ = *sp;
			x += tx;
		}
		y += ty;
	}
}

struct surface_t * map_software_zoom(struct surface_t * surface, struct rect_t * rect, u32_t w, u32_t h)
{
	struct surface_t * zoom;
	struct rect_t clipped;

	if(w <= 0 || h <= 0)
		return NULL;

	if(!surface)
		return NULL;

	if (!surface->pixels)
		return NULL;

	if (surface->info.bits_per_pixel < 8)
		return NULL;

	clipped.x = 0;
	clipped.y = 0;
	clipped.w = surface->w;
	clipped.h = surface->h;

	if(rect)
	{
		if (!rect_intersect(rect, &clipped, &clipped))
			return NULL;
	}
	rect = &clipped;

	zoom = surface_alloc(NULL, w, h, surface->info.fmt);
	if(!zoom)
		return NULL;

	switch (surface->info.bytes_per_pixel)
	{
	case 1:
		break;

	case 2:
		break;

	case 3:
		break;

	case 4:
		software_zoom_4byte(zoom, surface, rect);
		break;

	default:
		surface_free(zoom);
		return NULL;
	}

	return zoom;
}
