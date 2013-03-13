/*
 * kernel/graphic/maps/software/sw_rotate.c
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

static void software_rotate_1byte(struct surface_t * dst, struct surface_t * src, struct rect_t * rect, enum rotate_type type)
{
	u8_t * dp, * sp;
	s32_t dx, dy, dw, dh;
	u32_t dpitch, spitch;
	u8_t bytes_per_pixel;
	u32_t len;
	u8_t * p, * q;

	dw = dst->w;
	dh = dst->h;
	dpitch = dst->pitch;
	spitch = src->pitch;
	bytes_per_pixel = src->info.bytes_per_pixel;
	len = bytes_per_pixel * dw;

	switch(type)
	{
	case ROTATE_DEGREE_0:
	{
		dp = dst->pixels;
		sp = src->pixels + rect->y * spitch + rect->x * bytes_per_pixel;

		for (dy = 0; dy < dh; dy++)
		{
			memcpy(dp, sp, len);

			dp += dpitch;
			sp += spitch;
		}
		break;
	}

	case ROTATE_DEGREE_90:
	{
		dp = dst->pixels;
		sp = src->pixels + rect->y * spitch + (rect->x + dh - 1) * bytes_per_pixel;

		for(dy = 0; dy < dh; dy++)
		{
			p = (u8_t *)dp;
			q = (u8_t *)sp;

			for(dx = 0; dx < dw; dx++)
			{
				*p++ = *q;
				q = (u8_t *)((u8_t *)q + spitch);
			}
			dp += dpitch;
			sp -= bytes_per_pixel;
		}
		break;
	}

	case ROTATE_DEGREE_180:
	{
		dp = dst->pixels;
		sp = src->pixels + (rect->y + dh - 1) * spitch + (rect->x + dw - 1) * bytes_per_pixel;

		for(dy = 0; dy < dh; dy++)
		{
			p = (u8_t *)dp;
			q = (u8_t *)sp;

			for(dx = 0; dx < dw; dx++)
			{
				*p++ = *q--;
			}
			dp += dpitch;
			sp -= spitch;
		}
		break;
	}

	case ROTATE_DEGREE_270:
	{
		dp = dst->pixels;
		sp = src->pixels + (rect->y + dw - 1) * spitch + rect->x * bytes_per_pixel;

		for(dy = 0; dy < dh; dy++)
		{
			p = (u8_t *)dp;
			q = (u8_t *)sp;

			for(dx = 0; dx < dw; dx++)
			{
				*p++ = *q;
				q = (u8_t *)((u8_t *)q - spitch);
			}
			dp += dpitch;
			sp += bytes_per_pixel;
		}
		break;
	}

	case ROTATE_MIRROR_H:
	{
		dp = dst->pixels;
		sp = src->pixels + rect->y * spitch + (rect->x + dw - 1) * bytes_per_pixel;

		for(dy = 0; dy < dh; dy++)
		{
			p = (u8_t *)dp;
			q = (u8_t *)sp;

			for(dx = 0; dx < dw; dx++)
			{
				*p++ = *q--;
			}
			dp += dpitch;
			sp += spitch;
		}
		break;
	}

	case ROTATE_MIRROR_V:
	{
		dp = dst->pixels;
		sp = src->pixels + (rect->y + dh - 1) * spitch + rect->x * bytes_per_pixel;

		for (dy = 0; dy < dh; dy++)
		{
			memcpy(dp, sp, len);

			dp += dpitch;
			sp -= spitch;
		}
		break;
	}

	default:
		return;
	}
}

static void software_rotate_2byte(struct surface_t * dst, struct surface_t * src, struct rect_t * rect, enum rotate_type type)
{
	u8_t * dp, * sp;
	s32_t dx, dy, dw, dh;
	u32_t dpitch, spitch;
	u8_t bytes_per_pixel;
	u32_t len;
	u16_t * p, * q;

	dw = dst->w;
	dh = dst->h;
	dpitch = dst->pitch;
	spitch = src->pitch;
	bytes_per_pixel = src->info.bytes_per_pixel;
	len = bytes_per_pixel * dw;

	switch(type)
	{
	case ROTATE_DEGREE_0:
	{
		dp = dst->pixels;
		sp = src->pixels + rect->y * spitch + rect->x * bytes_per_pixel;

		for (dy = 0; dy < dh; dy++)
		{
			memcpy(dp, sp, len);

			dp += dpitch;
			sp += spitch;
		}
		break;
	}

	case ROTATE_DEGREE_90:
	{
		dp = dst->pixels;
		sp = src->pixels + rect->y * spitch + (rect->x + dh - 1) * bytes_per_pixel;

		for(dy = 0; dy < dh; dy++)
		{
			p = (u16_t *)dp;
			q = (u16_t *)sp;

			for(dx = 0; dx < dw; dx++)
			{
				*p++ = *q;
				q = (u16_t *)((u8_t *)q + spitch);
			}
			dp += dpitch;
			sp -= bytes_per_pixel;
		}
		break;
	}

	case ROTATE_DEGREE_180:
	{
		dp = dst->pixels;
		sp = src->pixels + (rect->y + dh - 1) * spitch + (rect->x + dw - 1) * bytes_per_pixel;

		for(dy = 0; dy < dh; dy++)
		{
			p = (u16_t *)dp;
			q = (u16_t *)sp;

			for(dx = 0; dx < dw; dx++)
			{
				*p++ = *q--;
			}
			dp += dpitch;
			sp -= spitch;
		}
		break;
	}

	case ROTATE_DEGREE_270:
	{
		dp = dst->pixels;
		sp = src->pixels + (rect->y + dw - 1) * spitch + rect->x * bytes_per_pixel;

		for(dy = 0; dy < dh; dy++)
		{
			p = (u16_t *)dp;
			q = (u16_t *)sp;

			for(dx = 0; dx < dw; dx++)
			{
				*p++ = *q;
				q = (u16_t *)((u8_t *)q - spitch);
			}
			dp += dpitch;
			sp += bytes_per_pixel;
		}
		break;
	}

	case ROTATE_MIRROR_H:
	{
		dp = dst->pixels;
		sp = src->pixels + rect->y * spitch + (rect->x + dw - 1) * bytes_per_pixel;

		for(dy = 0; dy < dh; dy++)
		{
			p = (u16_t *)dp;
			q = (u16_t *)sp;

			for(dx = 0; dx < dw; dx++)
			{
				*p++ = *q--;
			}
			dp += dpitch;
			sp += spitch;
		}
		break;
	}

	case ROTATE_MIRROR_V:
	{
		dp = dst->pixels;
		sp = src->pixels + (rect->y + dh - 1) * spitch + rect->x * bytes_per_pixel;

		for (dy = 0; dy < dh; dy++)
		{
			memcpy(dp, sp, len);

			dp += dpitch;
			sp -= spitch;
		}
		break;
	}

	default:
		return;
	}
}

static void software_rotate_3byte(struct surface_t * dst, struct surface_t * src, struct rect_t * rect, enum rotate_type type)
{
	u8_t * dp, * sp;
	s32_t dx, dy, dw, dh;
	u32_t dpitch, spitch;
	u8_t bytes_per_pixel;
	u32_t len;
	u8_t * p, * q;

	dw = dst->w;
	dh = dst->h;
	dpitch = dst->pitch;
	spitch = src->pitch;
	bytes_per_pixel = src->info.bytes_per_pixel;
	len = bytes_per_pixel * dw;

	switch(type)
	{
	case ROTATE_DEGREE_0:
	{
		dp = dst->pixels;
		sp = src->pixels + rect->y * spitch + rect->x * bytes_per_pixel;

		for (dy = 0; dy < dh; dy++)
		{
			memcpy(dp, sp, len);

			dp += dpitch;
			sp += spitch;
		}
		break;
	}

	case ROTATE_DEGREE_90:
	{
		dp = dst->pixels;
		sp = src->pixels + rect->y * spitch + (rect->x + dh - 1) * bytes_per_pixel;

		for(dy = 0; dy < dh; dy++)
		{
			p = (u8_t *)dp;
			q = (u8_t *)sp;

			for(dx = 0; dx < dw; dx++)
			{
				*p++ = q[0];
				*p++ = q[1];
				*p++ = q[2];
				q += spitch;
			}
			dp += dpitch;
			sp -= bytes_per_pixel;
		}
		break;
	}

	case ROTATE_DEGREE_180:
	{
		dp = dst->pixels;
		sp = src->pixels + (rect->y + dh - 1) * spitch + (rect->x + dw - 1) * bytes_per_pixel;

		for(dy = 0; dy < dh; dy++)
		{
			p = (u8_t *)dp;
			q = (u8_t *)sp;

			for(dx = 0; dx < dw; dx++)
			{
				*p++ = q[0];
				*p++ = q[1];
				*p++ = q[2];
				q -= 3;
			}
			dp += dpitch;
			sp -= spitch;
		}
		break;
	}

	case ROTATE_DEGREE_270:
	{
		dp = dst->pixels;
		sp = src->pixels + (rect->y + dw - 1) * spitch + rect->x * bytes_per_pixel;

		for(dy = 0; dy < dh; dy++)
		{
			p = (u8_t *)dp;
			q = (u8_t *)sp;

			for(dx = 0; dx < dw; dx++)
			{
				*p++ = q[0];
				*p++ = q[1];
				*p++ = q[2];
				q -= spitch;
			}
			dp += dpitch;
			sp += bytes_per_pixel;
		}
		break;
	}

	case ROTATE_MIRROR_H:
	{
		dp = dst->pixels;
		sp = src->pixels + rect->y * spitch + (rect->x + dw - 1) * bytes_per_pixel;

		for(dy = 0; dy < dh; dy++)
		{
			p = (u8_t *)dp;
			q = (u8_t *)sp;

			for(dx = 0; dx < dw; dx++)
			{
				*p++ = q[0];
				*p++ = q[1];
				*p++ = q[2];
				q -= 3;
			}
			dp += dpitch;
			sp += spitch;
		}
		break;
	}

	case ROTATE_MIRROR_V:
	{
		dp = dst->pixels;
		sp = src->pixels + (rect->y + dh - 1) * spitch + rect->x * bytes_per_pixel;

		for (dy = 0; dy < dh; dy++)
		{
			memcpy(dp, sp, len);

			dp += dpitch;
			sp -= spitch;
		}
		break;
	}

	default:
		return;
	}
}

static void software_rotate_4byte(struct surface_t * dst, struct surface_t * src, struct rect_t * rect, enum rotate_type type)
{
	u8_t * dp, * sp;
	s32_t dx, dy, dw, dh;
	u32_t dpitch, spitch;
	u8_t bytes_per_pixel;
	u32_t len;
	u32_t * p, * q;

	dw = dst->w;
	dh = dst->h;
	dpitch = dst->pitch;
	spitch = src->pitch;
	bytes_per_pixel = src->info.bytes_per_pixel;
	len = bytes_per_pixel * dw;

	switch(type)
	{
	case ROTATE_DEGREE_0:
	{
		dp = dst->pixels;
		sp = src->pixels + rect->y * spitch + rect->x * bytes_per_pixel;

		for (dy = 0; dy < dh; dy++)
		{
			memcpy(dp, sp, len);

			dp += dpitch;
			sp += spitch;
		}
		break;
	}

	case ROTATE_DEGREE_90:
	{
		dp = dst->pixels;
		sp = src->pixels + rect->y * spitch + (rect->x + dh - 1) * bytes_per_pixel;

		for(dy = 0; dy < dh; dy++)
		{
			p = (u32_t *)dp;
			q = (u32_t *)sp;

			for(dx = 0; dx < dw; dx++)
			{
				*p++ = *q;
				q = (u32_t *)((u8_t *)q + spitch);
			}
			dp += dpitch;
			sp -= bytes_per_pixel;
		}
		break;
	}

	case ROTATE_DEGREE_180:
	{
		dp = dst->pixels;
		sp = src->pixels + (rect->y + dh - 1) * spitch + (rect->x + dw - 1) * bytes_per_pixel;

		for(dy = 0; dy < dh; dy++)
		{
			p = (u32_t *)dp;
			q = (u32_t *)sp;

			for(dx = 0; dx < dw; dx++)
			{
				*p++ = *q--;
			}
			dp += dpitch;
			sp -= spitch;
		}
		break;
	}

	case ROTATE_DEGREE_270:
	{
		dp = dst->pixels;
		sp = src->pixels + (rect->y + dw - 1) * spitch + rect->x * bytes_per_pixel;

		for(dy = 0; dy < dh; dy++)
		{
			p = (u32_t *)dp;
			q = (u32_t *)sp;

			for(dx = 0; dx < dw; dx++)
			{
				*p++ = *q;
				q = (u32_t *)((u8_t *)q - spitch);
			}
			dp += dpitch;
			sp += bytes_per_pixel;
		}
		break;
	}

	case ROTATE_MIRROR_H:
	{
		dp = dst->pixels;
		sp = src->pixels + rect->y * spitch + (rect->x + dw - 1) * bytes_per_pixel;

		for(dy = 0; dy < dh; dy++)
		{
			p = (u32_t *)dp;
			q = (u32_t *)sp;

			for(dx = 0; dx < dw; dx++)
			{
				*p++ = *q--;
			}
			dp += dpitch;
			sp += spitch;
		}
		break;
	}

	case ROTATE_MIRROR_V:
	{
		dp = dst->pixels;
		sp = src->pixels + (rect->y + dh - 1) * spitch + rect->x * bytes_per_pixel;

		for (dy = 0; dy < dh; dy++)
		{
			memcpy(dp, sp, len);

			dp += dpitch;
			sp -= spitch;
		}
		break;
	}

	default:
		return;
	}
}

struct surface_t * map_software_rotate(struct surface_t * surface, struct rect_t * rect, enum rotate_type type)
{
	struct surface_t * rotate;
	struct rect_t clipped;
	u32_t w, h;

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

	switch(type)
	{
	case ROTATE_DEGREE_0:
	case ROTATE_DEGREE_180:
	case ROTATE_MIRROR_H:
	case ROTATE_MIRROR_V:
		w = rect->w;
		h = rect->h;
		break;

	case ROTATE_DEGREE_90:
	case ROTATE_DEGREE_270:
		w = rect->h;
		h = rect->w;
		break;

	default:
		return NULL;
	}

	rotate = surface_alloc(NULL, w, h, surface->info.fmt);
	if(!rotate)
		return NULL;

	switch (surface->info.bytes_per_pixel)
	{
	case 1:
		software_rotate_1byte(rotate, surface, rect, type);
		break;

	case 2:
		software_rotate_2byte(rotate, surface, rect, type);
		break;

	case 3:
		software_rotate_3byte(rotate, surface, rect, type);
		break;

	case 4:
		software_rotate_4byte(rotate, surface, rect, type);
		break;

	default:
		surface_free(rotate);
		return NULL;
	}

	return rotate;
}
