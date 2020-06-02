/*
 * kernel/graphic/text.c
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
#include <graphic/text.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_CACHE_MANAGER_H

static void text_extent(struct text_t * txt)
{
	FT_BitmapGlyph bitmap;
	FT_Glyph glyph, gly;
	const char * p;
	uint32_t code;
	int col = 0, row = 0;
	int tw = 0, th = 0;
	int x = 0, y = 0, w = 0, h = 0;

	for(p = txt->utf8; ((utf8_to_ucs4(&code, 1, p, -1, &p) > 0) && (p - txt->utf8 <= txt->len));)
	{
		switch(code)
		{
		case '\r':
			tw = 0;
			th += 0;
			if(tw > w)
				w = tw;
			if(th > h)
				h = th;
			col = 0;
			break;

		case '\n':
			tw = 0;
			th += txt->size;
			if(tw > w)
				w = tw;
			if(th > h)
				h = th;
			col = 0;
			row++;
			break;

		case '\t':
			tw += txt->size * 2;
			th += 0;
			if(tw > w)
				w = tw;
			if(th > h)
				h = th;
			col++;
			break;

		default:
			glyph = font_glyph(txt->fctx, txt->family, txt->size, code);
			if(glyph)
			{
				if((txt->wrap > 0) && (tw + (glyph->advance.x >> 16) > txt->wrap))
				{
					tw = 0;
					th += txt->size;
					if(tw > w)
						w = tw;
					if(th > h)
						h = th;
					col = 0;
					row++;
				}
				tw += (glyph->advance.x >> 16);
				th += 0;
				if(tw > w)
					w = tw;
				if(th > h)
					h = th;
				if(FT_Glyph_Copy(glyph, &gly) == 0)
				{
					FT_Glyph_To_Bitmap(&gly, FT_RENDER_MODE_NORMAL, NULL, 1);
					bitmap = (FT_BitmapGlyph)gly;
					if(col == 0)
					{
						if(bitmap->left > x)
							x = bitmap->left;
					}
					if(row == 0)
					{
						if(bitmap->top > y)
							y = bitmap->top;
					}
					FT_Done_Glyph(gly);
				}
			}
			col++;
			break;
		}
	}
	region_init(&txt->e, x, y, w, h + txt->size);
}

void text_init(struct text_t * txt, const char * utf8, int len, struct color_t * c, int wrap, struct font_context_t * fctx, const char * family, int size)
{
	if(txt)
	{
		txt->utf8 = utf8;
		txt->len = (len < 0) ? strlen(utf8) : len;
		txt->c = c;
		txt->wrap = wrap;
		txt->fctx = fctx;
		txt->family = family;
		txt->size = (size > 0) ? size : 16;
		text_extent(txt);
	}
}

void text_set_text(struct text_t * txt, const char * utf8, int len)
{
	if(txt)
	{
		txt->utf8 = utf8;
		txt->len = (len < 0) ? strlen(utf8) : len;
		text_extent(txt);
	}
}

void text_set_color(struct text_t * txt, struct color_t * c)
{
	if(txt)
		txt->c = c;
}

void text_set_wrap(struct text_t * txt, int wrap)
{
	if(txt)
	{
		txt->wrap = wrap;
		text_extent(txt);
	}
}

void text_set_font_family(struct text_t * txt, const char * family)
{
	if(txt)
	{
		txt->family = family;
		text_extent(txt);
	}
}

void text_set_font_size(struct text_t * txt, int size)
{
	if(txt)
	{
		txt->size = (size > 0) ? size : 16;
		text_extent(txt);
	}
}

static inline void draw_font_bitmap(struct surface_t * s, struct region_t * clip, struct color_t * c, int x, int y, FT_Bitmap * bitmap)
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

void render_default_text(struct surface_t * s, struct region_t * clip, struct matrix_t * m, struct text_t * txt)
{
	FT_BitmapGlyph bitmap;
	FT_Glyph glyph, gly;
	FT_Matrix matrix;
	FT_Vector pen;
	const char * p;
	uint32_t code;
	int tx, ty, tw;

	matrix.xx = (FT_Fixed)(m->a * 65536);
	matrix.xy = -((FT_Fixed)(m->c * 65536));
	matrix.yx = -((FT_Fixed)(m->b * 65536));
	matrix.yy = (FT_Fixed)(m->d * 65536);
	tx = txt->e.x;
	ty = txt->e.y;
	tw = 0;
	pen.x = (FT_Pos)((m->tx + m->a * tx + m->c * ty) * 64);
	pen.y = (FT_Pos)((s->height - (m->ty + m->b * tx + m->d * ty)) * 64);

	for(p = txt->utf8; ((utf8_to_ucs4(&code, 1, p, -1, &p) > 0) && (p - txt->utf8 <= txt->len));)
	{
		switch(code)
		{
		case '\r':
			tx = txt->e.x;
			ty += 0;
			tw = 0;
			pen.x = (FT_Pos)((m->tx + m->a * tx + m->c * ty) * 64);
			pen.y = (FT_Pos)((s->height - (m->ty + m->b * tx + m->d * ty)) * 64);
			break;

		case '\n':
			tx = txt->e.x;
			ty += txt->size;
			tw = 0;
			pen.x = (FT_Pos)((m->tx + m->a * tx + m->c * ty) * 64);
			pen.y = (FT_Pos)((s->height - (m->ty + m->b * tx + m->d * ty)) * 64);
			break;

		case '\t':
			tx += txt->size * 2;
			ty += 0;
			tw += txt->size * 2;
			pen.x = (FT_Pos)((m->tx + m->a * tx + m->c * ty) * 64);
			pen.y = (FT_Pos)((s->height - (m->ty + m->b * tx + m->d * ty)) * 64);
			break;

		default:
			glyph = (FT_Glyph)font_glyph(txt->fctx, txt->family, txt->size, code);
			if(glyph)
			{
				if((txt->wrap > 0) && (tw + (glyph->advance.x >> 16) > txt->wrap))
				{
					tx = txt->e.x;
					ty += txt->size;
					tw = 0;
					pen.x = (FT_Pos)((m->tx + m->a * tx + m->c * ty) * 64);
					pen.y = (FT_Pos)((s->height - (m->ty + m->b * tx + m->d * ty)) * 64);
				}
				tw += (glyph->advance.x >> 16);
				if(FT_Glyph_Copy(glyph, &gly) == 0)
				{
					FT_Glyph_Transform(gly, &matrix, &pen);
					FT_Glyph_To_Bitmap(&gly, FT_RENDER_MODE_NORMAL, NULL, 1);
					bitmap = (FT_BitmapGlyph)gly;
					draw_font_bitmap(s, clip, txt->c, bitmap->left, s->height - bitmap->top, &bitmap->bitmap);
					pen.x += bitmap->root.advance.x >> 10;
					pen.y += bitmap->root.advance.y >> 10;
					FT_Done_Glyph(gly);
				}
			}
			break;
		}
	}
}
