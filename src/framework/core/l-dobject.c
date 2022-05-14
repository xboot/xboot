/*
 * framework/core/l-dobject.c
 *
 * Copyright(c) 2007-2022 Jianjun Jiang <8192542@qq.com>
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
#include <core/l-color.h>
#include <core/l-image.h>
#include <core/l-ninepatch.h>
#include <core/l-text.h>
#include <core/l-icon.h>
#include <core/l-window.h>
#include <core/l-dobject.h>

enum {
	MFLAG_TRANSLATE					= (0x1 << 0),
	MFLAG_ROTATE					= (0x1 << 1),
	MFLAG_SCALE						= (0x1 << 2),
	MFLAG_SKEW						= (0x1 << 3),
	MFLAG_ANCHOR					= (0x1 << 4),
	MFLAG_LOCAL_MATRIX				= (0x1 << 5),
	MFLAG_GLOBAL_MATRIX				= (0x1 << 6),
	MFLAG_GLOBAL_BOUNDS				= (0x1 << 7),
	MFLAG_DIRTY						= (0x1 << 8),
};

static inline struct matrix_t * dobject_local_matrix(struct ldobject_t * o)
{
	struct matrix_t * m = &o->local_matrix;
	if(o->mflag & MFLAG_LOCAL_MATRIX)
	{
		if(o->mflag & (MFLAG_ROTATE | MFLAG_SCALE | MFLAG_SKEW | MFLAG_ANCHOR))
		{
			if(o->mflag & (MFLAG_ROTATE | MFLAG_SKEW))
			{
				double rx = o->rotation + o->skewy;
				double ry = o->rotation - o->skewx;
				m->a = cos(rx);
				m->b = sin(rx);
				m->c = -sin(ry);
				m->d = cos(ry);
				if(o->mflag & MFLAG_SCALE)
				{
					m->a *= o->scalex;
					m->b *= o->scalex;
					m->c *= o->scaley;
					m->d *= o->scaley;
				}
			}
			else
			{
				m->a = o->scalex; m->b = 0;
				m->c = 0; m->d = o->scaley;
			}
			if(o->mflag & MFLAG_ANCHOR)
			{
				double anchorx = o->anchorx * o->width;
				double anchory = o->anchory * o->height;
				m->tx = o->x - (anchorx * m->a + anchory * m->c);
				m->ty = o->y - (anchorx * m->b + anchory * m->d);
			}
			else
			{
				m->tx = o->x;
				m->ty = o->y;
			}
		}
		else
		{
			m->a = 1; m->b = 0;
			m->c = 0; m->d = 1;
			m->tx = o->x; m->ty = o->y;
		}
		o->mflag &= ~MFLAG_LOCAL_MATRIX;
	}
	return m;
}

static inline struct matrix_t * dobject_global_matrix(struct ldobject_t * o)
{
	struct matrix_t * t, * m = &o->global_matrix;
	struct ldobject_t * p;
	if(o->mflag & MFLAG_GLOBAL_MATRIX)
	{
		p = o;
		memcpy(m, dobject_local_matrix(p), sizeof(struct matrix_t));
		while(p->parent)
		{
			p = p->parent;
			t = dobject_local_matrix(p);
			if(p->mflag & (MFLAG_ROTATE | MFLAG_SCALE | MFLAG_SKEW))
			{
				matrix_multiply(m, m, t);
			}
			else
			{
				m->tx += t->tx;
				m->ty += t->ty;
			}
		}
		o->mflag &= ~MFLAG_GLOBAL_MATRIX;
	}
	return m;
}

static inline struct region_t * dobject_global_bounds(struct ldobject_t * o)
{
	struct region_t * r = &o->global_bounds;
	if(o->mflag & MFLAG_GLOBAL_BOUNDS)
	{
		double x1 = 0;
		double y1 = 0;
		double x2 = o->width;
		double y2 = o->height;
		matrix_transform_bounds(dobject_global_matrix(o), &x1, &y1, &x2, &y2);
		region_init(r, x1, y1, x2 - x1 + 2, y2 - y1 + 2);
		o->mflag &= ~MFLAG_GLOBAL_BOUNDS;
	}
	return r;
}

static inline struct region_t * dobject_parent_global_bounds(struct ldobject_t * o)
{
	struct ldobject_t * parent = o->parent;
	if(parent)
		return dobject_global_bounds(parent);
	return NULL;
}

static inline struct region_t * dobject_dirty_bounds(struct ldobject_t * o)
{
	return &o->dirty_bounds;
}

static inline void dobject_mark(struct ldobject_t * o, int mark)
{
	o->mflag |= mark;
}

static void dobject_mark_children(struct ldobject_t * o, int mark)
{
	struct ldobject_t * pos;

	o->mflag |= mark;
	list_for_each_entry(pos, &o->children, entry)
	{
		dobject_mark_children(pos, mark);
	}
}

static inline void dobject_mark_dirty(struct ldobject_t * o)
{
	if(!(o->mflag & MFLAG_DIRTY))
	{
		region_clone(&o->dirty_bounds, dobject_global_bounds(o));
		o->mflag |= MFLAG_DIRTY;
	}
}

static void dobject_draw_image(struct ldobject_t * o, struct window_t * w)
{
	struct limage_t * img = o->priv;
	surface_blit(w->s, dobject_parent_global_bounds(o), dobject_global_matrix(o), img->s, RENDER_TYPE_GOOD);
}

static void dobject_draw_ninepatch(struct ldobject_t * o, struct window_t * w)
{
	struct lninepatch_t * ninepatch = o->priv;
	struct surface_t * s = w->s;
	struct matrix_t m;
	if(ninepatch->lt)
	{
		memcpy(&m, dobject_global_matrix(o), sizeof(struct matrix_t));
		matrix_translate(&m, 0, 0);
		surface_blit(s, dobject_parent_global_bounds(o), &m, ninepatch->lt, RENDER_TYPE_FAST);
	}
	if(ninepatch->mt)
	{
		memcpy(&m, dobject_global_matrix(o), sizeof(struct matrix_t));
		matrix_translate(&m, ninepatch->left, 0);
		matrix_scale(&m, ninepatch->__sx, 1);
		surface_blit(s, dobject_parent_global_bounds(o), &m, ninepatch->mt, RENDER_TYPE_FAST);
	}
	if(ninepatch->rt)
	{
		memcpy(&m, dobject_global_matrix(o), sizeof(struct matrix_t));
		matrix_translate(&m, ninepatch->__w - ninepatch->right, 0);
		surface_blit(s, dobject_parent_global_bounds(o), &m, ninepatch->rt, RENDER_TYPE_FAST);
	}
	if(ninepatch->lm)
	{
		memcpy(&m, dobject_global_matrix(o), sizeof(struct matrix_t));
		matrix_translate(&m, 0, ninepatch->top);
		matrix_scale(&m, 1, ninepatch->__sy);
		surface_blit(s, dobject_parent_global_bounds(o), &m, ninepatch->lm, RENDER_TYPE_FAST);
	}
	if(ninepatch->mm)
	{
		memcpy(&m, dobject_global_matrix(o), sizeof(struct matrix_t));
		matrix_translate(&m, ninepatch->left, ninepatch->top);
		matrix_scale(&m, ninepatch->__sx, ninepatch->__sy);
		surface_blit(s, dobject_parent_global_bounds(o), &m, ninepatch->mm, RENDER_TYPE_FAST);
	}
	if(ninepatch->rm)
	{
		memcpy(&m, dobject_global_matrix(o), sizeof(struct matrix_t));
		matrix_translate(&m, ninepatch->__w - ninepatch->right, ninepatch->top);
		matrix_scale(&m, 1, ninepatch->__sy);
		surface_blit(s, dobject_parent_global_bounds(o), &m, ninepatch->rm, RENDER_TYPE_FAST);
	}
	if(ninepatch->lb)
	{
		memcpy(&m, dobject_global_matrix(o), sizeof(struct matrix_t));
		matrix_translate(&m, 0, ninepatch->__h - ninepatch->bottom);
		surface_blit(s, dobject_parent_global_bounds(o), &m, ninepatch->lb, RENDER_TYPE_FAST);
	}
	if(ninepatch->mb)
	{
		memcpy(&m, dobject_global_matrix(o), sizeof(struct matrix_t));
		matrix_translate(&m, ninepatch->left, ninepatch->__h - ninepatch->bottom);
		matrix_scale(&m, ninepatch->__sx, 1);
		surface_blit(s, dobject_parent_global_bounds(o), &m, ninepatch->mb, RENDER_TYPE_FAST);
	}
	if(ninepatch->rb)
	{
		memcpy(&m, dobject_global_matrix(o), sizeof(struct matrix_t));
		matrix_translate(&m, ninepatch->__w - ninepatch->right, ninepatch->__h - ninepatch->bottom);
		surface_blit(s, dobject_parent_global_bounds(o), &m, ninepatch->rb, RENDER_TYPE_FAST);
	}
}

static void dobject_draw_text(struct ldobject_t * o, struct window_t * w)
{
	struct ltext_t * text = o->priv;
	surface_text(w->s, dobject_parent_global_bounds(o), dobject_global_matrix(o), &text->txt);
}

static void dobject_draw_icon(struct ldobject_t * o, struct window_t * w)
{
	struct licon_t * icon = o->priv;
	surface_icon(w->s, dobject_parent_global_bounds(o), dobject_global_matrix(o), &icon->ico);
}

static void dobject_draw_container(struct ldobject_t * o, struct window_t * w)
{
	if(o->bgcolor.a != 0)
		surface_fill(w->s, dobject_parent_global_bounds(o), dobject_global_matrix(o), o->width, o->height, &o->bgcolor, RENDER_TYPE_GOOD);
}

static int l_dobject_new(lua_State * L)
{
	enum dobject_type_t dtype;
	void (*draw)(struct ldobject_t *, struct window_t *);
	void * userdata;
	double width = luaL_optnumber(L, 1, 0);
	double height = luaL_optnumber(L, 2, 0);
	if(luaL_testudata(L, 3, MT_IMAGE))
	{
		dtype = DOBJECT_TYPE_IMAGE;
		draw = dobject_draw_image;
		userdata = lua_touserdata(L, 3);
	}
	else if(luaL_testudata(L, 3, MT_NINEPATCH))
	{
		dtype = DOBJECT_TYPE_NINEPATCH;
		draw = dobject_draw_ninepatch;
		userdata = lua_touserdata(L, 3);
	}
	else if(luaL_testudata(L, 3, MT_TEXT))
	{
		dtype = DOBJECT_TYPE_TEXT;
		draw = dobject_draw_text;
		userdata = lua_touserdata(L, 3);
	}
	else if(luaL_testudata(L, 3, MT_ICON))
	{
		dtype = DOBJECT_TYPE_ICON;
		draw = dobject_draw_icon;
		userdata = lua_touserdata(L, 3);
	}
	else
	{
		dtype = DOBJECT_TYPE_CONTAINER;
		draw = dobject_draw_container;
		userdata = NULL;
	}
	struct ldobject_t * o = lua_newuserdata(L, sizeof(struct ldobject_t));

	o->parent = NULL;
	init_list_head(&o->entry);
	init_list_head(&o->children);
	o->width = width;
	o->height = height;
	o->x = 0;
	o->y = 0;
	o->rotation = 0;
	o->scalex = 1;
	o->scaley = 1;
	o->skewx = 0;
	o->skewy = 0;
	o->anchorx = 0;
	o->anchory = 0;
	o->bgcolor.r = 0;
	o->bgcolor.g = 0;
	o->bgcolor.b = 0;
	o->bgcolor.a = 0;
	o->ctype = COLLIDER_TYPE_NONE;
	o->visible = 1;
	o->touchable = 1;
	o->mflag = 0;
	matrix_init_identity(&o->local_matrix);
	matrix_init_identity(&o->global_matrix);
	region_init(&o->global_bounds, o->x, o->y, o->width, o->height);
	region_init(&o->dirty_bounds, o->x, o->y, o->width, o->height);
	o->dtype = dtype;
	o->draw = draw;
	o->priv = userdata;

	luaL_setmetatable(L, MT_DOBJECT);
	return 1;
}

static const luaL_Reg l_dobject[] = {
	{"new",	l_dobject_new},
	{NULL,	NULL}
};

static int m_dobject_gc(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	if(o->ctype == COLLIDER_TYPE_POLYGON)
	{
		if((o->hit.polygon.length > 0) && o->hit.polygon.points)
		{
			free(o->hit.polygon.points);
			o->hit.polygon.points = NULL;
			o->hit.polygon.length = 0;
		}
	}
	return 0;
}

static int m_add_child(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	struct ldobject_t * c = luaL_checkudata(L, 2, MT_DOBJECT);
	if(c->parent != o)
	{
		if(c->parent)
		{
			dobject_mark_dirty(c);
			c->parent = o;
			list_add_tail(&c->entry, &o->children);
		}
		else
		{
			c->parent = o;
			list_add_tail(&c->entry, &o->children);
			c->mflag &= ~MFLAG_DIRTY;
			dobject_mark_dirty(c);
		}
		dobject_mark_children(c, MFLAG_GLOBAL_MATRIX | MFLAG_GLOBAL_BOUNDS);
	}
	return 0;
}

static int m_remove_child(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	struct ldobject_t * c = luaL_checkudata(L, 2, MT_DOBJECT);
	struct region_t * r;
	if(c->parent == o)
	{
		dobject_mark_dirty(c);
		r = dobject_dirty_bounds(o);
		if(!(o->mflag & MFLAG_DIRTY))
		{
			region_clone(r, dobject_dirty_bounds(c));
			o->mflag |= MFLAG_DIRTY;
		}
		else
		{
			region_union(r, r, dobject_dirty_bounds(c));
		}
		c->mflag &= ~MFLAG_DIRTY;
		c->parent = NULL;
		list_del_init(&c->entry);
		dobject_mark_children(c, MFLAG_GLOBAL_MATRIX | MFLAG_GLOBAL_BOUNDS);
	}
	return 0;
}

static int m_to_front(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	if(o->parent && !list_is_last(&o->entry, &o->parent->children))
	{
		dobject_mark_dirty(o);
		list_move_tail(&o->entry, &o->parent->children);
	}
	return 0;
}

static int m_to_back(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	if(o->parent && !list_is_first(&o->entry, &o->parent->children))
	{
		dobject_mark_dirty(o);
		list_move(&o->entry, &o->parent->children);
	}
	return 0;
}

static int m_set_width(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	double width = luaL_checknumber(L, 2);
	if(o->width != width)
	{
		dobject_mark_dirty(o);
		o->width = width;
		dobject_mark(o, MFLAG_LOCAL_MATRIX);
		dobject_mark_children(o, MFLAG_GLOBAL_MATRIX | MFLAG_GLOBAL_BOUNDS);
	}
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
	double height = luaL_checknumber(L, 2);
	if(o->height != height)
	{
		dobject_mark_dirty(o);
		o->height = height;
		dobject_mark(o, MFLAG_LOCAL_MATRIX);
		dobject_mark_children(o, MFLAG_GLOBAL_MATRIX | MFLAG_GLOBAL_BOUNDS);
	}
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
	double width = luaL_checknumber(L, 2);
	double height = luaL_checknumber(L, 3);
	if((o->width != width) || (o->height != height))
	{
		dobject_mark_dirty(o);
		o->width = width;
		o->height = height;
		dobject_mark(o, MFLAG_LOCAL_MATRIX);
		dobject_mark_children(o, MFLAG_GLOBAL_MATRIX | MFLAG_GLOBAL_BOUNDS);
	}
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
	double x = luaL_checknumber(L, 2);
	if(o->x != x)
	{
		dobject_mark_dirty(o);
		o->x = x;
		if((o->x == 0.0) && (o->y == 0.0))
			o->mflag &= ~MFLAG_TRANSLATE;
		else
			o->mflag |= MFLAG_TRANSLATE;
		dobject_mark(o, MFLAG_LOCAL_MATRIX);
		dobject_mark_children(o, MFLAG_GLOBAL_MATRIX | MFLAG_GLOBAL_BOUNDS);
	}
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
	double y = luaL_checknumber(L, 2);
	if(o->y != y)
	{
		dobject_mark_dirty(o);
		o->y = y;
		if((o->x == 0.0) && (o->y == 0.0))
			o->mflag &= ~MFLAG_TRANSLATE;
		else
			o->mflag |= MFLAG_TRANSLATE;
		dobject_mark(o, MFLAG_LOCAL_MATRIX);
		dobject_mark_children(o, MFLAG_GLOBAL_MATRIX | MFLAG_GLOBAL_BOUNDS);
	}
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
	double x = luaL_checknumber(L, 2);
	double y = luaL_checknumber(L, 3);
	if((o->x != x) || (o->y != y))
	{
		dobject_mark_dirty(o);
		o->x = x;
		o->y = y;
		if((o->x == 0.0) && (o->y == 0.0))
			o->mflag &= ~MFLAG_TRANSLATE;
		else
			o->mflag |= MFLAG_TRANSLATE;
		dobject_mark(o, MFLAG_LOCAL_MATRIX);
		dobject_mark_children(o, MFLAG_GLOBAL_MATRIX | MFLAG_GLOBAL_BOUNDS);
	}
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
	double rotation = luaL_checknumber(L, 2) * (M_PI / 180.0);
	if(o->rotation != rotation)
	{
		dobject_mark_dirty(o);
		o->rotation = rotation;
		if(o->rotation == 0.0)
			o->mflag &= ~MFLAG_ROTATE;
		else
			o->mflag |= MFLAG_ROTATE;
		dobject_mark(o, MFLAG_LOCAL_MATRIX);
		dobject_mark_children(o, MFLAG_GLOBAL_MATRIX | MFLAG_GLOBAL_BOUNDS);
	}
	return 0;
}

static int m_get_rotation(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	lua_pushnumber(L, o->rotation * (180.0 / M_PI));
	return 1;
}

static int m_set_scale_x(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	double scalex = luaL_checknumber(L, 2);
	if(o->scalex != scalex)
	{
		dobject_mark_dirty(o);
		o->scalex = scalex;
		if((o->scalex == 1.0) && (o->scaley == 1.0))
			o->mflag &= ~MFLAG_SCALE;
		else
			o->mflag |= MFLAG_SCALE;
		dobject_mark(o, MFLAG_LOCAL_MATRIX);
		dobject_mark_children(o, MFLAG_GLOBAL_MATRIX | MFLAG_GLOBAL_BOUNDS);
	}
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
	double scaley = luaL_checknumber(L, 2);
	if(o->scaley != scaley)
	{
		dobject_mark_dirty(o);
		o->scaley = scaley;
		if((o->scalex == 1.0) && (o->scaley == 1.0))
			o->mflag &= ~MFLAG_SCALE;
		else
			o->mflag |= MFLAG_SCALE;
		dobject_mark(o, MFLAG_LOCAL_MATRIX);
		dobject_mark_children(o, MFLAG_GLOBAL_MATRIX | MFLAG_GLOBAL_BOUNDS);
	}
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
	double scalex = luaL_checknumber(L, 2);
	double scaley = luaL_checknumber(L, 3);
	if((o->scalex != scalex) || (o->scaley != scaley))
	{
		dobject_mark_dirty(o);
		o->scalex = scalex;
		o->scaley = scaley;
		if((o->scalex == 1.0) && (o->scaley == 1.0))
			o->mflag &= ~MFLAG_SCALE;
		else
			o->mflag |= MFLAG_SCALE;
		dobject_mark(o, MFLAG_LOCAL_MATRIX);
		dobject_mark_children(o, MFLAG_GLOBAL_MATRIX | MFLAG_GLOBAL_BOUNDS);
	}
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
	double skewx = luaL_checknumber(L, 2) * (M_PI / 180.0);
	if(o->skewx != skewx)
	{
		dobject_mark_dirty(o);
		o->skewx = skewx;
		if((o->skewx == 0.0) && (o->skewy == 0.0))
			o->mflag &= ~MFLAG_SKEW;
		else
			o->mflag |= MFLAG_SKEW;
		dobject_mark(o, MFLAG_LOCAL_MATRIX);
		dobject_mark_children(o, MFLAG_GLOBAL_MATRIX | MFLAG_GLOBAL_BOUNDS);
	}
	return 0;
}

static int m_get_skew_x(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	lua_pushnumber(L, o->skewx * (180.0 / M_PI));
	return 1;
}

static int m_set_skew_y(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	double skewy = luaL_checknumber(L, 2) * (M_PI / 180.0);
	if(o->skewy != skewy)
	{
		dobject_mark_dirty(o);
		o->skewy = skewy;
		if((o->skewx == 0.0) && (o->skewy == 0.0))
			o->mflag &= ~MFLAG_SKEW;
		else
			o->mflag |= MFLAG_SKEW;
		dobject_mark(o, MFLAG_LOCAL_MATRIX);
		dobject_mark_children(o, MFLAG_GLOBAL_MATRIX | MFLAG_GLOBAL_BOUNDS);
	}
	return 0;
}

static int m_get_skew_y(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	lua_pushnumber(L, o->skewy * (180.0 / M_PI));
	return 1;
}

static int m_set_skew(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	double skewx = luaL_checknumber(L, 2) * (M_PI / 180.0);
	double skewy = luaL_checknumber(L, 3) * (M_PI / 180.0);
	if((o->skewx != skewx) || (o->skewy != skewy))
	{
		dobject_mark_dirty(o);
		o->skewx = skewx;
		o->skewy = skewy;
		if((o->skewx == 0.0) && (o->skewy == 0.0))
			o->mflag &= ~MFLAG_SKEW;
		else
			o->mflag |= MFLAG_SKEW;
		dobject_mark(o, MFLAG_LOCAL_MATRIX);
		dobject_mark_children(o, MFLAG_GLOBAL_MATRIX | MFLAG_GLOBAL_BOUNDS);
	}
	return 0;
}

static int m_get_skew(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	lua_pushnumber(L, o->skewx * (180.0 / M_PI));
	lua_pushnumber(L, o->skewy * (180.0 / M_PI));
	return 2;
}

static int m_set_archor(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	double anchorx = luaL_checknumber(L, 2);
	double anchory = luaL_checknumber(L, 3);
	if((o->anchorx != anchorx) || (o->anchory != anchory))
	{
		dobject_mark_dirty(o);
		o->anchorx = anchorx;
		o->anchory = anchory;
		if((o->anchorx == 0.0) && (o->anchory == 0.0))
			o->mflag &= ~MFLAG_ANCHOR;
		else
			o->mflag |= MFLAG_ANCHOR;
		dobject_mark(o, MFLAG_LOCAL_MATRIX);
		dobject_mark_children(o, MFLAG_GLOBAL_MATRIX | MFLAG_GLOBAL_BOUNDS);
	}
	return 0;
}

static int m_get_archor(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	lua_pushnumber(L, o->anchorx);
	lua_pushnumber(L, o->anchory);
	return 2;
}

static int m_set_background_color(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	struct color_t * c = luaL_checkudata(L, 2, MT_COLOR);
	if((o->bgcolor.r != c->r) || (o->bgcolor.g != c->g) || (o->bgcolor.b != c->b) || (o->bgcolor.a != c->a))
	{
		dobject_mark_dirty(o);
		memcpy(&o->bgcolor, c, sizeof(struct color_t));
	}
	return 0;
}

static int m_get_background_color(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	struct color_t * c = lua_newuserdata(L, sizeof(struct color_t));
	memcpy(c, &o->bgcolor, sizeof(struct color_t));
	luaL_setmetatable(L, MT_COLOR);
	return 1;
}

static int m_set_collider(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	const char * type = luaL_optstring(L, 2, "");
	double * p = NULL;
	int i, n = 0;
	if(o->ctype == COLLIDER_TYPE_POLYGON)
	{
		if((o->hit.polygon.length > 0) && o->hit.polygon.points)
		{
			free(o->hit.polygon.points);
			o->hit.polygon.points = NULL;
			o->hit.polygon.length = 0;
		}
	}
	switch(shash(type))
	{
	case 0xf679fe97: /* "circle" */
		o->ctype = COLLIDER_TYPE_CIRCLE;
		o->hit.circle.x = luaL_optnumber(L, 3, o->width / 2);
		o->hit.circle.y = luaL_optnumber(L, 4, o->height / 2);
		o->hit.circle.radius = luaL_optnumber(L, 5, (o->width < o->height ? o->width : o->height) / 2);
		break;
	case 0x66448f53: /* "ellipse" */
		o->ctype = COLLIDER_TYPE_ELLIPSE;
		o->hit.ellipse.x = luaL_optnumber(L, 3, o->width / 2);
		o->hit.ellipse.y = luaL_optnumber(L, 4, o->height / 2);
		o->hit.ellipse.width = luaL_optnumber(L, 5, o->width / 2);
		o->hit.ellipse.height = luaL_optnumber(L, 6, o->height / 2);
		break;
	case 0xe1f5207a: /* "rectangle" */
		o->ctype = COLLIDER_TYPE_RECTANGLE;
		o->hit.rectangle.x = luaL_optnumber(L, 3, 0);
		o->hit.rectangle.y = luaL_optnumber(L, 4, 0);
		o->hit.rectangle.width = luaL_optnumber(L, 5, o->width);
		o->hit.rectangle.height = luaL_optnumber(L, 6, o->height);
		break;
	case 0x4b99d0b8: /* "rounded-rectangle" */
		o->ctype = COLLIDER_TYPE_ROUNDED_RECTANGLE;
		o->hit.rounded_rectangle.x = luaL_optnumber(L, 3, 0);
		o->hit.rounded_rectangle.y = luaL_optnumber(L, 4, 0);
		o->hit.rounded_rectangle.width = luaL_optnumber(L, 5, o->width);
		o->hit.rounded_rectangle.height = luaL_optnumber(L, 6, o->height);
		o->hit.rounded_rectangle.radius = luaL_optnumber(L, 7, (o->width < o->height ? o->width : o->height) / 5);
		break;
	case 0xbc0d44cd: /* "polygon" */
		o->ctype = COLLIDER_TYPE_POLYGON;
		if(lua_istable(L, 3))
		{
			n = lua_rawlen(L, 3) & ~0x1;
			if(n > 0)
			{
				p = malloc(sizeof(double) * n);
				for(i = 0; i < n; i++)
				{
					lua_rawgeti(L, 3, i + 1);
					p[i] = luaL_checknumber(L, -1);
					lua_pop(L, 1);
				}
			}
		}
		o->hit.polygon.points = p;
		o->hit.polygon.length = n;
		break;
	default:
		o->ctype = COLLIDER_TYPE_NONE;
		break;
	}
	return 0;
}

