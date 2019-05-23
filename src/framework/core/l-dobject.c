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
#include <framework/core/l-window.h>
#include <framework/core/l-dobject.h>

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

enum layout_direction_t {
	LAYOUT_DIRECTION_ROW 			= 0,
	LAYOUT_DIRECTION_ROW_REVERSE	= 1,
	LAYOUT_DIRECTION_COLUMN			= 2,
	LAYOUT_DIRECTION_COLUMN_REVERSE	= 3,
};

enum layout_justify_t {
	LAYOUT_JUSTIFY_START			= 0,
	LAYOUT_JUSTIFY_END				= 1,
	LAYOUT_JUSTIFY_CENTER			= 2,
	LAYOUT_JUSTIFY_BETWEEN			= 3,
	LAYOUT_JUSTIFY_AROUND			= 4,
	LAYOUT_JUSTIFY_EVENLY			= 5,
};

enum layout_align_t {
	LAYOUT_ALIGN_START				= 0,
	LAYOUT_ALIGN_END				= 1,
	LAYOUT_ALIGN_CENTER				= 2,
	LAYOUT_ALIGN_STRETCH			= 3,
};

enum layout_align_self_t {
	LAYOUT_ALIGN_SELF_AUTO			= 0,
	LAYOUT_ALIGN_SELF_START			= 1,
	LAYOUT_ALIGN_SELF_END			= 2,
	LAYOUT_ALIGN_SELF_CENTER		= 3,
	LAYOUT_ALIGN_SELF_STRETCH		= 4,
};

static inline void dobject_layout_set_enable(struct ldobject_t * o, int enable)
{
	o->layout.style &= ~(0x1 << 0);
	o->layout.style |= (enable ? 1 : 0) << 0;
}

static inline int dobject_layout_get_enable(struct ldobject_t * o)
{
	return (o->layout.style >> 0) & 0x1;
}

static inline void dobject_layout_set_special(struct ldobject_t * o, int enable)
{
	o->layout.style &= ~(0x1 << 1);
	o->layout.style |= (enable ? 1 : 0) << 1;
}

static inline int dobject_layout_get_special(struct ldobject_t * o)
{
	return (o->layout.style >> 1) & 0x1;
}

static inline void dobject_layout_set_direction(struct ldobject_t * o, enum layout_direction_t direction)
{
	o->layout.style &= ~(0x3 << 2);
	o->layout.style |= direction << 2;
}

static inline enum layout_direction_t dobject_layout_get_direction(struct ldobject_t * o)
{
	return (o->layout.style >> 2) & 0x3;
}

static inline void dobject_layout_set_justify(struct ldobject_t * o, enum layout_justify_t justify)
{
	o->layout.style &= ~(0xf << 4);
	o->layout.style |= justify << 4;
}

static inline enum layout_justify_t dobject_layout_get_justify(struct ldobject_t * o)
{
	return (o->layout.style >> 4) & 0xf;
}

static inline void dobject_layout_set_align(struct ldobject_t * o, enum layout_align_t align)
{
	o->layout.style &= ~(0xf << 8);
	o->layout.style |= align << 8;
}

static inline enum layout_align_t dobject_layout_get_align(struct ldobject_t * o)
{
	return (o->layout.style >> 8) & 0xf;
}

static inline void dobject_layout_set_align_self(struct ldobject_t * o, enum layout_align_self_t align)
{
	o->layout.style &= ~(0xf << 12);
	o->layout.style |= align << 12;
}

static inline enum layout_align_self_t dobject_layout_get_align_self(struct ldobject_t * o)
{
	return (o->layout.style >> 12) & 0xf;
}

static inline double dobject_layout_main_leading_margin(struct ldobject_t * o)
{
	struct ldobject_t * parent = o->parent;
	if(parent)
	{
		switch(dobject_layout_get_direction(parent))
		{
		case LAYOUT_DIRECTION_ROW:
		case LAYOUT_DIRECTION_ROW_REVERSE:
			return o->layout.margin.left;
		case LAYOUT_DIRECTION_COLUMN:
		case LAYOUT_DIRECTION_COLUMN_REVERSE:
			return o->layout.margin.top;
		default:
			break;
		}
	}
	return o->layout.margin.left;
}

static inline double dobject_layout_cross_leading_margin(struct ldobject_t * o)
{
	struct ldobject_t * parent = o->parent;
	if(parent)
	{
		switch(dobject_layout_get_direction(parent))
		{
		case LAYOUT_DIRECTION_ROW:
		case LAYOUT_DIRECTION_ROW_REVERSE:
			return o->layout.margin.top;
		case LAYOUT_DIRECTION_COLUMN:
		case LAYOUT_DIRECTION_COLUMN_REVERSE:
			return o->layout.margin.left;
		default:
			break;
		}
	}
	return o->layout.margin.top;
}

