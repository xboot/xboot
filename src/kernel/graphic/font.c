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
#include <graphic/font.h>
#include <vfs/vfs.h>
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

static inline int family_hash(const char ** s, uint32_t * v)
{
	char c;

	if(s && *s && **s)
	{
		*v = 5381;
		while((c = **s))
		{
			(*s)++;
			switch(c)
			{
			case ',':
			case ';':
			case ':':
			case '|':
			case '\0':
				return 1;
			default:
				*v = (*v << 5) + *v + c;
				break;
			}
		}
		return 1;
	}
	return 0;
}

static FT_Error ftcface_requester(FTC_FaceID id, FT_Library lib, FT_Pointer data, FT_Face * face)
{
	struct font_context_t * ctx = (struct font_context_t *)data;
	struct font_t * pos, * n;
	const char * p;
	uint32_t v;

	list_for_each_entry_safe(pos, n, &ctx->list, list)
	{
		p = pos->family;
		if(family_hash(&p, &v) && (v == (uint32_t)(unsigned long)id))
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
	FTC_SBitCache_New((FTC_Manager)ctx->manager, (FTC_SBitCache *)&ctx->sbit);
	FTC_ImageCache_New((FTC_Manager)ctx->manager, (FTC_ImageCache *)&ctx->image);
	init_list_head(&ctx->list);

	font_add(ctx, NULL, "roboto-thin",			"/framework/assets/fonts/Roboto-Thin.ttf");
	font_add(ctx, NULL, "roboto-Thin-italic",	"/framework/assets/fonts/Roboto-ThinItalic.ttf");
	font_add(ctx, NULL, "roboto-light",			"/framework/assets/fonts/Roboto-Light.ttf");
	font_add(ctx, NULL, "roboto-light-italic",	"/framework/assets/fonts/Roboto-LightItalic.ttf");
	font_add(ctx, NULL, "roboto-regular",		"/framework/assets/fonts/Roboto-Regular.ttf");
	font_add(ctx, NULL, "roboto-italic",		"/framework/assets/fonts/Roboto-Italic.ttf");
	font_add(ctx, NULL, "roboto-medium",		"/framework/assets/fonts/Roboto-Medium.ttf");
	font_add(ctx, NULL, "roboto-medium-italic",	"/framework/assets/fonts/Roboto-MediumItalic.ttf");
	font_add(ctx, NULL, "roboto-bold",			"/framework/assets/fonts/Roboto-Bold.ttf");
	font_add(ctx, NULL, "roboto-bold-italic",	"/framework/assets/fonts/Roboto-BoldItalic.ttf");
	font_add(ctx, NULL, "roboto-black",			"/framework/assets/fonts/Roboto-Black.ttf");
	font_add(ctx, NULL, "roboto-black-italic",	"/framework/assets/fonts/Roboto-BlackItalic.ttf");

	font_add(ctx, NULL, "fa-solid",				"/framework/assets/fonts/fa-solid-900.ttf");

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
		free(ctx);
	}
}