static int m_get_collider(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	int i;
	switch(o->ctype)
	{
	case COLLIDER_TYPE_NONE:
		lua_pushstring(L, "none");
		return 1;
	case COLLIDER_TYPE_CIRCLE:
		lua_pushstring(L, "circle");
		lua_pushnumber(L, o->hit.circle.x);
		lua_pushnumber(L, o->hit.circle.y);
		lua_pushnumber(L, o->hit.circle.radius);
		return 4;
	case COLLIDER_TYPE_ELLIPSE:
		lua_pushstring(L, "ellipse");
		lua_pushnumber(L, o->hit.ellipse.x);
		lua_pushnumber(L, o->hit.ellipse.y);
		lua_pushnumber(L, o->hit.ellipse.width);
		lua_pushnumber(L, o->hit.ellipse.height);
		return 5;
	case COLLIDER_TYPE_RECTANGLE:
		lua_pushstring(L, "rectangle");
		lua_pushnumber(L, o->hit.rectangle.x);
		lua_pushnumber(L, o->hit.rectangle.y);
		lua_pushnumber(L, o->hit.rectangle.width);
		lua_pushnumber(L, o->hit.rectangle.height);
		return 5;
	case COLLIDER_TYPE_ROUNDED_RECTANGLE:
		lua_pushstring(L, "rounded-rectangle");
		lua_pushnumber(L, o->hit.rounded_rectangle.x);
		lua_pushnumber(L, o->hit.rounded_rectangle.y);
		lua_pushnumber(L, o->hit.rounded_rectangle.width);
		lua_pushnumber(L, o->hit.rounded_rectangle.height);
		lua_pushnumber(L, o->hit.rounded_rectangle.radius);
		return 6;
	case COLLIDER_TYPE_POLYGON:
		lua_pushstring(L, "polygon");
		lua_newtable(L);
		if((o->hit.polygon.length > 0) && o->hit.polygon.points)
		{
			for(i = 0; i < o->hit.polygon.length; i++)
			{
				lua_pushnumber(L, o->hit.polygon.points[i]);
				lua_rawseti(L, -2, i + 1);
			}
		}
		return 2;
	default:
		break;
	}
	return 0;
}

