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
#include FT_CACHE_MANAGER_H

struct font_description_t {
	const char * family;
	const char * path;
};

static struct font_description_t fdesc[] = {
	{"roboto",				"/framework/assets/fonts/Roboto-Regular.ttf"},
	{"roboto-italic",		"/framework/assets/fonts/Roboto-Italic.ttf"},
	{"roboto-bold",			"/framework/assets/fonts/Roboto-Bold.ttf"},
	{"roboto-bold-italic",	"/framework/assets/fonts/Roboto-BoldItalic.ttf"},
	{"font-awesome",		"/framework/assets/fonts/FontAwesome.ttf"},
};

static FT_Error ftcface_requester(FTC_FaceID id, FT_Library lib, FT_Pointer data, FT_Face * face)
{
	struct font_context_t * ctx = (struct font_context_t *)data;
	uint32_t faceid = (uint32_t)(unsigned long)id;
	char key[32];
	int i;

	sprintf(key, "%d", faceid);
	*face = hmap_search(ctx->map, key);
	if(*face)
		return 0;
	for(i = 0; i < ARRAY_SIZE(fdesc); i++)
	{
		if(shash(fdesc[i].family) == faceid)
		{
			if(FT_New_Face((FT_Library)ctx->library, fdesc[i].path, 0, face) == 0)
			{
				FT_Select_Charmap(*face, FT_ENCODING_UNICODE);
				hmap_add(ctx->map, key, *face);
				return 0;
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
	FTC_SBitCache_New((FTC_Manager)ctx->manager, (FTC_SBitCache *)&ctx->sbit);
	ctx->map = hmap_alloc(0);

	return ctx;
}

void font_context_free(struct font_context_t * ctx)
{
	if(ctx)
	{
		hmap_free(ctx->map);
		FTC_Manager_Done((FTC_Manager)ctx->manager);
		FT_Done_FreeType((FT_Library)ctx->library);
	}
}

void font_install(struct font_context_t * ctx, const char * family, const char * path)
{
	uint32_t faceid = (uint32_t)((unsigned long)shash(family));
	char key[32];
	FT_Face face;

	sprintf(key, "%d", faceid);
	if(ctx && path && !hmap_search(ctx->map, key))
	{
		if(FT_New_Face((FT_Library)ctx->library, path, 0, &face) == 0)
		{
			FT_Select_Charmap(face, FT_ENCODING_UNICODE);
			hmap_add(ctx->map, key, face);
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
	uint32_t faceid = (uint32_t)((unsigned long)shash(family));
	char key[32];
	FT_Face face;

	sprintf(key, "%d", faceid);
	if(ctx && xfs && path && !hmap_search(ctx->map, key))
	{
		if(ft_new_xfs_face(xfs, (FT_Library)ctx->library, path, 0, &face) == 0)
		{
			FT_Select_Charmap(face, FT_ENCODING_UNICODE);
			hmap_add(ctx->map, key, face);
		}
	}
}

void font_bitmap_lookup(struct font_context_t * ctx, const char * family, int size, uint32_t code, struct font_bitmap_t * bitmap)
{
	FTC_ImageTypeRec type;
	FTC_SBit sbit;
	FT_UInt glyph;
	char buf[512];
	char * r, * p;
	int i;

	type.face_id = (FTC_FaceID)(0);
	type.width = size;
	type.height = size;
	type.flags = FT_LOAD_DEFAULT | FT_LOAD_RENDER | FT_LOAD_FORCE_AUTOHINT;
	strlcpy(buf, family ? family : "roboto", sizeof(buf));
	p = buf;
	while((r = strsep(&p, ",;:|")) != NULL)
	{
		type.face_id = (FTC_FaceID)((unsigned long)shash(r));
		if((glyph = FTC_CMapCache_Lookup((FTC_CMapCache)ctx->cmap, type.face_id, -1, code)) != 0)
		{
			if(FTC_SBitCache_Lookup((FTC_SBitCache)ctx->sbit, (FTC_ImageType)&type, glyph, &sbit, NULL) == 0)
			{
				bitmap->width = sbit->width;
				bitmap->height = sbit->height;
				bitmap->left = sbit->left;
				bitmap->top = sbit->top;
				bitmap->pitch = sbit->pitch;
				bitmap->xadvance = sbit->xadvance;
				bitmap->yadvance = sbit->yadvance;
				bitmap->buffer = sbit->buffer;
				return;
			}
		}
	}
	for(i = 0; i < ARRAY_SIZE(fdesc); i++)
	{
		type.face_id = (FTC_FaceID)((unsigned long)shash(fdesc[i].family));
		if((glyph = FTC_CMapCache_Lookup((FTC_CMapCache)ctx->cmap, type.face_id, -1, code)) != 0)
		{
			if(FTC_SBitCache_Lookup((FTC_SBitCache)ctx->sbit, (FTC_ImageType)&type, glyph, &sbit, NULL) == 0)
			{
				bitmap->width = sbit->width;
				bitmap->height = sbit->height;
				bitmap->left = sbit->left;
				bitmap->top = sbit->top;
				bitmap->pitch = sbit->pitch;
				bitmap->xadvance = sbit->xadvance;
				bitmap->yadvance = sbit->yadvance;
				bitmap->buffer = sbit->buffer;
				return;
			}
		}
	}
	type.face_id = (FTC_FaceID)((unsigned long)shash("roboto"));
	if((glyph = FTC_CMapCache_Lookup((FTC_CMapCache)ctx->cmap, type.face_id, -1, 0xfffd)) != 0)
	{
		if(FTC_SBitCache_Lookup((FTC_SBitCache)ctx->sbit, (FTC_ImageType)&type, glyph, &sbit, NULL) == 0)
		{
			bitmap->width = sbit->width;
			bitmap->height = sbit->height;
			bitmap->left = sbit->left;
			bitmap->top = sbit->top;
			bitmap->pitch = sbit->pitch;
			bitmap->xadvance = sbit->xadvance;
			bitmap->yadvance = sbit->yadvance;
			bitmap->buffer = sbit->buffer;
			return;
		}
	}
	bitmap->width = 0;
	bitmap->height = 0;
	bitmap->left = 0;
	bitmap->top = 0;
	bitmap->pitch = 0;
	bitmap->xadvance = 0;
	bitmap->yadvance = 0;
	bitmap->buffer = NULL;
}
