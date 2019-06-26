/*
 * framework/core/l-ninepatch.c
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
#include <framework/core/l-ninepatch.h>

static inline int detect_black_pixel(unsigned char * p)
{
	return (((p[0] == 0) && (p[1] == 0) && (p[2] == 0) && (p[3] != 0)) ? 1 : 0);
}

void ninepatch_stretch(struct lninepatch_t * ninepatch, double width, double height)
{
	int lr = ninepatch->left + ninepatch->right;
	int tb = ninepatch->top + ninepatch->bottom;

	if(width < ninepatch->width)
		width = ninepatch->width;
	if(height < ninepatch->height)
		height = ninepatch->height;
	ninepatch->__w = width;
	ninepatch->__h = height;
	ninepatch->__sx = (ninepatch->__w - lr) / (ninepatch->width - lr);
	ninepatch->__sy = (ninepatch->__h - tb) / (ninepatch->height - tb);
}

static bool_t to_ninepatch(struct surface_t * surface, struct lninepatch_t * patch)
{
	struct surface_t * s;
	unsigned char * data;
	int width, height;
	int stride;
	int w, h;
	int i;

	if(!surface || !patch)
		return FALSE;

	width = surface_get_width(surface);
	height = surface_get_height(surface);
	if(width < 3 || height < 3)
		return FALSE;

	/* Nine patch chunk */
	s = surface_clone(surface);
	data = surface_get_pixels(s);
	stride = surface_get_stride(s);

	/* Nine patch default size */
	width = width - 2;
	height = height - 2;
	patch->width = width;
	patch->height = height;

	/* Stretch information */
	patch->left = 0;
	patch->right = 0;
	patch->top = 0;
	patch->right = 0;

	for(i = 0; i < width; i++)
	{
		if(detect_black_pixel(&data[(i + 1) * 4]))
		{
			patch->left = i;
			break;
		}
	}
	for(i = width - 1; i >= 0; i--)
	{
		if(detect_black_pixel(&data[(i + 1) * 4]))
		{
			patch->right = width - 1 - i;
			break;
		}
	}
	for(i = 0; i < height; i++)
	{
		if(detect_black_pixel(&data[stride * (i + 1)]))
		{
			patch->top = i;
			break;
		}
	}
	for(i = height - 1; i >= 0; i--)
	{
		if(detect_black_pixel(&data[stride * (i + 1)]))
		{
			patch->bottom = height - 1 - i;
			break;
		}
	}
	surface_free(s);

	/* Left top */
	w = patch->left;
	h = patch->top;
	if(w > 0 && h > 0)
	{
		s = surface_alloc(patch->left, patch->top, NULL);
		surface_shape_set_source_surface(s, surface, -1, -1);
		surface_shape_paint(s, 1);
		patch->lt = s;
	}
	else
	{
		patch->lt = NULL;
	}

	/* Middle top */
	w = width - patch->left - patch->right;
	h = patch->top;
	if(w > 0 && h > 0)
	{
		s = surface_alloc(w, h, NULL);
		surface_shape_set_source_surface(s, surface, -patch->left - 1, -1);
		surface_shape_paint(s, 1);
		patch->mt = s;
	}
	else
	{
		patch->mt = NULL;
	}

	/* Right top */
	w = patch->right;
	h = patch->top;
	if(w > 0 && h > 0)
	{
		s = surface_alloc(w, h, NULL);
		surface_shape_set_source_surface(s, surface, -(width - patch->right) - 1, -1);
		surface_shape_paint(s, 1);
		patch->rt = s;
	}
	else
	{
		patch->rt = NULL;
	}

	/* Left Middle */
	w = patch->left;
	h = height - patch->top - patch->bottom;
	if(w > 0 && h > 0)
	{
		s = surface_alloc(w, h, NULL);
		surface_shape_set_source_surface(s, surface, -1, -patch->top - 1);
		surface_shape_paint(s, 1);
		patch->lm = s;
	}
	else
	{
		patch->lm = NULL;
	}

	/* Middle Middle */
	w = width - patch->left - patch->right;
	h = height - patch->top - patch->bottom;
	if(w > 0 && h > 0)
	{
		s = surface_alloc(w, h, NULL);
		surface_shape_set_source_surface(s, surface, -patch->left - 1, -patch->top - 1);
		surface_shape_paint(s, 1);
		patch->mm = s;
	}
	else
	{
		patch->mm = NULL;
	}

	/* Right middle */
	w = patch->right;
	h = height - patch->top - patch->bottom;
	if(w > 0 && h > 0)
	{
		s = surface_alloc(w, h, NULL);
		surface_shape_set_source_surface(s, surface, -(width - patch->right) - 1, -patch->top - 1);
		surface_shape_paint(s, 1);
		patch->rm = s;
	}
	else
	{
		patch->rm = NULL;
	}

	/* Left bottom */
	w = patch->left;
	h = patch->bottom;
	if(w > 0 && h > 0)
	{
		s = surface_alloc(w, h, NULL);
		surface_shape_set_source_surface(s, surface, -1, -(height - patch->bottom) - 1);
		surface_shape_paint(s, 1);
		patch->lb = s;
	}
	else
	{
		patch->lb = NULL;
	}

	/* Middle bottom */
	w = width - patch->left - patch->right;
	h = patch->bottom;
	if(w > 0 && h > 0)
	{
		s = surface_alloc(w, h, NULL);
		surface_shape_set_source_surface(s, surface, -patch->left - 1, -(height - patch->bottom) - 1);
		surface_shape_paint(s, 1);
		patch->mb = s;
	}
	else
	{
		patch->mb = NULL;
	}

	/* Right bottom */
	w = patch->right;
	h = patch->bottom;
	if(w > 0 && h > 0)
	{
		s = surface_alloc(w, h, NULL);
		surface_shape_set_source_surface(s, surface, -(width - patch->right) - 1, -(height - patch->bottom) - 1);
		surface_shape_paint(s, 1);
		patch->rb = s;
	}
	else
	{
		patch->rb = NULL;
	}

	ninepatch_stretch(patch, width, height);
	return TRUE;
}

