/*
 * kernel/graphic/surface.c
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

#include <xboot/module.h>
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

	memset(&surface->maps, 0, sizeof(struct surface_maps));
	surface_set_maps(&surface->maps);

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

	surface->priv = NULL;
	return surface;
}
EXPORT_SYMBOL(surface_alloc);

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
EXPORT_SYMBOL(surface_alloc_from_gimage);

void surface_free(struct surface_t * surface)
{
	if(!surface)
		return;

	if(surface->pixels && (surface->flag == SURFACE_PIXELS_NEEDFREE))
		free(surface->pixels);

	free(surface);
}
EXPORT_SYMBOL(surface_free);

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
EXPORT_SYMBOL(surface_set_clip_rect);

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
EXPORT_SYMBOL(surface_get_clip_rect);

u32_t surface_map_color(struct surface_t * surface, struct color_t * col)
{
	if(!surface || !col)
		return 0;

	return map_pixel_color(&surface->info, col);
}
EXPORT_SYMBOL(surface_map_color);

void surface_unmap_color(struct surface_t * surface, u32_t c, struct color_t * col)
{
	if(!surface || !col)
		return;

	unmap_pixel_color(&surface->info, c, col);
}
EXPORT_SYMBOL(surface_unmap_color);

bool_t surface_fill(struct surface_t * surface, struct rect_t * rect, u32_t c, enum blend_mode mode)
{
	if(!surface || !surface->maps.fill)
		return FALSE;

	return (surface->maps.fill(surface, rect, c, mode));
}
EXPORT_SYMBOL(surface_fill);

bool_t surface_blit(struct surface_t * dst, struct rect_t * dst_rect, struct surface_t * src, struct rect_t * src_rect, enum blend_mode mode)
{
	if(!dst || !src)
		return FALSE;

	if(!dst->maps.blit)
		return FALSE;

	return (dst->maps.blit(dst, dst_rect, src, src_rect, mode));
}
EXPORT_SYMBOL(surface_blit);

struct surface_t * surface_scale(struct surface_t * surface, struct rect_t * rect, u32_t w, u32_t h)
{
	if(!surface || !surface->maps.scale)
		return NULL;

	return (surface->maps.scale(surface, rect, w, h));
}
EXPORT_SYMBOL(surface_scale);

struct surface_t * surface_rotate(struct surface_t * surface, struct rect_t * rect, enum rotate_type type)
{
	if(!surface || !surface->maps.rotate)
		return NULL;

	return (surface->maps.rotate(surface, rect, type));
}
EXPORT_SYMBOL(surface_rotate);
