/*
 * kernel/graphic/svg.c
 *
 * Copyright(c) 2007-2021 Jianjun Jiang <8192542@qq.com>
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
#include <stdio.h>
#include <math.h>
#include <graphic/svg.h>

#define SVG_KAPPA90			(0.5522847493f)
#define SVG_XML_MAX_ATTRIBS	(256)
#define SVG_MAX_ATTR		(128)
#define SVG_MAX_DASHES		(8)

enum {
	SVG_XML_TAG				= 0,
	SVG_XML_CONTENT			= 1,
};

enum {
	SVG_ALIGN_MIN			= 0,
	SVG_ALIGN_MID			= 1,
	SVG_ALIGN_MAX			= 2,
};

enum {
	SVG_ALIGN_NONE			= 0,
	SVG_ALIGN_MEET			= 1,
	SVG_ALIGN_SLICE			= 2,
};

enum svg_gradient_space_t {
	SVG_SPACE_USER			= 0,
	SVG_SPACE_OBJECT		= 1,
};

enum svg_units_t {
	SVG_UNITS_USER			= 0,
	SVG_UNITS_PX			= 1,
	SVG_UNITS_PT			= 2,
	SVG_UNITS_PC			= 3,
	SVG_UNITS_MM			= 4,
	SVG_UNITS_CM			= 5,
	SVG_UNITS_IN			= 6,
	SVG_UNITS_PERCENT		= 7,
	SVG_UNITS_EM			= 8,
	SVG_UNITS_EX			= 9,
};

struct svg_coordinate_t {
	float value;
	enum svg_units_t units;
};

struct svg_linear_data_t {
	struct svg_coordinate_t x1, y1, x2, y2;
};

struct svg_radial_data_t {
	struct svg_coordinate_t cx, cy, r, fx, fy;
};

struct svg_gradient_data_t
{
	char id[64];
	char ref[64];
	enum svg_paint_type_t type;
	union {
		struct svg_linear_data_t linear;
		struct svg_radial_data_t radial;
	};
	enum svg_spread_type_t spread;
	enum svg_gradient_space_t space;
	float xform[6];
	int nstops;
	struct svg_gradient_stop_t * stops;
	struct svg_gradient_data_t * next;
};

struct svg_attrib_t
{
	char id[64];
	float xform[6];
	struct color_t fill_color;
	struct color_t stroke_color;
	float opacity;
	float fill_opacity;
	float stroke_opacity;
	char fill_gradient[64];
	char stroke_gradient[64];
	float stroke_width;
	float stroke_dash_offset;
	float stroke_dash_array[SVG_MAX_DASHES];
	int stroke_dash_count;
	enum svg_line_join_t stroke_line_join;
	enum svg_line_cap_t stroke_line_cap;
	float miter_limit;
	enum svg_fill_rule_t fill_rule;
	float font_size;
	struct color_t stop_color;
	float stop_opacity;
	float stop_offset;
	int has_fill;
	int has_stroke;
	int visible;
};

struct svg_parser_t
{
	struct svg_attrib_t attr[SVG_MAX_ATTR];
	int attrhead;
	float * pts;
	int npts;
	int cpts;
	struct svg_path_t * plist;
	struct svg_t * svg;
	struct svg_gradient_data_t * gradients;
	struct svg_shape_t * tail;
	float view_minx, view_miny, view_width, view_height;
	int alignx, aligny, aligntype;
	float dpi;
	int pathflag;
	int defsflag;
};

static inline int svg_isspace(char c)
{
	return strchr(" \t\n\v\f\r", c) != 0;
}

static inline int svg_isdigit(char c)
{
	return ((c >= '0') && (c <= '9'));
}

static inline int svg_isnum(char c)
{
	return strchr("0123456789+-.eE", c) != 0;
}

static inline float svg_minf(float a, float b)
{
	return a < b ? a : b;
}

static inline float svg_maxf(float a, float b)
{
	return a > b ? a : b;
}

static double svg_atof(const char * s)
{
	char * cur = (char *)s;
	char * end = NULL;
	double res = 0.0, sign = 1.0;
	long long intPart = 0, fracPart = 0;
	char hasIntPart = 0, hasFracPart = 0;

	if(*cur == '+')
	{
		cur++;
	}
	else if(*cur == '-')
	{
		sign = -1;
		cur++;
	}
	if(svg_isdigit(*cur))
	{
		intPart = strtoll(cur, &end, 10);
		if(cur != end)
		{
			res = (double)intPart;
			hasIntPart = 1;
			cur = end;
		}
	}
	if(*cur == '.')
	{
		cur++;
		if(svg_isdigit(*cur))
		{
			fracPart = strtoll(cur, &end, 10);
			if(cur != end)
			{
				res += (double)fracPart / pow(10.0, (double)(end - cur));
				hasFracPart = 1;
				cur = end;
			}
		}
	}
	if(!hasIntPart && !hasFracPart)
		return 0.0;
	if(*cur == 'e' || *cur == 'E')
	{
		long expPart = 0;
		cur++;
		expPart = strtol(cur, &end, 10);
		if(cur != end)
		{
			res *= pow(10.0, (double)expPart);
		}
	}
	return res * sign;
}

static void svg_parse_content(char * s, void (*ccb)(void *, const char *), void * ud)
{
	while(*s && svg_isspace(*s))
		s++;
	if(!*s)
		return;
	if(ccb)
		(*ccb)(ud, s);
}

static void svg_parse_element(char * s, void (*scb)(void *, const char *, const char **), void (*ecb)(void *, const char *), void * ud)
{
	const char * attr[SVG_XML_MAX_ATTRIBS];
	int nattr = 0;
	char * name;
	int start = 0;
	int end = 0;
	char quote;

	while(*s && svg_isspace(*s))
		s++;
	if(*s == '/')
	{
		s++;
		end = 1;
	}
	else
	{
		start = 1;
	}
	if(!*s || *s == '?' || *s == '!')
		return;

	name = s;
	while(*s && !svg_isspace(*s))
		s++;
	if(*s)
		*s++ = '\0';

	while(!end && *s && (nattr < SVG_XML_MAX_ATTRIBS - 3))
	{
		char * name = NULL;
		char * value = NULL;

		while(*s && svg_isspace(*s))
			s++;
		if(!*s)
			break;
		if(*s == '/')
		{
			end = 1;
			break;
		}
		name = s;
		while(*s && !svg_isspace(*s) && *s != '=')
			s++;
		if(*s)
		{
			*s++ = '\0';
		}
		while(*s && *s != '\"' && *s != '\'')
			s++;
		if(!*s)
			break;
		quote = *s;
		s++;
		value = s;
		while(*s && (*s != quote))
			s++;
		if(*s)
			*s++ = '\0';

		if(name && value)
		{
			attr[nattr++] = name;
			attr[nattr++] = value;
		}
	}

	attr[nattr++] = 0;
	attr[nattr++] = 0;
	if(start && scb)
		(*scb)(ud, name, attr);
	if(end && ecb)
		(*ecb)(ud, name);
}

static void svg_parse_xml(char * svgstr, void (*scb)(void *, const char *, const char **), void (*ecb)(void *, const char *), void (*ccb)(void *, const char *), void * ud)
{
	char * s = svgstr;
	char * mark = s;
	int state = SVG_XML_CONTENT;
	while(*s)
	{
		if((*s == '<') && (state == SVG_XML_CONTENT))
		{
			*s++ = '\0';
			svg_parse_content(mark, ccb, ud);
			mark = s;
			state = SVG_XML_TAG;
		}
		else if((*s == '>') && (state == SVG_XML_TAG))
		{
			*s++ = '\0';
			svg_parse_element(mark, scb, ecb, ud);
			mark = s;
			state = SVG_XML_CONTENT;
		}
		else
		{
			s++;
		}
	}
}

static void svg_xform_identity(float * t)
{
	t[0] = 1.0f; t[1] = 0.0f;
	t[2] = 0.0f; t[3] = 1.0f;
	t[4] = 0.0f; t[5] = 0.0f;
}

static void svg_xform_set_translation(float * t, float tx, float ty)
{
	t[0] = 1.0f; t[1] = 0.0f;
	t[2] = 0.0f; t[3] = 1.0f;
	t[4] = tx;   t[5] = ty;
}

static void svg_xform_set_scale(float * t, float sx, float sy)
{
	t[0] = sx;   t[1] = 0.0f;
	t[2] = 0.0f; t[3] = sy;
	t[4] = 0.0f; t[5] = 0.0f;
}

static void svg_xform_set_skewx(float * t, float a)
{
	t[0] = 1.0f;    t[1] = 0.0f;
	t[2] = tanf(a); t[3] = 1.0f;
	t[4] = 0.0f;    t[5] = 0.0f;
}

static void svg_xform_set_skewy(float * t, float a)
{
	t[0] = 1.0f; t[1] = tanf(a);
	t[2] = 0.0f; t[3] = 1.0f;
	t[4] = 0.0f; t[5] = 0.0f;
}

static void svg_xform_set_rotation(float * t, float a)
{
	float cs = cosf(a);
	float sn = sinf(a);
	t[0] = cs;   t[1] = sn;
	t[2] = -sn;  t[3] = cs;
	t[4] = 0.0f; t[5] = 0.0f;
}

static void svg_xform_multiply(float * t, float * s)
{
	float t0 = t[0] * s[0] + t[1] * s[2];
	float t2 = t[2] * s[0] + t[3] * s[2];
	float t4 = t[4] * s[0] + t[5] * s[2] + s[4];
	t[1] = t[0] * s[1] + t[1] * s[3];
	t[3] = t[2] * s[1] + t[3] * s[3];
	t[5] = t[4] * s[1] + t[5] * s[3] + s[5];
	t[0] = t0;
	t[2] = t2;
	t[4] = t4;
}

static void svg_xform_inverse(float * inv, float * t)
{
	double det = (double)t[0] * t[3] - (double)t[2] * t[1];
	double invdet;
	if(det > -1e-6 && det < 1e-6)
	{
		svg_xform_identity(t);
		return;
	}
	invdet = 1.0 / det;
	inv[0] = (float)(t[3] * invdet);
	inv[2] = (float)(-t[2] * invdet);
	inv[4] = (float)(((double)t[2] * t[5] - (double)t[3] * t[4]) * invdet);
	inv[1] = (float)(-t[1] * invdet);
	inv[3] = (float)(t[0] * invdet);
	inv[5] = (float)(((double)t[1] * t[4] - (double)t[0] * t[5]) * invdet);
}

static void svg_xform_premultiply(float * t, float * s)
{
	float s2[6];
	memcpy(s2, s, sizeof(float)*6);
	svg_xform_multiply(s2, t);
	memcpy(t, s2, sizeof(float)*6);
}

static void svg_xform_point(float * dx, float * dy, float x, float y, float * t)
{
	*dx = x * t[0] + y * t[2] + t[4];
	*dy = x * t[1] + y * t[3] + t[5];
}

static void svg_xform_vec(float * dx, float * dy, float x, float y, float * t)
{
	*dx = x * t[0] + y * t[2];
	*dy = x * t[1] + y * t[3];
}

static int svg_pt_in_bounds(float * pt, float * bounds)
{
	return ((pt[0] >= bounds[0]) && (pt[0] <= bounds[2]) && (pt[1] >= bounds[1]) && (pt[1] <= bounds[3]));
}

static double svg_eval_bezier(double t, double p0, double p1, double p2, double p3)
{
	double it = 1.0 - t;
	return it * it * it * p0 + 3.0 * it * it * t * p1 + 3.0 * it * t * t * p2 + t * t * t * p3;
}

static void svg_curve_bounds(float * bounds, float * curve)
{
	int i, j, count;
	double roots[2], a, b, c, b2ac, t, v;
	float * v0 = &curve[0];
	float * v1 = &curve[2];
	float * v2 = &curve[4];
	float * v3 = &curve[6];

	bounds[0] = svg_minf(v0[0], v3[0]);
	bounds[1] = svg_minf(v0[1], v3[1]);
	bounds[2] = svg_maxf(v0[0], v3[0]);
	bounds[3] = svg_maxf(v0[1], v3[1]);

	if(svg_pt_in_bounds(v1, bounds) && svg_pt_in_bounds(v2, bounds))
		return;

	for(i = 0; i < 2; i++)
	{
		a = -3.0 * v0[i] + 9.0 * v1[i] - 9.0 * v2[i] + 3.0 * v3[i];
		b = 6.0 * v0[i] - 12.0 * v1[i] + 6.0 * v2[i];
		c = 3.0 * v1[i] - 3.0 * v0[i];
		count = 0;
		if(fabs(a) < (1e-12))
		{
			if(fabs(b) > (1e-12))
			{
				t = -c / b;
				if(t > (1e-12) && t < 1.0 - (1e-12))
					roots[count++] = t;
			}
		}
		else
		{
			b2ac = b * b - 4.0 * c * a;
			if(b2ac > (1e-12))
			{
				t = (-b + sqrt(b2ac)) / (2.0 * a);
				if(t > (1e-12) && t < 1.0 - (1e-12))
					roots[count++] = t;
				t = (-b - sqrt(b2ac)) / (2.0 * a);
				if(t > (1e-12) && t < 1.0 - (1e-12))
					roots[count++] = t;
			}
		}
		for(j = 0; j < count; j++)
		{
			v = svg_eval_bezier(roots[j], v0[i], v1[i], v2[i], v3[i]);
			bounds[0 + i] = svg_minf(bounds[0 + i], (float)v);
			bounds[2 + i] = svg_maxf(bounds[2 + i], (float)v);
		}
	}
}

static struct svg_parser_t * svg_parser_alloc(void)
{
	struct svg_parser_t * p;

	p = malloc(sizeof(struct svg_parser_t));
	if(!p)
		return NULL;
	memset(p, 0, sizeof(struct svg_parser_t));

	p->svg = malloc(sizeof(struct svg_t));
	if(!p->svg)
	{
		free(p);
		return NULL;
	}
	memset(p->svg, 0, sizeof(struct svg_t));

	svg_xform_identity(p->attr[0].xform);
	memset(p->attr[0].id, 0, sizeof(p->attr[0].id));
	memset(&p->attr[0].fill_color, 0, sizeof(struct color_t));
	memset(&p->attr[0].stroke_color, 0, sizeof(struct color_t));
	p->attr[0].opacity = 1;
	p->attr[0].fill_opacity = 1;
	p->attr[0].stroke_opacity = 1;
	p->attr[0].stop_opacity = 1;
	p->attr[0].stroke_width = 1;
	p->attr[0].stroke_line_join = SVG_JOIN_MITER;
	p->attr[0].stroke_line_cap = SVG_CAP_BUTT;
	p->attr[0].miter_limit = 4;
	p->attr[0].fill_rule = SVG_FILLRULE_NONZERO;
	p->attr[0].has_fill = 1;
	p->attr[0].visible = 1;

	return p;
}

static void svg_delete_paths(struct svg_path_t * path)
{
	struct svg_path_t * n;
	while(path)
	{
		n = path->next;
		if(path->pts)
			free(path->pts);
		free(path);
		path = n;
	}
}

static void svg_delete_paint(struct svg_paint_t * paint)
{
	if((paint->type == SVG_PAINT_LINEAR_GRADIENT) || (paint->type == SVG_PAINT_RADIAL_GRADIENT))
		free(paint->gradient);
}

static void svg_delete_gradient_data(struct svg_gradient_data_t * grad)
{
	struct svg_gradient_data_t * n;
	while(grad)
	{
		n = grad->next;
		free(grad->stops);
		free(grad);
		grad = n;
	}
}

static void svg_parser_free(struct svg_parser_t * p)
{
	if(p)
	{
		svg_delete_paths(p->plist);
		svg_delete_gradient_data(p->gradients);
		svg_free(p->svg);
		free(p->pts);
		free(p);
	}
}

static void svg_reset_path(struct svg_parser_t * p)
{
	p->npts = 0;
}

static void svg_add_point(struct svg_parser_t * p, float x, float y)
{
	if(p->npts + 1 > p->cpts)
	{
		p->cpts = p->cpts ? p->cpts * 2 : 8;
		p->pts = realloc(p->pts, p->cpts * 2 * sizeof(float));
		if(!p->pts)
			return;
	}
	p->pts[p->npts * 2 + 0] = x;
	p->pts[p->npts * 2 + 1] = y;
	p->npts++;
}

static void svg_move_to(struct svg_parser_t * p, float x, float y)
{
	if(p->npts > 0)
	{
		p->pts[(p->npts - 1) * 2 + 0] = x;
		p->pts[(p->npts - 1) * 2 + 1] = y;
	}
	else
	{
		svg_add_point(p, x, y);
	}
}

static void svg_line_to(struct svg_parser_t * p, float x, float y)
{
	float px, py, dx, dy;
	if(p->npts > 0)
	{
		px = p->pts[(p->npts - 1) * 2 + 0];
		py = p->pts[(p->npts - 1) * 2 + 1];
		dx = x - px;
		dy = y - py;
		svg_add_point(p, px + dx / 3.0f, py + dy / 3.0f);
		svg_add_point(p, x - dx / 3.0f, y - dy / 3.0f);
		svg_add_point(p, x, y);
	}
}

static void svg_cubic_bezto(struct svg_parser_t * p, float cpx1, float cpy1, float cpx2, float cpy2, float x, float y)
{
	svg_add_point(p, cpx1, cpy1);
	svg_add_point(p, cpx2, cpy2);
	svg_add_point(p, x, y);
}

static struct svg_attrib_t * svg_get_attr(struct svg_parser_t * p)
{
	return &p->attr[p->attrhead];
}

static void svg_push_attr(struct svg_parser_t * p)
{
	if(p->attrhead < SVG_MAX_ATTR - 1)
	{
		p->attrhead++;
		memcpy(&p->attr[p->attrhead], &p->attr[p->attrhead - 1], sizeof(struct svg_attrib_t));
	}
}

static void svg_pop_attr(struct svg_parser_t * p)
{
	if(p->attrhead > 0)
		p->attrhead--;
}

static float svg_actual_origx(struct svg_parser_t * p)
{
	return p->view_minx;
}

static float svg_actual_origy(struct svg_parser_t * p)
{
	return p->view_miny;
}

static float svg_actual_width(struct svg_parser_t * p)
{
	return p->view_width;
}

static float svg_actual_height(struct svg_parser_t * p)
{
	return p->view_height;
}

static float svg_actual_length(struct svg_parser_t * p)
{
	float w = svg_actual_width(p);
	float h = svg_actual_height(p);
	return sqrtf(w * w + h * h) / sqrtf(2.0f);
}

static float svg_convert_to_pixels(struct svg_parser_t * p, struct svg_coordinate_t c, float orig, float length)
{
	struct svg_attrib_t * attr = svg_get_attr(p);
	switch(c.units)
	{
	case SVG_UNITS_USER:
		return c.value;
	case SVG_UNITS_PX:
		return c.value;
	case SVG_UNITS_PT:
		return c.value / 72.0f * p->dpi;
	case SVG_UNITS_PC:
		return c.value / 6.0f * p->dpi;
	case SVG_UNITS_MM:
		return c.value / 25.4f * p->dpi;
	case SVG_UNITS_CM:
		return c.value / 2.54f * p->dpi;
	case SVG_UNITS_IN:
		return c.value * p->dpi;
	case SVG_UNITS_EM:
		return c.value * attr->font_size;
	case SVG_UNITS_EX:
		return c.value * attr->font_size * 0.52f;
	case SVG_UNITS_PERCENT:
		return orig + c.value / 100.0f * length;
	default:
		return c.value;
	}
	return c.value;
}

static struct svg_gradient_data_t * svg_find_gradient_data(struct svg_parser_t * p, const char * id)
{
	struct svg_gradient_data_t * grad = p->gradients;
	while(grad)
	{
		if(strcmp(grad->id, id) == 0)
			return grad;
		grad = grad->next;
	}
	return NULL;
}

static struct svg_gradient_t * svg_create_gradient(struct svg_parser_t * p, const char * id, const float * bounds, enum svg_paint_type_t * type)
{
	struct svg_attrib_t * attr = svg_get_attr(p);
	struct svg_gradient_data_t * data = NULL;
	struct svg_gradient_data_t * ref = NULL;
	struct svg_gradient_stop_t * stops = NULL;
	struct svg_gradient_t * grad;
	float ox, oy, sw, sh, sl;
	int nstops = 0;

	data = svg_find_gradient_data(p, id);
	if(data == NULL)
		return NULL;

	ref = data;
	while(ref != NULL)
	{
		if(stops == NULL && ref->stops != NULL)
		{
			stops = ref->stops;
			nstops = ref->nstops;
			break;
		}
		ref = svg_find_gradient_data(p, ref->ref);
	}
	if(stops == NULL)
		return NULL;

	grad = (struct svg_gradient_t*)malloc(sizeof(struct svg_gradient_t) + sizeof(struct svg_gradient_stop_t) * (nstops - 1));
	if(grad == NULL)
		return NULL;

	if(data->space == SVG_SPACE_OBJECT)
	{
		ox = bounds[0];
		oy = bounds[1];
		sw = bounds[2] - bounds[0];
		sh = bounds[3] - bounds[1];
	}
	else
	{
		ox = svg_actual_origx(p);
		oy = svg_actual_origy(p);
		sw = svg_actual_width(p);
		sh = svg_actual_height(p);
	}
	sl = sqrtf(sw * sw + sh * sh) / sqrtf(2.0f);

	if(data->type == SVG_PAINT_LINEAR_GRADIENT)
	{
		float x1, y1, x2, y2, dx, dy;
		x1 = svg_convert_to_pixels(p, data->linear.x1, ox, sw);
		y1 = svg_convert_to_pixels(p, data->linear.y1, oy, sh);
		x2 = svg_convert_to_pixels(p, data->linear.x2, ox, sw);
		y2 = svg_convert_to_pixels(p, data->linear.y2, oy, sh);
		dx = x2 - x1;
		dy = y2 - y1;
		grad->xform[0] = dy;
		grad->xform[1] = -dx;
		grad->xform[2] = dx;
		grad->xform[3] = dy;
		grad->xform[4] = x1;
		grad->xform[5] = y1;
	}
	else
	{
		float cx, cy, fx, fy, r;
		cx = svg_convert_to_pixels(p, data->radial.cx, ox, sw);
		cy = svg_convert_to_pixels(p, data->radial.cy, oy, sh);
		fx = svg_convert_to_pixels(p, data->radial.fx, ox, sw);
		fy = svg_convert_to_pixels(p, data->radial.fy, oy, sh);
		r = svg_convert_to_pixels(p, data->radial.r, 0, sl);
		grad->xform[0] = r;
		grad->xform[1] = 0;
		grad->xform[2] = 0;
		grad->xform[3] = r;
		grad->xform[4] = cx;
		grad->xform[5] = cy;
		grad->fx = fx / r;
		grad->fy = fy / r;
	}

	svg_xform_multiply(grad->xform, data->xform);
	svg_xform_multiply(grad->xform, attr->xform);
	grad->spread = data->spread;
	memcpy(grad->stops, stops, nstops * sizeof(struct svg_gradient_stop_t));
	grad->nstops = nstops;
	*type = data->type;

	return grad;
}

static float svg_get_average_scale(float * t)
{
	float sx = sqrtf(t[0] * t[0] + t[2] * t[2]);
	float sy = sqrtf(t[1] * t[1] + t[3] * t[3]);
	return (sx + sy) * 0.5f;
}

static void svg_get_local_bounds(float * bounds, struct svg_shape_t * shape, float * xform)
{
	struct svg_path_t * path;
	float curve[4 * 2], curveBounds[4];
	int i, first = 1;
	for(path = shape->paths; path != NULL; path = path->next)
	{
		svg_xform_point(&curve[0], &curve[1], path->pts[0], path->pts[1], xform);
		for(i = 0; i < path->npts - 1; i += 3)
		{
			svg_xform_point(&curve[2], &curve[3], path->pts[(i + 1) * 2], path->pts[(i + 1) * 2 + 1], xform);
			svg_xform_point(&curve[4], &curve[5], path->pts[(i + 2) * 2], path->pts[(i + 2) * 2 + 1], xform);
			svg_xform_point(&curve[6], &curve[7], path->pts[(i + 3) * 2], path->pts[(i + 3) * 2 + 1], xform);
			svg_curve_bounds(curveBounds, curve);
			if(first)
			{
				bounds[0] = curveBounds[0];
				bounds[1] = curveBounds[1];
				bounds[2] = curveBounds[2];
				bounds[3] = curveBounds[3];
				first = 0;
			}
			else
			{
				bounds[0] = svg_minf(bounds[0], curveBounds[0]);
				bounds[1] = svg_minf(bounds[1], curveBounds[1]);
				bounds[2] = svg_maxf(bounds[2], curveBounds[2]);
				bounds[3] = svg_maxf(bounds[3], curveBounds[3]);
			}
			curve[0] = curve[6];
			curve[1] = curve[7];
		}
	}
}

static void svg_add_shape(struct svg_parser_t * p)
{
	struct svg_attrib_t * attr = svg_get_attr(p);
	float scale = 1.0f;
	struct svg_shape_t * shape;
	struct svg_path_t * path;
	int i;

	if(!p->plist)
		return;

	shape = (struct svg_shape_t *)malloc(sizeof(struct svg_shape_t));
	if(!shape)
		return;
	memset(shape, 0, sizeof(struct svg_shape_t));

	memcpy(shape->id, attr->id, sizeof(shape->id));
	scale = svg_get_average_scale(attr->xform);
	shape->stroke_width = attr->stroke_width * scale;
	shape->stroke_dash_offset = attr->stroke_dash_offset * scale;
	shape->stroke_dash_count = attr->stroke_dash_count;
	for(i = 0; i < attr->stroke_dash_count; i++)
		shape->stroke_dash_array[i] = attr->stroke_dash_array[i] * scale;
	shape->stroke_line_join = attr->stroke_line_join;
	shape->stroke_line_cap = attr->stroke_line_cap;
	shape->miter_limit = attr->miter_limit;
	shape->fill_rule = attr->fill_rule;
	shape->opacity = attr->opacity;
	shape->paths = p->plist;
	p->plist = NULL;

	shape->bounds[0] = shape->paths->bounds[0];
	shape->bounds[1] = shape->paths->bounds[1];
	shape->bounds[2] = shape->paths->bounds[2];
	shape->bounds[3] = shape->paths->bounds[3];
	for(path = shape->paths->next; path != NULL; path = path->next)
	{
		shape->bounds[0] = svg_minf(shape->bounds[0], path->bounds[0]);
		shape->bounds[1] = svg_minf(shape->bounds[1], path->bounds[1]);
		shape->bounds[2] = svg_maxf(shape->bounds[2], path->bounds[2]);
		shape->bounds[3] = svg_maxf(shape->bounds[3], path->bounds[3]);
	}

	if(attr->has_fill == 0)
	{
		shape->fill.type = SVG_PAINT_NONE;
	}
	else if(attr->has_fill == 1)
	{
		shape->fill.type = SVG_PAINT_COLOR;
		color_init(&shape->fill.color, attr->fill_color.r, attr->fill_color.g, attr->fill_color.b, attr->fill_opacity * 255);
	}
	else if(attr->has_fill == 2)
	{
		float inv[6], bounds[4];
		svg_xform_inverse(inv, attr->xform);
		svg_get_local_bounds(bounds, shape, inv);
		shape->fill.gradient = svg_create_gradient(p, attr->fill_gradient, bounds, &shape->fill.type);
		if(shape->fill.gradient == NULL)
			shape->fill.type = SVG_PAINT_NONE;
	}

	if(attr->has_stroke == 0)
	{
		shape->stroke.type = SVG_PAINT_NONE;
	}
	else if(attr->has_stroke == 1)
	{
		shape->stroke.type = SVG_PAINT_COLOR;
		color_init(&shape->stroke.color, attr->stroke_color.r, attr->stroke_color.g, attr->stroke_color.b, attr->stroke_opacity * 255);
	}
	else if(attr->has_stroke == 2)
	{
		float inv[6], bounds[4];
		svg_xform_inverse(inv, attr->xform);
		svg_get_local_bounds(bounds, shape, inv);
		shape->stroke.gradient = svg_create_gradient(p, attr->stroke_gradient, bounds, &shape->stroke.type);
		if(shape->stroke.gradient == NULL)
			shape->stroke.type = SVG_PAINT_NONE;
	}
	shape->visible = attr->visible ? 1 : 0;

	if(p->svg->shapes == NULL)
		p->svg->shapes = shape;
	else
		p->tail->next = shape;
	p->tail = shape;
}

static void svg_add_path(struct svg_parser_t * p, int closed)
{
	struct svg_attrib_t * attr = svg_get_attr(p);
	struct svg_path_t * path = NULL;
	float bounds[4];
	float * curve;
	int i;

	if(p->npts < 4)
		return;

	if(closed)
		svg_line_to(p, p->pts[0], p->pts[1]);

	path = (struct svg_path_t *)malloc(sizeof(struct svg_path_t));
	if(!path)
		return;
	memset(path, 0, sizeof(struct svg_path_t));

	path->pts = (float *)malloc(p->npts * 2 * sizeof(float));
	if(!path->pts)
	{
		free(path);
		return;
	}
	path->closed = closed;
	path->npts = p->npts;

	for(i = 0; i < p->npts; ++i)
		svg_xform_point(&path->pts[i * 2], &path->pts[i * 2 + 1], p->pts[i * 2], p->pts[i * 2 + 1], attr->xform);

	for(i = 0; i < path->npts - 1; i += 3)
	{
		curve = &path->pts[i * 2];
		svg_curve_bounds(bounds, curve);
		if(i == 0)
		{
			path->bounds[0] = bounds[0];
			path->bounds[1] = bounds[1];
			path->bounds[2] = bounds[2];
			path->bounds[3] = bounds[3];
		}
		else
		{
			path->bounds[0] = svg_minf(path->bounds[0], bounds[0]);
			path->bounds[1] = svg_minf(path->bounds[1], bounds[1]);
			path->bounds[2] = svg_maxf(path->bounds[2], bounds[2]);
			path->bounds[3] = svg_maxf(path->bounds[3], bounds[3]);
		}
	}
	path->next = p->plist;
	p->plist = path;
}

static const char * svg_parse_number(const char * s, char * it, const int size)
{
	const int last = size - 1;
	int i = 0;

	if(*s == '-' || *s == '+')
	{
		if(i < last)
			it[i++] = *s;
		s++;
	}
	while(*s && svg_isdigit(*s))
	{
		if(i < last)
			it[i++] = *s;
		s++;
	}
	if(*s == '.')
	{
		if(i < last)
			it[i++] = *s;
		s++;
		while(*s && svg_isdigit(*s))
		{
			if(i < last)
				it[i++] = *s;
			s++;
		}
	}
	if((*s == 'e' || *s == 'E') && (s[1] != 'm' && s[1] != 'x'))
	{
		if(i < last)
			it[i++] = *s;
		s++;
		if(*s == '-' || *s == '+')
		{
			if(i < last)
				it[i++] = *s;
			s++;
		}
		while(*s && svg_isdigit(*s))
		{
			if(i < last)
				it[i++] = *s;
			s++;
		}
	}
	it[i] = '\0';

	return s;
}

static const char * svg_get_next_path_item(const char * s, char * it)
{
	it[0] = '\0';
	while(*s && (svg_isspace(*s) || *s == ','))
		s++;
	if(!*s)
		return s;
	if(*s == '-' || *s == '+' || *s == '.' || svg_isdigit(*s))
	{
		s = svg_parse_number(s, it, 64);
	}
	else
	{
		it[0] = *s++;
		it[1] = '\0';
		return s;
	}
	return s;
}

static float svg_parse_opacity(const char * str)
{
	float val = svg_atof(str);
	if(val < 0.0f)
		val = 0.0f;
	if(val > 1.0f)
		val = 1.0f;
	return val;
}

static float svg_parse_miter_limit(const char * str)
{
	float val = svg_atof(str);
	if(val < 0.0f)
		val = 0.0f;
	return val;
}

static enum svg_units_t svg_parse_units(const char * units)
{
	if(units[0] == 'p' && units[1] == 'x')
		return SVG_UNITS_PX;
	else if(units[0] == 'p' && units[1] == 't')
		return SVG_UNITS_PT;
	else if(units[0] == 'p' && units[1] == 'c')
		return SVG_UNITS_PC;
	else if(units[0] == 'm' && units[1] == 'm')
		return SVG_UNITS_MM;
	else if(units[0] == 'c' && units[1] == 'm')
		return SVG_UNITS_CM;
	else if(units[0] == 'i' && units[1] == 'n')
		return SVG_UNITS_IN;
	else if(units[0] == '%')
		return SVG_UNITS_PERCENT;
	else if(units[0] == 'e' && units[1] == 'm')
		return SVG_UNITS_EM;
	else if(units[0] == 'e' && units[1] == 'x')
		return SVG_UNITS_EX;
	return SVG_UNITS_USER;
}

static struct svg_coordinate_t svg_parse_coordinate_raw(const char * str)
{
	struct svg_coordinate_t coord = { 0, SVG_UNITS_USER };
	char buf[64];
	coord.units = svg_parse_units(svg_parse_number(str, buf, 64));
	coord.value = svg_atof(buf);
	return coord;
}

static struct svg_coordinate_t svg_coord(float v, enum svg_units_t units)
{
	struct svg_coordinate_t coord = { v, units };
	return coord;
}

static float svg_parse_coordinate(struct svg_parser_t * p, const char * str, float orig, float length)
{
	struct svg_coordinate_t coord = svg_parse_coordinate_raw(str);
	return svg_convert_to_pixels(p, coord, orig, length);
}

static int svg_parse_transform_args(const char * str, float * args, int maxNa, int * na)
{
	const char * end;
	const char * ptr;
	char it[64];

	*na = 0;
	ptr = str;
	while(*ptr && *ptr != '(')
		++ptr;
	if(*ptr == 0)
		return 1;
	end = ptr;
	while(*end && *end != ')')
		++end;
	if(*end == 0)
		return 1;

	while(ptr < end)
	{
		if(*ptr == '-' || *ptr == '+' || *ptr == '.' || svg_isdigit(*ptr))
		{
			if(*na >= maxNa)
				return 0;
			ptr = svg_parse_number(ptr, it, 64);
			args[(*na)++] = (float)svg_atof(it);
		}
		else
		{
			++ptr;
		}
	}
	return (int)(end - str);
}

static int svg_parse_matrix(float * xform, const char * str)
{
	float t[6];
	int na = 0;
	int len = svg_parse_transform_args(str, t, 6, &na);
	if(na != 6)
		return len;
	memcpy(xform, t, sizeof(float) * 6);
	return len;
}

static int svg_parse_translate(float * xform, const char * str)
{
	float args[2];
	float t[6];
	int na = 0;
	int len = svg_parse_transform_args(str, args, 2, &na);
	if(na == 1)
		args[1] = 0.0;
	svg_xform_set_translation(t, args[0], args[1]);
	memcpy(xform, t, sizeof(float) * 6);
	return len;
}

static int svg_parse_scale(float * xform, const char * str)
{
	float args[2];
	int na = 0;
	float t[6];
	int len = svg_parse_transform_args(str, args, 2, &na);
	if(na == 1)
		args[1] = args[0];
	svg_xform_set_scale(t, args[0], args[1]);
	memcpy(xform, t, sizeof(float) * 6);
	return len;
}

static int svg_parse_skewx(float * xform, const char * str)
{
	float args[1];
	int na = 0;
	float t[6];
	int len = svg_parse_transform_args(str, args, 1, &na);
	svg_xform_set_skewx(t, args[0] / 180.0f * M_PI);
	memcpy(xform, t, sizeof(float) * 6);
	return len;
}

static int svg_parse_skewy(float * xform, const char * str)
{
	float args[1];
	int na = 0;
	float t[6];
	int len = svg_parse_transform_args(str, args, 1, &na);
	svg_xform_set_skewy(t, args[0] / 180.0f * M_PI);
	memcpy(xform, t, sizeof(float) * 6);
	return len;
}

static int svg_parse_rotate(float * xform, const char * str)
{
	float args[3];
	int na = 0;
	float m[6];
	float t[6];
	int len = svg_parse_transform_args(str, args, 3, &na);
	if(na == 1)
		args[1] = args[2] = 0.0f;
	svg_xform_identity(m);

	if(na > 1)
	{
		svg_xform_set_translation(t, -args[1], -args[2]);
		svg_xform_multiply(m, t);
	}
	svg_xform_set_rotation(t, args[0] / 180.0f * M_PI);
	svg_xform_multiply(m, t);
	if(na > 1)
	{
		svg_xform_set_translation(t, args[1], args[2]);
		svg_xform_multiply(m, t);
	}
	memcpy(xform, m, sizeof(float) * 6);

	return len;
}

static void svg_parse_transform(float * xform, const char * str)
{
	float t[6];

	svg_xform_identity(xform);
	while(*str)
	{
		if(strncmp(str, "matrix", 6) == 0)
			str += svg_parse_matrix(t, str);
		else if(strncmp(str, "translate", 9) == 0)
			str += svg_parse_translate(t, str);
		else if(strncmp(str, "scale", 5) == 0)
			str += svg_parse_scale(t, str);
		else if(strncmp(str, "rotate", 6) == 0)
			str += svg_parse_rotate(t, str);
		else if(strncmp(str, "skewX", 5) == 0)
			str += svg_parse_skewx(t, str);
		else if(strncmp(str, "skewY", 5) == 0)
			str += svg_parse_skewy(t, str);
		else
		{
			++str;
			continue;
		}
		svg_xform_premultiply(xform, t);
	}
}

static void svg_parse_url(char * id, const char * str)
{
	int i = 0;
	str += 4;
	if(*str == '#')
		str++;
	while((i < 63) && (*str != ')'))
	{
		id[i] = *str++;
		i++;
	}
	id[i] = '\0';
}

static char svg_parse_line_cap(const char * str)
{
	if(strcmp(str, "butt") == 0)
		return SVG_CAP_BUTT;
	else if(strcmp(str, "round") == 0)
		return SVG_CAP_ROUND;
	else if(strcmp(str, "square") == 0)
		return SVG_CAP_SQUARE;
	return SVG_CAP_BUTT;
}

static char svg_parse_line_join(const char * str)
{
	if(strcmp(str, "miter") == 0)
		return SVG_JOIN_MITER;
	else if(strcmp(str, "round") == 0)
		return SVG_JOIN_ROUND;
	else if(strcmp(str, "bevel") == 0)
		return SVG_JOIN_BEVEL;
	return SVG_JOIN_MITER;
}

static char svg_parse_fill_rule(const char * str)
{
	if(strcmp(str, "nonzero") == 0)
		return SVG_FILLRULE_NONZERO;
	else if(strcmp(str, "evenodd") == 0)
		return SVG_FILLRULE_EVENODD;
	return SVG_FILLRULE_NONZERO;
}

static const char * svg_get_next_dash_item(const char * s, char * it)
{
	int n = 0;
	it[0] = '\0';
	while(*s && (svg_isspace(*s) || *s == ','))
		s++;
	while(*s && (!svg_isspace(*s) && *s != ','))
	{
		if(n < 63)
			it[n++] = *s;
		s++;
	}
	it[n++] = '\0';
	return s;
}

static int svg_parse_stroke_dash_array(struct svg_parser_t * p, const char * str, float * stroke_dash_array)
{
	char item[64];
	int count = 0, i;
	float sum = 0.0f;

	if(str[0] == 'n')
		return 0;
	while(*str)
	{
		str = svg_get_next_dash_item(str, item);
		if(!*item)
			break;
		if(count < SVG_MAX_DASHES)
			stroke_dash_array[count++] = fabsf(svg_parse_coordinate(p, item, 0.0f, svg_actual_length(p)));
	}
	for(i = 0; i < count; i++)
		sum += stroke_dash_array[i];
	if(sum <= 1e-6f)
		count = 0;
	return count;
}

static void svg_parse_style(struct svg_parser_t * p, const char * str);

static int svg_parse_attr(struct svg_parser_t * p, const char * name, const char * value)
{
	struct svg_attrib_t * attr = svg_get_attr(p);
	float xform[6];

	if(!attr)
		return 0;

	if(strcmp(name, "style") == 0)
	{
		svg_parse_style(p, value);
	}
	else if(strcmp(name, "display") == 0)
	{
		if(strcmp(value, "none") == 0)
			attr->visible = 0;
	}
	else if(strcmp(name, "fill") == 0)
	{
		if(strcmp(value, "none") == 0)
		{
			attr->has_fill = 0;
		}
		else if(strncmp(value, "url(", 4) == 0)
		{
			attr->has_fill = 2;
			svg_parse_url(attr->fill_gradient, value);
		}
		else
		{
			attr->has_fill = 1;
			color_init_string(&attr->fill_color, value);
		}
	}
	else if(strcmp(name, "opacity") == 0)
	{
		attr->opacity = svg_parse_opacity(value);
	}
	else if(strcmp(name, "fill-opacity") == 0)
	{
		attr->fill_opacity = svg_parse_opacity(value);
	}
	else if(strcmp(name, "stroke") == 0)
	{
		if(strcmp(value, "none") == 0)
		{
			attr->has_stroke = 0;
		}
		else if(strncmp(value, "url(", 4) == 0)
		{
			attr->has_stroke = 2;
			svg_parse_url(attr->stroke_gradient, value);
		}
		else
		{
			attr->has_stroke = 1;
			color_init_string(&attr->stroke_color, value);
		}
	}
	else if(strcmp(name, "stroke-width") == 0)
	{
		attr->stroke_width = svg_parse_coordinate(p, value, 0.0f, svg_actual_length(p));
	}
	else if(strcmp(name, "stroke-dasharray") == 0)
	{
		attr->stroke_dash_count = svg_parse_stroke_dash_array(p, value, attr->stroke_dash_array);
	}
	else if(strcmp(name, "stroke-dashoffset") == 0)
	{
		attr->stroke_dash_offset = svg_parse_coordinate(p, value, 0.0f, svg_actual_length(p));
	}
	else if(strcmp(name, "stroke-opacity") == 0)
	{
		attr->stroke_opacity = svg_parse_opacity(value);
	}
	else if(strcmp(name, "stroke-linecap") == 0)
	{
		attr->stroke_line_cap = svg_parse_line_cap(value);
	}
	else if(strcmp(name, "stroke-linejoin") == 0)
	{
		attr->stroke_line_join = svg_parse_line_join(value);
	}
	else if(strcmp(name, "stroke-miterlimit") == 0)
	{
		attr->miter_limit = svg_parse_miter_limit(value);
	}
	else if(strcmp(name, "fill-rule") == 0)
	{
		attr->fill_rule = svg_parse_fill_rule(value);
	}
	else if(strcmp(name, "font-size") == 0)
	{
		attr->font_size = svg_parse_coordinate(p, value, 0.0f, svg_actual_length(p));
	}
	else if(strcmp(name, "transform") == 0)
	{
		svg_parse_transform(xform, value);
		svg_xform_premultiply(attr->xform, xform);
	}
	else if(strcmp(name, "stop-color") == 0)
	{
		color_init_string(&attr->stop_color, value);
	}
	else if(strcmp(name, "stop-opacity") == 0)
	{
		attr->stop_opacity = svg_parse_opacity(value);
	}
	else if(strcmp(name, "offset") == 0)
	{
		attr->stop_offset = svg_parse_coordinate(p, value, 0.0f, 1.0f);
	}
	else if(strcmp(name, "id") == 0)
	{
		strncpy(attr->id, value, 63);
		attr->id[63] = '\0';
	}
	else if(strcmp(name, "x") == 0)
	{
		svg_xform_set_translation(xform, svg_atof(value), 0);
		svg_xform_premultiply(attr->xform, xform);
	}
	else if(strcmp(name, "y") == 0)
	{
		svg_xform_set_translation(xform, 0, svg_atof(value));
		svg_xform_premultiply(attr->xform, xform);
	}
	else
	{
		return 0;
	}
	return 1;
}

static int svg_parse_name_value(struct svg_parser_t * p, const char * start, const char * end)
{
	const char * str;
	const char * val;
	char name[512];
	char value[512];
	int n;

	str = start;
	while(str < end && *str != ':')
		++str;
	val = str;
	while(str > start && (*str == ':' || svg_isspace(*str)))
		--str;
	++str;
	n = (int)(str - start);
	if(n > 511)
		n = 511;
	if(n)
		memcpy(name, start, n);
	name[n] = 0;
	while(val < end && (*val == ':' || svg_isspace(*val)))
		++val;
	n = (int)(end - val);
	if(n > 511)
		n = 511;
	if(n)
		memcpy(value, val, n);
	value[n] = 0;
	return svg_parse_attr(p, name, value);
}

static void svg_parse_style(struct svg_parser_t * p, const char * str)
{
	const char * start;
	const char * end;

	while(*str)
	{
		while(*str && svg_isspace(*str))
			++str;
		start = str;
		while(*str && *str != ';')
			++str;
		end = str;
		while(end > start && (*end == ';' || svg_isspace(*end)))
			--end;
		++end;
		svg_parse_name_value(p, start, end);
		if(*str)
			++str;
	}
}

static void svg_parse_attribs(struct svg_parser_t * p, const char ** attr)
{
	int i;
	for(i = 0; attr[i]; i += 2)
	{
		if(strcmp(attr[i], "style") == 0)
			svg_parse_style(p, attr[i + 1]);
		else
			svg_parse_attr(p, attr[i], attr[i + 1]);
	}
}

static int svg_get_args_per_element(char cmd)
{
	switch(cmd)
	{
	case 'v':
	case 'V':
	case 'h':
	case 'H':
		return 1;
	case 'm':
	case 'M':
	case 'l':
	case 'L':
	case 't':
	case 'T':
		return 2;
	case 'q':
	case 'Q':
	case 's':
	case 'S':
		return 4;
	case 'c':
	case 'C':
		return 6;
	case 'a':
	case 'A':
		return 7;
	}
	return 0;
}

static void svg_path_move_to(struct svg_parser_t * p, float * cpx, float * cpy, float * args, int rel)
{
	if(rel)
	{
		*cpx += args[0];
		*cpy += args[1];
	}
	else
	{
		*cpx = args[0];
		*cpy = args[1];
	}
	svg_move_to(p, *cpx, *cpy);
}

static void svg_path_line_to(struct svg_parser_t * p, float * cpx, float * cpy, float * args, int rel)
{
	if(rel)
	{
		*cpx += args[0];
		*cpy += args[1];
	}
	else
	{
		*cpx = args[0];
		*cpy = args[1];
	}
	svg_line_to(p, *cpx, *cpy);
}

static void svg_path_hline_to(struct svg_parser_t * p, float * cpx, float * cpy, float * args, int rel)
{
	if(rel)
		*cpx += args[0];
	else
		*cpx = args[0];
	svg_line_to(p, *cpx, *cpy);
}

static void svg_path_vline_to(struct svg_parser_t * p, float * cpx, float * cpy, float * args, int rel)
{
	if(rel)
		*cpy += args[0];
	else
		*cpy = args[0];
	svg_line_to(p, *cpx, *cpy);
}

static void svg_path_cubic_bez_to(struct svg_parser_t * p, float * cpx, float * cpy, float * cpx2, float * cpy2, float * args, int rel)
{
	float x2, y2, cx1, cy1, cx2, cy2;

	if(rel)
	{
		cx1 = *cpx + args[0];
		cy1 = *cpy + args[1];
		cx2 = *cpx + args[2];
		cy2 = *cpy + args[3];
		x2 = *cpx + args[4];
		y2 = *cpy + args[5];
	}
	else
	{
		cx1 = args[0];
		cy1 = args[1];
		cx2 = args[2];
		cy2 = args[3];
		x2 = args[4];
		y2 = args[5];
	}
	svg_cubic_bezto(p, cx1, cy1, cx2, cy2, x2, y2);
	*cpx2 = cx2;
	*cpy2 = cy2;
	*cpx = x2;
	*cpy = y2;
}

static void svg_path_cubic_bez_short_To(struct svg_parser_t * p, float * cpx, float * cpy, float * cpx2, float * cpy2, float * args, int rel)
{
	float x1, y1, x2, y2, cx1, cy1, cx2, cy2;

	x1 = *cpx;
	y1 = *cpy;
	if(rel)
	{
		cx2 = *cpx + args[0];
		cy2 = *cpy + args[1];
		x2 = *cpx + args[2];
		y2 = *cpy + args[3];
	}
	else
	{
		cx2 = args[0];
		cy2 = args[1];
		x2 = args[2];
		y2 = args[3];
	}
	cx1 = 2 * x1 - *cpx2;
	cy1 = 2 * y1 - *cpy2;
	svg_cubic_bezto(p, cx1, cy1, cx2, cy2, x2, y2);
	*cpx2 = cx2;
	*cpy2 = cy2;
	*cpx = x2;
	*cpy = y2;
}

static void svg_path_quad_bez_to(struct svg_parser_t * p, float * cpx, float * cpy, float * cpx2, float * cpy2, float * args, int rel)
{
	float x1, y1, x2, y2, cx, cy;
	float cx1, cy1, cx2, cy2;

	x1 = *cpx;
	y1 = *cpy;
	if(rel)
	{
		cx = *cpx + args[0];
		cy = *cpy + args[1];
		x2 = *cpx + args[2];
		y2 = *cpy + args[3];
	}
	else
	{
		cx = args[0];
		cy = args[1];
		x2 = args[2];
		y2 = args[3];
	}
	cx1 = x1 + 2.0f / 3.0f * (cx - x1);
	cy1 = y1 + 2.0f / 3.0f * (cy - y1);
	cx2 = x2 + 2.0f / 3.0f * (cx - x2);
	cy2 = y2 + 2.0f / 3.0f * (cy - y2);
	svg_cubic_bezto(p, cx1, cy1, cx2, cy2, x2, y2);
	*cpx2 = cx;
	*cpy2 = cy;
	*cpx = x2;
	*cpy = y2;
}

static void svg_path_quad_bez_short_to(struct svg_parser_t * p, float * cpx, float * cpy, float * cpx2, float * cpy2, float * args, int rel)
{
	float x1, y1, x2, y2, cx, cy;
	float cx1, cy1, cx2, cy2;

	x1 = *cpx;
	y1 = *cpy;
	if(rel)
	{
		x2 = *cpx + args[0];
		y2 = *cpy + args[1];
	}
	else
	{
		x2 = args[0];
		y2 = args[1];
	}
	cx = 2 * x1 - *cpx2;
	cy = 2 * y1 - *cpy2;
	cx1 = x1 + 2.0f / 3.0f * (cx - x1);
	cy1 = y1 + 2.0f / 3.0f * (cy - y1);
	cx2 = x2 + 2.0f / 3.0f * (cx - x2);
	cy2 = y2 + 2.0f / 3.0f * (cy - y2);
	svg_cubic_bezto(p, cx1, cy1, cx2, cy2, x2, y2);
	*cpx2 = cx;
	*cpy2 = cy;
	*cpx = x2;
	*cpy = y2;
}

static inline float svg_sqr(float x)
{
	return x * x;
}

static inline float svg_vmag(float x, float y)
{
	return sqrtf(x * x + y * y);
}

static inline float svg_vecrat(float ux, float uy, float vx, float vy)
{
	return (ux * vx + uy * vy) / (svg_vmag(ux, uy) * svg_vmag(vx, vy));
}

static inline float svg_vecang(float ux, float uy, float vx, float vy)
{
	float r = svg_vecrat(ux, uy, vx, vy);
	if(r < -1.0f)
		r = -1.0f;
	if(r > 1.0f)
		r = 1.0f;
	return ((ux * vy < uy * vx) ? -1.0f : 1.0f) * acosf(r);
}

static void svg_path_arc_to(struct svg_parser_t * p, float * cpx, float * cpy, float * args, int rel)
{
	float rx, ry, rotx;
	float x1, y1, x2, y2, cx, cy, dx, dy, d;
	float x1p, y1p, cxp, cyp, s, sa, sb;
	float ux, uy, vx, vy, a1, da;
	float x, y, tanx, tany, a, px = 0, py = 0, ptanx = 0, ptany = 0, t[6];
	float sinrx, cosrx;
	int fa, fs;
	int i, ndivs;
	float hda, kappa;

	rx = fabsf(args[0]);
	ry = fabsf(args[1]);
	rotx = args[2] / 180.0f * M_PI;
	fa = fabsf(args[3]) > 1e-6 ? 1 : 0;
	fs = fabsf(args[4]) > 1e-6 ? 1 : 0;
	x1 = *cpx;
	y1 = *cpy;
	if(rel)
	{
		x2 = *cpx + args[5];
		y2 = *cpy + args[6];
	}
	else
	{
		x2 = args[5];
		y2 = args[6];
	}

	dx = x1 - x2;
	dy = y1 - y2;
	d = sqrtf(dx * dx + dy * dy);
	if(d < 1e-6f || rx < 1e-6f || ry < 1e-6f)
	{
		svg_line_to(p, x2, y2);
		*cpx = x2;
		*cpy = y2;
		return;
	}

	sinrx = sinf(rotx);
	cosrx = cosf(rotx);

	x1p = cosrx * dx / 2.0f + sinrx * dy / 2.0f;
	y1p = -sinrx * dx / 2.0f + cosrx * dy / 2.0f;
	d = svg_sqr(x1p) / svg_sqr(rx) + svg_sqr(y1p) / svg_sqr(ry);
	if(d > 1)
	{
		d = sqrtf(d);
		rx *= d;
		ry *= d;
	}
	s = 0.0f;
	sa = svg_sqr(rx) * svg_sqr(ry) - svg_sqr(rx) * svg_sqr(y1p) - svg_sqr(ry) * svg_sqr(x1p);
	sb = svg_sqr(rx) * svg_sqr(y1p) + svg_sqr(ry) * svg_sqr(x1p);
	if(sa < 0.0f)
		sa = 0.0f;
	if(sb > 0.0f)
		s = sqrtf(sa / sb);
	if(fa == fs)
		s = -s;
	cxp = s * rx * y1p / ry;
	cyp = s * -ry * x1p / rx;

	cx = (x1 + x2) / 2.0f + cosrx * cxp - sinrx * cyp;
	cy = (y1 + y2) / 2.0f + sinrx * cxp + cosrx * cyp;

	ux = (x1p - cxp) / rx;
	uy = (y1p - cyp) / ry;
	vx = (-x1p - cxp) / rx;
	vy = (-y1p - cyp) / ry;
	a1 = svg_vecang(1.0f, 0.0f, ux, uy);
	da = svg_vecang(ux, uy, vx, vy);

	if(fs == 0 && da > 0)
		da -= 2 * M_PI;
	else if(fs == 1 && da < 0)
		da += 2 * M_PI;

	t[0] = cosrx;
	t[1] = sinrx;
	t[2] = -sinrx;
	t[3] = cosrx;
	t[4] = cx;
	t[5] = cy;

	ndivs = (int)(fabsf(da) / (M_PI * 0.5f) + 1.0f);
	hda = (da / (float)ndivs) / 2.0f;
	kappa = fabsf(4.0f / 3.0f * (1.0f - cosf(hda)) / sinf(hda));
	if(da < 0.0f)
		kappa = -kappa;

	for(i = 0; i <= ndivs; i++)
	{
		a = a1 + da * ((float)i / (float)ndivs);
		dx = cosf(a);
		dy = sinf(a);
		svg_xform_point(&x, &y, dx * rx, dy * ry, t);
		svg_xform_vec(&tanx, &tany, -dy * rx * kappa, dx * ry * kappa, t);
		if(i > 0)
			svg_cubic_bezto(p, px + ptanx, py + ptany, x - tanx, y - tany, x, y);
		px = x;
		py = y;
		ptanx = tanx;
		ptany = tany;
	}
	*cpx = x2;
	*cpy = y2;
}

static void svg_parse_path(struct svg_parser_t * p, const char ** attr)
{
	const char * s = NULL;
	char cmd = '\0';
	float args[10];
	int nargs;
	int rargs = 0;
	float cpx, cpy, cpx2, cpy2;
	const char * tmp[4];
	int closed;
	int i;
	char item[64];

	for(i = 0; attr[i]; i += 2)
	{
		if(strcmp(attr[i], "d") == 0)
		{
			s = attr[i + 1];
		}
		else
		{
			tmp[0] = attr[i];
			tmp[1] = attr[i + 1];
			tmp[2] = 0;
			tmp[3] = 0;
			svg_parse_attribs(p, tmp);
		}
	}
	if(s)
	{
		svg_reset_path(p);
		cpx = 0;
		cpy = 0;
		cpx2 = 0;
		cpy2 = 0;
		closed = 0;
		nargs = 0;
		while(*s)
		{
			s = svg_get_next_path_item(s, item);
			if(!*item)
				break;
			if(svg_isnum(item[0]))
			{
				if(nargs < 10)
					args[nargs++] = (float)svg_atof(item);
				if(nargs >= rargs)
				{
					switch(cmd)
					{
					case 'm':
					case 'M':
						svg_path_move_to(p, &cpx, &cpy, args, cmd == 'm' ? 1 : 0);
						cmd = (cmd == 'm') ? 'l' : 'L';
						rargs = svg_get_args_per_element(cmd);
						cpx2 = cpx;
						cpy2 = cpy;
						break;
					case 'l':
					case 'L':
						svg_path_line_to(p, &cpx, &cpy, args, cmd == 'l' ? 1 : 0);
						cpx2 = cpx;
						cpy2 = cpy;
						break;
					case 'H':
					case 'h':
						svg_path_hline_to(p, &cpx, &cpy, args, cmd == 'h' ? 1 : 0);
						cpx2 = cpx;
						cpy2 = cpy;
						break;
					case 'V':
					case 'v':
						svg_path_vline_to(p, &cpx, &cpy, args, cmd == 'v' ? 1 : 0);
						cpx2 = cpx;
						cpy2 = cpy;
						break;
					case 'C':
					case 'c':
						svg_path_cubic_bez_to(p, &cpx, &cpy, &cpx2, &cpy2, args, cmd == 'c' ? 1 : 0);
						break;
					case 'S':
					case 's':
						svg_path_cubic_bez_short_To(p, &cpx, &cpy, &cpx2, &cpy2, args, cmd == 's' ? 1 : 0);
						break;
					case 'Q':
					case 'q':
						svg_path_quad_bez_to(p, &cpx, &cpy, &cpx2, &cpy2, args, cmd == 'q' ? 1 : 0);
						break;
					case 'T':
					case 't':
						svg_path_quad_bez_short_to(p, &cpx, &cpy, &cpx2, &cpy2, args, cmd == 't' ? 1 : 0);
						break;
					case 'A':
					case 'a':
						svg_path_arc_to(p, &cpx, &cpy, args, cmd == 'a' ? 1 : 0);
						cpx2 = cpx;
						cpy2 = cpy;
						break;
					default:
						if(nargs >= 2)
						{
							cpx = args[nargs - 2];
							cpy = args[nargs - 1];
							cpx2 = cpx;
							cpy2 = cpy;
						}
						break;
					}
					nargs = 0;
				}
			}
			else
			{
				cmd = item[0];
				rargs = svg_get_args_per_element(cmd);
				if(cmd == 'M' || cmd == 'm')
				{
					if(p->npts > 0)
						svg_add_path(p, closed);
					svg_reset_path(p);
					closed = 0;
					nargs = 0;
				}
				else if(cmd == 'Z' || cmd == 'z')
				{
					closed = 1;
					if(p->npts > 0)
					{
						cpx = p->pts[0];
						cpy = p->pts[1];
						cpx2 = cpx;
						cpy2 = cpy;
						svg_add_path(p, closed);
					}
					svg_reset_path(p);
					svg_move_to(p, cpx, cpy);
					closed = 0;
					nargs = 0;
				}
			}
		}
		if(p->npts)
			svg_add_path(p, closed);
	}
	svg_add_shape(p);
}

static void svg_parse_rect(struct svg_parser_t * p, const char ** attr)
{
	float x = 0.0f;
	float y = 0.0f;
	float w = 0.0f;
	float h = 0.0f;
	float rx = -1.0f;
	float ry = -1.0f;
	int i;

	for(i = 0; attr[i]; i += 2)
	{
		if(strcmp(attr[i], "x") == 0)
			x = svg_parse_coordinate(p, attr[i + 1], svg_actual_origx(p), svg_actual_width(p));
		else if(strcmp(attr[i], "y") == 0)
			y = svg_parse_coordinate(p, attr[i + 1], svg_actual_origy(p), svg_actual_height(p));
		else if(strcmp(attr[i], "width") == 0)
			w = svg_parse_coordinate(p, attr[i + 1], 0.0f, svg_actual_width(p));
		else if(strcmp(attr[i], "height") == 0)
			h = svg_parse_coordinate(p, attr[i + 1], 0.0f, svg_actual_height(p));
		else if(strcmp(attr[i], "rx") == 0)
			rx = fabsf(svg_parse_coordinate(p, attr[i + 1], 0.0f, svg_actual_width(p)));
		else if(strcmp(attr[i], "ry") == 0)
			ry = fabsf(svg_parse_coordinate(p, attr[i + 1], 0.0f, svg_actual_height(p)));
		else
			svg_parse_attr(p, attr[i], attr[i + 1]);
	}
	if(rx < 0.0f && ry > 0.0f)
		rx = ry;
	if(ry < 0.0f && rx > 0.0f)
		ry = rx;
	if(rx < 0.0f)
		rx = 0.0f;
	if(ry < 0.0f)
		ry = 0.0f;
	if(rx > w / 2.0f)
		rx = w / 2.0f;
	if(ry > h / 2.0f)
		ry = h / 2.0f;
	if(w != 0.0f && h != 0.0f)
	{
		svg_reset_path(p);
		if(rx < 0.00001f || ry < 0.0001f)
		{
			svg_move_to(p, x, y);
			svg_line_to(p, x + w, y);
			svg_line_to(p, x + w, y + h);
			svg_line_to(p, x, y + h);
		}
		else
		{
			svg_move_to(p, x + rx, y);
			svg_line_to(p, x + w - rx, y);
			svg_cubic_bezto(p, x + w - rx * (1 - SVG_KAPPA90), y, x + w, y + ry * (1 - SVG_KAPPA90), x + w, y + ry);
			svg_line_to(p, x + w, y + h - ry);
			svg_cubic_bezto(p, x + w, y + h - ry * (1 - SVG_KAPPA90), x + w - rx * (1 - SVG_KAPPA90), y + h, x + w - rx, y + h);
			svg_line_to(p, x + rx, y + h);
			svg_cubic_bezto(p, x + rx * (1 - SVG_KAPPA90), y + h, x, y + h - ry * (1 - SVG_KAPPA90), x, y + h - ry);
			svg_line_to(p, x, y + ry);
			svg_cubic_bezto(p, x, y + ry * (1 - SVG_KAPPA90), x + rx * (1 - SVG_KAPPA90), y, x + rx, y);
		}
		svg_add_path(p, 1);
		svg_add_shape(p);
	}
}

static void svg_parse_circle(struct svg_parser_t * p, const char ** attr)
{
	float cx = 0.0f;
	float cy = 0.0f;
	float r = 0.0f;
	int i;

	for(i = 0; attr[i]; i += 2)
	{
		if(!svg_parse_attr(p, attr[i], attr[i + 1]))
		{
			if(strcmp(attr[i], "cx") == 0)
				cx = svg_parse_coordinate(p, attr[i + 1], svg_actual_origx(p), svg_actual_width(p));
			if(strcmp(attr[i], "cy") == 0)
				cy = svg_parse_coordinate(p, attr[i + 1], svg_actual_origy(p), svg_actual_height(p));
			if(strcmp(attr[i], "r") == 0)
				r = fabsf(svg_parse_coordinate(p, attr[i + 1], 0.0f, svg_actual_length(p)));
		}
	}
	if(r > 0.0f)
	{
		svg_reset_path(p);
		svg_move_to(p, cx + r, cy);
		svg_cubic_bezto(p, cx + r, cy + r * SVG_KAPPA90, cx + r * SVG_KAPPA90, cy + r, cx, cy + r);
		svg_cubic_bezto(p, cx - r * SVG_KAPPA90, cy + r, cx - r, cy + r * SVG_KAPPA90, cx - r, cy);
		svg_cubic_bezto(p, cx - r, cy - r * SVG_KAPPA90, cx - r * SVG_KAPPA90, cy - r, cx, cy - r);
		svg_cubic_bezto(p, cx + r * SVG_KAPPA90, cy - r, cx + r, cy - r * SVG_KAPPA90, cx + r, cy);
		svg_add_path(p, 1);
		svg_add_shape(p);
	}
}

static void svg_parse_ellipse(struct svg_parser_t * p, const char ** attr)
{
	float cx = 0.0f;
	float cy = 0.0f;
	float rx = 0.0f;
	float ry = 0.0f;
	int i;

	for(i = 0; attr[i]; i += 2)
	{
		if(!svg_parse_attr(p, attr[i], attr[i + 1]))
		{
			if(strcmp(attr[i], "cx") == 0)
				cx = svg_parse_coordinate(p, attr[i + 1], svg_actual_origx(p), svg_actual_width(p));
			if(strcmp(attr[i], "cy") == 0)
				cy = svg_parse_coordinate(p, attr[i + 1], svg_actual_origy(p), svg_actual_height(p));
			if(strcmp(attr[i], "rx") == 0)
				rx = fabsf(svg_parse_coordinate(p, attr[i + 1], 0.0f, svg_actual_width(p)));
			if(strcmp(attr[i], "ry") == 0)
				ry = fabsf(svg_parse_coordinate(p, attr[i + 1], 0.0f, svg_actual_height(p)));
		}
	}
	if(rx > 0.0f && ry > 0.0f)
	{
		svg_reset_path(p);
		svg_move_to(p, cx + rx, cy);
		svg_cubic_bezto(p, cx + rx, cy + ry * SVG_KAPPA90, cx + rx * SVG_KAPPA90, cy + ry, cx, cy + ry);
		svg_cubic_bezto(p, cx - rx * SVG_KAPPA90, cy + ry, cx - rx, cy + ry * SVG_KAPPA90, cx - rx, cy);
		svg_cubic_bezto(p, cx - rx, cy - ry * SVG_KAPPA90, cx - rx * SVG_KAPPA90, cy - ry, cx, cy - ry);
		svg_cubic_bezto(p, cx + rx * SVG_KAPPA90, cy - ry, cx + rx, cy - ry * SVG_KAPPA90, cx + rx, cy);
		svg_add_path(p, 1);
		svg_add_shape(p);
	}
}

static void svg_parse_line(struct svg_parser_t * p, const char ** attr)
{
	float x1 = 0.0;
	float y1 = 0.0;
	float x2 = 0.0;
	float y2 = 0.0;
	int i;

	for(i = 0; attr[i]; i += 2)
	{
		if(!svg_parse_attr(p, attr[i], attr[i + 1]))
		{
			if(strcmp(attr[i], "x1") == 0)
				x1 = svg_parse_coordinate(p, attr[i + 1], svg_actual_origx(p), svg_actual_width(p));
			if(strcmp(attr[i], "y1") == 0)
				y1 = svg_parse_coordinate(p, attr[i + 1], svg_actual_origy(p), svg_actual_height(p));
			if(strcmp(attr[i], "x2") == 0)
				x2 = svg_parse_coordinate(p, attr[i + 1], svg_actual_origx(p), svg_actual_width(p));
			if(strcmp(attr[i], "y2") == 0)
				y2 = svg_parse_coordinate(p, attr[i + 1], svg_actual_origy(p), svg_actual_height(p));
		}
	}
	svg_reset_path(p);
	svg_move_to(p, x1, y1);
	svg_line_to(p, x2, y2);
	svg_add_path(p, 0);
	svg_add_shape(p);
}

static void svg_parse_poly(struct svg_parser_t * p, const char ** attr, int closeFlag)
{
	int i;
	const char* s;
	float args[2];
	int nargs, npts = 0;
	char item[64];

	svg_reset_path(p);
	for(i = 0; attr[i]; i += 2)
	{
		if(!svg_parse_attr(p, attr[i], attr[i + 1]))
		{
			if(strcmp(attr[i], "points") == 0)
			{
				s = attr[i + 1];
				nargs = 0;
				while(*s)
				{
					s = svg_get_next_path_item(s, item);
					args[nargs++] = (float)svg_atof(item);
					if(nargs >= 2)
					{
						if(npts == 0)
							svg_move_to(p, args[0], args[1]);
						else
							svg_line_to(p, args[0], args[1]);
						nargs = 0;
						npts++;
					}
				}
			}
		}
	}
	svg_add_path(p, (char)closeFlag);
	svg_add_shape(p);
}

static void svg_parse_svg(struct svg_parser_t * p, const char ** attr)
{
	int i;
	for(i = 0; attr[i]; i += 2)
	{
		if(!svg_parse_attr(p, attr[i], attr[i + 1]))
		{
			if(strcmp(attr[i], "width") == 0)
			{
				p->svg->width = svg_parse_coordinate(p, attr[i + 1], 0.0f, 0.0f);
			}
			else if(strcmp(attr[i], "height") == 0)
			{
				p->svg->height = svg_parse_coordinate(p, attr[i + 1], 0.0f, 0.0f);
			}
			else if(strcmp(attr[i], "viewBox") == 0)
			{
				const char *s = attr[i + 1];
				char buf[64];
				s = svg_parse_number(s, buf, 64);
				p->view_minx = svg_atof(buf);
				while(*s && (svg_isspace(*s) || *s == '%' || *s == ','))
					s++;
				if(!*s)
					return;
				s = svg_parse_number(s, buf, 64);
				p->view_miny = svg_atof(buf);
				while(*s && (svg_isspace(*s) || *s == '%' || *s == ','))
					s++;
				if(!*s)
					return;
				s = svg_parse_number(s, buf, 64);
				p->view_width = svg_atof(buf);
				while(*s && (svg_isspace(*s) || *s == '%' || *s == ','))
					s++;
				if(!*s)
					return;
				s = svg_parse_number(s, buf, 64);
				p->view_height = svg_atof(buf);
			}
			else if(strcmp(attr[i], "preserveAspectRatio") == 0)
			{
				if(strstr(attr[i + 1], "none") != 0)
				{
					p->aligntype = SVG_ALIGN_NONE;
				}
				else
				{
					if(strstr(attr[i + 1], "xMin") != 0)
						p->alignx = SVG_ALIGN_MIN;
					else if(strstr(attr[i + 1], "xMid") != 0)
						p->alignx = SVG_ALIGN_MID;
					else if(strstr(attr[i + 1], "xMax") != 0)
						p->alignx = SVG_ALIGN_MAX;
					if(strstr(attr[i + 1], "yMin") != 0)
						p->aligny = SVG_ALIGN_MIN;
					else if(strstr(attr[i + 1], "yMid") != 0)
						p->aligny = SVG_ALIGN_MID;
					else if(strstr(attr[i + 1], "yMax") != 0)
						p->aligny = SVG_ALIGN_MAX;
					p->aligntype = SVG_ALIGN_MEET;
					if(strstr(attr[i + 1], "slice") != 0)
						p->aligntype = SVG_ALIGN_SLICE;
				}
			}
		}
	}
}

static void svg_parse_gradient(struct svg_parser_t * p, const char ** attr, enum svg_paint_type_t type)
{
	int i;
	struct svg_gradient_data_t * grad = malloc(sizeof(struct svg_gradient_data_t));
	if(grad == NULL)
		return;
	memset(grad, 0, sizeof(struct svg_gradient_data_t));
	grad->space = SVG_SPACE_OBJECT;
	grad->type = type;
	if(grad->type == SVG_PAINT_LINEAR_GRADIENT)
	{
		grad->linear.x1 = svg_coord(0.0f, SVG_UNITS_PERCENT);
		grad->linear.y1 = svg_coord(0.0f, SVG_UNITS_PERCENT);
		grad->linear.x2 = svg_coord(100.0f, SVG_UNITS_PERCENT);
		grad->linear.y2 = svg_coord(0.0f, SVG_UNITS_PERCENT);
	}
	else if(grad->type == SVG_PAINT_RADIAL_GRADIENT)
	{
		grad->radial.cx = svg_coord(50.0f, SVG_UNITS_PERCENT);
		grad->radial.cy = svg_coord(50.0f, SVG_UNITS_PERCENT);
		grad->radial.r = svg_coord(50.0f, SVG_UNITS_PERCENT);
	}
	svg_xform_identity(grad->xform);
	for(i = 0; attr[i]; i += 2)
	{
		if(strcmp(attr[i], "id") == 0)
		{
			strncpy(grad->id, attr[i + 1], 63);
			grad->id[63] = '\0';
		}
		else if(!svg_parse_attr(p, attr[i], attr[i + 1]))
		{
			if(strcmp(attr[i], "gradientUnits") == 0)
			{
				if(strcmp(attr[i + 1], "objectBoundingBox") == 0)
					grad->space = SVG_SPACE_OBJECT;
				else
					grad->space = SVG_SPACE_USER;
			}
			else if(strcmp(attr[i], "gradientTransform") == 0)
			{
				svg_parse_transform(grad->xform, attr[i + 1]);
			}
			else if(strcmp(attr[i], "cx") == 0)
			{
				grad->radial.cx = svg_parse_coordinate_raw(attr[i + 1]);
			}
			else if(strcmp(attr[i], "cy") == 0)
			{
				grad->radial.cy = svg_parse_coordinate_raw(attr[i + 1]);
			}
			else if(strcmp(attr[i], "r") == 0)
			{
				grad->radial.r = svg_parse_coordinate_raw(attr[i + 1]);
			}
			else if(strcmp(attr[i], "fx") == 0)
			{
				grad->radial.fx = svg_parse_coordinate_raw(attr[i + 1]);
			}
			else if(strcmp(attr[i], "fy") == 0)
			{
				grad->radial.fy = svg_parse_coordinate_raw(attr[i + 1]);
			}
			else if(strcmp(attr[i], "x1") == 0)
			{
				grad->linear.x1 = svg_parse_coordinate_raw(attr[i + 1]);
			}
			else if(strcmp(attr[i], "y1") == 0)
			{
				grad->linear.y1 = svg_parse_coordinate_raw(attr[i + 1]);
			}
			else if(strcmp(attr[i], "x2") == 0)
			{
				grad->linear.x2 = svg_parse_coordinate_raw(attr[i + 1]);
			}
			else if(strcmp(attr[i], "y2") == 0)
			{
				grad->linear.y2 = svg_parse_coordinate_raw(attr[i + 1]);
			}
			else if(strcmp(attr[i], "spreadMethod") == 0)
			{
				if(strcmp(attr[i + 1], "pad") == 0)
					grad->spread = SVG_SPREAD_PAD;
				else if(strcmp(attr[i + 1], "reflect") == 0)
					grad->spread = SVG_SPREAD_REFLECT;
				else if(strcmp(attr[i + 1], "repeat") == 0)
					grad->spread = SVG_SPREAD_REPEAT;
			}
			else if(strcmp(attr[i], "xlink:href") == 0)
			{
				const char *href = attr[i + 1];
				strncpy(grad->ref, href + 1, 62);
				grad->ref[62] = '\0';
			}
		}
	}
	grad->next = p->gradients;
	p->gradients = grad;
}

static void svg_parse_gradient_stop(struct svg_parser_t * p, const char ** attr)
{
	struct svg_attrib_t * curattr = svg_get_attr(p);
	struct svg_gradient_data_t * grad;
	struct svg_gradient_stop_t * stop;
	int i, idx;

	memset(&curattr->stop_color, 0, sizeof(struct color_t));
	curattr->stop_offset = 0;
	curattr->stop_opacity = 1.0f;
	for(i = 0; attr[i]; i += 2)
	{
		svg_parse_attr(p, attr[i], attr[i + 1]);
	}
	grad = p->gradients;
	if(grad == NULL)
		return;
	grad->nstops++;
	grad->stops = realloc(grad->stops, sizeof(struct svg_gradient_stop_t) * grad->nstops);
	if(grad->stops == NULL)
		return;
	idx = grad->nstops - 1;
	for(i = 0; i < grad->nstops - 1; i++)
	{
		if(curattr->stop_offset < grad->stops[i].offset)
		{
			idx = i;
			break;
		}
	}
	if(idx != grad->nstops - 1)
	{
		for(i = grad->nstops - 1; i > idx; i--)
			grad->stops[i] = grad->stops[i - 1];
	}
	stop = &grad->stops[idx];
	color_init(&stop->color, curattr->stop_color.r, curattr->stop_color.g, curattr->stop_color.b, curattr->stop_opacity * 255);
	stop->offset = curattr->stop_offset;
}

static void svg_start_element(void * ud, const char * el, const char ** attr)
{
	struct svg_parser_t * p = (struct svg_parser_t *)ud;

	if(p->defsflag)
	{
		if(strcmp(el, "linearGradient") == 0)
		{
			svg_parse_gradient(p, attr, SVG_PAINT_LINEAR_GRADIENT);
		}
		else if(strcmp(el, "radialGradient") == 0)
		{
			svg_parse_gradient(p, attr, SVG_PAINT_RADIAL_GRADIENT);
		}
		else if(strcmp(el, "stop") == 0)
		{
			svg_parse_gradient_stop(p, attr);
		}
		return;
	}
	if(strcmp(el, "g") == 0)
	{
		svg_push_attr(p);
		svg_parse_attribs(p, attr);
	}
	else if(strcmp(el, "path") == 0)
	{
		if(p->pathflag)
			return;
		svg_push_attr(p);
		svg_parse_path(p, attr);
		svg_pop_attr(p);
	}
	else if(strcmp(el, "rect") == 0)
	{
		svg_push_attr(p);
		svg_parse_rect(p, attr);
		svg_pop_attr(p);
	}
	else if(strcmp(el, "circle") == 0)
	{
		svg_push_attr(p);
		svg_parse_circle(p, attr);
		svg_pop_attr(p);
	}
	else if(strcmp(el, "ellipse") == 0)
	{
		svg_push_attr(p);
		svg_parse_ellipse(p, attr);
		svg_pop_attr(p);
	}
	else if(strcmp(el, "line") == 0)
	{
		svg_push_attr(p);
		svg_parse_line(p, attr);
		svg_pop_attr(p);
	}
	else if(strcmp(el, "polyline") == 0)
	{
		svg_push_attr(p);
		svg_parse_poly(p, attr, 0);
		svg_pop_attr(p);
	}
	else if(strcmp(el, "polygon") == 0)
	{
		svg_push_attr(p);
		svg_parse_poly(p, attr, 1);
		svg_pop_attr(p);
	}
	else if(strcmp(el, "linearGradient") == 0)
	{
		svg_parse_gradient(p, attr, SVG_PAINT_LINEAR_GRADIENT);
	}
	else if(strcmp(el, "radialGradient") == 0)
	{
		svg_parse_gradient(p, attr, SVG_PAINT_RADIAL_GRADIENT);
	}
	else if(strcmp(el, "stop") == 0)
	{
		svg_parse_gradient_stop(p, attr);
	}
	else if(strcmp(el, "defs") == 0)
	{
		p->defsflag = 1;
	}
	else if(strcmp(el, "svg") == 0)
	{
		svg_push_attr(p);
		svg_parse_svg(p, attr);
	}
}

static void svg_end_element(void * ud, const char * el)
{
	struct svg_parser_t * p = (struct svg_parser_t *)ud;

	if(strcmp(el, "g") == 0)
	{
		svg_pop_attr(p);
	}
	else if(strcmp(el, "path") == 0)
	{
		p->pathflag = 0;
	}
	else if(strcmp(el, "defs") == 0)
	{
		p->defsflag = 0;
	}
	else if(strcmp(el, "svg") == 0)
	{
		svg_pop_attr(p);
	}
}

static void svg_content(void * ud, const char * s)
{
}

static void svg_image_bounds(struct svg_parser_t * p, float * bounds)
{
	struct svg_shape_t* shape;
	shape = p->svg->shapes;
	if(shape == NULL)
	{
		bounds[0] = bounds[1] = bounds[2] = bounds[3] = 0.0;
		return;
	}
	bounds[0] = shape->bounds[0];
	bounds[1] = shape->bounds[1];
	bounds[2] = shape->bounds[2];
	bounds[3] = shape->bounds[3];
	for(shape = shape->next; shape != NULL; shape = shape->next)
	{
		bounds[0] = svg_minf(bounds[0], shape->bounds[0]);
		bounds[1] = svg_minf(bounds[1], shape->bounds[1]);
		bounds[2] = svg_maxf(bounds[2], shape->bounds[2]);
		bounds[3] = svg_maxf(bounds[3], shape->bounds[3]);
	}
}

static float svg_view_align(float content, float container, int type)
{
	if(type == SVG_ALIGN_MIN)
		return 0;
	else if(type == SVG_ALIGN_MAX)
		return container - content;
	return (container - content) * 0.5f;
}

static void svg_scale_gradient(struct svg_gradient_t * grad, float tx, float ty, float sx, float sy)
{
	float t[6];
	svg_xform_set_translation(t, tx, ty);
	svg_xform_multiply(grad->xform, t);
	svg_xform_set_scale(t, sx, sy);
	svg_xform_multiply(grad->xform, t);
}

static void svg_scale_to_viewbox(struct svg_parser_t * p, const char * units)
{
	struct svg_shape_t * shape;
	struct svg_path_t * path;
	float tx, ty, sx, sy, us, bounds[4], t[6], avgs;
	float * pt;
	int i;

	svg_image_bounds(p, bounds);
	if(p->view_width == 0)
	{
		if(p->svg->width > 0)
		{
			p->view_width = p->svg->width;
		}
		else
		{
			p->view_minx = bounds[0];
			p->view_width = bounds[2] - bounds[0];
		}
	}
	if(p->view_height == 0)
	{
		if(p->svg->height > 0)
		{
			p->view_height = p->svg->height;
		}
		else
		{
			p->view_miny = bounds[1];
			p->view_height = bounds[3] - bounds[1];
		}
	}
	if(p->svg->width == 0)
		p->svg->width = p->view_width;
	if(p->svg->height == 0)
		p->svg->height = p->view_height;

	tx = -p->view_minx;
	ty = -p->view_miny;
	sx = p->view_width > 0 ? p->svg->width / p->view_width : 0;
	sy = p->view_height > 0 ? p->svg->height / p->view_height : 0;
	us = 1.0f / svg_convert_to_pixels(p, svg_coord(1.0f, svg_parse_units(units)), 0.0f, 1.0f);
	if(p->aligntype == SVG_ALIGN_MEET)
	{
		sx = sy = svg_minf(sx, sy);
		tx += svg_view_align(p->view_width * sx, p->svg->width, p->alignx) / sx;
		ty += svg_view_align(p->view_height * sy, p->svg->height, p->aligny) / sy;
	}
	else if(p->aligntype == SVG_ALIGN_SLICE)
	{
		sx = sy = svg_maxf(sx, sy);
		tx += svg_view_align(p->view_width * sx, p->svg->width, p->alignx) / sx;
		ty += svg_view_align(p->view_height * sy, p->svg->height, p->aligny) / sy;
	}
	sx *= us;
	sy *= us;
	avgs = (sx + sy) / 2.0f;
	for(shape = p->svg->shapes; shape != NULL; shape = shape->next)
	{
		shape->bounds[0] = (shape->bounds[0] + tx) * sx;
		shape->bounds[1] = (shape->bounds[1] + ty) * sy;
		shape->bounds[2] = (shape->bounds[2] + tx) * sx;
		shape->bounds[3] = (shape->bounds[3] + ty) * sy;
		for(path = shape->paths; path != NULL; path = path->next)
		{
			path->bounds[0] = (path->bounds[0] + tx) * sx;
			path->bounds[1] = (path->bounds[1] + ty) * sy;
			path->bounds[2] = (path->bounds[2] + tx) * sx;
			path->bounds[3] = (path->bounds[3] + ty) * sy;
			for(i = 0; i < path->npts; i++)
			{
				pt = &path->pts[i * 2];
				pt[0] = (pt[0] + tx) * sx;
				pt[1] = (pt[1] + ty) * sy;
			}
		}
		if(shape->fill.type == SVG_PAINT_LINEAR_GRADIENT || shape->fill.type == SVG_PAINT_RADIAL_GRADIENT)
		{
			svg_scale_gradient(shape->fill.gradient, tx, ty, sx, sy);
			memcpy(t, shape->fill.gradient->xform, sizeof(float) * 6);
			svg_xform_inverse(shape->fill.gradient->xform, t);
		}
		if(shape->stroke.type == SVG_PAINT_LINEAR_GRADIENT || shape->stroke.type == SVG_PAINT_RADIAL_GRADIENT)
		{
			svg_scale_gradient(shape->stroke.gradient, tx, ty, sx, sy);
			memcpy(t, shape->stroke.gradient->xform, sizeof(float) * 6);
			svg_xform_inverse(shape->stroke.gradient->xform, t);
		}
		shape->stroke_width *= avgs;
		shape->stroke_dash_offset *= avgs;
		for(i = 0; i < shape->stroke_dash_count; i++)
			shape->stroke_dash_array[i] *= avgs;
	}
}

struct svg_t * svg_alloc(char * svgstr)
{
	struct svg_parser_t * p;
	struct svg_t * svg;

	p = svg_parser_alloc();
	if(!p)
		return NULL;

	p->dpi = 96;
	svg_parse_xml(svgstr, svg_start_element, svg_end_element, svg_content, p);
	svg_scale_to_viewbox(p, "px");
	svg = p->svg;
	p->svg = NULL;
	svg_parser_free(p);

	return svg;
}

struct svg_t * svg_alloc_from_xfs(struct xfs_context_t * ctx, const char * filename)
{
	struct xfs_file_t * file;
	struct svg_t * svg;
	char * buf;
	size_t len;

	if(!(file = xfs_open_read(ctx, filename)))
		return NULL;
	len = xfs_length(file);
	if(len <= 0)
	{
		xfs_close(file);
		return NULL;
	}
	buf = malloc(len + 1);
	if(!buf)
	{
		xfs_close(file);
		return NULL;
	}
	xfs_read(file, buf, len);
	buf[len] = '\0';
	xfs_close(file);
	svg = svg_alloc(buf);
	free(buf);

	return svg;
}

void svg_free(struct svg_t * svg)
{
	struct svg_shape_t * shape;
	struct svg_shape_t * n;

	if(svg)
	{
		shape = svg->shapes;
		while(shape)
		{
			n = shape->next;
			svg_delete_paths(shape->paths);
			svg_delete_paint(&shape->fill);
			svg_delete_paint(&shape->stroke);
			free(shape);
			shape = n;
		}
		free(svg);
	}
}