static inline double dobject_layout_main_trailing_margin(struct ldobject_t * o)
{
	struct ldobject_t * parent = o->parent;
	if(parent)
	{
		switch(dobject_layout_get_direction(parent))
		{
		case LAYOUT_DIRECTION_ROW:
		case LAYOUT_DIRECTION_ROW_REVERSE:
			return o->layout.margin.right;
		case LAYOUT_DIRECTION_COLUMN:
		case LAYOUT_DIRECTION_COLUMN_REVERSE:
			return o->layout.margin.bottom;
		default:
			break;
		}
	}
	return o->layout.margin.right;
}

static inline double dobject_layout_cross_trailing_margin(struct ldobject_t * o)
{
	struct ldobject_t * parent = o->parent;
	if(parent)
	{
		switch(dobject_layout_get_direction(parent))
		{
		case LAYOUT_DIRECTION_ROW:
		case LAYOUT_DIRECTION_ROW_REVERSE:
			return o->layout.margin.bottom;
		case LAYOUT_DIRECTION_COLUMN:
		case LAYOUT_DIRECTION_COLUMN_REVERSE:
			return o->layout.margin.right;
		default:
			break;
		}
	}
	return o->layout.margin.bottom;
}

static inline double dobject_layout_main_margin(struct ldobject_t * o)
{
	struct ldobject_t * parent = o->parent;
	if(parent)
	{
		switch(dobject_layout_get_direction(parent))
		{
		case LAYOUT_DIRECTION_ROW:
		case LAYOUT_DIRECTION_ROW_REVERSE:
			return o->layout.margin.left + o->layout.margin.right;
		case LAYOUT_DIRECTION_COLUMN:
		case LAYOUT_DIRECTION_COLUMN_REVERSE:
			return o->layout.margin.top + o->layout.margin.bottom;
		default:
			break;
		}
	}
	return o->layout.margin.left + o->layout.margin.right;
}

static inline double dobject_layout_cross_margin(struct ldobject_t * o)
{
	struct ldobject_t * parent = o->parent;
	if(parent)
	{
		switch(dobject_layout_get_direction(parent))
		{
		case LAYOUT_DIRECTION_ROW:
		case LAYOUT_DIRECTION_ROW_REVERSE:
			return o->layout.margin.top + o->layout.margin.bottom;
		case LAYOUT_DIRECTION_COLUMN:
		case LAYOUT_DIRECTION_COLUMN_REVERSE:
			return o->layout.margin.left + o->layout.margin.right;
		default:
			break;
		}
	}
	return o->layout.margin.top + o->layout.margin.bottom;
}

static inline double dobject_layout_main_size(struct ldobject_t * o)
{
	struct ldobject_t * parent = o->parent;
	double basis = o->layout.basis;
	if(parent)
	{
		switch(dobject_layout_get_direction(parent))
		{
		case LAYOUT_DIRECTION_ROW:
		case LAYOUT_DIRECTION_ROW_REVERSE:
			if(basis <= 0.0)
			{
				if(isnan(o->layout.width))
					o->layout.width = o->width;
				basis = o->layout.width;
			}
			else if(basis <= 1.0)
			{
				basis = parent->width * basis;
			}
			break;
		case LAYOUT_DIRECTION_COLUMN:
		case LAYOUT_DIRECTION_COLUMN_REVERSE:
			if(basis <= 0.0)
			{
				if(isnan(o->layout.height))
					o->layout.height = o->height;
				basis = o->layout.height;
			}
			else if(basis <= 1.0)
			{
				basis = parent->height * basis;
			}
			break;
		default:
			break;
		}
	}
	return basis;
}

static inline double dobject_layout_cross_size(struct ldobject_t * o)
{
	struct ldobject_t * parent = o->parent;
	if(parent)
	{
		switch(dobject_layout_get_direction(parent))
		{
		case LAYOUT_DIRECTION_ROW:
		case LAYOUT_DIRECTION_ROW_REVERSE:
			if(isnan(o->layout.height))
				o->layout.height = o->height;
			return o->layout.height;
		case LAYOUT_DIRECTION_COLUMN:
		case LAYOUT_DIRECTION_COLUMN_REVERSE:
			if(isnan(o->layout.width))
				o->layout.width = o->width;
			return o->layout.width;
		default:
			break;
		}
	}
	if(isnan(o->layout.height))
		o->layout.height = o->height;
	return o->layout.height;
}

static inline double dobject_layout_container_main_size(struct ldobject_t * o)
{
	switch(dobject_layout_get_direction(o))
	{
	case LAYOUT_DIRECTION_ROW:
	case LAYOUT_DIRECTION_ROW_REVERSE:
		return o->width;
	case LAYOUT_DIRECTION_COLUMN:
	case LAYOUT_DIRECTION_COLUMN_REVERSE:
		return o->height;
	default:
		break;
	}
	return o->width;
}

