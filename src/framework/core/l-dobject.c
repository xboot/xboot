/*
 * framework/core/l-dobject.c
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
#include <framework/core/l-image.h>
#include <framework/core/l-ninepatch.h>
#include <framework/core/l-shape.h>
#include <framework/core/l-text.h>
#include <framework/core/l-dobject.h>

enum {
	MFLAG_TRANSLATE				= (0x1 << 0),
	MFLAG_ROTATE				= (0x1 << 1),
	MFLAG_SCALE					= (0x1 << 2),
	MFLAG_SKEW					= (0x1 << 3),
	MFLAG_ANCHOR				= (0x1 << 4),
	MFLAG_LOCAL_MATRIX			= (0x1 << 5),
	MFLAG_GLOBAL_MATRIX			= (0x1 << 6),
};

static inline cairo_matrix_t * dobject_local_matrix(struct ldobject_t * o)
{
	cairo_matrix_t * m = &o->local_matrix;
	if(o->mflag & MFLAG_LOCAL_MATRIX)
	{
		if((o->mflag & (MFLAG_TRANSLATE | MFLAG_ROTATE | MFLAG_SCALE | MFLAG_SKEW | MFLAG_ANCHOR)) == MFLAG_TRANSLATE)
		{
			m->xx = 1; m->yx = 0;
			m->xy = 0; m->yy = 1;
			m->x0 = o->x; m->y0 = o->y;
		}
		else
		{
			if(o->mflag & (MFLAG_ROTATE | MFLAG_SKEW))
			{
				double rx = o->rotation + o->skewy;
				double ry = o->rotation - o->skewx;
				m->xx = cos(rx);
				m->yx = sin(rx);
				m->xy = -sin(ry);
				m->yy = cos(ry);
				if(o->mflag & MFLAG_SCALE)
				{
					m->xx *= o->scalex;
					m->yx *= o->scalex;
					m->xy *= o->scaley;
					m->yy *= o->scaley;
				}
			}
			else
			{
				m->xx = o->scalex; m->yx = 0;
				m->xy = 0; m->yy = o->scaley;
			}
			if(o->mflag & MFLAG_ANCHOR)
			{
				double anchorx = o->anchorx * o->width;
				double anchory = o->anchory * o->height;
				m->x0 = o->x - (anchorx * m->xx + anchory * m->xy);
				m->y0 = o->y - (anchorx * m->yx + anchory * m->yy);
			}
			else
			{
				m->x0 = o->x;
				m->y0 = o->y;
			}
		}
		o->mflag &= ~MFLAG_LOCAL_MATRIX;
	}
	return m;
}

static inline cairo_matrix_t * dobject_global_matrix(struct ldobject_t * o)
{
	cairo_matrix_t * m = &o->global_matrix;
	if(o->mflag & MFLAG_GLOBAL_MATRIX)
	{
		memcpy(m, dobject_local_matrix(o), sizeof(cairo_matrix_t));
		while(o->parent && (o->parent != o))
		{
			o = o->parent;
			cairo_matrix_multiply(m, m, dobject_local_matrix(o));
		}
		o->mflag &= ~MFLAG_GLOBAL_MATRIX;
	}
	return m;
}

static void dobject_draw_image(lua_State * L, struct ldobject_t * o)
{
	struct display_t * disp = ((struct vmctx_t *)luahelper_vmctx(L))->disp;
	struct limage_t * img = o->priv;
	cairo_t * cr = disp->cr;
	cairo_save(cr);
	cairo_set_matrix(cr, dobject_global_matrix(o));
	cairo_set_source_surface(cr, img->cs, 0, 0);
	cairo_paint_with_alpha(cr, o->alpha);
	cairo_restore(cr);
}

static void dobject_draw_ninepatch(lua_State * L, struct ldobject_t * o)
{
	struct display_t * disp = ((struct vmctx_t *)luahelper_vmctx(L))->disp;
	struct lninepatch_t * ninepatch = o->priv;
	cairo_t * cr = disp->cr;
	cairo_save(cr);
	cairo_set_matrix(cr, dobject_global_matrix(o));
	if(ninepatch->lt)
	{
		cairo_save(cr);
		cairo_translate(cr, 0, 0);
		cairo_set_source_surface(cr, ninepatch->lt, 0, 0);
		cairo_pattern_set_filter(cairo_get_source(cr), CAIRO_FILTER_FAST);
		cairo_paint_with_alpha(cr, o->alpha);
		cairo_restore(cr);
	}
	if(ninepatch->mt)
	{
		cairo_save(cr);
		cairo_translate(cr, ninepatch->left, 0);
		cairo_scale(cr, ninepatch->__sx, 1);
		cairo_set_source_surface(cr, ninepatch->mt, 0, 0);
		cairo_pattern_set_filter(cairo_get_source(cr), CAIRO_FILTER_FAST);
		cairo_paint_with_alpha(cr, o->alpha);
		cairo_restore(cr);
	}
	if(ninepatch->rt)
	{
		cairo_save(cr);
		cairo_translate(cr, ninepatch->__w - ninepatch->right, 0);
		cairo_set_source_surface(cr, ninepatch->rt, 0, 0);
		cairo_pattern_set_filter(cairo_get_source(cr), CAIRO_FILTER_FAST);
		cairo_paint_with_alpha(cr, o->alpha);
		cairo_restore(cr);
	}
	if(ninepatch->lm)
	{
		cairo_save(cr);
		cairo_translate(cr, 0, ninepatch->top);
		cairo_scale(cr, 1, ninepatch->__sy);
		cairo_set_source_surface(cr, ninepatch->lm, 0, 0);
		cairo_pattern_set_filter(cairo_get_source(cr), CAIRO_FILTER_FAST);
		cairo_paint_with_alpha(cr, o->alpha);
		cairo_restore(cr);
	}
	if(ninepatch->mm)
	{
		cairo_save(cr);
		cairo_translate(cr, ninepatch->left, ninepatch->top);
		cairo_scale(cr, ninepatch->__sx, ninepatch->__sy);
		cairo_set_source_surface(cr, ninepatch->mm, 0, 0);
		cairo_pattern_set_filter(cairo_get_source(cr), CAIRO_FILTER_FAST);
		cairo_paint_with_alpha(cr, o->alpha);
		cairo_restore(cr);
	}
	if(ninepatch->rm)
	{
		cairo_save(cr);
		cairo_translate(cr, ninepatch->__w - ninepatch->right, ninepatch->top);
		cairo_scale(cr, 1, ninepatch->__sy);
		cairo_set_source_surface(cr, ninepatch->rm, 0, 0);
		cairo_pattern_set_filter(cairo_get_source(cr), CAIRO_FILTER_FAST);
		cairo_paint_with_alpha(cr, o->alpha);
		cairo_restore(cr);
	}
	if(ninepatch->lb)
	{
		cairo_save(cr);
		cairo_translate(cr, 0, ninepatch->__h - ninepatch->bottom);
		cairo_set_source_surface(cr, ninepatch->lb, 0, 0);
		cairo_paint_with_alpha(cr, o->alpha);
		cairo_pattern_set_filter(cairo_get_source(cr), CAIRO_FILTER_FAST);
		cairo_restore(cr);
	}
	if(ninepatch->mb)
	{
		cairo_save(cr);
		cairo_translate(cr, ninepatch->left, ninepatch->__h - ninepatch->bottom);
		cairo_scale(cr, ninepatch->__sx, 1);
		cairo_set_source_surface(cr, ninepatch->mb, 0, 0);
		cairo_pattern_set_filter(cairo_get_source(cr), CAIRO_FILTER_FAST);
		cairo_paint_with_alpha(cr, o->alpha);
		cairo_restore(cr);
	}
	if(ninepatch->rb)
	{
		cairo_save(cr);
		cairo_translate(cr, ninepatch->__w - ninepatch->right, ninepatch->__h - ninepatch->bottom);
		cairo_set_source_surface(cr, ninepatch->rb, 0, 0);
		cairo_paint_with_alpha(cr, o->alpha);
		cairo_pattern_set_filter(cairo_get_source(cr), CAIRO_FILTER_FAST);
		cairo_restore(cr);
	}
	cairo_restore(cr);
}

static void dobject_draw_shape(lua_State * L, struct ldobject_t * o)
{
	struct display_t * disp = ((struct vmctx_t *)luahelper_vmctx(L))->disp;
	struct lshape_t * shape = o->priv;
	cairo_t * cr = disp->cr;
	cairo_save(cr);
	cairo_set_matrix(cr, dobject_global_matrix(o));
	cairo_set_source_surface(cr, shape->cs, 0, 0);
	cairo_paint_with_alpha(cr, o->alpha);
	cairo_restore(cr);
}

static void dobject_draw_text(lua_State * L, struct ldobject_t * o)
{
	struct display_t * disp = ((struct vmctx_t *)luahelper_vmctx(L))->disp;
	struct ltext_t * text = o->priv;
	cairo_matrix_t * m = dobject_global_matrix(o);
	cairo_t * cr = disp->cr;
	cairo_save(cr);
	cairo_set_scaled_font(cr, text->font);
	cairo_move_to(cr, m->x0, m->y0);
	cairo_set_matrix(cr, m);
	cairo_move_to(cr, 0, text->metric.height);
	cairo_text_path(cr, text->utf8);
	cairo_set_source(cr, text->pattern);
	cairo_fill(cr);
	cairo_restore(cr);
}

static int l_dobject_new(lua_State * L)
{
	struct ldobject_t * o = lua_newuserdata(L, sizeof(struct ldobject_t));

	o->parent = o;
	init_list_head(&o->entry);
	init_list_head(&o->children);

	o->width = luaL_optnumber(L, 1, 0);
	o->height = luaL_optnumber(L, 2, 0);
	o->x = 0;
	o->y = 0;
	o->rotation = 0;
	o->scalex = 1;
	o->scaley = 1;
	o->skewx = 0;
	o->skewy = 0;
	o->anchorx = 0;
	o->anchory = 0;
	o->alpha = 1;
	o->alignment = ALIGN_NONE;
	o->visible = 1;
	o->touchable = 1;
	o->mflag = 0;
	cairo_matrix_init_identity(&o->local_matrix);
	cairo_matrix_init_identity(&o->global_matrix);

	if(luaL_testudata(L, 3, MT_IMAGE))
	{
		o->draw = dobject_draw_image;
		o->priv = lua_touserdata(L, 3);
	}
	else if(luaL_testudata(L, 3, MT_NINEPATCH))
	{
		o->draw = dobject_draw_ninepatch;
		o->priv = lua_touserdata(L, 3);
	}
	else if(luaL_testudata(L, 3, MT_SHAPE))
	{
		o->draw = dobject_draw_shape;
		o->priv = lua_touserdata(L, 3);
	}
	else if(luaL_testudata(L, 3, MT_TEXT))
	{
		o->draw = dobject_draw_text;
		o->priv = lua_touserdata(L, 3);
	}
	else
	{
		o->draw = NULL;
		o->priv = NULL;
	}

	luaL_setmetatable(L, MT_DOBJECT);
	return 1;
}

static const luaL_Reg l_dobject[] = {
	{"new",	l_dobject_new},
	{NULL,	NULL}
};

static void dobject_mark_with_children(struct ldobject_t * o, int mark)
{
	struct ldobject_t * pos, * n;

	if(o)
	{
		o->mflag |= mark;
		list_for_each_entry_safe(pos, n, &(o->children), entry)
		{
			dobject_mark_with_children(pos, mark);
		}
	}
}

static int m_add_child(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	struct ldobject_t * c = luaL_checkudata(L, 2, MT_DOBJECT);
	if(c->parent != o)
	{
		c->parent = o;
		if(lua_toboolean(L, 3))
			list_add_tail(&c->entry, &o->children);
		else
			list_add(&c->entry, &o->children);
		dobject_mark_with_children(c, MFLAG_GLOBAL_MATRIX);
	}
	return 0;
}

static int m_remove_child(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	struct ldobject_t * c = luaL_checkudata(L, 2, MT_DOBJECT);
	if(c->parent == o)
	{
		c->parent = c;
		list_del_init(&c->entry);
		dobject_mark_with_children(c, MFLAG_GLOBAL_MATRIX);
	}
	return 0;
}

static int m_to_front(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	if(o->parent && (o->parent != o))
		list_move(&o->entry, &o->parent->children);
	return 0;
}

static int m_to_back(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	if(o->parent && (o->parent != o))
		list_move_tail(&o->entry, &o->parent->children);
	return 0;
}

static int m_set_width(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	o->width = round(luaL_checknumber(L, 2));
	o->mflag |= MFLAG_LOCAL_MATRIX;
	dobject_mark_with_children(o, MFLAG_GLOBAL_MATRIX);
	return 0;
}

static int m_get_width(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	lua_pushnumber(L, o->width);
	return 1;
}

static int m_set_height(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	o->height = round(luaL_checknumber(L, 2));
	o->mflag |= MFLAG_LOCAL_MATRIX;
	dobject_mark_with_children(o, MFLAG_GLOBAL_MATRIX);
	return 0;
}

static int m_get_height(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	lua_pushnumber(L, o->height);
	return 1;
}

static int m_set_size(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	o->width = round(luaL_checknumber(L, 2));
	o->height = round(luaL_checknumber(L, 3));
	o->mflag |= MFLAG_LOCAL_MATRIX;
	dobject_mark_with_children(o, MFLAG_GLOBAL_MATRIX);
	return 0;
}

static int m_get_size(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	lua_pushnumber(L, o->width);
	lua_pushnumber(L, o->height);
	return 2;
}

static int m_set_x(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	o->x = luaL_checknumber(L, 2);
	if((o->x == 0.0) && (o->y == 0.0))
		o->mflag &= ~MFLAG_TRANSLATE;
	else
		o->mflag |= MFLAG_TRANSLATE;
	o->mflag |= MFLAG_LOCAL_MATRIX;
	dobject_mark_with_children(o, MFLAG_GLOBAL_MATRIX);
	return 0;
}

static int m_get_x(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	lua_pushnumber(L, o->x);
	return 1;
}

static int m_set_y(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	o->y = luaL_checknumber(L, 2);
	if((o->x == 0.0) && (o->y == 0.0))
		o->mflag &= ~MFLAG_TRANSLATE;
	else
		o->mflag |= MFLAG_TRANSLATE;
	o->mflag |= MFLAG_LOCAL_MATRIX;
	dobject_mark_with_children(o, MFLAG_GLOBAL_MATRIX);
	return 0;
}

static int m_get_y(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	lua_pushnumber(L, o->y);
	return 1;
}

static int m_set_position(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	o->x = luaL_checknumber(L, 2);
	o->y = luaL_checknumber(L, 3);
	if((o->x == 0.0) && (o->y == 0.0))
		o->mflag &= ~MFLAG_TRANSLATE;
	else
		o->mflag |= MFLAG_TRANSLATE;
	o->mflag |= MFLAG_LOCAL_MATRIX;
	dobject_mark_with_children(o, MFLAG_GLOBAL_MATRIX);
	return 0;
}

static int m_get_position(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	lua_pushnumber(L, o->x);
	lua_pushnumber(L, o->y);
	return 2;
}

static int m_set_rotation(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	o->rotation = luaL_checknumber(L, 2) * (M_PI / 180.0);
	if(o->rotation == 0.0)
		o->mflag &= ~MFLAG_ROTATE;
	else
		o->mflag |= MFLAG_ROTATE;
	o->mflag |= MFLAG_LOCAL_MATRIX;
	dobject_mark_with_children(o, MFLAG_GLOBAL_MATRIX);
	return 0;
}

static int m_get_rotation(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	lua_pushnumber(L, fmod(o->rotation / (M_PI / 180.0), 360.0));
	return 1;
}

static int m_set_scale_x(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	o->scalex = luaL_checknumber(L, 2);
	if((o->scalex == 1.0) && (o->scaley == 1.0))
		o->mflag &= ~MFLAG_SCALE;
	else
		o->mflag |= MFLAG_SCALE;
	o->mflag |= MFLAG_LOCAL_MATRIX;
	dobject_mark_with_children(o, MFLAG_GLOBAL_MATRIX);
	return 0;
}

static int m_get_scale_x(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	lua_pushnumber(L, o->scalex);
	return 1;
}

static int m_set_scale_y(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	o->scaley = luaL_checknumber(L, 2);
	if((o->scalex == 1.0) && (o->scaley == 1.0))
		o->mflag &= ~MFLAG_SCALE;
	else
		o->mflag |= MFLAG_SCALE;
	o->mflag |= MFLAG_LOCAL_MATRIX;
	dobject_mark_with_children(o, MFLAG_GLOBAL_MATRIX);
	return 0;
}

static int m_get_scale_y(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	lua_pushnumber(L, o->scaley);
	return 1;
}

static int m_set_scale(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	o->scalex = luaL_checknumber(L, 2);
	o->scaley = luaL_checknumber(L, 3);
	if((o->scalex == 1.0) && (o->scaley == 1.0))
		o->mflag &= ~MFLAG_SCALE;
	else
		o->mflag |= MFLAG_SCALE;
	o->mflag |= MFLAG_LOCAL_MATRIX;
	dobject_mark_with_children(o, MFLAG_GLOBAL_MATRIX);
	return 0;
}

static int m_get_scale(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	lua_pushnumber(L, o->scalex);
	lua_pushnumber(L, o->scaley);
	return 2;
}

static int m_set_skew_x(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	o->skewx = luaL_checknumber(L, 2) * (M_PI / 180.0);
	if((o->skewx == 0.0) && (o->skewy == 0.0))
		o->mflag &= ~MFLAG_SKEW;
	else
		o->mflag |= MFLAG_SKEW;
	o->mflag |= MFLAG_LOCAL_MATRIX;
	dobject_mark_with_children(o, MFLAG_GLOBAL_MATRIX);
	return 0;
}

static int m_get_skew_x(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	lua_pushnumber(L, o->skewx);
	return 1;
}

static int m_set_skew_y(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	o->skewy = luaL_checknumber(L, 2) * (M_PI / 180.0);
	if((o->skewx == 0.0) && (o->skewy == 0.0))
		o->mflag &= ~MFLAG_SKEW;
	else
		o->mflag |= MFLAG_SKEW;
	o->mflag |= MFLAG_LOCAL_MATRIX;
	dobject_mark_with_children(o, MFLAG_GLOBAL_MATRIX);
	return 0;
}

static int m_get_skew_y(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	lua_pushnumber(L, o->skewy);
	return 1;
}

static int m_set_skew(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	o->skewx = luaL_checknumber(L, 2) * (M_PI / 180.0);
	o->skewy = luaL_checknumber(L, 3) * (M_PI / 180.0);
	if((o->skewx == 0.0) && (o->skewy == 0.0))
		o->mflag &= ~MFLAG_SKEW;
	else
		o->mflag |= MFLAG_SKEW;
	o->mflag |= MFLAG_LOCAL_MATRIX;
	dobject_mark_with_children(o, MFLAG_GLOBAL_MATRIX);
	return 0;
}

static int m_get_skew(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	lua_pushnumber(L, fmod(o->skewx / (M_PI / 180.0), 360.0));
	lua_pushnumber(L, fmod(o->skewy / (M_PI / 180.0), 360.0));
	return 2;
}

static int m_set_archor(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	o->anchorx = luaL_checknumber(L, 2);
	o->anchory = luaL_checknumber(L, 3);
	if((o->anchorx == 0.0) && (o->anchory == 0.0))
		o->mflag &= ~MFLAG_ANCHOR;
	else
		o->mflag |= MFLAG_ANCHOR;
	o->mflag |= MFLAG_LOCAL_MATRIX;
	dobject_mark_with_children(o, MFLAG_GLOBAL_MATRIX);
	return 0;
}

static int m_get_archor(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	lua_pushnumber(L, o->anchorx);
	lua_pushnumber(L, o->anchory);
	return 2;
}

static int m_set_alpha(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	o->alpha = luaL_checknumber(L, 2);
	return 0;
}

static int m_get_alpha(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	lua_pushnumber(L, o->alpha);
	return 1;
}

static int m_set_alignment(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	o->alignment = (enum alignment_t)luaL_checkinteger(L, 2);
	return 0;
}

static int m_get_alignment(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	lua_pushinteger(L, o->alignment);
	return 1;
}

static int m_set_visible(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	o->visible = lua_toboolean(L, 2) ? 1 : 0;
	return 0;
}

static int m_get_visible(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	lua_pushboolean(L, o->visible);
	return 1;
}

static int m_set_touchable(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	o->touchable = lua_toboolean(L, 2) ? 1 : 0;
	return 0;
}

static int m_get_touchable(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	lua_pushboolean(L, o->touchable);
	return 1;
}

static int m_global_to_local(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	double nx, x = luaL_checknumber(L, 2);
	double ny, y = luaL_checknumber(L, 3);
	cairo_matrix_t * m = dobject_global_matrix(o);
	double id = 1.0 / (m->xx * m->yy - m->xy * m->yx);
	nx = ((x - m->x0) * m->yy + (m->y0 - y) * m->xy) * id;
	ny = ((y - m->y0) * m->xx + (m->x0 - x) * m->yx) * id;
	lua_pushnumber(L, nx);
	lua_pushnumber(L, ny);
	return 2;
}

static int m_local_to_global(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	double nx, x = luaL_checknumber(L, 2);
	double ny, y = luaL_checknumber(L, 3);
	cairo_matrix_t * m = dobject_global_matrix(o);
	nx = m->xx * x + m->xy * y + m->x0;
	ny = m->yx * x + m->yy * y + m->y0;
	lua_pushnumber(L, nx);
	lua_pushnumber(L, ny);
	return 2;
}

static int m_hit_test_point(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	if(o->visible && o->touchable)
	{
		double nx, x = luaL_checknumber(L, 2);
		double ny, y = luaL_checknumber(L, 3);
		cairo_matrix_t * m = dobject_global_matrix(o);
		double id = 1.0 / (m->xx * m->yy - m->xy * m->yx);
		nx = ((x - m->x0) * m->yy + (m->y0 - y) * m->xy) * id;
		ny = ((y - m->y0) * m->xx + (m->x0 - x) * m->yx) * id;
		lua_pushboolean(L, ((nx >= 0) && (ny >= 0) && (nx <= o->width) && (ny <= o->height)) ? 1 : 0);
		return 1;
	}
	lua_pushboolean(L, 0);
	return 0;
}

static int m_bounds(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	double x1 = 0;
	double y1 = 0;
	double x2 = o->width;
	double y2 = o->height;
	_cairo_matrix_transform_bounding_box(dobject_global_matrix(o), &x1, &y1, &x2, &y2, NULL);
	lua_pushnumber(L, x1);
	lua_pushnumber(L, y1);
	lua_pushnumber(L, x2 - x1);
	lua_pushnumber(L, y2 - y1);
	return 4;
}

static inline void dobject_translate(struct ldobject_t * o, double dx, double dy)
{
	o->x = o->x + dx;
	o->y = o->y + dy;
	if((o->x == 0.0) && (o->y == 0.0))
		o->mflag &= ~MFLAG_TRANSLATE;
	else
		o->mflag |= MFLAG_TRANSLATE;
	o->mflag |= MFLAG_LOCAL_MATRIX;
	dobject_mark_with_children(o, MFLAG_GLOBAL_MATRIX);
}

static inline void dobject_translate_fill(struct ldobject_t * o, double x, double y, double w, double h)
{
	o->x = x;
	o->y = y;
	if((o->x == 0.0) && (o->y == 0.0))
		o->mflag &= ~MFLAG_TRANSLATE;
	else
		o->mflag |= MFLAG_TRANSLATE;

	if(o->width != 0.0 && o->height != 0.0)
	{
		o->scalex = w / o->width;
		o->scaley = h / o->height;
		if((o->scalex == 1.0) && (o->scaley == 1.0))
			o->mflag &= ~MFLAG_SCALE;
		else
			o->mflag |= MFLAG_SCALE;
	}
	o->rotation = 0;
	o->skewx = 0;
	o->skewy = 0;
	o->anchorx = 0;
	o->anchory = 0;
	o->mflag |= MFLAG_LOCAL_MATRIX;
	dobject_mark_with_children(o, MFLAG_GLOBAL_MATRIX);
}

static int m_layout(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	struct ldobject_t * c = luaL_checkudata(L, 2, MT_DOBJECT);
	double rx1 = luaL_optnumber(L, 3, 0);
	double ry1 = luaL_optnumber(L, 4, 0);
	double rx2 = luaL_optnumber(L, 5, o->width);
	double ry2 = luaL_optnumber(L, 6, o->height);

	if(c->alignment <= ALIGN_NONE)
	{
	}
	else if((c->alignment <= ALIGN_CENTER))
	{
		double ox1 = rx1;
		double oy1 = ry1;
		double ox2 = rx2;
		double oy2 = ry2;
		double cx1 = 0;
		double cy1 = 0;
		double cx2 = c->width;
		double cy2 = c->height;
		_cairo_matrix_transform_bounding_box(dobject_local_matrix(c), &cx1, &cy1, &cx2, &cy2, NULL);

		switch(c->alignment)
		{
		case ALIGN_LEFT:
			dobject_translate(c, ox1 - cx1, 0);
			rx1 += cx2 - cx1;
			break;
		case ALIGN_TOP:
			dobject_translate(c, 0, oy1 - cy1);
			ry1 += cy2 - cy1;
			break;
		case ALIGN_RIGHT:
			dobject_translate(c, ox2 - cx2, 0);
			rx2 -= cx2 - cx1;
			break;
		case ALIGN_BOTTOM:
			dobject_translate(c, 0, oy2 - cy2);
			ry2 -= cy2 - cy1;
			break;
		case ALIGN_LEFT_TOP:
			dobject_translate(c, ox1 - cx1, oy1 - cy1);
			rx1 += cx2 - cx1;
			ry1 += cy2 - cy1;
			break;
		case ALIGN_RIGHT_TOP:
			dobject_translate(c, ox2 - cx2, oy1 - cy1);
			rx2 -= cx2 - cx1;
			ry1 += cy2 - cy1;
			break;
		case ALIGN_LEFT_BOTTOM:
			dobject_translate(c, ox1 - cx1, oy2 - cy2);
			rx1 += cx2 - cx1;
			ry2 -= cy2 - cy1;
			break;
		case ALIGN_RIGHT_BOTTOM:
			dobject_translate(c, ox2 - cx2, oy2 - cy2);
			rx2 -= cx2 - cx1;
			ry2 -= cy2 - cy1;
			break;
		case ALIGN_LEFT_CENTER:
			dobject_translate(c, ox1 - cx1, oy1 - cy1 + ((oy2 - oy1) - (cy2 - cy1)) / 2);
			rx1 += cx2 - cx1;
			break;
		case ALIGN_TOP_CENTER:
			dobject_translate(c, ox1 - cx1 + ((ox2 - ox1) - (cx2 - cx1)) / 2, oy1 - cy1);
			ry1 += cy2 - cy1;
			break;
		case ALIGN_RIGHT_CENTER:
			dobject_translate(c, ox2 - cx2, oy1 - cy1 + ((oy2 - oy1) - (cy2 - cy1)) / 2);
			rx2 -= cx2 - cx1;
			break;
		case ALIGN_BOTTOM_CENTER:
			dobject_translate(c, ox1 - cx1 + ((ox2 - ox1) - (cx2 - cx1)) / 2, oy2 - cy2);
			ry2 -= cy2 - cy1;
			break;
		case ALIGN_HORIZONTAL_CENTER:
			dobject_translate(c, ox1 - cx1 + ((ox2 - ox1) - (cx2 - cx1)) / 2, 0);
			break;
		case ALIGN_VERTICAL_CENTER:
			dobject_translate(c, 0, oy1 - cy1 + ((oy2 - oy1) - (cy2 - cy1)) / 2);
			break;
		case ALIGN_CENTER:
			dobject_translate(c, ox1 - cx1 + ((ox2 - ox1) - (cx2 - cx1)) / 2, oy1 - cy1 + ((oy2 - oy1) - (cy2 - cy1)) / 2);
			break;
		default:
			break;
		}
	}
	else if((c->alignment <= ALIGN_CENTER_FILL))
	{
		double w;
		double h;

		switch(c->alignment)
		{
		case ALIGN_LEFT_FILL:
			w = c->width * c->scalex;
			h = ry2 - ry1;
			dobject_translate_fill(c, rx1, ry1, w, h);
			rx1 += w;
			break;
		case ALIGN_TOP_FILL:
			w = rx2 - rx1;
			h = c->height * c->scaley;
			dobject_translate_fill(c, rx1, ry1, w, h);
			ry1 += h;
			break;
		case ALIGN_RIGHT_FILL:
			w = c->width * c->scalex;
			h = ry2 - ry1;
			dobject_translate_fill(c, rx2 - w, ry1, w, h);
			rx2 -= w;
			break;
		case ALIGN_BOTTOM_FILL:
			w = rx2 - rx1;
			h = c->height * c->scaley;
			dobject_translate_fill(c, rx1, ry2 - h, w, h);
			ry2 -= h;
			break;
		case ALIGN_HORIZONTAL_FILL:
			w = rx2 - rx1;
			h = c->height * c->scaley;
			dobject_translate_fill(c, rx1, c->y, w, h);
			break;
		case ALIGN_VERTICAL_FILL:
			w = c->width * c->scalex;
			h = ry2 - ry1;
			dobject_translate_fill(c, c->x, ry1, w, h);
			break;
		case ALIGN_CENTER_FILL:
			w = rx2 - rx1;
			h = ry2 - ry1;
			dobject_translate_fill(c, rx1, ry1, w, h);
			rx1 += w;
			ry1 += h;
			break;
		default:
			break;
		}
	}

	lua_pushnumber(L, rx1);
	lua_pushnumber(L, ry1);
	lua_pushnumber(L, rx2);
	lua_pushnumber(L, ry2);
	return 4;
}

static int m_draw(lua_State * L)
{
	struct display_t * disp = ((struct vmctx_t *)luahelper_vmctx(L))->disp;
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	if(o->visible && o->draw)
	{
		o->draw(L, o);
		if(disp->showobj)
		{
			cairo_t * cr = disp->cr;
			cairo_save(cr);
			cairo_set_matrix(cr, dobject_global_matrix(o));
			cairo_set_line_width(cr, 1);
			cairo_rectangle(cr, 0, 0, o->width, o->height);
			cairo_set_source_rgba(cr, 1, 0, 0, 0.1);
			cairo_fill_preserve(cr);
			cairo_set_source_rgba(cr, 0, 0, 1, 0.9);
			cairo_stroke(cr);
			cairo_restore(cr);
		}
	}
	return 0;
}

static const luaL_Reg m_dobject[] = {
	{"addChild",		m_add_child},
	{"removeChild",		m_remove_child},
	{"toFront",			m_to_front},
	{"toBack",			m_to_back},
	{"setWidth",		m_set_width},
	{"getWidth",		m_get_width},
	{"setHeight",		m_set_height},
	{"getHeight",		m_get_height},
	{"setSize",			m_set_size},
	{"getSize",			m_get_size},
	{"setX",			m_set_x},
	{"getX",			m_get_x},
	{"setY",			m_set_y},
	{"getY",			m_get_y},
	{"setPosition",		m_set_position},
	{"getPosition",		m_get_position},
	{"setRotation",		m_set_rotation},
	{"getRotation",		m_get_rotation},
	{"setScaleX",		m_set_scale_x},
	{"getScaleX",		m_get_scale_x},
	{"setScaleY",		m_set_scale_y},
	{"getScaleY",		m_get_scale_y},
	{"setScale",		m_set_scale},
	{"getScale",		m_get_scale},
	{"setSkewX",		m_set_skew_x},
	{"getSkewX",		m_get_skew_x},
	{"setSkewY",		m_set_skew_y},
	{"getSkewY",		m_get_skew_y},
	{"setSkew",			m_set_skew},
	{"getSkew",			m_get_skew},
	{"setAnchor",		m_set_archor},
	{"getAnchor",		m_get_archor},
	{"setAlpha",		m_set_alpha},
	{"getAlpha",		m_get_alpha},
	{"setAlignment",	m_set_alignment},
	{"getAlignment",	m_get_alignment},
	{"setVisible",		m_set_visible},
	{"getVisible",		m_get_visible},
	{"setTouchable",	m_set_touchable},
	{"getTouchable",	m_get_touchable},
	{"globalToLocal",	m_global_to_local},
	{"localToGlobal",	m_local_to_global},
	{"hitTestPoint",	m_hit_test_point},
	{"bounds",			m_bounds},
	{"layout",			m_layout},
	{"draw",			m_draw},
	{NULL, NULL}
};

int luaopen_dobject(lua_State * L)
{
	luaL_newlib(L, l_dobject);
	/* enum alignment_t */
	luahelper_set_intfield(L, "ALIGN_NONE", 				ALIGN_NONE);
	luahelper_set_intfield(L, "ALIGN_LEFT", 				ALIGN_LEFT);
	luahelper_set_intfield(L, "ALIGN_TOP", 					ALIGN_TOP);
	luahelper_set_intfield(L, "ALIGN_RIGHT", 				ALIGN_RIGHT);
	luahelper_set_intfield(L, "ALIGN_BOTTOM", 				ALIGN_BOTTOM);
	luahelper_set_intfield(L, "ALIGN_LEFT_TOP", 			ALIGN_LEFT_TOP);
	luahelper_set_intfield(L, "ALIGN_RIGHT_TOP", 			ALIGN_RIGHT_TOP);
	luahelper_set_intfield(L, "ALIGN_LEFT_BOTTOM", 			ALIGN_LEFT_BOTTOM);
	luahelper_set_intfield(L, "ALIGN_RIGHT_BOTTOM", 		ALIGN_RIGHT_BOTTOM);
	luahelper_set_intfield(L, "ALIGN_LEFT_CENTER", 			ALIGN_LEFT_CENTER);
	luahelper_set_intfield(L, "ALIGN_TOP_CENTER", 			ALIGN_TOP_CENTER);
	luahelper_set_intfield(L, "ALIGN_RIGHT_CENTER", 		ALIGN_RIGHT_CENTER);
	luahelper_set_intfield(L, "ALIGN_BOTTOM_CENTER",		ALIGN_BOTTOM_CENTER);
	luahelper_set_intfield(L, "ALIGN_HORIZONTAL_CENTER",	ALIGN_HORIZONTAL_CENTER);
	luahelper_set_intfield(L, "ALIGN_VERTICAL_CENTER", 		ALIGN_VERTICAL_CENTER);
	luahelper_set_intfield(L, "ALIGN_CENTER", 				ALIGN_CENTER);
	luahelper_set_intfield(L, "ALIGN_LEFT_FILL", 			ALIGN_LEFT_FILL);
	luahelper_set_intfield(L, "ALIGN_TOP_FILL", 			ALIGN_TOP_FILL);
	luahelper_set_intfield(L, "ALIGN_RIGHT_FILL", 			ALIGN_RIGHT_FILL);
	luahelper_set_intfield(L, "ALIGN_BOTTOM_FILL", 			ALIGN_BOTTOM_FILL);
	luahelper_set_intfield(L, "ALIGN_HORIZONTAL_FILL", 		ALIGN_HORIZONTAL_FILL);
	luahelper_set_intfield(L, "ALIGN_VERTICAL_FILL", 		ALIGN_VERTICAL_FILL);
	luahelper_set_intfield(L, "ALIGN_CENTER_FILL", 			ALIGN_CENTER_FILL);
	luahelper_create_metatable(L, MT_DOBJECT, m_dobject);
	return 1;
}
