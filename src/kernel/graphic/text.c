/*
 * kernel/graphic/text.c
 *
 * Copyright(c) 2007-2021 Jianjun Jiang <8192542@qq.com>
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

static void text_metrics(struct text_t * txt)
{
	FTC_SBit sbit;
	const char * p;
	uint32_t code;
	int col = 0, row = 0;
	int tw = 0, th = 0, lh = 0;
	int x = 0, y = 0, w = 0, h = 0;

	p = txt->utf8;
	while(*p)
	{
		p = utf8_to_code(p, &code);
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
			lh = 0;
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
			sbit = (FTC_SBit)font_lookup_bitmap(txt->fctx, txt->family, txt->size, code);
			if(sbit)
			{
				if((txt->wrap > 0) && (tw + sbit->xadvance > txt->wrap))
				{
					tw = 0;
					th += txt->size;
					lh = 0;
					if(tw > w)
						w = tw;
					if(th > h)
						h = th;
					col = 0;
					row++;
				}
				tw += sbit->xadvance;
				th += 0;
				if(sbit->yadvance + sbit->height > lh)
					lh = sbit->yadvance + sbit->height;
				if(tw > w)
					w = tw;
				if(th > h)
					h = th;
				if(col == 0)
				{
					if(sbit->left > x)
						x = sbit->left;
				}
				if(row == 0)
				{
					if(sbit->top > y)
						y = sbit->top;
				}
			}
			col++;
			break;
		}
	}
	txt->metrics.ox = x;
	txt->metrics.oy = y;
	txt->metrics.width = w;
	txt->metrics.height = h + lh;
}

void text_init(struct text_t * txt, const char * utf8, struct color_t * c, int wrap, struct font_context_t * fctx, const char * family, int size)
{
	if(txt)
	{
		txt->utf8 = utf8;
		txt->c = c;
		txt->wrap = wrap;
		txt->fctx = fctx;
		txt->family = family;
		txt->size = (size > 0) ? size : 16;
		text_metrics(txt);
	}
}

void text_set_text(struct text_t * txt, const char * utf8)
{
	if(txt)
	{
		txt->utf8 = utf8;
		text_metrics(txt);
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
		text_metrics(txt);
	}
}

void text_set_family(struct text_t * txt, const char * family)
{
	if(txt)
	{
		txt->family = family;
		text_metrics(txt);
	}
}

void text_set_size(struct text_t * txt, int size)
{
	if(txt)
	{
		txt->size = (size > 0) ? size : 16;
		text_metrics(txt);
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
	color = color_get_premult(c);

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
	FTC_SBit sbit;
	FT_BitmapGlyph bitmap;
	FT_Glyph glyph, gly;
	FT_Matrix matrix;
	FT_Vector pen;
	const char * p;
	uint32_t code;
	int tx, ty, tw;

	if((m->a == 1.0) && (m->b == 0.0) && (m->c == 0.0) && (m->d == 1.0))
	{
		tx = txt->metrics.ox;
		ty = txt->metrics.oy;
		tw = 0;
		pen.x = (FT_Pos)(m->tx + tx);
		pen.y = (FT_Pos)(m->ty + ty);

		p = txt->utf8;
		while(*p)
		{
			p = utf8_to_code(p, &code);
			switch(code)
			{
			case '\r':
				tx = txt->metrics.ox;
				ty += 0;
				tw = 0;
				pen.x = (FT_Pos)(m->tx + tx);
				pen.y = (FT_Pos)(m->ty + ty);
				break;

			case '\n':
				tx = txt->metrics.ox;
				ty += txt->size;
				tw = 0;
				pen.x = (FT_Pos)(m->tx + tx);
				pen.y = (FT_Pos)(m->ty + ty);
				break;

			case '\t':
				tx += txt->size * 2;
				ty += 0;
				tw += txt->size * 2;
				pen.x = (FT_Pos)(m->tx + tx);
				pen.y = (FT_Pos)(m->ty + ty);
				break;

			default:
				sbit = (FTC_SBit)font_lookup_bitmap(txt->fctx, txt->family, txt->size, code);
				if(sbit)
				{
					if((txt->wrap > 0) && (tw + sbit->xadvance > txt->wrap))
					{
						tx = txt->metrics.ox;
						ty += txt->size;
						tw = 0;
						pen.x = (FT_Pos)(m->tx + tx);
						pen.y = (FT_Pos)(m->ty + ty);
					}
					tw += sbit->xadvance;
					{
						draw_font_bitmap(s, clip, txt->c, pen.x, pen.y - sbit->top, sbit);
						pen.x += sbit->xadvance;
						pen.y += sbit->yadvance;
					}
				}
				break;
			}
		}
	}
	else
	{
		matrix.xx = (FT_Fixed)(m->a * 65536);
		matrix.xy = -((FT_Fixed)(m->c * 65536));
		matrix.yx = -((FT_Fixed)(m->b * 65536));
		matrix.yy = (FT_Fixed)(m->d * 65536);
		tx = txt->metrics.ox;
		ty = txt->metrics.oy;
		tw = 0;
		pen.x = (FT_Pos)((m->tx + m->a * tx + m->c * ty) * 64);
		pen.y = (FT_Pos)((s->height - (m->ty + m->b * tx + m->d * ty)) * 64);

		p = txt->utf8;
		while(*p)
		{
			p = utf8_to_code(p, &code);
			switch(code)
			{
			case '\r':
				tx = txt->metrics.ox;
				ty += 0;
				tw = 0;
				pen.x = (FT_Pos)((m->tx + m->a * tx + m->c * ty) * 64);
				pen.y = (FT_Pos)((s->height - (m->ty + m->b * tx + m->d * ty)) * 64);
				break;

			case '\n':
				tx = txt->metrics.ox;
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
				glyph = (FT_Glyph)font_lookup_glyph(txt->fctx, txt->family, txt->size, code);
				if(glyph)
				{
					if((txt->wrap > 0) && (tw + (glyph->advance.x >> 16) > txt->wrap))
					{
						tx = txt->metrics.ox;
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
						draw_font_glyph(s, clip, txt->c, bitmap->left, s->height - bitmap->top, &bitmap->bitmap);
						pen.x += bitmap->root.advance.x >> 10;
						pen.y += bitmap->root.advance.y >> 10;
						FT_Done_Glyph(gly);
					}
				}
				break;
			}
		}
	}
}