static int m_set_visible(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	int visible = lua_toboolean(L, 2);
	if(o->visible != visible)
	{
		dobject_mark_dirty(o);
		o->visible = visible;
	}
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
	o->touchable = lua_toboolean(L, 2);
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
	struct matrix_t * m = dobject_global_matrix(o);
	double id = 1.0 / (m->a * m->d - m->c * m->b);
	nx = ((x - m->tx) * m->d + (m->ty - y) * m->c) * id;
	ny = ((y - m->ty) * m->a + (m->tx - x) * m->b) * id;
	lua_pushnumber(L, nx);
	lua_pushnumber(L, ny);
	return 2;
}

static int m_local_to_global(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	double nx, x = luaL_checknumber(L, 2);
	double ny, y = luaL_checknumber(L, 3);
	struct matrix_t * m = dobject_global_matrix(o);
	nx = m->a * x + m->c * y + m->tx;
	ny = m->b * x + m->d * y + m->ty;
	lua_pushnumber(L, nx);
	lua_pushnumber(L, ny);
	return 2;
}

static inline int circle_hit_test_point(struct ldobject_t * o, double x, double y)
{
	double r = o->hit.circle.radius;
	double r2, dx, dy;
	if(r > 0)
	{
		r2 = r * r;
		dx = o->hit.circle.x - x;
		dy = o->hit.circle.y - y;
		dx *= dx;
		dy *= dy;
		return (dx + dy <= r2) ? 1 : 0;
	}
	return 0;
}

