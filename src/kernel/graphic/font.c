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

static void * search_face(struct font_context_t * ctx, const char * family)
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

	strlcpy(buffer, family ? family : "", sizeof(buffer));
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
