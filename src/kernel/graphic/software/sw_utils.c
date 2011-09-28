/*
 * kernel/graphic/software/sw_utils.c
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

inline void surface_set_pixel(struct surface_t * surface, s32_t x, s32_t y, u32_t c)
{
	u8_t * p;

	switch (surface->info.bytes_per_pixel)
	{
	case 1:
		p = surface->pixels + y * surface->pitch + x;
		*((u8_t *) p) = (u8_t) c;
		break;

	case 2:
		p = surface->pixels + y * surface->pitch + x * 2;
		*((u16_t *) p) = (u16_t) c;
		break;

	case 3:
		p = surface->pixels + y * surface->pitch + x * 3;
		p[0] = c & 0xff;
		p[1] = (c >> 8) & 0xff;
		p[2] = (c >> 16) & 0xff;
		break;

	case 4:
		p = surface->pixels + y * surface->pitch + x * 4;
		*((u32_t *) p) = (u32_t) c;
		break;

	default:
		break;
	}
}

inline u32_t surface_get_pixel(struct surface_t * surface, s32_t x, s32_t y)
{
	u32_t c;
	u8_t * p;

	switch (surface->info.bytes_per_pixel)
	{
	case 1:
		p = surface->pixels + y * surface->pitch + x;
		c = *((u8_t *) p);
		break;

	case 2:
		p = surface->pixels + y * surface->pitch + x * 2;
		c = *((u16_t *) p);
		break;

	case 3:
		p = surface->pixels + y * surface->pitch + x * 3;
		c = p[0] | (p[1] << 8) | (p[2] << 16);
		break;

	case 4:
		p = surface->pixels + y * surface->pitch + x * 4;
		c = *((u32_t *) p);
		break;

	default:
		return 0;
	}

	return c;
}
