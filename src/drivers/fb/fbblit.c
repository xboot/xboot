/*
 * drivers/fb/fbblit.c
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

#include <configs.h>
#include <default.h>
#include <types.h>
#include <xboot.h>
#include <malloc.h>
#include <byteorder.h>
#include <fb/fb.h>
#include <fb/bitmap.h>
#include <fb/fbcolor.h>
#include <fb/fbpixel.h>
#include <fb/fbblit.h>

/*
 * generic replacing blitter that works for every supported mode
 */
static void bitmap_blit_replace_generic(struct bitmap * dst, struct bitmap * src, u32_t x, u32_t y, u32_t w, u32_t h, u32_t ox, u32_t oy)
{
	u8_t r, g, b, a;
	u32_t sc, dc;
	u32_t i, j;

	for(j = 0; j < h; j++)
	{
		for(i = 0; i < w; i++)
		{
			sc = bitmap_get_pixel(src, i + ox, j + oy);
			bitmap_unmap_color(src, sc, &r, &g, &b, &a);
			dc = bitmap_map_color(dst, r, g, b, a);
			bitmap_set_pixel(dst, x + i, y + j, dc);
		}
	}
}

/*
 * block copy replacing blitter that works with modes multiple of 8 bits
 */
static void bitmap_blit_replace_directN(struct bitmap * dst, struct bitmap * src, u32_t x, u32_t y, u32_t w, u32_t h, u32_t ox, u32_t oy)
{
	u8_t * ps, * pd;
	u32_t j;
	u32_t pitch = w * src->info.bytes_per_pixel;

	for(j = 0; j < h; j++)
	{
		ps = bitmap_get_pointer(src, ox, j + oy);
		pd = bitmap_get_pointer(dst, x, y + j);

		memmove(pd, ps, pitch);
	}
}

/*
 * optimized replacing blitter for 1-bit to 32-bit
 */
static void bitmap_blit_replace_32bit_1bit(struct bitmap * dst, struct bitmap * src, u32_t x, u32_t y, u32_t w, u32_t h, u32_t ox, u32_t oy)
{
	u8_t * ps, * pd;
	u32_t fg, bg;
	u32_t i, j;
	u32_t dskip, sskipbyte, sskipbit;
	s32_t pos;
	u8_t mask;

	dskip = dst->info.pitch - dst->info.bytes_per_pixel * w;
	sskipbyte = (src->info.width - w) >> 3;
	sskipbit = (src->info.width - w) & 7;

	pos = oy * src->info.width + ox;
	ps = (u8_t *)src->data + (pos >> 3);
	mask = 1 << (~pos & 7);
	pd = bitmap_get_pointer(dst, x, y);

	fg = bitmap_map_color(dst, src->info.fg_r, src->info.fg_g, src->info.fg_b, src->info.fg_a);
	bg = bitmap_map_color(dst, src->info.bg_r, src->info.bg_g, src->info.bg_b, src->info.bg_a);

	for(j = 0; j < h; j++)
	{
		for(i = 0; i < w; i++)
		{
			if(*ps & mask)
				*((u32_t *)pd) = fg;
			else
				*((u32_t *)pd) = bg;

			mask >>= 1;
			if(! mask)
			{
				ps++;
				mask = 0x80;
			}

			pd += 4;
		}

		ps += sskipbyte;
		if(mask >> sskipbit)
			mask >>= sskipbit;
		else
		{
			ps++;
			mask <<= 8 - sskipbit;
		}
		pd += dskip;
	}
}

/*
 * optimized replacing blitter for 1-bit to 24-bit
 */