static int l_ninepatch_new(lua_State * L)
{
	const char * filename = luaL_checkstring(L, 1);
	struct lninepatch_t * ninepatch = lua_newuserdata(L, sizeof(struct lninepatch_t));
	struct surface_t * s = surface_alloc_from_xfs(((struct vmctx_t *)luahelper_vmctx(L))->xfs, filename);
	if(!s)
		return 0;
	bool_t result = to_ninepatch(s, ninepatch);
	surface_free(s);
	if(!result)
		return 0;
	luaL_setmetatable(L, MT_NINEPATCH);
	return 1;
}

static const luaL_Reg l_ninepatch[] = {
	{"new",	l_ninepatch_new},
	{NULL,	NULL}
};

static int m_ninepatch_gc(lua_State * L)
{
	struct lninepatch_t * ninepatch = luaL_checkudata(L, 1, MT_NINEPATCH);
	if(ninepatch->lt)
		surface_free(ninepatch->lt);
	if(ninepatch->mt)
		surface_free(ninepatch->mt);
	if(ninepatch->rt)
		surface_free(ninepatch->rt);
	if(ninepatch->lm)
		surface_free(ninepatch->lm);
	if(ninepatch->mm)
		surface_free(ninepatch->mm);
	if(ninepatch->rm)
		surface_free(ninepatch->rm);
	if(ninepatch->lb)
		surface_free(ninepatch->lb);
	if(ninepatch->mb)
		surface_free(ninepatch->mb);
	if(ninepatch->rb)
		surface_free(ninepatch->rb);
	return 0;
}

static int m_ninepatch_set_width(lua_State * L)
{
	struct lninepatch_t * ninepatch = luaL_checkudata(L, 1, MT_NINEPATCH);
	double w = luaL_checknumber(L, 2);
	ninepatch_stretch(ninepatch, w, ninepatch->__h);
	return 0;
}

static int m_ninepatch_get_width(lua_State * L)
{
	struct lninepatch_t * ninepatch = luaL_checkudata(L, 1, MT_NINEPATCH);
	lua_pushnumber(L, ninepatch->__w);
	return 1;
}

static int m_ninepatch_set_height(lua_State * L)
{
	struct lninepatch_t * ninepatch = luaL_checkudata(L, 1, MT_NINEPATCH);
	double h = luaL_checknumber(L, 2);
	ninepatch_stretch(ninepatch, ninepatch->__w, h);
	return 0;
}

static int m_ninepatch_get_height(lua_State * L)
{
	struct lninepatch_t * ninepatch = luaL_checkudata(L, 1, MT_NINEPATCH);
	lua_pushnumber(L, ninepatch->__h);
	return 1;
}

static int m_ninepatch_set_size(lua_State * L)
{
	struct lninepatch_t * ninepatch = luaL_checkudata(L, 1, MT_NINEPATCH);
	double w = luaL_checknumber(L, 2);
	double h = luaL_checknumber(L, 3);
	ninepatch_stretch(ninepatch, w, h);
	return 0;
}

static int m_ninepatch_get_size(lua_State * L)
{
	struct lninepatch_t * ninepatch = luaL_checkudata(L, 1, MT_NINEPATCH);
	lua_pushnumber(L, ninepatch->__w);
	lua_pushnumber(L, ninepatch->__h);
	return 2;
}

static const luaL_Reg m_ninepatch[] = {
	{"__gc",		m_ninepatch_gc},
	{"setWidth",	m_ninepatch_set_width},
	{"getWidth",	m_ninepatch_get_width},
	{"setHeight",	m_ninepatch_set_height},
	{"getHeight",	m_ninepatch_get_height},
	{"setSize",		m_ninepatch_set_size},
	{"getSize",		m_ninepatch_get_size},
	{NULL,			NULL}
};

int luaopen_ninepatch(lua_State * L)
{
	luaL_newlib(L, l_ninepatch);
	luahelper_create_metatable(L, MT_NINEPATCH, m_ninepatch);
	return 1;
}
