/*
 * libx/rect.c
 *
 * Copyright(c) 2007-2013 jianjun jiang <jerryjianjun@gmail.com>
 * official site: http://xboot.org
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <xboot/module.h>
#include <rect.h>

/*
 * Returns true if the rectangle has no area.
 */
bool_t rect_is_empty(const struct rect_t * r)
{
	if((r->w <= 0) || (r->h <= 0))
		return TRUE;

	return FALSE;
}
EXPORT_SYMBOL(rect_is_empty);

/*
 * Returns true if the two rectangles are equal.
 */
bool_t rect_is_equal(const struct rect_t * a, const struct rect_t * b)
{
	if( (a->x == b->x) && (a->y == b->y) &&
		(a->w == b->w) && (a->h == b->h) )
	{
		return TRUE;
	}
	return FALSE;
}
EXPORT_SYMBOL(rect_is_equal);

/*
 * Determine whether two rectangles intersect.
 * return TRUE if there is an intersection, FALSE otherwise.
 */
bool_t rect_has_intersection(const struct rect_t * a, const struct rect_t * b)
{
	s32_t a_min, a_max, b_min, b_max;

	/* Horizontal intersection */
	a_min = a->x;
	a_max = a_min + a->w;
	b_min = b->x;
	b_max = b_min + b->w;

	if (b_min > a_min)
		a_min = b_min;
	if (b_max < a_max)
		a_max = b_max;
	if (a_max <= a_min)
		return FALSE;

	/* Vertical intersection */
	a_min = a->y;
	a_max = a_min + a->h;
	b_min = b->y;
	b_max = b_min + b->h;

	if (b_min > a_min)
		a_min = b_min;
	if (b_max < a_max)
		a_max = b_max;
	if (a_max <= a_min)
		return FALSE;

	return TRUE;
}
EXPORT_SYMBOL(rect_has_intersection);

/*
 * Calculate the intersection of two rectangles.
 * return TRUE if there is an intersection, FALSE otherwise.
 */
bool_t rect_intersect(const struct rect_t * a, const struct rect_t * b, struct rect_t * r)
{
	s32_t a_min, a_max, b_min, b_max;

    /* Horizontal intersection */
    a_min = a->x;
    a_max = a_min + a->w;
    b_min = b->x;
    b_max = b_min + b->w;

    if (b_min > a_min)
        a_min = b_min;
    r->x = a_min;

    if (b_max < a_max)
        a_max = b_max;
    r->w = a_max - a_min;

    /* Vertical intersection */
    a_min = a->y;
    a_max = a_min + a->h;
    b_min = b->y;
    b_max = b_min + b->h;

    if (b_min > a_min)
        a_min = b_min;
    r->y = a_min;

    if (b_max < a_max)
        a_max = b_max;
    r->h = a_max - a_min;

    return !(rect_is_empty(r));
}
EXPORT_SYMBOL(rect_intersect);

/*
 * Calculate the union of two rectangles.
 */
void rect_union(const struct rect_t * a, const struct rect_t * b, struct rect_t * r)
{
	s32_t a_min, a_max, b_min, b_max;

	/* Horizontal union */
	a_min = a->x;
	a_max = a_min + a->w;
	b_min = b->x;
	b_max = b_min + b->w;

	if (b_min < a_min)
		a_min = b_min;
	r->x = a_min;

	if (b_max > a_max)
		a_max = b_max;
	r->w = a_max - a_min;

	/* Vertical intersection */
	a_min = a->y;
	a_max = a_min + a->h;
	b_min = b->y;
	b_max = b_min + b->h;

	if (b_min < a_min)
		a_min = b_min;
	r->y = a_min;

	if (b_max > a_max)
		a_max = b_max;
	r->h = a_max - a_min;
}
EXPORT_SYMBOL(rect_union);

/*
 * Calculate a minimal rectangle enclosing a set of points
 * return TRUE if any points were within the clipping rect
 */
bool_t rect_enclose_points(const struct point_t * p, int cnt,
		const struct rect_t * clip, struct rect_t * r)
{
	s32_t minx = 0;
	s32_t miny = 0;
	s32_t maxx = 0;
	s32_t maxy = 0;
	s32_t x, y;
	int i;

	if (cnt < 1)
		return FALSE;

	if (clip)
	{
		bool_t added = FALSE;
		s32_t clip_minx = clip->x;
		s32_t clip_miny = clip->y;
		s32_t clip_maxx = clip->x + clip->w - 1;
		s32_t clip_maxy = clip->y + clip->h - 1;

		for (i = 0; i < cnt; ++i)
		{
			x = p[i].x;
			y = p[i].y;

			if (x < clip_minx || x > clip_maxx || y < clip_miny || y
					> clip_maxy)
			{
				continue;
			}
			if (!added)
			{
				minx = maxx = x;
				miny = maxy = y;
				added = TRUE;
				continue;
			}
			if (x < minx)
			{
				minx = x;
			}
			else if (x > maxx)
			{
				maxx = x;
			}
			if (y < miny)
			{
				miny = y;
			}
			else if (y > maxy)
			{
				maxy = y;
			}
		}

		if (!added)
			return FALSE;
	}
	else
	{
		/* No clipping, always add the first point */
		minx = maxx = p[0].x;
		miny = maxy = p[0].y;

		for (i = 1; i < cnt; ++i)
		{
			x = p[i].x;
			y = p[i].y;

			if (x < minx)
			{
				minx = x;
			}
			else if (x > maxx)
			{
				maxx = x;
			}
			if (y < miny)
			{
				miny = y;
			}
			else if (y > maxy)
			{
				maxy = y;
			}
		}
	}

	if (r)
	{
		r->x = minx;
		r->y = miny;
		r->w = (maxx - minx) + 1;
		r->h = (maxy - miny) + 1;
	}

	return TRUE;
}
EXPORT_SYMBOL(rect_enclose_points);

