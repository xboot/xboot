/*
 * framework/display/l-font.c
 *
 * Copyright(c) 2007-2018 Jianjun Jiang <8192542@qq.com>
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

#include <cairo.h>
#include <cairoint.h>
#include <cairo-ft.h>
#include <xfs/xfs.h>
#include <framework/display/l-display.h>

struct lfont_t {
	FT_Library library;
	FT_Face fface;
	cairo_font_face_t * face;
	cairo_scaled_font_t * sfont;
};

cairo_scaled_font_t * luaL_checkudata_scaled_font(lua_State * L, int ud, const char * tname)
{
	struct lfont_t * font = luaL_checkudata(L, ud, tname);
	return font->sfont;
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

static FT_Stream FT_New_Xfs_Stream(lua_State * L, const char * pathname)
{
	struct xfs_context_t * ctx = ((struct vmctx_t *)luahelper_vmctx(L))->xfs;
	FT_Stream stream = NULL;
	struct xfs_file_t * file;

	stream = malloc(sizeof(*stream));
	if(!stream)
		return NULL;

	file = xfs_open_read(ctx, pathname);
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

static FT_Error FT_New_Xfs_Face(lua_State * L, FT_Library library, const char * pathname, FT_Long face_index, FT_Face * aface)
{
	FT_Open_Args args;

	if(!pathname)
		return -1;

	args.flags = FT_OPEN_STREAM;
	args.pathname = (char *)pathname;
	args.stream = FT_New_Xfs_Stream(L, pathname);

	return FT_Open_Face(library, &args, face_index, aface);
}

static int l_font_new(lua_State * L)
{
	const char * family = luaL_checkstring(L, 1);
	struct lfont_t * font = lua_newuserdata(L, sizeof(struct lfont_t));
	if(FT_Init_FreeType(&font->library))
		return 0;
	if(FT_New_Xfs_Face(L, font->library, family, 0, &font->fface))
	{
		FT_Done_FreeType(font->library);
		return 0;
	}
	font->face = cairo_ft_font_face_create_for_ft_face(font->fface, 0);
	if(font->face->status != CAIRO_STATUS_SUCCESS)
	{
		FT_Done_Face(font->fface);
		FT_Done_FreeType(font->library);
		cairo_font_face_destroy(font->face);
		return 0;
	}
	cairo_font_options_t * options = cairo_font_options_create();
	cairo_matrix_t identity;
	cairo_matrix_init_identity(&identity);
	font->sfont = cairo_scaled_font_create(font->face, &identity, &identity, options);
	cairo_font_options_destroy(options);
	if(cairo_scaled_font_status(font->sfont) != CAIRO_STATUS_SUCCESS)
	{
		FT_Done_Face(font->fface);
		FT_Done_FreeType(font->library);
		cairo_font_face_destroy(font->face);
		cairo_scaled_font_destroy(font->sfont);
		return 0;
	}
	luaL_setmetatable(L, MT_FONT);
	return 1;
}

static const luaL_Reg l_font[] = {
	{"new",	l_font_new},
	{NULL,	NULL}
};

static int m_font_gc(lua_State * L)
{
	struct lfont_t * font = luaL_checkudata(L, 1, MT_FONT);
	FT_Done_Face(font->fface);
	FT_Done_FreeType(font->library);
	cairo_font_face_destroy(font->face);
	cairo_scaled_font_destroy(font->sfont);
	return 0;
}

static int m_font_size(lua_State * L)
{
	struct lfont_t * font = luaL_checkudata(L, 1, MT_FONT);
	const char * text = luaL_optstring(L, 2, NULL);
	cairo_text_extents_t extents;
	cairo_scaled_font_text_extents(font->sfont, text, &extents);
	lua_pushnumber(L, extents.width);
	lua_pushnumber(L, extents.height + extents.y_bearing);
	return 2;
}

static const luaL_Reg m_font[] = {
	{"__gc",		m_font_gc},
	{"size",		m_font_size},
	{NULL,			NULL}
};

int luaopen_font(lua_State * L)
{
	luaL_newlib(L, l_font);
	luahelper_create_metatable(L, MT_FONT, m_font);
	return 1;
}
