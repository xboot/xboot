/*
 * kernel/graphic/maps/software/sw_scale.c
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

/*
 * nearest neighbor
 */
static void software_scale_1byte(struct surface_t * dst, struct surface_t * src, struct rect_t * rect)
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

static void software_scale_2byte(struct surface_t * dst, struct surface_t * src, struct rect_t * rect)
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

static void software_scale_3byte(struct surface_t * dst, struct surface_t * src, struct rect_t * rect)
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

static void software_scale_4byte(struct surface_t * dst, struct surface_t * src, struct rect_t * rect)
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

struct surface_t * map_software_scale(struct surface_t * surface, struct rect_t * rect, u32_t w, u32_t h)
{
	struct surface_t * scale;
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

	scale = surface_alloc(NULL, w, h, surface->info.fmt);
	if(!scale)
		return NULL;

	switch (surface->info.bytes_per_pixel)
	{
	case 1:
		software_scale_1byte(scale, surface, rect);
		break;

	case 2:
		software_scale_2byte(scale, surface, rect);
		break;

	case 3:
		software_scale_3byte(scale, surface, rect);
		break;

	case 4:
		software_scale_4byte(scale, surface, rect);
		break;

	default:
		surface_free(scale);
		return NULL;
	}

	return scale;
}
