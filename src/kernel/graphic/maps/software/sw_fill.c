/*
 * kernel/graphic/maps/software/sw_fill.c
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

static void software_fill_replace_1byte(struct surface_t * surface,
		struct rect_t * rect, u32_t c)
{
	u8_t * p, *q;
	u8_t * t;
	u32_t len, skip;
	u32_t i;
	u8_t fill = (u8_t) (c & 0xff);

	u32_t x = rect->x;
	u32_t y = rect->y;
	u32_t w = rect->w;
	u32_t h = rect->h;

	len = surface->info.bytes_per_pixel * w;
	skip = surface->pitch;
	t = (u8_t *) (surface->pixels + y * surface->pitch + x
			* surface->info.bytes_per_pixel);
	p = q = (u8_t *) t;

	for (i = 0; i < w; i++)
		*t++ = fill;

	for (i = 1; i < h; i++)
	{
		q += skip;
		memcpy(q, p, len);
	}
}

static void software_fill_replace_2byte(struct surface_t * surface,
		struct rect_t * rect, u32_t c)
{
	u8_t * p, *q;
	u16_t * t;
	u32_t len, skip;
	u32_t i;
	u16_t fill = (u16_t)(c & 0xffff);

	u32_t x = rect->x;
	u32_t y = rect->y;
	u32_t w = rect->w;
	u32_t h = rect->h;

	len = surface->info.bytes_per_pixel * w;
	skip = surface->pitch;
	t = (u16_t *) (surface->pixels + y * surface->pitch + x
			* surface->info.bytes_per_pixel);
	p = q = (u8_t *) t;

	for (i = 0; i < w; i++)
		*t++ = fill;

	for (i = 1; i < h; i++)
	{
		q += skip;
		memcpy(q, p, len);
	}
}

static void software_fill_replace_3byte(struct surface_t * surface,
		struct rect_t * rect, u32_t c)
{
	u8_t * p, *q;
	u8_t * t;
	u32_t len, skip;
	u32_t i;
	u8_t fill0 = (u8_t) ((c >> 0) & 0xff);
	u8_t fill1 = (u8_t) ((c >> 8) & 0xff);
	u8_t fill2 = (u8_t) ((c >> 16) & 0xff);

	u32_t x = rect->x;
	u32_t y = rect->y;
	u32_t w = rect->w;
	u32_t h = rect->h;

	len = surface->info.bytes_per_pixel * w;
	skip = surface->pitch;
	t = (u8_t *) (surface->pixels + y * surface->pitch + x
			* surface->info.bytes_per_pixel);
	p = q = (u8_t *) t;

	for (i = 0; i < w; i++)
	{
#if (BYTE_ORDER == BIG_ENDIAN)
		*t++ = fill2;
		*t++ = fill1;
		*t++ = fill0;
#else
		*t++ = fill0;
		*t++ = fill1;
		*t++ = fill2;
#endif
	}

	for (i = 1; i < h; i++)
	{
		q += skip;
		memcpy(q, p, len);
	}
}

static void software_fill_replace_4byte(struct surface_t * surface,
		struct rect_t * rect, u32_t c)
{
	u8_t * p, *q;
	u32_t * t;
	u32_t len, skip;
	u32_t i;

	u32_t x = rect->x;
	u32_t y = rect->y;
	u32_t w = rect->w;
	u32_t h = rect->h;

	len = surface->info.bytes_per_pixel * w;
	skip = surface->pitch;
	t = (u32_t *) (surface->pixels + y * surface->pitch + x
			* surface->info.bytes_per_pixel);
	p = q = (u8_t *) t;

	for (i = 0; i < w; i++)
		*t++ = c;

	for (i = 1; i < h; i++)
	{
		q += skip;
		memcpy(q, p, len);
	}
}

static inline void software_fill_replace(struct surface_t * surface,
		struct rect_t * rect, u32_t c)
{
	switch (surface->info.bytes_per_pixel)
	{
	case 1:
		software_fill_replace_1byte(surface, rect, c);
		break;

	case 2:
		software_fill_replace_2byte(surface, rect, c);
		break;

	case 3:
		software_fill_replace_3byte(surface, rect, c);
		break;

	case 4:
		software_fill_replace_4byte(surface, rect, c);
		break;

	default:
		break;
	}
}

static inline void software_fill_alpha(struct surface_t * surface,
		struct rect_t * rect, u32_t c)
{
	struct color_t dcol, scol;
	struct color_t col;
	u32_t dc;
	u8_t alpha;
	u32_t x, y, w, h;
	s32_t i, j;

	unmap_pixel_color(&surface->info, c, &scol);
	alpha = scol.a;

	if(alpha == 0xff)
	{
		software_fill_replace(surface, rect, c);
		return;
	}

	if(alpha == 0)
		return;

	x = rect->x;
	y = rect->y;
	w = rect->w;
	h = rect->h;

	switch (surface->info.bytes_per_pixel)
	{
	case 1:
	{
		u8_t * p = (u8_t *)surface_sw_get_pointer(surface, x, y);
		u32_t skip = surface->pitch - w;

		for(j = 0; j < h; j++)
		{
			for(i = 0; i < w; i++)
			{
				unmap_pixel_color(&surface->info, *p, &dcol);

				col.r = (((scol.r - dcol.r) * alpha) >> 8) + dcol.r;
				col.g = (((scol.g - dcol.g) * alpha) >> 8) + dcol.g;
				col.b = (((scol.b - dcol.b) * alpha) >> 8) + dcol.b;
				col.a = (((scol.a - dcol.a) * alpha) >> 8) + dcol.a;

				*p = map_pixel_color(&surface->info, &col);
				p++;
			}
			p += skip;
		}

		break;
	}

	case 2:
	{
		u16_t * p = (u16_t *)surface_sw_get_pointer(surface, x, y);
		u32_t skip = (surface->pitch - 2 * w) / 2;

		for(j = 0; j < h; j++)
		{
			for(i = 0; i < w; i++)
			{
				unmap_pixel_color(&surface->info, *p, &dcol);

				col.r = (((scol.r - dcol.r) * alpha) >> 8) + dcol.r;
				col.g = (((scol.g - dcol.g) * alpha) >> 8) + dcol.g;
				col.b = (((scol.b - dcol.b) * alpha) >> 8) + dcol.b;
				col.a = (((scol.a - dcol.a) * alpha) >> 8) + dcol.a;

				*p = map_pixel_color(&surface->info, &col);
				p++;
			}
			p += skip;
		}

		break;
	}

	case 3:
	{
		for(j = 0; j < h; j++)
		{
			for(i = 0; i < w; i++)
			{
				dc = surface_sw_get_pixel(surface, x + i, y + j);
				unmap_pixel_color(&surface->info, dc, &dcol);

				col.r = (((scol.r - dcol.r) * alpha) >> 8) + dcol.r;
				col.g = (((scol.g - dcol.g) * alpha) >> 8) + dcol.g;
				col.b = (((scol.b - dcol.b) * alpha) >> 8) + dcol.b;
				col.a = (((scol.a - dcol.a) * alpha) >> 8) + dcol.a;

				dc = map_pixel_color(&surface->info, &col);
				surface_sw_set_pixel(surface, x + i, y + j, dc);
			}
		}
		break;
	}

	case 4:
	{
		u32_t * p = (u32_t *)surface_sw_get_pointer(surface, x, y);
		u32_t skip = (surface->pitch - 4 * w) / 4;

		for(j = 0; j < h; j++)
		{
			for(i = 0; i < w; i++)
			{
				unmap_pixel_color(&surface->info, *p, &dcol);

				col.r = (((scol.r - dcol.r) * alpha) >> 8) + dcol.r;
				col.g = (((scol.g - dcol.g) * alpha) >> 8) + dcol.g;
				col.b = (((scol.b - dcol.b) * alpha) >> 8) + dcol.b;
				col.a = (((scol.a - dcol.a) * alpha) >> 8) + dcol.a;

				*p = map_pixel_color(&surface->info, &col);
				p++;
			}
			p += skip;
		}
		break;
	}

	default:
		break;
	}
}

bool_t map_software_fill(struct surface_t * surface,
		struct rect_t * rect, u32_t c, enum blend_mode mode)
{
	struct rect_t clipped;

	if (!surface)
		return FALSE;

	if (!surface->pixels)
		return FALSE;

	if (surface->info.bits_per_pixel < 8)
		return FALSE;

	if (rect)
	{
		if (!rect_intersect(rect, &surface->clip, &clipped))
		{
			return TRUE;
		}
		rect = &clipped;
	}
	else
	{
		rect = &surface->clip;
	}

	if(mode == BLEND_MODE_REPLACE)
	{
		software_fill_replace(surface, rect, c);
	}
	else if(mode == BLEND_MODE_ALPHA)
	{
		software_fill_alpha(surface, rect, c);
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}
