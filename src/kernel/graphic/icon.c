/*
 * kernel/graphic/icon.c
 *
 * Copyright(c) 2007-2020 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <ctype.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <charset.h>
#include <graphic/surface.h>
#include <graphic/font.h>
#include <graphic/icon.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_CACHE_MANAGER_H

static void icon_extent(struct icon_t * ico)
{
	FTC_SBit sbit;

	sbit = (FTC_SBit)font_lookup_bitmap(ico->fctx, ico->family, min(ico->width, ico->height), ico->code);
	if(sbit)
		region_init(&ico->e, sbit->left, sbit->top, sbit->xadvance, sbit->yadvance + sbit->height);
	else
		region_init(&ico->e, 0, 0, 0, 0);
}

void icon_init(struct icon_t * ico, uint32_t code, struct color_t * c, struct font_context_t * fctx, const char * family, int width, int height)
{
	if(ico)
	{
		ico->code = code;
		ico->c = c;
		ico->fctx = fctx;
		ico->family = family;
		ico->width = (width > 0) ? width : 16;
		ico->height = (height > 0) ? height : 16;
		icon_extent(ico);
	}
}

void icon_set_code(struct icon_t * ico, uint32_t code)
{
	if(ico)
	{
		ico->code = code;
		icon_extent(ico);
	}
}

void icon_set_color(struct icon_t * ico, struct color_t * c)
{
	if(ico)
		ico->c = c;
}

void icon_set_family(struct icon_t * ico, const char * family)
{
	if(ico)
	{
		ico->family = family;
		icon_extent(ico);
	}
}

void icon_set_size(struct icon_t * ico, int width, int height)
{
	if(ico)
	{
		ico->width = (width > 0) ? width : 16;
		ico->height = (height > 0) ? height : 16;
		icon_extent(ico);
	}
}

static inline void draw_font_bitmap(struct surface_t * s, struct region_t * clip, struct color_t * c, int x, int y, FTC_SBit sbit)
{
	struct region_t region, r;
	uint32_t color;
	uint32_t * dp, dv;
	uint8_t * sp, gray;
	uint8_t da, dr, dg, db;
	uint8_t sr, sg, sb, sa;
	uint8_t ta, tr, tg, tb;
	int dx, dy, dw, dh;
	int sx, sy;
	int dskip, sskip;
	int i, j, t;

	region_init(&r, 0, 0, s->width, s->height);
	if(clip)
	{
		if(!region_intersect(&r, &r, clip))
			return;
	}
	region_init(&region, x, y, sbit->width, sbit->height);
	if(!region_intersect(&r, &r, &region))
		return;

	dx = r.x;
	dy = r.y;
	dw = r.w;
	dh = r.h;
	sx = r.x - x;
	sy = r.y - y;
	dskip = s->width - dw;
	sskip = sbit->pitch - dw;
	dp = (uint32_t *)s->pixels + dy * s->width + dx;
	sp = (uint8_t *)sbit->buffer + sy * sbit->pitch + sx;
	color = (c->a << 24) | (c->r << 16) | (c->g << 8) | (c->b << 0);

	for(j = 0; j < dh; j++)
	{
		for(i = 0; i < dw; i++)
		{
			gray = *sp;
			if(gray != 0)
			{
				if(gray == 255)
				{
					*dp = color;
				}
				else
				{
					sr = idiv255(c->r * gray);
					sg = idiv255(c->g * gray);
					sb = idiv255(c->b * gray);
					sa = idiv255(c->a * gray);
					dv = *dp;
					da = (dv >> 24) & 0xff;
					dr = (dv >> 16) & 0xff;
					dg = (dv >> 8) & 0xff;
					db = (dv >> 0) & 0xff;
					t = sa + (sa >> 8);
					ta = (((sa + da) << 8) - da * t) >> 8;
					tr = (((sr + dr) << 8) - dr * t) >> 8;
					tg = (((sg + dg) << 8) - dg * t) >> 8;
					tb = (((sb + db) << 8) - db * t) >> 8;
					*dp = (ta << 24) | (tr << 16) | (tg << 8) | (tb << 0);
				}
			}
			sp++;
			dp++;
		}
		dp += dskip;
		sp += sskip;
	}
}

static inline void draw_font_glyph(struct surface_t * s, struct region_t * clip, struct color_t * c, int x, int y, FT_Bitmap * bitmap)
{
	struct region_t region, r;
	uint32_t color;
	uint32_t * dp, dv;
	uint8_t * sp, gray;
	uint8_t da, dr, dg, db;
	uint8_t sr, sg, sb, sa;
	uint8_t ta, tr, tg, tb;
	int dx, dy, dw, dh;
	int sx, sy;
	int dskip, sskip;
	int i, j, t;

	region_init(&r, 0, 0, s->width, s->height);
	if(clip)
	{
		if(!region_intersect(&r, &r, clip))
			return;
	}
	region_init(&region, x, y, bitmap->width, bitmap->rows);
	if(!region_intersect(&r, &r, &region))
		return;

	dx = r.x;
	dy = r.y;
	dw = r.w;
	dh = r.h;
	sx = r.x - x;
	sy = r.y - y;
	dskip = s->width - dw;
	sskip = bitmap->pitch - dw;
	dp = (uint32_t *)s->pixels + dy * s->width + dx;
	sp = (uint8_t *)bitmap->buffer + sy * bitmap->pitch + sx;
	color = (c->a << 24) | (c->r << 16) | (c->g << 8) | (c->b << 0);

	for(j = 0; j < dh; j++)
	{
		for(i = 0; i < dw; i++)
		{
			gray = *sp;
			if(gray != 0)
			{
				if(gray == 255)
				{
					*dp = color;
				}
				else
				{
					sr = idiv255(c->r * gray);
					sg = idiv255(c->g * gray);
					sb = idiv255(c->b * gray);
					sa = idiv255(c->a * gray);
					dv = *dp;
					da = (dv >> 24) & 0xff;
					dr = (dv >> 16) & 0xff;
					dg = (dv >> 8) & 0xff;
					db = (dv >> 0) & 0xff;
					t = sa + (sa >> 8);
					ta = (((sa + da) << 8) - da * t) >> 8;
					tr = (((sr + dr) << 8) - dr * t) >> 8;
					tg = (((sg + dg) << 8) - dg * t) >> 8;
					tb = (((sb + db) << 8) - db * t) >> 8;
					*dp = (ta << 24) | (tr << 16) | (tg << 8) | (tb << 0);
				}
			}
			sp++;
			dp++;
		}
		dp += dskip;
		sp += sskip;
	}
}

void render_default_icon(struct surface_t * s, struct region_t * clip, struct matrix_t * m, struct icon_t * ico)
{
	FTC_SBit sbit;
	FT_BitmapGlyph bitmap;
	FT_Glyph glyph, gly;
	FT_Matrix matrix;
	FT_Vector pen;
	int w, h;

	if((m->a == 1.0) && (m->b == 0.0) && (m->c == 0.0) && (m->d == 1.0))
	{
		sbit = (FTC_SBit)font_lookup_bitmap(ico->fctx, ico->family, min(ico->width, ico->height), ico->code);
		if(sbit)
		{
			pen.x = (FT_Pos)(m->tx + (ico->width - sbit->width) / 2);
			pen.y = (FT_Pos)(m->ty + (ico->height - sbit->height) / 2);
			draw_font_bitmap(s, clip, ico->c, pen.x, pen.y, sbit);
		}
	}
	else
	{
		glyph = (FT_Glyph)font_lookup_glyph(ico->fctx, ico->family, min(ico->width, ico->height), ico->code);
		if(glyph)
		{
			if(FT_Glyph_Copy(glyph, &gly) == 0)
			{
				matrix.xx = (FT_Fixed)(m->a * 65536);
				matrix.xy = -((FT_Fixed)(m->c * 65536));
				matrix.yx = -((FT_Fixed)(m->b * 65536));
				matrix.yy = (FT_Fixed)(m->d * 65536);
				pen.x = (FT_Pos)((m->tx + m->a * ico->e.x + m->c * ico->e.y) * 64);
				pen.y = (FT_Pos)((s->height - (m->ty + m->b * ico->e.x + m->d * ico->e.y)) * 64);
				FT_Glyph_Transform(gly, &matrix, &pen);
				FT_Glyph_To_Bitmap(&gly, FT_RENDER_MODE_NORMAL, NULL, 1);
				bitmap = (FT_BitmapGlyph)gly;
				w = (bitmap->root.advance.x >> 16);
				h = -(bitmap->root.advance.y >> 16) + bitmap->bitmap.rows;
				draw_font_glyph(s, clip, ico->c, bitmap->left + (ico->width - w) / 2, s->height - bitmap->top + (ico->height - h) / 2, &bitmap->bitmap);
				FT_Done_Glyph(gly);
			}
		}
	}
}