static void bitmap_blit_replace_24bit_1bit(struct bitmap * dst, struct bitmap * src, u32_t x, u32_t y, u32_t w, u32_t h, u32_t ox, u32_t oy)
{
	u8_t * ps, * pd;
	u32_t fg, bg;
	u32_t i, j;
	u32_t dskip, sskipbyte, sskipbit;
	s32_t pos;
	u8_t mask;

	dskip = dst->info.pitch - dst->info.bytes_per_pixel * w;
	sskipbyte = (src->info.width - w) >> 3;
	sskipbit = (src->info.width - w) & 7;

	pos = oy * src->info.width + ox;
	ps = (u8_t *)src->data + (pos >> 3);
	mask = 1 << (~pos & 7);
	pd = bitmap_get_pointer(dst, x, y);

	fg = bitmap_map_color(dst, src->info.fg_r, src->info.fg_g, src->info.fg_b, src->info.fg_a);
	bg = bitmap_map_color(dst, src->info.bg_r, src->info.bg_g, src->info.bg_b, src->info.bg_a);

	for(j = 0; j < h; j++)
	{
		for(i = 0; i < w; i++)
		{
			if(*ps & mask)
				*((u32_t *)pd) = fg;
			else
				*((u32_t *)pd) = bg;

			mask >>= 1;
			if(! mask)
			{
				ps++;
				mask = 0x80;
			}

			pd += 3;
		}

		if(*ps & mask)
		{
			*pd++ = fg & 0xff;
			*pd++ = (fg & 0xff00) >> 8;
			*pd++ = (fg & 0xff0000) >> 16;
		}
		else
		{
			*pd++ = bg & 0xff;
			*pd++ = (bg & 0xff00) >> 8;
			*pd++ = (bg & 0xff0000) >> 16;
		}
		mask >>= 1;
		if(! mask)
		{
			ps++;
			mask = 0x80;
		}

		ps += sskipbyte;
		if(mask >> sskipbit)
			mask >>= sskipbit;
		else
		{
			ps++;
			mask <<= 8 - sskipbit;
		}
		pd += dskip;
	}
}

/*
 * optimized replacing blitter for 1-bit to 16-bit
 */
static void bitmap_blit_replace_16bit_1bit(struct bitmap * dst, struct bitmap * src, u32_t x, u32_t y, u32_t w, u32_t h, u32_t ox, u32_t oy)
{
	u8_t * ps, * pd;
	u32_t fg, bg;
	u32_t i, j;
	u32_t dskip, sskipbyte, sskipbit;
	s32_t pos;
	u8_t mask;

	dskip = dst->info.pitch - dst->info.bytes_per_pixel * w;
	sskipbyte = (src->info.width - w) >> 3;
	sskipbit = (src->info.width - w) & 7;

	pos = oy * src->info.width + ox;
	ps = (u8_t *)src->data + (pos >> 3);
	mask = 1 << (~pos & 7);
	pd = bitmap_get_pointer(dst, x, y);

	fg = bitmap_map_color(dst, src->info.fg_r, src->info.fg_g, src->info.fg_b, src->info.fg_a);
	bg = bitmap_map_color(dst, src->info.bg_r, src->info.bg_g, src->info.bg_b, src->info.bg_a);

	for(j = 0; j < h; j++)
	{
		for(i = 0; i < w; i++)
		{
			if(*ps & mask)
				*((u16_t *)pd) = fg;
			else
				*((u16_t *)pd) = bg;

			mask >>= 1;
			if(! mask)
			{
				ps++;
				mask = 0x80;
			}

			pd += 2;
		}

		ps += sskipbyte;
		if(mask >> sskipbit)
			mask >>= sskipbit;
		else
		{
			ps++;
			mask <<= 8 - sskipbit;
		}
		pd += dskip;
	}
}

/*
 * optimized replacing blitter for 1-bit to 8-bit
 */
static void bitmap_blit_replace_8bit_1bit(struct bitmap * dst, struct bitmap * src, u32_t x, u32_t y, u32_t w, u32_t h, u32_t ox, u32_t oy)
{
	u8_t * ps, * pd;
	u32_t fg, bg;
	u32_t i, j;
	u32_t dskip, sskipbyte, sskipbit;
	s32_t pos;
	u8_t mask;

	dskip = dst->info.pitch - dst->info.bytes_per_pixel * w;
	sskipbyte = (src->info.width - w) >> 3;
	sskipbit = (src->info.width - w) & 7;

	pos = oy * src->info.width + ox;
	ps = (u8_t *)src->data + (pos >> 3);
	mask = 1 << (~pos & 7);
	pd = bitmap_get_pointer(dst, x, y);

	fg = bitmap_map_color(dst, src->info.fg_r, src->info.fg_g, src->info.fg_b, src->info.fg_a);
	bg = bitmap_map_color(dst, src->info.bg_r, src->info.bg_g, src->info.bg_b, src->info.bg_a);

	for(j = 0; j < h; j++)
	{
		for(i = 0; i < w; i++)
		{
			if(*ps & mask)
				*((u8_t *)pd) = fg;
			else
				*((u8_t *)pd) = bg;

			mask >>= 1;
			if(! mask)
			{
				ps++;
				mask = 0x80;
			}

			pd++;
		}

		ps += sskipbyte;
		if(mask >> sskipbit)
			mask >>= sskipbit;
		else
		{
			ps++;
			mask <<= 8 - sskipbit;
		}
		pd += dskip;
	}
}

