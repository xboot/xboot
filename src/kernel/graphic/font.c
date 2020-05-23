/*
 * kernel/graphic/font.c
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
#include <graphic/font.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_CACHE_MANAGER_H

struct font_t {
	struct list_head list;
	struct xfs_context_t * xfs;
	char * family;
	char * path;
};

static unsigned long ft_xfs_stream_io(FT_Stream stream, unsigned long offset, unsigned char * buffer, unsigned long count)
{
	struct xfs_file_t * file = ((struct xfs_file_t *)stream->descriptor.pointer);

	if(!count && offset > stream->size)
		return 1;
	if(stream->pos != offset)
		xfs_seek(file, offset);
	return (unsigned long)xfs_read(file, buffer, count);
}

static void ft_xfs_stream_close(FT_Stream stream)
{
	struct xfs_file_t * file = ((struct xfs_file_t *)stream->descriptor.pointer);

	xfs_close(file);
	stream->descriptor.pointer = NULL;
	stream->size = 0;
	stream->base = 0;
	free(stream);
}

static FT_Stream ft_new_xfs_stream(struct xfs_context_t * xfs, const char * pathname)
{
	FT_Stream stream = NULL;
	struct xfs_file_t * file;

	stream = malloc(sizeof(*stream));
	if(!stream)
		return NULL;

	file = xfs_open_read(xfs, pathname);
	if(!file)
	{
		free(stream);
		return NULL;
	}

	stream->size = xfs_length(file);
	if(!stream->size)
	{
		xfs_close(file);
		free(stream);
		return NULL;
	}
	xfs_seek(file, 0);

	stream->descriptor.pointer = file;
	stream->pathname.pointer = (char *)pathname;
	stream->read = ft_xfs_stream_io;
	stream->close = ft_xfs_stream_close;

    return stream;
}

static FT_Error ft_new_xfs_face(struct xfs_context_t * xfs, FT_Library library, const char * pathname, FT_Long index, FT_Face * face)
{
	FT_Open_Args args;

	if(!pathname)
		return -1;
	args.flags = FT_OPEN_STREAM;
	args.pathname = (char *)pathname;
	args.stream = ft_new_xfs_stream(xfs, pathname);
	return FT_Open_Face(library, &args, index, face);
}

static FT_Error ftcface_requester(FTC_FaceID id, FT_Library lib, FT_Pointer data, FT_Face * face)
{
	struct font_context_t * ctx = (struct font_context_t *)data;
	struct font_t * pos, * n;

	list_for_each_entry_safe(pos, n, &ctx->list, list)
	{
		if(shash(pos->family) == (uint32_t)(unsigned long)id)
		{
			if(pos->xfs)
			{
				if(ft_new_xfs_face(pos->xfs, (FT_Library)ctx->library, pos->path, 0, face) == 0)
				{
					FT_Select_Charmap(*face, FT_ENCODING_UNICODE);
					return 0;
				}
			}
			else
			{
				if(FT_New_Face((FT_Library)ctx->library, pos->path, 0, face) == 0)
				{
					FT_Select_Charmap(*face, FT_ENCODING_UNICODE);
					return 0;
				}
			}
		}
	}
	return -1;
}

struct font_context_t * font_context_alloc(void)
{
	struct font_context_t * ctx;

	ctx = malloc(sizeof(struct font_context_t));
	if(!ctx)
		return NULL;
	FT_Init_FreeType((FT_Library *)&ctx->library);
	FTC_Manager_New((FT_Library)ctx->library, 0, 0, 0, ftcface_requester, ctx, (FTC_Manager *)&ctx->manager);
	FTC_CMapCache_New((FTC_Manager)ctx->manager, (FTC_CMapCache *)&ctx->cmap);
	FTC_ImageCache_New((FTC_Manager)ctx->manager, (FTC_ImageCache *)&ctx->image);
	init_list_head(&ctx->list);
	font_install(ctx, NULL, "roboto", "/framework/assets/fonts/Roboto-Regular.ttf");
	font_install(ctx, NULL, "roboto-italic", "/framework/assets/fonts/Roboto-Italic.ttf");
	font_install(ctx, NULL, "roboto-bold", "/framework/assets/fonts/Roboto-Bold.ttf");
	font_install(ctx, NULL, "roboto-bold-italic", "/framework/assets/fonts/Roboto-BoldItalic.ttf");
	font_install(ctx, NULL, "font-awesome", "/framework/assets/fonts/FontAwesome.ttf");

	return ctx;
}

void font_context_free(struct font_context_t * ctx)
{
	struct font_t * pos, * n;

	if(ctx)
	{
		list_for_each_entry_safe(pos, n, &ctx->list, list)
		{
			if(pos->family)
				free(pos->family);
			if(pos->path)
				free(pos->path);
			free(pos);
		}
		FTC_Manager_Done((FTC_Manager)ctx->manager);
		FT_Done_FreeType((FT_Library)ctx->library);
	}
}

void font_install(struct font_context_t * ctx, struct xfs_context_t * xfs, const char * family, const char * path)
{
	struct font_t * f;

	if(ctx && family && path)
	{
		f = malloc(sizeof(struct font_t));
		if(f)
		{
			f->xfs = xfs;
			f->family = strdup(family);
			f->path = strdup(path);
			list_add_tail(&f->list, &ctx->list);
		}
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

static inline FT_Glyph font_glyph_lookup(struct font_context_t * ctx, const char * family, int size, uint32_t code)
{
	struct font_t * pos, * n;
	FTC_ScalerRec scaler;
	FT_Glyph glyph;
	FT_UInt index;
	char buf[512];
	char * p, * r;

	scaler.width = size;
	scaler.height = size;
	scaler.pixel = 1;
	scaler.x_res = 0;
	scaler.y_res = 0;
	strlcpy(buf, family ? family : "roboto", sizeof(buf));
	p = buf;
	while((r = strsep(&p, ",;:|")) != NULL)
	{
		scaler.face_id = (FTC_FaceID)((unsigned long)shash(r));
		if((index = FTC_CMapCache_Lookup((FTC_CMapCache)ctx->cmap, scaler.face_id, -1, code)) != 0)
		{
			if(FTC_ImageCache_LookupScaler((FTC_ImageCache)ctx->image, &scaler, FT_LOAD_DEFAULT, index, &glyph, NULL) == 0)
				return glyph;
		}
	}
	list_for_each_entry_safe(pos, n, &ctx->list, list)
	{
		scaler.face_id = (FTC_FaceID)((unsigned long)shash(pos->family));
		if((index = FTC_CMapCache_Lookup((FTC_CMapCache)ctx->cmap, scaler.face_id, -1, code)) != 0)
		{
			if(FTC_ImageCache_LookupScaler((FTC_ImageCache)ctx->image, &scaler, FT_LOAD_DEFAULT, index, &glyph, NULL) == 0)
				return glyph;
		}
	}
	scaler.face_id = (FTC_FaceID)((unsigned long)shash("roboto"));
	if((index = FTC_CMapCache_Lookup((FTC_CMapCache)ctx->cmap, scaler.face_id, -1, 0xfffd)) != 0)
	{
		if(FTC_ImageCache_LookupScaler((FTC_ImageCache)ctx->image, &scaler, FT_LOAD_DEFAULT, index, &glyph, NULL) == 0)
			return glyph;
	}
	return NULL;
}

void calc_text_extent(struct text_t * txt)
{
	FT_BitmapGlyph bitmap;
	FT_Glyph glyph, gly;
	const char * p;
	uint32_t code;
	int x = 0, y = 0, w = 0, h = 0;
	int flag = 0;

	for(p = txt->utf8; utf8_to_ucs4(&code, 1, p, -1, &p) > 0;)
	{
		glyph = font_glyph_lookup(txt->fctx, txt->family, txt->size, code);
		if(glyph && (FT_Glyph_Copy(glyph, &gly) == 0))
		{
			FT_Glyph_To_Bitmap(&gly, FT_RENDER_MODE_NORMAL, NULL, 1);
			bitmap = (FT_BitmapGlyph)gly;
			w += (bitmap->root.advance.x >> 16);
			if(bitmap->bitmap.rows + (bitmap->root.advance.y >> 16) > h)
				h = bitmap->bitmap.rows + (bitmap->root.advance.y >> 16);
			if(!flag)
			{
				x = bitmap->left;
				flag = 1;
			}
			if(bitmap->top > y)
				y = bitmap->top;
			FT_Done_Glyph(gly);
		}
	}
	region_init(&txt->e, x, y, w, h);
}

void render_default_text(struct surface_t * s, struct region_t * clip, struct matrix_t * m, struct text_t * txt)
{
	FT_BitmapGlyph bitmap;
	FT_Glyph glyph, gly;
	FT_Matrix matrix;
	FT_Vector pen;
	const char * p;
	uint32_t code;
	int tx = txt->e.x;
	int ty = txt->e.y;

	matrix.xx = (FT_Fixed)(m->a * 65536);
	matrix.xy = -((FT_Fixed)(m->c * 65536));
	matrix.yx = -((FT_Fixed)(m->b * 65536));
	matrix.yy = (FT_Fixed)(m->d * 65536);
	pen.x = (FT_Pos)((m->tx + m->a * tx + m->c * ty) * 64);
	pen.y = (FT_Pos)((s->height - (m->ty + m->b * tx + m->d * ty)) * 64);

	for(p = txt->utf8; utf8_to_ucs4(&code, 1, p, -1, &p) > 0;)
	{
		glyph = font_glyph_lookup(txt->fctx, txt->family, txt->size, code);
		if(glyph && (FT_Glyph_Copy(glyph, &gly) == 0))
		{
			FT_Glyph_Transform(gly, &matrix, &pen);
			FT_Glyph_To_Bitmap(&gly, FT_RENDER_MODE_NORMAL, NULL, 1);
			bitmap = (FT_BitmapGlyph)gly;
			draw_font_bitmap(s, clip, &txt->c, bitmap->left, s->height - bitmap->top, &bitmap->bitmap);
			pen.x += bitmap->root.advance.x >> 10;
			pen.y += bitmap->root.advance.y >> 10;
			FT_Done_Glyph(gly);
		}
	}
}