/*
 * Use the Cohen-Sutherland algorithm for line clipping
 */
enum {
	CODE_BOTTOM		= 0x1,
	CODE_TOP		= 0x2,
	CODE_LEFT		= 0x4,
	CODE_RIGHT		= 0x8,
};

static int compute_outcode(const struct rect_t * r, s32_t x, s32_t y)
{
	int code = 0;

	if (y < 0)
	{
		code |= CODE_TOP;
	}
	else if (y >= r->y + r->h)
	{
		code |= CODE_BOTTOM;
	}
	if (x < 0)
	{
		code |= CODE_LEFT;
	}
	else if (x >= r->x + r->w)
	{
		code |= CODE_RIGHT;
	}

	return code;
}

/*
 * Calculate the intersection of a rectangle and line segment.
 * return TRUE if there is an intersection, FALSE otherwise.
 */
bool_t rect_intersect_with_line(const struct rect_t * r, s32_t * x1, s32_t * y1, s32_t * x2, s32_t * y2)
{
	s32_t x = 0;
	s32_t y = 0;
	s32_t _x1, _y1;
	s32_t _x2, _y2;
	s32_t rectx1;
	s32_t recty1;
	s32_t rectx2;
	s32_t recty2;
	int outcode1, outcode2;

	if (!r || !x1 || !y1 || !x2 || !y2)
	{
		return FALSE;
	}

	_x1 = *x1;
	_y1 = *y1;
	_x2 = *x2;
	_y2 = *y2;
	rectx1 = r->x;
	recty1 = r->y;
	rectx2 = r->x + r->w - 1;
	recty2 = r->y + r->h - 1;

	/* Check to see if entire line is inside rect */
	if (_x1 >= rectx1 && _x1 <= rectx2 && _x2 >= rectx1 && _x2 <= rectx2 && _y1
			>= recty1 && _y1 <= recty2 && _y2 >= recty1 && _y2 <= recty2)
	{
		return TRUE;
	}

	/* Check to see if entire line is to one side of rect */
	if ((_x1 < rectx1 && _x2 < rectx1) || (_x1 > rectx2 && _x2 > rectx2) || (_y1
			< recty1 && _y2 < recty1) || (_y1 > recty2 && _y2 > recty2))
	{
		return FALSE;
	}

	if (_y1 == _y2)
	{
		/* Horizontal line, easy to clip */
		if (_x1 < rectx1)
		{
			*x1 = rectx1;
		}
		else if (_x1 > rectx2)
		{
			*x1 = rectx2;
		}
		if (_x2 < rectx1)
		{
			*x2 = rectx1;
		}
		else if (_x2 > rectx2)
		{
			*x2 = rectx2;
		}
		return TRUE;
	}

	if (_x1 == _x2)
	{
		/* Vertical line, easy to clip */
		if (_y1 < recty1)
		{
			*y1 = recty1;
		}
		else if (_y1 > recty2)
		{
			*y1 = recty2;
		}
		if (_y2 < recty1)
		{
			*y2 = recty1;
		}
		else if (_y2 > recty2)
		{
			*y2 = recty2;
		}
		return TRUE;
	}

	/* More complicated Cohen-Sutherland algorithm */
	outcode1 = compute_outcode(r, _x1, _y1);
	outcode2 = compute_outcode(r, _x2, _y2);

	while (outcode1 || outcode2)
	{
		if (outcode1 & outcode2)
		{
			return FALSE;
		}

		if (outcode1)
		{
			if (outcode1 & CODE_TOP)
			{
				y = recty1;
				x = _x1 + ((_x2 - _x1) * (y - _y1)) / (_y2 - _y1);
			}
			else if (outcode1 & CODE_BOTTOM)
			{
				y = recty2;
				x = _x1 + ((_x2 - _x1) * (y - _y1)) / (_y2 - _y1);
			}
			else if (outcode1 & CODE_LEFT)
			{
				x = rectx1;
				y = _y1 + ((_y2 - _y1) * (x - _x1)) / (_x2 - _x1);
			}
			else if (outcode1 & CODE_RIGHT)
			{
				x = rectx2;
				y = _y1 + ((_y2 - _y1) * (x - _x1)) / (_x2 - _x1);
			}
			_x1 = x;
			_y1 = y;
			outcode1 = compute_outcode(r, x, y);
		}
		else
		{
			if (outcode2 & CODE_TOP)
			{
				y = recty1;
				x = _x1 + ((_x2 - _x1) * (y - _y1)) / (_y2 - _y1);
			}
			else if (outcode2 & CODE_BOTTOM)
			{
				y = recty2;
				x = _x1 + ((_x2 - _x1) * (y - _y1)) / (_y2 - _y1);
			}
			else if (outcode2 & CODE_LEFT)
			{
				x = rectx1;
				y = _y1 + ((_y2 - _y1) * (x - _x1)) / (_x2 - _x1);
			}
			else if (outcode2 & CODE_RIGHT)
			{
				x = rectx2;
				y = _y1 + ((_y2 - _y1) * (x - _x1)) / (_x2 - _x1);
			}
			_x2 = x;
			_y2 = y;
			outcode2 = compute_outcode(r, x, y);
		}
	}

	*x1 = _x1;
	*y1 = _y1;
	*x2 = _x2;
	*y2 = _y2;

	return TRUE;
}
EXPORT_SYMBOL(rect_intersect_with_line);