/*
 * optimized replacing blitter for RGBA8888 to BGRA8888
 */
static void bitmap_blit_replace_BGRA8888_RGBA8888(struct bitmap * dst, struct bitmap * src, u32_t x, u32_t y, u32_t w, u32_t h, u32_t ox, u32_t oy)
{
	u8_t * ps, * pd;
	u32_t i, j;
	u32_t sskip, dskip;
	u8_t r, g, b ,a;

	sskip = src->info.pitch - src->info.bytes_per_pixel * w;
	dskip = dst->info.pitch - dst->info.bytes_per_pixel * w;

	ps = bitmap_get_pointer(src, ox, oy);
	pd = bitmap_get_pointer(dst, x, y);

	for (j = 0; j < h; j++)
	{
		for (i = 0; i < w; i++)
		{
			r = *ps++;
			g = *ps++;
			b = *ps++;
			a = *ps++;

			*pd++ = b;
			*pd++ = g;
			*pd++ = r;
			*pd++ = a;
		}

		ps += sskip;
		pd += dskip;
	}
}

/*
 * optimized replacing blitter for RGBA8888 to RGB888
 */
static void bitmap_blit_replace_RGB888_RGBA8888(struct bitmap * dst, struct bitmap * src, u32_t x, u32_t y, u32_t w, u32_t h, u32_t ox, u32_t oy)
{
	u8_t * ps, * pd;
	u32_t sskip, dskip;
	u32_t i, j;
	u8_t r, g, b;

	sskip = src->info.pitch - src->info.bytes_per_pixel * w;
	dskip = dst->info.pitch - dst->info.bytes_per_pixel * w;

	ps = (u8_t *)bitmap_get_pointer(src, ox, oy);
	pd = (u8_t *)bitmap_get_pointer(dst, x, y);

	for(j = 0; j < h; j++)
	{
		for(i = 0; i < w; i++)
		{
			r = *ps++;
			g = *ps++;
			b = *ps++;

			*pd++ = r;
			*pd++ = g;
			*pd++ = b;
		}

		ps += sskip;
		pd += dskip;
	}
}

/*
 * optimized replacing blitter for RGBA8888 to BGR888
 */
static void bitmap_blit_replace_BGR888_RGBA8888(struct bitmap * dst, struct bitmap * src, u32_t x, u32_t y, u32_t w, u32_t h, u32_t ox, u32_t oy)
{
	u8_t * ps, * pd;
	u32_t sskip, dskip;
	u32_t i, j;
	u8_t r, g, b;

	sskip = src->info.pitch - src->info.bytes_per_pixel * w;
	dskip = dst->info.pitch - dst->info.bytes_per_pixel * w;

	ps = (u8_t *)bitmap_get_pointer(src, ox, oy);
	pd = (u8_t *)bitmap_get_pointer(dst, x, y);

	for(j = 0; j < h; j++)
	{
		for(i = 0; i < w; i++)
		{
			r = *ps++;
			g = *ps++;
			b = *ps++;

			*pd++ = b;
			*pd++ = g;
			*pd++ = r;
		}

		ps += sskip;
		pd += dskip;
	}
}

/*
 * optimized replacing blitter for RGB888 to RGBA8888
 */
