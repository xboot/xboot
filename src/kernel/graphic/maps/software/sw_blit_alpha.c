/*
 * kernel/graphic/maps/software/sw_blit_alpha.c
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

static void sw_blit_alpha_generic(struct surface_t * dst, struct rect_t * dst_rect, struct surface_t * src, struct rect_t * src_rect)
{
	struct color_t dcol, scol;
	struct color_t col;
	u8_t alpha;
	u32_t dc, sc;
	s32_t dx, dy, dw, dh;
	s32_t sx, sy;
	s32_t i, j;

	dx = dst_rect->x;
	dy = dst_rect->y;
	dw = dst_rect->w;
	dh = dst_rect->h;

	sx = src_rect->x;
	sy = src_rect->y;

	for(j = 0; j < dh; j++)
	{
		for(i = 0; i < dw; i++)
		{
			sc = surface_sw_get_pixel(src, sx + i, sy + j);
			unmap_pixel_color(&src->info, sc, &scol);
			alpha = scol.a;

			if(alpha == 0xff)
			{
				surface_sw_set_pixel(dst, dx + i, dy + j, sc);
				continue;
			}

			if(alpha == 0)
				continue;

			dc = surface_sw_get_pixel(dst, dx + i, dy + j);
			unmap_pixel_color(&dst->info, dc, &dcol);

			col.r = (((scol.r - dcol.r) * alpha) >> 8) + dcol.r;
			col.g = (((scol.g - dcol.g) * alpha) >> 8) + dcol.g;
			col.b = (((scol.b - dcol.b) * alpha) >> 8) + dcol.b;
			col.a = (((scol.a - dcol.a) * alpha) >> 8) + dcol.a;

			dc = map_pixel_color(&dst->info, &col);
			surface_sw_set_pixel(dst, dx + i, dy + j, dc);
		}
	}
}

void sw_blit_alpha(struct surface_t * dst, struct rect_t * dst_rect, struct surface_t * src, struct rect_t * src_rect)
{
	switch (dst->info.fmt)
	{
	default:
		break;
	}

	sw_blit_alpha_generic(dst, dst_rect, src, src_rect);
}
