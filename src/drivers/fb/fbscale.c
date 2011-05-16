/*
 * drivers/fb/fbscale.c
 *
 * Copyright (c) 2007-2010  jianjun jiang <jerryjianjun@gmail.com>
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

#include <xboot.h>
#include <types.h>
#include <malloc.h>
#include <byteorder.h>
#include <fb/fb.h>
#include <fb/bitmap.h>
#include <fb/fbpixel.h>
#include <fb/fbscale.h>

/*
 * nearest neighbor bitmap scaling algorithm.
 */
static bool_t scale_nn(struct bitmap * dst, struct bitmap * src)
{
	if(!dst || !src)
		return FALSE;

	if(dst->info.red_field_pos % 8 != 0
		|| dst->info.green_field_pos % 8 != 0
		|| dst->info.blue_field_pos % 8 != 0
		|| dst->info.alpha_field_pos % 8 != 0)
		return FALSE;

	if(src->info.red_field_pos % 8 != 0
		|| src->info.green_field_pos % 8 != 0
		|| src->info.blue_field_pos % 8 != 0
		|| src->info.alpha_field_pos % 8 != 0)
		return FALSE;

	if(dst->info.red_field_pos != src->info.red_field_pos
		|| dst->info.red_mask_size != src->info.red_mask_size
		|| dst->info.green_field_pos != src->info.green_field_pos
		|| dst->info.green_mask_size != src->info.green_mask_size
		|| dst->info.blue_field_pos != src->info.blue_field_pos
		|| dst->info.blue_mask_size != src->info.blue_mask_size
		|| dst->info.alpha_field_pos != src->info.alpha_field_pos
		|| dst->info.alpha_mask_size != src->info.alpha_mask_size)
		return FALSE;

	if(dst->info.bytes_per_pixel != src->info.bytes_per_pixel)
		return FALSE;

	if(dst->info.width == 0 || dst->info.height == 0
		|| src->info.width == 0 || src->info.height == 0)
		return FALSE;

	u8_t * ddata = dst->data;
	u8_t * sdata = src->data;
	s32_t dw = dst->info.width;
	s32_t dh = dst->info.height;
	s32_t sw = src->info.width;
	s32_t sh = src->info.height;
	s32_t dstride = dst->info.pitch;
	s32_t sstride = src->info.pitch;
	s32_t bytes_per_pixel = dst->info.bytes_per_pixel;

	s32_t dx, dy, sx, sy, comp;
	u8_t *dptr, *sptr;

	for(dy = 0; dy < dh; dy++)
	{
		for (dx = 0; dx < dw; dx++)
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

			/* copy the pixel color value. */
			for(comp = 0; comp < bytes_per_pixel; comp++)
				dptr[comp] = sptr[comp];
		}
	}

	return TRUE;
}

/*
 * bilinear interpolation image scaling algorithm.
 */
static bool_t scale_bilinear(struct bitmap * dst, struct bitmap * src)
{
	if(!dst || !src)
		return FALSE;

	if(dst->info.red_field_pos % 8 != 0
		|| dst->info.green_field_pos % 8 != 0
		|| dst->info.blue_field_pos % 8 != 0
		|| dst->info.alpha_field_pos % 8 != 0)
		return FALSE;

	if(src->info.red_field_pos % 8 != 0
		|| src->info.green_field_pos % 8 != 0
		|| src->info.blue_field_pos % 8 != 0
		|| src->info.alpha_field_pos % 8 != 0)
		return FALSE;

	if(dst->info.red_field_pos != src->info.red_field_pos
		|| dst->info.red_mask_size != src->info.red_mask_size
		|| dst->info.green_field_pos != src->info.green_field_pos
		|| dst->info.green_mask_size != src->info.green_mask_size
		|| dst->info.blue_field_pos != src->info.blue_field_pos
		|| dst->info.blue_mask_size != src->info.blue_mask_size
		|| dst->info.alpha_field_pos != src->info.alpha_field_pos
		|| dst->info.alpha_mask_size != src->info.alpha_mask_size)
		return FALSE;

	if(dst->info.bytes_per_pixel != src->info.bytes_per_pixel)
		return FALSE;

	if(dst->info.width == 0 || dst->info.height == 0
		|| src->info.width == 0 || src->info.height == 0)
		return FALSE;

	u8_t * ddata = dst->data;
	u8_t * sdata = src->data;
	s32_t dw = dst->info.width;
	s32_t dh = dst->info.height;
	s32_t sw = src->info.width;
	s32_t sh = src->info.height;
	s32_t dstride = dst->info.pitch;
	s32_t sstride = src->info.pitch;
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

	return TRUE;
}

/*
 * create a new scaled version of the src bitmap
 */
bool_t bitmap_create_scaled(struct bitmap ** dst, u32_t w, u32_t h, struct bitmap * src, enum bitmap_scale_method method)
{
	*dst = 0;

	if(src == 0)
		return FALSE;

	if(src->info.red_field_pos % 8 != 0
		|| src->info.green_field_pos % 8 != 0
		|| src->info.blue_field_pos % 8 != 0
		|| src->info.alpha_field_pos % 8 != 0)
		return FALSE;

	if(src->info.width == 0 || src->info.height == 0)
		return FALSE;

	if(src->info.bytes_per_pixel * 8 != src->info.bpp)
		return FALSE;

	/* Create the new bitmap. */
	if(!bitmap_create(dst, w, h, src->info.fmt))
		return FALSE;

	switch(method)
	{
	case BITMAP_SCALE_METHOD_FASTEST:
		if(!scale_nn(*dst, src))
		{
			bitmap_destroy(*dst);
			return FALSE;
		}
		break;

	case BITMAP_SCALE_METHOD_BEST:
		if(!scale_bilinear(*dst, src))
		{
			bitmap_destroy(*dst);
			return FALSE;
		}
		break;

	default:
		bitmap_destroy(*dst);
		return FALSE;
	}

	return TRUE;
}