static inline int ellipse_hit_test_point(struct ldobject_t * o, double x, double y)
{
	double w = o->hit.ellipse.width;
	double h = o->hit.ellipse.height;
	double normx, normy;
	if((w > 0) && (h > 0))
	{
		normx = (x - o->hit.ellipse.x) / w;
		normy = (y - o->hit.ellipse.y) / h;
		normx *= normx;
		normy *= normy;
		return (normx + normy <= 1) ? 1 : 0;
	}
	return 0;
}

static inline int rectangle_hit_test_point(struct ldobject_t * o, double x, double y)
{
	double w = o->hit.rectangle.width;
	double h = o->hit.rectangle.height;
	double rx = o->hit.rectangle.x;
	double ry = o->hit.rectangle.y;
	if((w > 0) && (h > 0))
	{
		if((x >= rx) && (x < rx + w) && (y >= ry) && (y < ry + h))
			return 1;
	}
	return 0;
}

static inline int rounded_rectangle_hit_test_point(struct ldobject_t * o, double x, double y)
{
	double w = o->hit.rounded_rectangle.width;
	double h = o->hit.rounded_rectangle.height;
	double rx = o->hit.rounded_rectangle.x;
	double ry = o->hit.rounded_rectangle.y;
	double r = o->hit.rounded_rectangle.radius;
	double r2, dx, dy;
	if((w > 0) && (h > 0))
	{
		if((x >= rx) && (x <= rx + w))
		{
			if((y >= ry) && (y <= ry + h))
			{
				if(((y >= ry + r) && (y <= ry + h - r)) || ((x >= rx + r) && (x <= rx + w - r)))
					return 1;
				dx = x - (rx + r);
				dy = y - (ry + r);
				r2 = r * r;
				if(dx * dx + dy * dy <= r2)
					return 1;
				dx = x - (rx + w - r);
				if(dx * dx + dy * dy <= r2)
					return 1;
				dy = y - (ry + h - r);
				if(dx * dx + dy * dy <= r2)
					return 1;
				dx = x - (rx + r);
				if(dx * dx + dy * dy <= r2)
					return 1;
			}
		}
	}
	return 0;
}

