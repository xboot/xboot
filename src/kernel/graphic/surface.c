/*
 * kernel/graphic/surface.c
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

struct surface_t * surface_alloc(void * pixels, u32_t w, u32_t h, enum pixel_format fmt)
{
	struct surface_t * surface;
	u32_t pitch, size;

	if( (w <= 0) && (h <= 0) )
		return NULL;

	surface = (struct surface_t *)malloc(sizeof(struct surface_t));
	if(! surface)
		return NULL;

	set_pixel_info(&surface->info, fmt);

	pitch = w * surface->info.bytes_per_pixel;
	switch(surface->info.bits_per_pixel)
	{
	case 1:
		pitch = (pitch + 7) / 8;
		break;
	case 4:
		pitch = (pitch + 1) / 2;
		break;
	default:
		break;
	}
	pitch = (pitch + 0x3) & ~0x3;

	surface->w = w;
	surface->h = h;
	surface->pitch = pitch;
	surface_set_clip_rect(surface, NULL);
	set_surface_maps(&surface->maps);

	if(pixels)
	{
		surface->pixels = pixels;
		surface->flag = SURFACE_PIXELS_DONTFREE;
	}
	else
	{
		size = surface->h * surface->pitch;
		surface->pixels = malloc(size);
		if (!surface->pixels)
		{
			free(surface);
			return NULL;
		}

		surface->flag = SURFACE_PIXELS_NEEDFREE;
		memset(surface->pixels, 0xff, size);
	}

	surface->pirv = NULL;
	return surface;
}

struct surface_t * surface_alloc_from_gimage(const struct gimage * image)
{
	if(!image)
		return NULL;

	switch (image->bytes_per_pixel)
	{
	case 3:
		return surface_alloc(image->pixels, image->width, image->height, PIXEL_FORMAT_BGR_888);

	case 4:
		return surface_alloc(image->pixels, image->width, image->height, PIXEL_FORMAT_ABGR_8888);

	default:
		break;
	}

	return NULL;
}

void surface_free(struct surface_t * surface)
{
	if(!surface)
		return;

	if(surface->pixels && (surface->flag == SURFACE_PIXELS_NEEDFREE))
		free(surface->pixels);

	free(surface);
}

bool_t surface_set_clip_rect(struct surface_t * surface, struct rect_t * rect)
{
	struct rect_t full;

	if (!surface)
		return FALSE;

	if (!rect)
	{
		surface->clip.x = 0;
		surface->clip.y = 0;
		surface->clip.w = surface->w;
		surface->clip.h = surface->h;

		return TRUE;
	}

	full.x = 0;
	full.y = 0;
	full.w = surface->w;
	full.h = surface->h;

	return rect_intersect(rect, &full, &surface->clip);
}

bool_t surface_get_clip_rect(struct surface_t * surface, struct rect_t * rect)
{
	if (!surface)
		return FALSE;

	if (!rect)
		return FALSE;

	rect->x = surface->clip.x;
	rect->y = surface->clip.y;
	rect->w = surface->clip.w;
	rect->h = surface->clip.h;

	return TRUE;
}

u32_t surface_map_color(struct surface_t * surface, struct color_t * col)
{
	if(!surface || !col)
		return 0;

	return map_pixel_color(&surface->info, col);
}

void surface_unmap_color(struct surface_t * surface, u32_t c, struct color_t * col)
{
	if(!surface || !col)
		return;

	unmap_pixel_color(&surface->info, c, col);
}

bool_t surface_draw_points(struct surface_t * surface, const struct point_t * points, u32_t count, u32_t c, enum blend_mode mode)
{
	if(!surface || !surface->maps.draw_points)
		return FALSE;

	return (surface->maps.draw_points(surface, points, count, c, mode));
}

bool_t surface_draw_lines(struct surface_t * surface, const struct point_t * points, u32_t count, u32_t c, enum blend_mode mode)
{
	if(!surface || !surface->maps.draw_lines)
		return FALSE;

	return (surface->maps.draw_lines(surface, points, count, c, mode));
}

bool_t surface_fill_rects(struct surface_t * surface, const struct rect_t * rects, u32_t count, u32_t c, enum blend_mode mode)
{
	if(!surface || !surface->maps.fill_rects)
		return FALSE;

	return (surface->maps.fill_rects(surface, rects, count, c, mode));
}

bool_t surface_blit(struct surface_t * dst, struct rect_t * dst_rect, struct surface_t * src, struct rect_t * src_rect, enum blend_mode mode)
{
	if(!dst || !src)
		return FALSE;

	if(!dst->maps.blit)
		return FALSE;

	return (dst->maps.blit(dst, dst_rect, src, src_rect, mode));
}
