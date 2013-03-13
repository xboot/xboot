/*
 * kernel/graphic/maps/software/sw_utils.c
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

inline u8_t * surface_sw_get_pointer(struct surface_t * surface, s32_t x, s32_t y)
{
	u8_t * p;

	switch (surface->info.bytes_per_pixel)
	{
	case 1:
		p = surface->pixels + y * surface->pitch + x;
		break;

	case 2:
		p = surface->pixels + y * surface->pitch + x * 2;
		break;

	case 3:
		p = surface->pixels + y * surface->pitch + x * 3;
		break;

	case 4:
		p = surface->pixels + y * surface->pitch + x * 4;
		break;

	default:
		return 0;
	}

	return p;
}

inline u32_t surface_sw_get_pixel(struct surface_t * surface, s32_t x, s32_t y)
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
#if (BYTE_ORDER == BIG_ENDIAN)
		c = p[2] | (p[1] << 8) | (p[0] << 16);
#else
		c = p[0] | (p[1] << 8) | (p[2] << 16);
#endif
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

inline void surface_sw_set_pixel(struct surface_t * surface, s32_t x, s32_t y, u32_t c)
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
#if (BYTE_ORDER == BIG_ENDIAN)
		p[0] = (c >> 16) & 0xff;
		p[1] = (c >> 8) & 0xff;
		p[2] = c & 0xff;
#else
		p[0] = c & 0xff;
		p[1] = (c >> 8) & 0xff;
		p[2] = (c >> 16) & 0xff;
#endif
		break;

	case 4:
		p = surface->pixels + y * surface->pitch + x * 4;
		*((u32_t *) p) = (u32_t) c;
		break;

	default:
		break;
	}
}

inline void surface_sw_set_pixel_with_alpha(struct surface_t * surface, s32_t x, s32_t y, u32_t c)
{
	struct color_t dcol, scol;
	struct color_t col;
	u32_t dc;
	u8_t alpha;

	unmap_pixel_color(&surface->info, c, &scol);
	alpha = scol.a;

	if(alpha == 0xff)
	{
		surface_sw_set_pixel(surface, x, y, c);
		return;
	}

	if(alpha == 0)
	{
		return;
	}

	dc = surface_sw_get_pixel(surface, x, y);
	unmap_pixel_color(&surface->info, dc, &dcol);

	col.r = (((scol.r - dcol.r) * alpha) >> 8) + dcol.r;
	col.g = (((scol.g - dcol.g) * alpha) >> 8) + dcol.g;
	col.b = (((scol.b - dcol.b) * alpha) >> 8) + dcol.b;
	col.a = (((scol.a - dcol.a) * alpha) >> 8) + dcol.a;

	dc = map_pixel_color(&surface->info, &col);
	surface_sw_set_pixel(surface, x, y, dc);
}
