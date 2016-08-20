/*
 * driver/fb/sw/sw_texture.c
 *
 * Copyright(c) 2007-2016 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
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

#include <fb/sw/sw.h>

struct texture_t * sw_render_alloc_texture(struct render_t * render, void * pixels, u32_t w, u32_t h, enum pixel_format_t format)
{
	struct texture_t * texture;
	pixman_image_t * img;
	u32_t pitch;

	if(!render)
		return NULL;

	if( (w <= 0) && (h <= 0) )
		return NULL;

	texture = (struct texture_t *)malloc(sizeof(struct texture_t));
	if(! texture)
		return NULL;

	switch(format)
	{
	case PIXEL_FORMAT_ARGB32:
		pitch = w * 4;
		break;
	case PIXEL_FORMAT_RGB24:
		pitch = w * 3;
		break;
	case PIXEL_FORMAT_A8:
		pitch = w * 1;
		break;
	case PIXEL_FORMAT_A1:
		pitch = (w + 7) / 8;
		break;
	case PIXEL_FORMAT_RGB16_565:
		pitch = w * 2;
		break;
	case PIXEL_FORMAT_RGB30:
		pitch = w * 4;
		break;
	default:
		pitch = w * 4;
		break;
	}
	pitch = (pitch + 0x3) & ~0x3;

	img = pixman_image_create_bits_no_clear(pixel_format_to_pixman_format_code(format), w, h, pixels, pitch);
	if(!img)
	{
		free(texture);
		return NULL;
	}

	texture->width = w;
	texture->height = h;
	texture->pitch = pitch;
	texture->format = format;
	texture->pixels = pixman_image_get_data(img);
	texture->priv = img;

	return texture;
}

struct texture_t * sw_render_alloc_texture_similar(struct render_t * render, u32_t w, u32_t h)
{
	return sw_render_alloc_texture(render, NULL, w, h, render->format);
}

void sw_render_free_texture(struct render_t * render, struct texture_t * texture)
{
	if(!render || !texture)
		return;

	pixman_image_unref((pixman_image_t *)texture->priv);
	free(texture);
}