static inline int polygon_hit_test_point(struct ldobject_t * o, double x, double y)
{
	double * p = o->hit.polygon.points;
	int n = o->hit.polygon.length / 2;
	int c = 0;
	int i, j;
	for(i = 0, j = n - 1; i < n; j = i++)
	{
		if(((p[(i << 1) + 1] > y) != (p[(j << 1) + 1] > y)) && (x < (p[j << 1] - p[i << 1]) * (y - p[(i << 1) + 1]) / (p[(j << 1) + 1] - p[(i << 1) + 1]) + p[i << 1]))
			c = !c;
	}
	return c;
}

static int m_hit_test_point(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	int hit = 0;
	if(o->visible && o->touchable)
	{
		double nx, x = luaL_checknumber(L, 2);
		double ny, y = luaL_checknumber(L, 3);
		struct matrix_t * m = dobject_global_matrix(o);
		double id = 1.0 / (m->a * m->d - m->c * m->b);
		nx = ((x - m->tx) * m->d + (m->ty - y) * m->c) * id;
		ny = ((y - m->ty) * m->a + (m->tx - x) * m->b) * id;
		switch(o->ctype)
		{
		case COLLIDER_TYPE_NONE:
			if((nx >= 0) && (nx < o->width) && (ny >= 0) && (ny < o->height))
				hit = 1;
			break;
		case COLLIDER_TYPE_CIRCLE:
			hit = circle_hit_test_point(o, nx, ny);
			break;
		case COLLIDER_TYPE_ELLIPSE:
			hit = ellipse_hit_test_point(o, nx, ny);
			break;
		case COLLIDER_TYPE_RECTANGLE:
			hit = rectangle_hit_test_point(o, nx, ny);
			break;
		case COLLIDER_TYPE_ROUNDED_RECTANGLE:
			hit = rounded_rectangle_hit_test_point(o, nx, ny);
			break;
		case COLLIDER_TYPE_POLYGON:
			hit = polygon_hit_test_point(o, nx, ny);
			break;
		default:
			break;
		}
	}
	lua_pushboolean(L, hit);
	return 1;
}

