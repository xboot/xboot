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
#include <graphic/text.h>
#include <ft2build.h>
#include FT_FREETYPE_H

static void calc_text_extent(struct text_t * txt)
{
	const char * p;
	u32_t code;
	int glyph;
	FT_Face face;
	int x = 0, y = 0, w = 0, h = 0;
	int flag = 0;

	for(p = txt->utf8; utf8_to_ucs4(&code, 1, p, -1, &p) > 0;)
	{
		glyph = search_glyph(txt->fctx, txt->family, code, (void **)(&face));
		if(glyph == 0)
			glyph = search_glyph(txt->fctx, "roboto", 0xfffd, (void **)(&face));
		FT_Set_Pixel_Sizes(face, txt->size, txt->size);
		FT_Load_Glyph(face, glyph, FT_LOAD_BITMAP_METRICS_ONLY);
		w += face->glyph->metrics.horiAdvance;
		if(face->glyph->metrics.vertAdvance > h)
			h = face->glyph->metrics.vertAdvance;
		if(!flag)
		{
			x = face->glyph->metrics.horiBearingX;
			flag = 1;
		}
		if(face->glyph->metrics.horiBearingY > y)
			y = face->glyph->metrics.horiBearingY;
	}
	region_init(&txt->e, (x >> 6) + 4, (y >> 6) + 4, (w >> 6) + 8, (h >> 6) + 8);
}

struct text_t * text_alloc(const char * utf8, struct color_t * c, struct font_context_t * fctx, const char * family, int size)
{
	struct text_t * txt;

	if(!fctx)
		return NULL;

	txt = malloc(sizeof(struct text_t));
	if(!txt)
		return NULL;

	txt->utf8 = strdup(utf8 ? utf8 : "");
	if(c)
		memcpy(&txt->c, c, sizeof(struct color_t));
	else
		color_init(&txt->c, 0xff, 0xff, 0xff, 0xff);
	txt->fctx = fctx;
	txt->family = strdup(family ? family : "roboto");
	txt->size = (size > 0) ? size : 24;
	calc_text_extent(txt);

	return txt;
}

void text_free(struct text_t * txt)
{
	if(txt)
	{
		if(txt->utf8)
			free(txt->utf8);
		if(txt->family)
			free(txt->family);
		free(txt);
	}
}

void text_set_text(struct text_t * txt, const char * utf8)
{
	if(txt)
	{
		if(txt->utf8)
			free(txt->utf8);
		txt->utf8 = strdup(utf8 ? utf8 : "");
		calc_text_extent(txt);
	}
}

void text_set_color(struct text_t * txt, struct color_t * c)
{
	if(txt)
	{
		if(c)
			memcpy(&txt->c, c, sizeof(struct color_t));
		else
			color_init(&txt->c, 0xff, 0xff, 0xff, 0xff);
	}
}

void text_set_font_family(struct text_t * txt, const char * family)
{
	if(txt)
	{
		if(txt->family)
			free(txt->family);
		txt->utf8 = strdup(family ? family : "roboto");
		calc_text_extent(txt);
	}
}

void text_set_font_size(struct text_t * txt, int size)
{
	if(txt)
	{
		txt->size = (size > 0) ? size : 24;
		calc_text_extent(txt);
	}
}

static inline void draw_font_bitmap(struct surface_t * s, struct region_t * clip, struct color_t * c, int x, int y, FT_Bitmap * bitmap)
{
	struct region_t r, region;
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

	for(j = 0; j < dh; j++)
	{
		for(i = 0; i < dw; i++)
		{
			gray = *sp;
			if(gray != 0)
			{
				if(gray == 255)
				{
					*dp = (c->a << 24) | (c->r << 16) | (c->g << 8) | (c->b << 0);
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
	const char * p;
	u32_t code;
	int glyph;
	FT_Face face;
	FT_Matrix matrix;
	FT_Vector pen;
	int tx = txt->e.x;
	int ty = txt->e.y;

	matrix.xx = (FT_Fixed)(m->a * 65536.0);
	matrix.xy = -((FT_Fixed)(m->c * 65536.0));
	matrix.yx = -((FT_Fixed)(m->b * 65536.0));
	matrix.yy = (FT_Fixed)(m->d * 65536.0);
	pen.x = (FT_Pos)((m->tx + m->a * tx + m->c * ty) * 64);
	pen.y = (FT_Pos)((s->height - (m->ty + m->b * tx + m->d * ty)) * 64);

	for(p = txt->utf8; utf8_to_ucs4(&code, 1, p, -1, &p) > 0;)
	{
		glyph = search_glyph(txt->fctx, txt->family, code, (void **)(&face));
		if(glyph == 0)
			glyph = search_glyph(txt->fctx, "roboto", 0xfffd, (void **)(&face));
		FT_Set_Pixel_Sizes(face, txt->size, txt->size);
		FT_Set_Transform(face, &matrix, &pen);
		FT_Load_Glyph(face, glyph, FT_LOAD_RENDER);
		draw_font_bitmap(s, clip, &txt->c, face->glyph->bitmap_left, s->height - face->glyph->bitmap_top, &face->glyph->bitmap);
		pen.x += face->glyph->advance.x;
		pen.y += face->glyph->advance.y;
	}
}
