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

/*
 * nearest neighbor
 */
static void software_zoom_nn_1byte(struct surface_t * dst, struct surface_t * src, struct rect_t * rect)
{
	u8_t * dp, * sp;
	u8_t * tp, * op;
	s32_t dx, dy, dw, dh;
	s32_t dpitch, spitch;
	s32_t tx, ty;
	s32_t x, y;

	dw = dst->w;
	dh = dst->h;
	dpitch = dst->pitch;
	spitch = src->pitch;

	tx = (rect->w << 16) / dw;
	ty = (rect->h << 16) / dh;

	dp = dst->pixels;
	op = (u8_t *)(src->pixels + rect->y * spitch + rect->x * src->info.bytes_per_pixel);
	y = 0;

	for(dy = 0; dy < dh; dy++)
	{
		tp = (u8_t *)(op + (y >> 16) * spitch);
		x = 0;
		for (dx = 0; dx < dw; dx++)
		{
			sp = (u8_t *)((u8_t *)tp + (x >> 16));
			dp[dx] = *sp;
			x += tx;
		}
		dp = (u8_t *)((u8_t *)dp + dpitch);
		y += ty;
	}
}

static void software_zoom_nn_2byte(struct surface_t * dst, struct surface_t * src, struct rect_t * rect)
{
	u16_t * dp, * sp;
	u8_t * tp, * op;
	s32_t dx, dy, dw, dh;
	s32_t dpitch, spitch;
	s32_t tx, ty;
	s32_t x, y;

	dw = dst->w;
	dh = dst->h;
	dpitch = dst->pitch;
	spitch = src->pitch;

	tx = (rect->w << 16) / dw;
	ty = (rect->h << 16) / dh;

	dp = dst->pixels;
	op = (u8_t *)(src->pixels + rect->y * spitch + rect->x * src->info.bytes_per_pixel);
	y = 0;

	for(dy = 0; dy < dh; dy++)
	{
		tp = (u8_t *)(op + (y >> 16) * spitch);
		x = 0;
		for (dx = 0; dx < dw; dx++)
		{
			sp = (u16_t *)((u16_t *)tp + (x >> 16));
			dp[dx] = *sp;
			x += tx;
		}
		dp = (u16_t *)((u8_t *)dp + dpitch);
		y += ty;
	}
}

static void software_zoom_nn_3byte(struct surface_t * dst, struct surface_t * src, struct rect_t * rect)
{
	u8_t * dp, * sp;
	u8_t * tp, * op;
	s32_t dx, dy, dw, dh;
	s32_t dpitch, spitch;
	s32_t tx, ty;
	s32_t x, y, n;

	dw = dst->w;
	dh = dst->h;
	dpitch = dst->pitch;
	spitch = src->pitch;

	tx = (rect->w << 16) / dw;
	ty = (rect->h << 16) / dh;

	dp = dst->pixels;
	op = (u8_t *)(src->pixels + rect->y * spitch + rect->x * src->info.bytes_per_pixel);
	y = 0;

	for(dy = 0; dy < dh; dy++)
	{
		tp = (u8_t *)(op + (y >> 16) * spitch);
		x = 0;
		for (dx = 0, n = 0; dx < dw; dx++, n += 3)
		{
			sp = (u8_t *)((u8_t *)tp + (x >> 16) * 3);
			dp[n + 0] = sp[0];
			dp[n + 1] = sp[1];
			dp[n + 2] = sp[2];
			x += tx;
		}
		dp = (u8_t *)((u8_t *)dp + dpitch);
		y += ty;
	}
}

static void software_zoom_nn_4byte(struct surface_t * dst, struct surface_t * src, struct rect_t * rect)
{
	u32_t * dp, * sp;
	u8_t * tp, * op;
	s32_t dx, dy, dw, dh;
	s32_t dpitch, spitch;
	s32_t tx, ty;
	s32_t x, y;

	dw = dst->w;
	dh = dst->h;
	dpitch = dst->pitch;
	spitch = src->pitch;

	tx = (rect->w << 16) / dw;
	ty = (rect->h << 16) / dh;

	dp = dst->pixels;
	op = (u8_t *)(src->pixels + rect->y * spitch + rect->x * src->info.bytes_per_pixel);
	y = 0;

	for(dy = 0; dy < dh; dy++)
	{
		tp = (u8_t *)(op + (y >> 16) * spitch);
		x = 0;
		for (dx = 0; dx < dw; dx++)
		{
			sp = (u32_t *)((u32_t *)tp + (x >> 16));
			dp[dx] = *sp;
			x += tx;
		}
		dp = (u32_t *)((u8_t *)dp + dpitch);
		y += ty;
	}
}

#if 0
/*
 * bilinear interpolation
 */
static void software_zoom_bilinear_4byte(struct surface_t * dst, struct surface_t * src, struct rect_t * rect)
{
	u8_t * ddata = dst->pixels;
	u8_t * sdata = src->pixels;
	s32_t dw = dst->w;
	s32_t dh = dst->h;
	s32_t sw = src->w;
	s32_t sh = src->h;
	s32_t dstride = dst->pitch;
	s32_t sstride = src->pitch;
	s32_t bytes_per_pixel = dst->info.bytes_per_pixel;

	s32_t dx, dy, sx, sy, comp;
	u8_t *dptr, *sptr;

	for(dy = 0; dy < dh; dy++)
	{
		for(dx = 0; dx < dw; dx++)
		{
			/*
			 * compute the source coordinate that the destination coordinate maps to.
			 * sx/sw = dx/dw  =>  sx = sw*dx/dw.
			 */
			sx = sw * dx / dw;
			sy = sh * dy / dh;

			/* get the address of the pixels in src and dst */
			dptr = ddata + dy * dstride + dx * bytes_per_pixel;
			sptr = sdata + sy * sstride + sx * bytes_per_pixel;

			/*
			 * if we have enough space to do so, use bilinear interpolation.
			 * otherwise, fall back to nearest neighbor for this pixel.
			 */
			if(sx < sw - 1 && sy < sh - 1)
			{
				s32_t u = (256 * sw * dx / dw) - (sx * 256);
				s32_t v = (256 * sh * dy / dh) - (sy * 256);

				for(comp = 0; comp < bytes_per_pixel; comp++)
				{
					/* get the component's values for the four source corner pixels */
					u8_t f00 = sptr[comp];
					u8_t f10 = sptr[comp + bytes_per_pixel];
					u8_t f01 = sptr[comp + sstride];
					u8_t f11 = sptr[comp + sstride + bytes_per_pixel];

					/* do linear s32_terpolations along the top and bottom rows of the box */
					u8_t f0y = (256 - v) * f00 / 256 + v * f01 / 256;
					u8_t f1y = (256 - v) * f10 / 256 + v * f11 / 256;

					/* interpolate vertically */
					u8_t fxy = (256 - u) * f0y / 256 + u * f1y / 256;

					dptr[comp] = fxy;
				}
			}
			else
			{
				for(comp = 0; comp < bytes_per_pixel; comp++)
					dptr[comp] = sptr[comp];
			}
		}
	}
}
#endif

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
		software_zoom_nn_1byte(zoom, surface, rect);
		break;

	case 2:
		software_zoom_nn_2byte(zoom, surface, rect);
		break;

	case 3:
		software_zoom_nn_3byte(zoom, surface, rect);
		break;

	case 4:
		software_zoom_nn_4byte(zoom, surface, rect);
		break;

	default:
		surface_free(zoom);
		return NULL;
	}

	return zoom;
}