bool_t rect_align(const struct rect_t * a, const struct rect_t * b, struct rect_t * r, enum align_t align)
{
	s32_t dx1, dy1;
	s32_t dx2, dy2;
	s32_t dw, dh;

	if(!a || !b || !r)
		return FALSE;

	dx1 = a->x - b->x;
	dy1 = a->y - b->y;
	dx2 = (a->x + a->w) - (b->x + b->w);
	dy2 = (a->y + a->h) - (b->y + b->h);
	dw = a->w - b->w;
	dh = a->h - b->h;

	switch(align)
	{
	case ALIGN_LEFT:
		r->x = b->x + dx1;
		r->y = b->y;
		r->w = b->w;
		r->h = b->h;
		break;

	case ALIGN_TOP:
		r->x = b->x;
		r->y = b->y + dy1;
		r->w = b->w;
		r->h = b->h;
		break;

	case ALIGN_RIGHT:
		r->x = b->x + dx2;
		r->y = b->y;
		r->w = b->w;
		r->h = b->h;
		break;

	case ALIGN_BOTTOM:
		r->x = b->x;
		r->y = b->y + dy2;
		r->w = b->w;
		r->h = b->h;
		break;

	case ALIGN_LEFT_TOP:
		r->x = b->x + dx1;
		r->y = b->y + dy1;
		r->w = b->w;
		r->h = b->h;
		break;

	case ALIGN_RIGHT_TOP:
		r->x = b->x + dx2;
		r->y = b->y + dy1;
		r->w = b->w;
		r->h = b->h;
		break;

	case ALIGN_LEFT_BOTTOM:
		r->x = b->x + dx1;
		r->y = b->y + dy2;
		r->w = b->w;
		r->h = b->h;
		break;

	case ALIGN_RIGHT_BOTTOM:
		r->x = b->x + dx2;
		r->y = b->y + dy2;
		r->w = b->w;
		r->h = b->h;
		break;

	case ALIGN_LEFT_CENTER:
		r->x = b->x + dx1;
		r->y = b->y + dy1 + (dh >> 1);
		r->w = b->w;
		r->h = b->h;
		break;

	case ALIGN_TOP_CENTER:
		r->x = b->x + dx1 + (dw >> 1);
		r->y = b->y + dy1;
		r->w = b->w;
		r->h = b->h;
		break;

	case ALIGN_RIGHT_CENTER:
		r->x = b->x + dx2;
		r->y = b->y + dy1 + (dh >> 1);
		r->w = b->w;
		r->h = b->h;
		break;

	case ALIGN_BOTTOM_CENTER:
		r->x = b->x + dx1 + (dw >> 1);
		r->y = b->y + dy2;
		r->w = b->w;
		r->h = b->h;
		break;

	case ALIGN_CENTER_HORIZONTAL:
		r->x = b->x + dx1 + (dw >> 1);
		r->y = b->y;
		r->w = b->w;
		r->h = b->h;
		break;

	case ALIGN_CENTER_VERTICAL:
		r->x = b->x;
		r->y = b->y + dy1 + (dh >> 1);
		r->w = b->w;
		r->h = b->h;
		break;

	case ALIGN_CENTER:
		r->x = b->x + dx1 + (dw >> 1);
		r->y = b->y + dy1 + (dh >> 1);
		r->w = b->w;
		r->h = b->h;
		break;

	default:
		return FALSE;
	}

	return TRUE;
}
EXPORT_SYMBOL(rect_align);