static inline double dobject_layout_container_cross_size(struct ldobject_t * o)
{
	switch(dobject_layout_get_direction(o))
	{
	case LAYOUT_DIRECTION_ROW:
	case LAYOUT_DIRECTION_ROW_REVERSE:
		return o->height;
	case LAYOUT_DIRECTION_COLUMN:
	case LAYOUT_DIRECTION_COLUMN_REVERSE:
		return o->width;
	default:
		break;
	}
	return o->height;
}

static void dobject_layout(struct ldobject_t * o)
{
	struct ldobject_t * pos;
	double consumed, grow, shrink, cms, ccs;
	double space, offset, between;
	double basis, ms, cp, cs;
	enum layout_direction_t direction;
	enum layout_align_t align;
	int count, n;

	if(list_empty(&o->children))
		return;

	consumed = 0;
	grow = 0;
	shrink = 0;
	count = 0;
	n = 0;
	list_for_each_entry(pos, &o->children, entry)
	{
		if(dobject_layout_get_enable(pos))
		{
			if(dobject_layout_get_special(pos))
			{
				n++;
			}
			else
			{
				basis = dobject_layout_main_size(pos);
				consumed += basis + dobject_layout_main_margin(pos);
				grow += pos->layout.grow;
				shrink += pos->layout.shrink * basis;
				count++;
			}
		}
	}

	if((count > 0) || (n > 0))
	{
		cms = dobject_layout_container_main_size(o);
		ccs = dobject_layout_container_cross_size(o);
		direction = dobject_layout_get_direction(o);
		align = dobject_layout_get_align(o);
		space = cms - consumed;
		offset = 0;
		between = 0;
		if((space > 0) && (grow == 0))
		{
			switch(dobject_layout_get_justify(o))
			{
			case LAYOUT_JUSTIFY_START:
				break;
			case LAYOUT_JUSTIFY_END:
				offset = space;
				break;
			case LAYOUT_JUSTIFY_CENTER:
				offset = space / 2;
				break;
			case LAYOUT_JUSTIFY_BETWEEN:
				if(count > 1)
					between = space / (count - 1);
				break;
			case LAYOUT_JUSTIFY_AROUND:
				if(count > 0)
					between = space / count;
				offset = between / 2;
				break;
			case LAYOUT_JUSTIFY_EVENLY:
				if(count > 0)
					between = space / (count + 1);
				offset = between;
				break;
			default:
				break;
			}
		}

		list_for_each_entry(pos, &o->children, entry)
		{
			if(dobject_layout_get_enable(pos))
			{
				if(dobject_layout_get_special(pos))
				{
					pos->layout.x = 0;
					pos->layout.y = 0;
					pos->layout.w = o->width;
					pos->layout.h = o->height;
				}
				else
				{
					switch(dobject_layout_get_align_self(pos))
					{
					case LAYOUT_ALIGN_SELF_AUTO:
						break;
					case LAYOUT_ALIGN_SELF_START:
						align = LAYOUT_ALIGN_START;
						break;
					case LAYOUT_ALIGN_SELF_END:
						align = LAYOUT_ALIGN_END;
						break;
					case LAYOUT_ALIGN_SELF_CENTER:
						align = LAYOUT_ALIGN_CENTER;
						break;
					case LAYOUT_ALIGN_SELF_STRETCH:
						align = LAYOUT_ALIGN_STRETCH;
						break;
					default:
						break;
					}

					switch(align)
					{
					case LAYOUT_ALIGN_START:
						cs = dobject_layout_cross_size(pos);
						cp = dobject_layout_cross_leading_margin(pos);
						break;
					case LAYOUT_ALIGN_END:
						cs = dobject_layout_cross_size(pos);
						cp = ccs - (cs + dobject_layout_cross_trailing_margin(pos));
						break;
					case LAYOUT_ALIGN_CENTER:
						cs = dobject_layout_cross_size(pos);
						cp = (ccs - (cs + dobject_layout_cross_margin(pos))) / 2 + dobject_layout_cross_leading_margin(pos);
						break;
					case LAYOUT_ALIGN_STRETCH:
						cs = ccs - dobject_layout_cross_margin(pos);
						cp = dobject_layout_cross_leading_margin(pos);
						break;
					default:
						cs = dobject_layout_cross_size(pos);
						cp = dobject_layout_cross_leading_margin(pos);
						break;
					}

					ms = basis = dobject_layout_main_size(pos);
					if((space >= 0) && (pos->layout.grow > 0))
						ms += space * (pos->layout.grow / grow);
					else if((space < 0) && (pos->layout.shrink > 0))
						ms += space * (pos->layout.shrink * basis / shrink);

					switch(direction)
					{
					case LAYOUT_DIRECTION_ROW:
						pos->layout.h = cs;
						pos->layout.y = cp;
						pos->layout.w = ms;
						pos->layout.x = offset + dobject_layout_main_leading_margin(pos);
						break;
					case LAYOUT_DIRECTION_ROW_REVERSE:
						pos->layout.h = cs;
						pos->layout.y = cp;
						pos->layout.w = ms;
						pos->layout.x = cms - (offset + dobject_layout_main_trailing_margin(pos) + ms);
						break;
					case LAYOUT_DIRECTION_COLUMN:
						pos->layout.w = cs;
						pos->layout.x = cp;
						pos->layout.h = ms;
						pos->layout.y = offset + dobject_layout_main_leading_margin(pos);
						break;
					case LAYOUT_DIRECTION_COLUMN_REVERSE:
						pos->layout.w = cs;
						pos->layout.x = cp;
						pos->layout.h = ms;
						pos->layout.y = cms - (offset + dobject_layout_main_trailing_margin(pos) + ms);
						break;
					default:
						break;
					}
					offset = offset + ms + dobject_layout_main_margin(pos) + between;
				}

				pos->layout.x = round(pos->layout.x);
				pos->layout.y = round(pos->layout.y);
				pos->layout.w = round(pos->layout.w);
				pos->layout.h = round(pos->layout.h);
				if(pos->layout.w < 1)
					pos->layout.w = 1;
				if(pos->layout.h < 1)
					pos->layout.h = 1;
			}
		}
	}

	list_for_each_entry(pos, &o->children, entry)
	{
		if(dobject_layout_get_enable(pos))
		{
			double width, height;
			double scalex, scaley;

			switch(pos->dtype)
			{
			case DOBJECT_TYPE_CONTAINER:
				width = pos->layout.w;
				height = pos->layout.h;
				scalex = 1.0;
				scaley = 1.0;
				break;
			case DOBJECT_TYPE_IMAGE:
				width = pos->width;
				height = pos->height;
				if(width != 0.0 && height != 0.0)
				{
					scalex = pos->layout.w / width;
					scaley = pos->layout.h / height;
				}
				else
				{
					scalex = pos->scalex;
					scaley = pos->scaley;
				}
				break;
			case DOBJECT_TYPE_NINEPATCH:
				width = pos->layout.w;
				height = pos->layout.h;
				scalex = 1.0;
				scaley = 1.0;
				ninepatch_stretch(pos->priv, width, height);
				break;
			case DOBJECT_TYPE_SHAPE:
				width = pos->width;
				height = pos->height;
				if(width != 0.0 && height != 0.0)
				{
					scalex = pos->layout.w / width;
					scaley = pos->layout.h / height;
				}
				else
				{
					scalex = pos->scalex;
					scaley = pos->scaley;
				}
				break;
			case DOBJECT_TYPE_TEXT:
				width = pos->width;
				height = pos->height;
				if(width != 0.0 && height != 0.0)
				{
					scalex = pos->layout.w / width;
					scaley = pos->layout.h / height;
				}
				else
				{
					scalex = pos->scalex;
					scaley = pos->scaley;
				}
				break;
			default:
				width = pos->width;
				height = pos->height;
				scalex = pos->scalex;
				scaley = pos->scaley;
				break;
			}

			if(pos->width != width || pos->height != height || pos->x != pos->layout.x || pos->y != pos->layout.y || pos->rotation != 0 ||
				pos->scalex != scalex || pos->scaley != scaley || pos->skewx != 0 || pos->skewy != 0 || pos->anchorx != 0 || pos->anchory != 0)
			{
				dobject_mark_dirty(pos);
			}
			pos->width = width;
			pos->height = height;
			pos->x = pos->layout.x;
			pos->y = pos->layout.y;
			pos->rotation = 0.0;
			pos->scalex = scalex;
			pos->scaley = scaley;
			pos->skewx = 0.0;
			pos->skewy = 0.0;
			pos->anchorx = 0.0;
			pos->anchory = 0.0;
			pos->mflag &= ~(MFLAG_TRANSLATE | MFLAG_ROTATE | MFLAG_SCALE | MFLAG_SKEW | MFLAG_ANCHOR | MFLAG_LOCAL_MATRIX | MFLAG_GLOBAL_MATRIX | MFLAG_GLOBAL_BOUNDS);
			pos->mflag |= MFLAG_LOCAL_MATRIX | MFLAG_GLOBAL_MATRIX | MFLAG_GLOBAL_BOUNDS;
			if((pos->x == 0.0) && (pos->y == 0.0))
				pos->mflag &= ~MFLAG_TRANSLATE;
			else
				pos->mflag |= MFLAG_TRANSLATE;
			if((pos->scalex == 1.0) && (pos->scaley == 1.0))
				pos->mflag &= ~MFLAG_SCALE;
			else
				pos->mflag |= MFLAG_SCALE;
		}
		dobject_layout(pos);
	}
}