static void bitmap_blit_replace_RGBA8888_RGB888(struct bitmap * dst, struct bitmap * src, u32_t x, u32_t y, u32_t w, u32_t h, u32_t ox, u32_t oy)
{
	u8_t * ps, * pd;
	u32_t sskip, dskip;
	u32_t i, j;
	u8_t r, g, b;

	sskip = src->info.pitch - src->info.bytes_per_pixel * w;
	dskip = dst->info.pitch - dst->info.bytes_per_pixel * w;

	ps = (u8_t *)bitmap_get_pointer(src, ox, oy);
	pd = (u8_t *)bitmap_get_pointer(dst, x, y);

	for(j = 0; j < h; j++)
	{
		for(i = 0; i < w; i++)
		{
			r = *ps++;
			g = *ps++;
			b = *ps++;

			*pd++ = r;
			*pd++ = g;
			*pd++ = b;

			*pd++ = 255;
		}

		ps += sskip;
		pd += dskip;
	}
}

/*
 * optimized replacing blitter for RGB888 to BGRA8888
 */
static void bitmap_blit_replace_BGRA8888_RGB888(struct bitmap * dst, struct bitmap * src, u32_t x, u32_t y, u32_t w, u32_t h, u32_t ox, u32_t oy)
{
	u8_t * ps, * pd;
	u32_t sskip, dskip;
	u32_t i, j;
	u8_t r, g, b;

	sskip = src->info.pitch - src->info.bytes_per_pixel * w;
	dskip = dst->info.pitch - dst->info.bytes_per_pixel * w;

	ps = (u8_t *)bitmap_get_pointer(src, ox, oy);
	pd = (u8_t *)bitmap_get_pointer(dst, x, y);

	for(j = 0; j < h; j++)
	{
		for(i = 0; i < w; i++)
		{
			r = *ps++;
			g = *ps++;
			b = *ps++;

			*pd++ = b;
			*pd++ = g;
			*pd++ = r;

			*pd++ = 255;
		}

		ps += sskip;
		pd += dskip;
	}
}

/*
 * optimized replacing blitter for RGB888 to BGR888
 */
static void bitmap_blit_replace_BGR888_RGB888(struct bitmap * dst, struct bitmap * src, u32_t x, u32_t y, u32_t w, u32_t h, u32_t ox, u32_t oy)
{
	u8_t * ps, * pd;
	u32_t sskip, dskip;
	u32_t i, j;
	u8_t r, g, b;

	sskip = src->info.pitch - src->info.bytes_per_pixel * w;
	dskip = dst->info.pitch - dst->info.bytes_per_pixel * w;

	ps = (u8_t *)bitmap_get_pointer(src, ox, oy);
	pd = (u8_t *)bitmap_get_pointer(dst, x, y);

	for(j = 0; j < h; j++)
	{
		for(i = 0; i < w; i++)
		{
			r = *ps++;
			g = *ps++;
			b = *ps++;

			*pd++ = b;
			*pd++ = g;
			*pd++ = r;
		}

		ps += sskip;
		pd += dskip;
	}
}

/*
 * generic blending blitter that works for every supported mode
 */
static void bitmap_blit_blend_generic(struct bitmap * dst, struct bitmap * src, u32_t x, u32_t y, u32_t w, u32_t h, u32_t ox, u32_t oy)
{
	u8_t sr, sg, sb, sa;
	u8_t dr, dg, db, da;
	u32_t sc, dc;
	u32_t i, j;

	for(j = 0; j < h; j++)
	{
		for(i = 0; i < w; i++)
		{
			sc = bitmap_get_pixel(src, i + ox, j + oy);
			bitmap_unmap_color(src, sc, &sr, &sg, &sb, &sa);

			if(sa == 0)
				continue;

			if(sa == 255)
			{
				dc = bitmap_map_color(dst, sr, sg, sb, sa);
				bitmap_set_pixel(dst, x + i, y + j, dc);
				continue;
			}

			dc = bitmap_get_pixel(dst, x + i, y + j);
			bitmap_unmap_color(dst, dc, &dr, &dg, &db, &da);

			dr = (((sr * sa) + (dr * (255 - sa))) / 255);
			dg = (((sg * sa) + (dg * (255 - sa))) / 255);
			db = (((sb * sa) + (db * (255 - sa))) / 255);
			da = sa;

			dc = bitmap_map_color(dst, dr, dg, db, da);
			bitmap_set_pixel(dst, x + i, y + j, dc);
		}
	}
}

