/*
 * kernel/graphic/software/sw_draw_points.c
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

#include <graphic/surface.h>

extern inline void surface_set_pixel(struct surface_t * surface, s32_t x, s32_t y, u32_t c);
extern inline u32_t surface_get_pixel(struct surface_t * surface, s32_t x, s32_t y);

bool_t software_draw_points(struct surface_t * surface, const struct point_t * points, u32_t count, u32_t c)
{
	s32_t minx, miny;
	s32_t maxx, maxy;
	s32_t i;
	s32_t x, y;

	if (!surface)
		return FALSE;

	if (!surface->pixels)
		return FALSE;

	if (surface->info.bits_per_pixel < 8)
		return FALSE;

	minx = surface->clip.x;
	maxx = surface->clip.x + surface->clip.w - 1;
	miny = surface->clip.y;
	maxy = surface->clip.y + surface->clip.h - 1;

	for (i = 0; i < count; i++)
	{
		x = points[i].x;
		y = points[i].y;

		if (x < minx || x > maxx || y < miny || y > maxy)
		{
			continue;
		}

		surface_set_pixel(surface, x, y, c);
	}

	return TRUE;
}