static int m_mark_dirty(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	dobject_mark_dirty(o);
	return 0;
}

static int m_get_bounds(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	struct region_t * r = dobject_global_bounds(o);
	lua_pushnumber(L, r->x);
	lua_pushnumber(L, r->y);
	lua_pushnumber(L, r->w);
	lua_pushnumber(L, r->h);
	return 4;
}

static void window_region_list_fill(struct window_t * w, struct ldobject_t * o)
{
	struct ldobject_t * pos;

	if(o->mflag & MFLAG_DIRTY)
	{
		window_region_list_add(w, dobject_global_bounds(o));
		window_region_list_add(w, dobject_dirty_bounds(o));
		o->mflag &= ~MFLAG_DIRTY;
	}

	list_for_each_entry(pos, &o->children, entry)
	{
		window_region_list_fill(w, pos);
	}
}

static void display_draw(struct window_t * w, struct ldobject_t * o)
{
	struct ldobject_t * pos;

	if(o->visible)
	{
		o->draw(o, w);
		list_for_each_entry(pos, &o->children, entry)
		{
			display_draw(w, pos);
		}
	}
}

static int m_render(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	struct window_t * w = luaL_checkudata(L, 2, MT_WINDOW);
	if(window_is_active(w))
	{
		window_region_list_clear(w);
		window_region_list_fill(w, o);
		window_present(w, o, (void (*)(struct window_t *, void *))display_draw);
	}
	return 0;
}