/*
 * common bitmap blitter
 */
void common_bitmap_blit(struct bitmap * dst, struct bitmap * src, enum blit_mode mode, u32_t x, u32_t y, u32_t w, u32_t h, u32_t ox, u32_t oy)
{
	/*
	 * blit replace mode
	 */
	if(mode == BLIT_MODE_REPLACE)
	{
		if(src->info.fmt == BITMAP_FORMAT_MONOCHROME)
		{
			switch(dst->info.bpp)
			{
			case 32:
				bitmap_blit_replace_32bit_1bit(dst, src, x, y, w, h, ox, oy);
				return;

			case 24:
				bitmap_blit_replace_24bit_1bit(dst, src, x, y, w, h, ox, oy);
				return;

			case 16:
				bitmap_blit_replace_16bit_1bit(dst, src, x, y, w, h, ox, oy);
				return;

			case 8:
				bitmap_blit_replace_8bit_1bit(dst, src, x, y, w, h, ox, oy);
				return;

			default:
				break;
			}
		}
		else if(src->info.fmt == BITMAP_FORMAT_RGBA_8888)
		{
			switch(dst->info.fmt)
			{
			case BITMAP_FORMAT_RGBA_8888:
				bitmap_blit_replace_directN(dst, src, x, y, w, h, ox, oy);
				return;

			case BITMAP_FORMAT_BGRA_8888:
				bitmap_blit_replace_BGRA8888_RGBA8888(dst, src, x, y, w, h, ox, oy);
				return;

			case BITMAP_FORMAT_RGB_888:
				bitmap_blit_replace_RGB888_RGBA8888(dst, src, x, y, w, h, ox, oy);
				return;

			case BITMAP_FORMAT_BGR_888:
				bitmap_blit_replace_BGR888_RGBA8888(dst, src, x, y, w, h, ox, oy);
				return;

			default:
				break;
			}
		}
		else if(src->info.fmt == BITMAP_FORMAT_RGB_888)
		{
			switch(dst->info.fmt)
			{
			case BITMAP_FORMAT_RGBA_8888:
				bitmap_blit_replace_RGBA8888_RGB888(dst, src, x, y, w, h, ox, oy);
				return;

			case BITMAP_FORMAT_BGRA_8888:
				bitmap_blit_replace_BGRA8888_RGB888(dst, src, x, y, w, h, ox, oy);
				return;

			case BITMAP_FORMAT_RGB_888:
				bitmap_blit_replace_directN(dst, src, x, y, w, h, ox, oy);
				return;

			case BITMAP_FORMAT_BGR_888:
				bitmap_blit_replace_BGR888_RGB888(dst, src, x, y, w, h, ox, oy);
				return;

			default:
				break;
			}
		}
		else if(src->info.fmt == BITMAP_FORMAT_BGRA_8888)
		{
			switch(dst->info.fmt)
			{
			case BITMAP_FORMAT_BGRA_8888:
				bitmap_blit_replace_directN(dst, src, x, y, w, h, ox, oy);
				return;

			default:
				break;
			}
		}
		else if(src->info.fmt == BITMAP_FORMAT_BGR_888)
		{
			switch(dst->info.fmt)
			{
			case BITMAP_FORMAT_BGR_888:
				bitmap_blit_replace_directN(dst, src, x, y, w, h, ox, oy);
				return;

			default:
				break;
			}
		}

		bitmap_blit_replace_generic(dst, src, x, y, w, h, ox, oy);
	}

	/*
	 * blit blend mode
	 */
	else if(mode == BLIT_MODE_BLEND)
	{
		if(src->info.fmt == BITMAP_FORMAT_MONOCHROME)
		{

		}
		else if(src->info.fmt == BITMAP_FORMAT_RGBA_8888)
		{

		}
		else if(src->info.fmt == BITMAP_FORMAT_BGRA_8888)
		{

		}
		else if(src->info.fmt == BITMAP_FORMAT_RGB_888)
		{

		}
		else if(src->info.fmt == BITMAP_FORMAT_BGR_888)
		{

		}

		bitmap_blit_blend_generic(dst, src, x, y, w, h, ox, oy);
	}
}