static void dobject_draw_image(struct ldobject_t * o, struct window_t * w)
{
	struct limage_t * img = o->priv;
	cairo_t * cr = w->cr;
	cairo_save(cr);
	cairo_set_matrix(cr, (cairo_matrix_t *)dobject_global_matrix(o));
	cairo_set_source_surface(cr, img->cs, 0, 0);
	cairo_paint_with_alpha(cr, o->alpha);
	cairo_restore(cr);
}

static void dobject_draw_ninepatch(struct ldobject_t * o, struct window_t * w)
{
	struct lninepatch_t * ninepatch = o->priv;
	cairo_t * cr = w->cr;
	cairo_save(cr);
	cairo_set_matrix(cr, (cairo_matrix_t *)dobject_global_matrix(o));
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
		cairo_pattern_set_filter(cairo_get_source(cr), CAIRO_FILTER_FAST);
		cairo_paint_with_alpha(cr, o->alpha);
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
		cairo_pattern_set_filter(cairo_get_source(cr), CAIRO_FILTER_FAST);
		cairo_paint_with_alpha(cr, o->alpha);
		cairo_restore(cr);
	}
	cairo_restore(cr);
}

static void dobject_draw_shape(struct ldobject_t * o, struct window_t * w)
{
	struct lshape_t * shape = o->priv;
	cairo_t * cr = w->cr;
	cairo_save(cr);
	cairo_set_matrix(cr, (cairo_matrix_t *)dobject_global_matrix(o));
	cairo_set_source_surface(cr, shape->cs, 0, 0);
	cairo_paint_with_alpha(cr, o->alpha);
	cairo_restore(cr);
}