static const luaL_Reg m_dobject[] = {
	{"__gc",				m_dobject_gc},
	{"addChild",			m_add_child},
	{"removeChild",			m_remove_child},
	{"toFront",				m_to_front},
	{"toBack",				m_to_back},
	{"setWidth",			m_set_width},
	{"getWidth",			m_get_width},
	{"setHeight",			m_set_height},
	{"getHeight",			m_get_height},
	{"setSize",				m_set_size},
	{"getSize",				m_get_size},
	{"setX",				m_set_x},
	{"getX",				m_get_x},
	{"setY",				m_set_y},
	{"getY",				m_get_y},
	{"setPosition",			m_set_position},
	{"getPosition",			m_get_position},
	{"setRotation",			m_set_rotation},
	{"getRotation",			m_get_rotation},
	{"setScaleX",			m_set_scale_x},
	{"getScaleX",			m_get_scale_x},
	{"setScaleY",			m_set_scale_y},
	{"getScaleY",			m_get_scale_y},
	{"setScale",			m_set_scale},
	{"getScale",			m_get_scale},
	{"setSkewX",			m_set_skew_x},
	{"getSkewX",			m_get_skew_x},
	{"setSkewY",			m_set_skew_y},
	{"getSkewY",			m_get_skew_y},
	{"setSkew",				m_set_skew},
	{"getSkew",				m_get_skew},
	{"setAnchor",			m_set_archor},
	{"getAnchor",			m_get_archor},
	{"setBackgroundColor",	m_set_background_color},
	{"getBackgroundColor",	m_get_background_color},
	{"setCollider",			m_set_collider},
	{"getCollider",			m_get_collider},
	{"setVisible",			m_set_visible},
	{"getVisible",			m_get_visible},
	{"setTouchable",		m_set_touchable},
	{"getTouchable",		m_get_touchable},
	{"globalToLocal",		m_global_to_local},
	{"localToGlobal",		m_local_to_global},
	{"hitTestPoint",		m_hit_test_point},
	{"markDirty",			m_mark_dirty},
	{"getBounds",			m_get_bounds},
	{"render",				m_render},
	{NULL, NULL}
};

int luaopen_dobject(lua_State * L)
{
	luaL_newlib(L, l_dobject);
	luahelper_create_metatable(L, MT_DOBJECT, m_dobject);
	return 1;
}