void * font_lookup_bitmap(struct font_context_t * ctx, const char * family, int size, uint32_t code)
{
	struct font_t * pos, * n;
	FTC_ScalerRec scaler;
	FTC_SBit sbit;
	FT_UInt index;
	const char * p;
	uint32_t v;

	scaler.width = size;
	scaler.height = size;
	scaler.pixel = 1;
	scaler.x_res = 0;
	scaler.y_res = 0;

	p = family ? family : "roboto-regular";
	while(family_hash(&p, &v))
	{
		scaler.face_id = (FTC_FaceID)((unsigned long)v);
		if((index = FTC_CMapCache_Lookup((FTC_CMapCache)ctx->cmap, scaler.face_id, -1, code)) != 0)
		{
			if(FTC_SBitCache_LookupScaler((FTC_SBitCache)ctx->sbit, &scaler, FT_LOAD_RENDER, index, &sbit, NULL) == 0)
				return (void *)sbit;
		}
	}
	list_for_each_entry_safe(pos, n, &ctx->list, list)
	{
		p = pos->family;
		if(family_hash(&p, &v))
		{
			scaler.face_id = (FTC_FaceID)((unsigned long)v);
			if((index = FTC_CMapCache_Lookup((FTC_CMapCache)ctx->cmap, scaler.face_id, -1, code)) != 0)
			{
				if(FTC_SBitCache_LookupScaler((FTC_SBitCache)ctx->sbit, &scaler, FT_LOAD_RENDER, index, &sbit, NULL) == 0)
					return (void *)sbit;
			}
		}
	}
	p = "roboto-regular";
	if(family_hash(&p, &v))
	{
		scaler.face_id = (FTC_FaceID)((unsigned long)v);
		if((index = FTC_CMapCache_Lookup((FTC_CMapCache)ctx->cmap, scaler.face_id, -1, 0xfffd)) != 0)
		{
			if(FTC_SBitCache_LookupScaler((FTC_SBitCache)ctx->sbit, &scaler, FT_LOAD_RENDER, index, &sbit, NULL) == 0)
				return (void *)sbit;
		}
	}
	return NULL;
}

void * font_lookup_glyph(struct font_context_t * ctx, const char * family, int size, uint32_t code)
{
	struct font_t * pos, * n;
	FTC_ScalerRec scaler;
	FT_Glyph glyph;
	FT_UInt index;
	const char * p;
	uint32_t v;

	scaler.width = size;
	scaler.height = size;
	scaler.pixel = 1;
	scaler.x_res = 0;
	scaler.y_res = 0;

	p = family ? family : "roboto-regular";
	while(family_hash(&p, &v))
	{
		scaler.face_id = (FTC_FaceID)((unsigned long)v);
		if((index = FTC_CMapCache_Lookup((FTC_CMapCache)ctx->cmap, scaler.face_id, -1, code)) != 0)
		{
			if(FTC_ImageCache_LookupScaler((FTC_ImageCache)ctx->image, &scaler, FT_LOAD_DEFAULT, index, &glyph, NULL) == 0)
				return (void *)glyph;
		}
	}
	list_for_each_entry_safe(pos, n, &ctx->list, list)
	{
		p = pos->family;
		if(family_hash(&p, &v))
		{
			scaler.face_id = (FTC_FaceID)((unsigned long)v);
			if((index = FTC_CMapCache_Lookup((FTC_CMapCache)ctx->cmap, scaler.face_id, -1, code)) != 0)
			{
				if(FTC_ImageCache_LookupScaler((FTC_ImageCache)ctx->image, &scaler, FT_LOAD_DEFAULT, index, &glyph, NULL) == 0)
					return (void *)glyph;
			}
		}
	}
	p = "roboto-regular";
	if(family_hash(&p, &v))
	{
		scaler.face_id = (FTC_FaceID)((unsigned long)v);
		if((index = FTC_CMapCache_Lookup((FTC_CMapCache)ctx->cmap, scaler.face_id, -1, 0xfffd)) != 0)
		{
			if(FTC_ImageCache_LookupScaler((FTC_ImageCache)ctx->image, &scaler, FT_LOAD_DEFAULT, index, &glyph, NULL) == 0)
				return (void *)glyph;
		}
	}
	return NULL;
}

void font_add(struct font_context_t * ctx, struct xfs_context_t * xfs, const char * family, const char * path)
{
	struct vfs_stat_t st;
	struct font_t * pos, * n;
	struct font_t * f;

	if(ctx && family && path)
	{
		if(xfs)
		{
			if(!xfs_isfile(xfs, path))
				return;
		}
		else
		{
			if((vfs_stat(path, &st) < 0) || !S_ISREG(st.st_mode))
				return;
		}
		list_for_each_entry_safe(pos, n, &ctx->list, list)
		{
			if(strcmp(pos->family, family) == 0)
				return;
		}
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