static void dobject_draw_text(struct ldobject_t * o, struct window_t * w)
{
	struct ltext_t * text = o->priv;
	struct matrix_t * m = dobject_global_matrix(o);
	cairo_t * cr = w->cr;
	cairo_save(cr);
	cairo_set_scaled_font(cr, text->font);
	cairo_move_to(cr, m->tx, m->ty);
	cairo_set_matrix(cr, (cairo_matrix_t *)m);
	cairo_move_to(cr, 0, text->metric.height);
	cairo_text_path(cr, text->utf8);
	cairo_set_source(cr, text->pattern);
	cairo_fill(cr);
	cairo_restore(cr);
}

static void dobject_draw_container(struct ldobject_t * o, struct window_t * w)
{
	if(o->background.alpha != 0.0)
	{
		cairo_t * cr = w->cr;
		cairo_save(cr);
		cairo_set_matrix(cr, (cairo_matrix_t *)dobject_global_matrix(o));
		cairo_rectangle(cr, 0, 0, o->width, o->height);
		cairo_set_source_rgba(cr, o->background.red, o->background.green, o->background.blue, o->background.alpha);
		cairo_fill(cr);
		cairo_restore(cr);
	}
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
	else if(luaL_testudata(L, 3, MT_SHAPE))
	{
		dtype = DOBJECT_TYPE_SHAPE;
		draw = dobject_draw_shape;
		userdata = lua_touserdata(L, 3);
	}
	else if(luaL_testudata(L, 3, MT_TEXT))
	{
		dtype = DOBJECT_TYPE_TEXT;
		draw = dobject_draw_text;
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
	o->alpha = 1;
	o->background.red = 0;
	o->background.green = 0;
	o->background.blue = 0;
	o->background.alpha = 0;
	o->layout.style = 0;
	o->layout.grow = 0;
	o->layout.shrink = 1;
	o->layout.basis = 0;
	o->layout.width = NAN;
	o->layout.height = NAN;
	o->layout.margin.left = 0;
	o->layout.margin.top = 0;
	o->layout.margin.right = 0;
	o->layout.margin.bottom = 0;
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
		o->layout.width = NAN;
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
		o->layout.height = NAN;
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
		o->layout.width = NAN;
		o->layout.height = NAN;
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

static int m_set_alpha(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	double alpha = luaL_checknumber(L, 2);
	if(o->alpha != alpha)
	{
		dobject_mark_dirty(o);
		o->alpha = alpha;
	}
	return 0;
}

static int m_get_alpha(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	lua_pushnumber(L, o->alpha);
	return 1;
}

static int m_set_background_color(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	double red = luaL_optnumber(L, 2, 1);
	double green = luaL_optnumber(L, 3, 1);
	double blue = luaL_optnumber(L, 4, 1);
	double alpha = luaL_optnumber(L, 5, 1);
	if((o->background.red != red) || (o->background.green != green) || (o->background.blue != blue) || (o->background.alpha != alpha))
	{
		dobject_mark_dirty(o);
		o->background.red = red;
		o->background.green = green;
		o->background.blue = blue;
		o->background.alpha = alpha;
	}
	return 0;
}

static int m_get_background_color(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	lua_pushnumber(L, o->background.red);
	lua_pushnumber(L, o->background.green);
	lua_pushnumber(L, o->background.blue);
	lua_pushnumber(L, o->background.alpha);
	return 4;
}

static int m_set_layout_enable(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	dobject_layout_set_enable(o, lua_toboolean(L, 2));
	return 0;
}

static int m_get_layout_enable(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	lua_pushboolean(L, dobject_layout_get_enable(o));
	return 1;
}

static int m_set_layout_special(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	dobject_layout_set_special(o, lua_toboolean(L, 2));
	return 0;
}

static int m_get_layout_special(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	lua_pushboolean(L, dobject_layout_get_special(o));
	return 1;
}

static int m_set_layout_direction(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	const char * type = luaL_optstring(L, 2, "row");
	switch(shash(type))
	{
	case 0x0b88a69d: /* "row" */
		dobject_layout_set_direction(o, LAYOUT_DIRECTION_ROW);
		break;
	case 0xf84b1686: /* "row-reverse" */
		dobject_layout_set_direction(o, LAYOUT_DIRECTION_ROW_REVERSE);
		break;
	case 0xf6e39413: /* "column" */
		dobject_layout_set_direction(o, LAYOUT_DIRECTION_COLUMN);
		break;
	case 0x839f19fc: /* "column-reverse" */
		dobject_layout_set_direction(o, LAYOUT_DIRECTION_COLUMN_REVERSE);
		break;
	default:
		break;
	}
	return 0;
}

static int m_get_layout_direction(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	switch(dobject_layout_get_direction(o))
	{
	case LAYOUT_DIRECTION_ROW:
		lua_pushstring(L, "row");
		break;
	case LAYOUT_DIRECTION_ROW_REVERSE:
		lua_pushstring(L, "row-reverse");
		break;
	case LAYOUT_DIRECTION_COLUMN:
		lua_pushstring(L, "column");
		break;
	case LAYOUT_DIRECTION_COLUMN_REVERSE:
		lua_pushstring(L, "column-reverse");
		break;
	default:
		lua_pushnil(L);
		break;
	}
	return 1;
}

static int m_set_layout_justify(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	const char * type = luaL_optstring(L, 2, "start");
	switch(shash(type))
	{
	case 0x106149d3: /* "start" */
		dobject_layout_set_justify(o, LAYOUT_JUSTIFY_START);
		break;
	case 0x0b886f1c: /* "end" */
		dobject_layout_set_justify(o, LAYOUT_JUSTIFY_END);
		break;
	case 0xf62fb286: /* "center" */
		dobject_layout_set_justify(o, LAYOUT_JUSTIFY_CENTER);
		break;
	case 0x6f99fd6f: /* "between" */
		dobject_layout_set_justify(o, LAYOUT_JUSTIFY_BETWEEN);
		break;
	case 0xf271318e: /* "around" */
		dobject_layout_set_justify(o, LAYOUT_JUSTIFY_AROUND);
		break;
	case 0xfc089c58: /* "evenly" */
		dobject_layout_set_justify(o, LAYOUT_JUSTIFY_EVENLY);
		break;
	default:
		break;
	}
	return 0;
}

static int m_get_layout_justify(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	switch(dobject_layout_get_justify(o))
	{
	case LAYOUT_JUSTIFY_START:
		lua_pushstring(L, "start");
		break;
	case LAYOUT_JUSTIFY_END:
		lua_pushstring(L, "end");
		break;
	case LAYOUT_JUSTIFY_CENTER:
		lua_pushstring(L, "center");
		break;
	case LAYOUT_JUSTIFY_BETWEEN:
		lua_pushstring(L, "between");
		break;
	case LAYOUT_JUSTIFY_AROUND:
		lua_pushstring(L, "around");
		break;
	case LAYOUT_JUSTIFY_EVENLY:
		lua_pushstring(L, "evenly");
		break;
	default:
		lua_pushnil(L);
		break;
	}
	return 1;
}

static int m_set_layout_align(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	const char * type = luaL_optstring(L, 2, "start");
	switch(shash(type))
	{
	case 0x106149d3: /* "start" */
		dobject_layout_set_align(o, LAYOUT_ALIGN_START);
		break;
	case 0x0b886f1c: /* "end" */
		dobject_layout_set_align(o, LAYOUT_ALIGN_END);
		break;
	case 0xf62fb286: /* "center" */
		dobject_layout_set_align(o, LAYOUT_ALIGN_CENTER);
		break;
	case 0xaf079762: /* "stretch" */
		dobject_layout_set_align(o, LAYOUT_ALIGN_STRETCH);
		break;
	default:
		break;
	}
	return 0;
}

static int m_get_layout_align(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	switch(dobject_layout_get_align(o))
	{
	case LAYOUT_ALIGN_START:
		lua_pushstring(L, "start");
		break;
	case LAYOUT_ALIGN_END:
		lua_pushstring(L, "end");
		break;
	case LAYOUT_ALIGN_CENTER:
		lua_pushstring(L, "center");
		break;
	case LAYOUT_ALIGN_STRETCH:
		lua_pushstring(L, "stretch");
		break;
	default:
		lua_pushnil(L);
		break;
	}
	return 1;
}

static int m_set_layout_align_self(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	const char * type = luaL_optstring(L, 2, "auto");
	switch(shash(type))
	{
	case 0x7c94415e: /* "auto" */
		dobject_layout_set_align_self(o, LAYOUT_ALIGN_SELF_AUTO);
		break;
	case 0x106149d3: /* "start" */
		dobject_layout_set_align_self(o, LAYOUT_ALIGN_SELF_START);
		break;
	case 0x0b886f1c: /* "end" */
		dobject_layout_set_align_self(o, LAYOUT_ALIGN_SELF_END);
		break;
	case 0xf62fb286: /* "center" */
		dobject_layout_set_align_self(o, LAYOUT_ALIGN_SELF_CENTER);
		break;
	case 0xaf079762: /* "stretch" */
		dobject_layout_set_align_self(o, LAYOUT_ALIGN_SELF_STRETCH);
		break;
	default:
		break;
	}
	return 0;
}

static int m_get_layout_align_self(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	switch(dobject_layout_get_align_self(o))
	{
	case LAYOUT_ALIGN_SELF_AUTO:
		lua_pushstring(L, "auto");
		break;
	case LAYOUT_ALIGN_SELF_START:
		lua_pushstring(L, "start");
		break;
	case LAYOUT_ALIGN_SELF_END:
		lua_pushstring(L, "end");
		break;
	case LAYOUT_ALIGN_SELF_CENTER:
		lua_pushstring(L, "center");
		break;
	case LAYOUT_ALIGN_SELF_STRETCH:
		lua_pushstring(L, "stretch");
		break;
	default:
		lua_pushnil(L);
		break;
	}
	return 1;
}

static int m_set_layout_grow(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	o->layout.grow = luaL_checknumber(L, 2);
	return 0;
}

static int m_get_layout_grow(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	lua_pushnumber(L, o->layout.grow);
	return 1;
}

static int m_set_layout_shrink(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	o->layout.shrink = luaL_checknumber(L, 2);
	return 0;
}

static int m_get_layout_shrink(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	lua_pushnumber(L, o->layout.shrink);
	return 1;
}

static int m_set_layout_basis(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	o->layout.basis = luaL_checknumber(L, 2);
	return 0;
}

static int m_get_layout_basis(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	lua_pushnumber(L, o->layout.basis);
	return 1;
}

static int m_set_layout_margin(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	o->layout.margin.left = luaL_optnumber(L, 2, 0);
	o->layout.margin.top = luaL_optnumber(L, 3, 0);
	o->layout.margin.right = luaL_optnumber(L, 4, 0);
	o->layout.margin.bottom = luaL_optnumber(L, 5, 0);
	return 0;
}

static int m_get_layout_margin(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	lua_pushnumber(L, o->layout.margin.left);
	lua_pushnumber(L, o->layout.margin.top);
	lua_pushnumber(L, o->layout.margin.right);
	lua_pushnumber(L, o->layout.margin.bottom);
	return 4;
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
	double visible = lua_toboolean(L, 2);
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
		struct ldobject_t * parent = o->parent;
		cairo_t * cr = w->cr;
		cairo_save(cr);
		if(parent && (parent->dtype == DOBJECT_TYPE_CONTAINER))
		{
			cairo_set_matrix(cr, (cairo_matrix_t *)dobject_global_matrix(parent));
			cairo_rectangle(cr, 0, 0, parent->width, parent->height);
			cairo_clip(cr);
		}
		o->draw(o, w);
		if(w->showobj)
		{
			cairo_save(cr);
			cairo_set_matrix(cr, (cairo_matrix_t *)dobject_global_matrix(o));
			cairo_set_line_width(cr, 1);
			cairo_rectangle(cr, 1, 1, o->width - 2, o->height - 2);
			cairo_set_source_rgba(cr, 1, 0, 0, 0.6);
			cairo_stroke(cr);
			if((o->ctype != COLLIDER_TYPE_NONE) && o->touchable)
			{
				struct matrix_t m;
				double x, y;
				double w, h;
				double r;
				double * p;
				int n, i;

				switch(o->ctype)
				{
				case COLLIDER_TYPE_CIRCLE:
					cairo_new_sub_path(cr);
					cairo_move_to(cr, o->hit.circle.x + o->hit.circle.radius, o->hit.circle.y);
					cairo_arc(cr, o->hit.circle.x, o->hit.circle.y, o->hit.circle.radius, 0, M_PI * 2);
					cairo_close_path(cr);
					break;
				case COLLIDER_TYPE_ELLIPSE:
					x = o->hit.ellipse.x;
					y = o->hit.ellipse.y;
					w = o->hit.ellipse.width;
					h = o->hit.ellipse.height;
					cairo_get_matrix(cr, (cairo_matrix_t *)(&m));
					cairo_translate(cr, x, y);
					cairo_scale(cr, 1, h / w);
					cairo_translate(cr, -x, -y);
					cairo_new_sub_path(cr);
					cairo_move_to(cr, x + w, y);
					cairo_arc(cr, x, y, w, 0, M_PI * 2);
					cairo_close_path(cr);
					cairo_set_matrix(cr, (cairo_matrix_t *)(&m));
					break;
				case COLLIDER_TYPE_RECTANGLE:
					cairo_new_sub_path(cr);
					cairo_rectangle(cr, o->hit.rectangle.x, o->hit.rectangle.y, o->hit.rectangle.width, o->hit.rectangle.height);
					cairo_close_path(cr);
					break;
				case COLLIDER_TYPE_ROUNDED_RECTANGLE:
					x = o->hit.rounded_rectangle.x;
					y = o->hit.rounded_rectangle.y;
					w = o->hit.rounded_rectangle.width;
					h = o->hit.rounded_rectangle.height;
					r = o->hit.rounded_rectangle.radius;
					cairo_new_sub_path(cr);
					cairo_move_to(cr, x + r, y);
					cairo_line_to(cr, x + w - r, y);
					cairo_arc(cr, x + w - r, y + r, r, - M_PI / 2, 0);
					cairo_line_to(cr, x + w, y + h - r);
					cairo_arc(cr, x + w - r, y + h - r, r, 0, M_PI / 2);
					cairo_line_to(cr, x + r, y + h);
					cairo_arc(cr, x + r, y + h - r, r, M_PI / 2, M_PI);
					cairo_arc(cr, x + r, y + r, r, M_PI, M_PI + M_PI / 2);
					cairo_close_path(cr);
					break;
				case COLLIDER_TYPE_POLYGON:
					p = o->hit.polygon.points;
					n = o->hit.polygon.length / 2;
					if(n > 0)
					{
						cairo_new_sub_path(cr);
						cairo_move_to(cr, p[0], p[1]);
						for(i = 1; i < n; i++)
							cairo_line_to(cr, p[i << 1], p[(i << 1) + 1]);
						cairo_close_path(cr);
					}
					break;
				default:
					break;
				}
				cairo_set_source_rgba(cr, 1, 1, 0, 0.6);
				cairo_fill(cr);
			}
			cairo_restore(cr);
		}
		list_for_each_entry(pos, &o->children, entry)
		{
			display_draw(w, pos);
		}
		cairo_restore(cr);
	}
}

static int m_render(lua_State * L)
{
	struct ldobject_t * o = luaL_checkudata(L, 1, MT_DOBJECT);
	struct window_t * w = luaL_checkudata(L, 2, MT_WINDOW);
	if(window_is_active(w))
	{
		dobject_layout(o);
		window_region_list_clear(w);
		window_region_list_fill(w, o);
		window_present(w, (void *)o, (void (*)(struct window_t *, void *))display_draw);
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
	{"setAlpha",			m_set_alpha},
	{"getAlpha",			m_get_alpha},
	{"setBackgroundColor",	m_set_background_color},
	{"getBackgroundColor",	m_get_background_color},
	{"setLayoutEnable",		m_set_layout_enable},
	{"getLayoutEnable",		m_get_layout_enable},
	{"setLayoutSpecial",	m_set_layout_special},
	{"getLayoutSpecial",	m_get_layout_special},
	{"setLayoutDirection",	m_set_layout_direction},
	{"getLayoutDirection",	m_get_layout_direction},
	{"setLayoutJustify",	m_set_layout_justify},
	{"getLayoutJustify",	m_get_layout_justify},
	{"setLayoutAlign",		m_set_layout_align},
	{"getLayoutAlign",		m_get_layout_align},
	{"setLayoutAlignSelf",	m_set_layout_align_self},
	{"getLayoutAlignSelf",	m_get_layout_align_self},
	{"setLayoutGrow",		m_set_layout_grow},
	{"getLayoutGrow",		m_get_layout_grow},
	{"setLayoutShrink",		m_set_layout_shrink},
	{"getLayoutShrink",		m_get_layout_shrink},
	{"setLayoutBasis",		m_set_layout_basis},
	{"getLayoutBasis",		m_get_layout_basis},
	{"setLayoutMargin",		m_set_layout_margin},
	{"getLayoutMargin",		m_get_layout_margin},
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
