/*
 * kernel/graphic/font.c
 *
 * Copyright(c) 2007-2019 Jianjun Jiang <8192542@qq.com>
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

#include <xboot.h>
#include <string.h>
#include <graphic/surface.h>
#include <graphic/font.h>
#include <ft2build.h>
#include FT_FREETYPE_H

struct font_description_t {
	const char * family;
	const char * path;
};

static struct font_description_t fdesc[] = {
	{"roboto",				"/framework/assets/fonts/Roboto-Regular.ttf"},
	{"roboto-italic",		"/framework/assets/fonts/Roboto-Italic.ttf"},
	{"roboto-bold",			"/framework/assets/fonts/Roboto-Bold.ttf"},
	{"roboto-bold-italic",	"/framework/assets/fonts/Roboto-BoldItalic.ttf"},
	{"droid-sans",			"/framework/assets/fonts/DroidSansFallback.ttf"},
};

struct font_context_t * font_context_alloc(void)
{
	struct font_context_t * ctx;

	ctx = malloc(sizeof(struct font_context_t));
	if(!ctx)
		return NULL;
	FT_Init_FreeType((FT_Library *)&ctx->library);
	ctx->map = hmap_alloc(0);
	return ctx;
}

static void face_done_callback(const char * key, void * value)
{
	if(value)
		FT_Done_Face((FT_Face)value);
}

void font_context_free(struct font_context_t * ctx)
{
	if(ctx)
	{
		hmap_walk(ctx->map, face_done_callback);
		hmap_free(ctx->map);
		FT_Done_FreeType((FT_Library)ctx->library);
	}
}

void font_install(struct font_context_t * ctx, const char * family, const char * path)
{
	FT_Face face;

	if(ctx && family && path && !hmap_search(ctx->map, family))
	{
		if(FT_New_Face((FT_Library)ctx->library, path, 0, &face) == 0)
		{
			FT_Select_Charmap(face, FT_ENCODING_UNICODE);
			hmap_add(ctx->map, family, face);
		}
	}
}

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

void font_install_from_xfs(struct font_context_t * ctx, struct xfs_context_t * xfs, const char * family, const char * path)
{
	FT_Face face;

	if(ctx && xfs && family && path && !hmap_search(ctx->map, family))
	{
		if(ft_new_xfs_face(xfs, (FT_Library)ctx->library, path, 0, &face) == 0)
		{
			FT_Select_Charmap(face, FT_ENCODING_UNICODE);
			hmap_add(ctx->map, family, face);
		}
	}
}

void font_uninstall(struct font_context_t * ctx, const char * family)
{
	FT_Face face;

	if(ctx && family)
	{
		face = hmap_search(ctx->map, family);
		if(face)
			FT_Done_Face(face);
	}
}

void * search_face(struct font_context_t * ctx, const char * family)
{
	FT_Face face;
	int i;

	face = hmap_search(ctx->map, family);
	if(face)
		return face;
	for(i = 0; i < ARRAY_SIZE(fdesc); i++)
	{
		if(strcmp(family, fdesc[i].family) == 0)
		{
			if(FT_New_Face((FT_Library)ctx->library, fdesc[i].path, 0, &face) == 0)
			{
				FT_Select_Charmap(face, FT_ENCODING_UNICODE);
				hmap_add(ctx->map, family, face);
				return (void *)face;
			}
		}
	}
	return NULL;
}

int search_glyph(struct font_context_t * ctx, const char * family, u32_t code, void ** face)
{
	char buffer[SZ_512];
	char * r, * p;
	int glyph;
	int i;

	strlcpy(buffer, family, sizeof(buffer));
	p = buffer;
	while((r = strsep(&p, ",;:|")) != NULL)
	{
		*face = search_face(ctx, r);
		if((glyph = FT_Get_Char_Index((FT_Face)(*face), code)) != 0)
			return glyph;
	}
	for(i = 0; i < ARRAY_SIZE(fdesc); i++)
	{
		*face = search_face(ctx, fdesc[i].family);
		if((glyph = FT_Get_Char_Index((FT_Face)(*face), code)) != 0)
			return glyph;
	}
	return 0;
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
					*dp = ta << 24 | tr << 16 | tg << 8 | tb << 0;
				}
			}
			sp++;
			dp++;
		}
		dp += dskip;
		sp += sskip;
	}
}

void render_default_text_output(struct surface_t * s, struct region_t * clip, struct matrix_t * m, const char * utf8, struct color_t * c, struct font_context_t * fctx, const char * family, int size)
{
	const char * p;
	u32_t code;
	int glyph;
	FT_Face face;
	FT_Matrix matrix;
	FT_Vector pen;

	matrix.xx = (FT_Fixed)(m->a * 0x10000);
	matrix.xy = (FT_Fixed)(m->c * 0x10000);
	matrix.yx = (FT_Fixed)(m->b * 0x10000);
	matrix.yy = (FT_Fixed)(m->d * 0x10000);
	pen.x = (FT_Pos)(m->tx * 64);
	pen.y = (FT_Pos)((s->height - m->ty) * 64);

	for(p = utf8; utf8_to_ucs4(&code, 1, p, -1, &p) > 0;)
	{
		glyph = search_glyph(fctx, family, code, (void **)(&face));
		if(glyph == 0)
			glyph = search_glyph(fctx, "roboto", 0xfffd, (void **)(&face));
		FT_Set_Pixel_Sizes(face, 0, size);
		FT_Set_Transform(face, &matrix, &pen);
		FT_Load_Glyph(face, glyph, FT_LOAD_RENDER);
		draw_font_bitmap(s, clip, c, 100 + face->glyph->bitmap_left, s->height - face->glyph->bitmap_top, &face->glyph->bitmap);
		pen.x += face->glyph->advance.x;
		pen.y += face->glyph->advance.y;
	}
}

void render_default_text_extent(struct surface_t * s, const char * utf8, struct font_context_t * fctx, const char * family, struct region_t * e)
{
}
