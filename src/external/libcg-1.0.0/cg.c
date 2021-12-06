/*
 * cg.c
 *
 * Copyright(c) 2007-2021 Jianjun Jiang <8192542@qq.com>
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

#include <cg.h>

#ifndef min
#define min(a, b)		({typeof(a) _amin = (a); typeof(b) _bmin = (b); (void)(&_amin == &_bmin); _amin < _bmin ? _amin : _bmin;})
#endif
#ifndef max
#define max(a, b)		({typeof(a) _amax = (a); typeof(b) _bmax = (b); (void)(&_amax == &_bmax); _amax > _bmax ? _amax : _bmax;})
#endif
#ifndef clamp
#define clamp(v, a, b)	min(max(a, v), b)
#endif

#define cg_array_init(array) \
	do { \
		array.data = NULL; \
		array.size = 0; \
		array.capacity = 0; \
	} while(0)

#define cg_array_ensure(array, count) \
	do { \
		if(array.size + count > array.capacity) { \
			int capacity = array.size + count; \
			int newcapacity = (array.capacity == 0) ? 8 : array.capacity; \
			while(newcapacity < capacity) { newcapacity <<= 1; } \
			array.data = realloc(array.data, (size_t)newcapacity * sizeof(array.data[0])); \
			array.capacity = newcapacity; \
		} \
	} while(0)

static inline void cg_color_init_rgba(struct cg_color_t * color, double r, double g, double b, double a)
{
	color->r = r;
	color->g = g;
	color->b = b;
	color->a = a;
}

static inline void cg_rect_init(struct cg_rect_t * rect, double x, double y, double w, double h)
{
	rect->x = x;
	rect->y = y;
	rect->w = w;
	rect->h = h;
}

void cg_matrix_init(struct cg_matrix_t * m, double a, double b, double c, double d, double tx, double ty)
{
	m->a = a;   m->b = b;
	m->c = c;   m->d = d;
	m->tx = tx; m->ty = ty;
}

void cg_matrix_init_identity(struct cg_matrix_t * m)
{
	m->a = 1;  m->b = 0;
	m->c = 0;  m->d = 1;
	m->tx = 0; m->ty = 0;
}

void cg_matrix_init_translate(struct cg_matrix_t * m, double tx, double ty)
{
	m->a = 1;   m->b = 0;
	m->c = 0;   m->d = 1;
	m->tx = tx; m->ty = ty;
}

void cg_matrix_init_scale(struct cg_matrix_t * m, double sx, double sy)
{
	m->a = sx; m->b = 0;
	m->c = 0;  m->d = sy;
	m->tx = 0; m->ty = 0;
}

void cg_matrix_init_rotate(struct cg_matrix_t * m, double r)
{
	double s = sin(r);
	double c = cos(r);

	m->a = c;   m->b = s;
	m->c = -s;  m->d = c;
	m->tx = 0;  m->ty = 0;
}

void cg_matrix_translate(struct cg_matrix_t * m, double tx, double ty)
{
	m->tx += m->a * tx + m->c * ty;
	m->ty += m->b * tx + m->d * ty;
}

void cg_matrix_scale(struct cg_matrix_t * m, double sx, double sy)
{
	m->a *= sx;
	m->b *= sx;
	m->c *= sy;
	m->d *= sy;
}

void cg_matrix_rotate(struct cg_matrix_t * m, double r)
{
	double s = sin(r);
	double c = cos(r);
	double ca = c * m->a;
	double cb = c * m->b;
	double cc = c * m->c;
	double cd = c * m->d;
	double sa = s * m->a;
	double sb = s * m->b;
	double sc = s * m->c;
	double sd = s * m->d;

	m->a = ca + sc;
	m->b = cb + sd;
	m->c = cc - sa;
	m->d = cd - sb;
}

void cg_matrix_multiply(struct cg_matrix_t * m, struct cg_matrix_t * m1, struct cg_matrix_t * m2)
{
	struct cg_matrix_t t;

	t.a = m1->a * m2->a;
	t.b = 0.0;
	t.c = 0.0;
	t.d = m1->d * m2->d;
	t.tx = m1->tx * m2->a + m2->tx;
	t.ty = m1->ty * m2->d + m2->ty;
	if(m1->b != 0.0 || m1->c != 0.0 || m2->b != 0.0 || m2->c != 0.0)
	{
		t.a += m1->b * m2->c;
		t.b += m1->a * m2->b + m1->b * m2->d;
		t.c += m1->c * m2->a + m1->d * m2->c;
		t.d += m1->c * m2->b;
		t.tx += m1->ty * m2->c;
		t.ty += m1->tx * m2->b;
	}
	memcpy(m, &t, sizeof(struct cg_matrix_t));
}

void cg_matrix_invert(struct cg_matrix_t * m)
{
	double a, b, c, d, tx, ty;
	double det;

	if((m->c == 0.0) && (m->b == 0.0))
	{
		m->tx = -m->tx;
		m->ty = -m->ty;
		if(m->a != 1.0)
		{
			if(m->a == 0.0)
				return;
			m->a = 1.0 / m->a;
			m->tx *= m->a;
		}
		if(m->d != 1.0)
		{
			if(m->d == 0.0)
				return;
			m->d = 1.0 / m->d;
			m->ty *= m->d;
		}
	}
	else
	{
		det = m->a * m->d - m->b * m->c;
		if(det != 0.0)
		{
			a  = m->a;
			b  = m->b;
			c  = m->c;
			d  = m->d;
			tx = m->tx;
			ty = m->ty;
			m->a = d / det;
			m->b = -b / det;
			m->c = -c / det;
			m->d = a / det;
			m->tx = (c * ty - d * tx) / det;
			m->ty = (b * tx - a * ty) / det;
		}
	}
}

void cg_matrix_map_point(struct cg_matrix_t * m, struct cg_point_t * p1, struct cg_point_t * p2)
{
	p2->x = p1->x * m->a + p1->y * m->c + m->tx;
	p2->y = p1->x * m->b + p1->y * m->d + m->ty;
}

struct cg_surface_t * cg_surface_create(int width, int height)
{
	struct cg_surface_t * surface = malloc(sizeof(struct cg_surface_t));
	surface->ref = 1;
	surface->width = width;
	surface->height = height;
	surface->stride = width << 2;
	surface->owndata = 1;
	surface->pixels = calloc(1, (size_t)(height * surface->stride));
	return surface;
}

struct cg_surface_t * cg_surface_create_for_data(int width, int height, void * pixels)
{
	struct cg_surface_t * surface = malloc(sizeof(struct cg_surface_t));
	surface->ref = 1;
	surface->width = width;
	surface->height = height;
	surface->stride = width << 2;
	surface->owndata = 0;
	surface->pixels = pixels;
	return surface;
}

void cg_surface_destroy(struct cg_surface_t * surface)
{
	if(surface)
	{
		if(--surface->ref == 0)
		{
			if(surface->owndata)
				free(surface->pixels);
			free(surface);
		}
	}
}

struct cg_surface_t * cg_surface_reference(struct cg_surface_t * surface)
{
	if(surface)
	{
		++surface->ref;
		return surface;
	}
	return NULL;
}

struct cg_path_t * cg_path_create(void)
{
	struct cg_path_t * path = malloc(sizeof(struct cg_path_t));
	path->ref = 1;
	path->contours = 0;
	path->start.x = 0.0;
	path->start.y = 0.0;
	cg_array_init(path->elements);
	cg_array_init(path->points);
	return path;
}

void cg_path_destroy(struct cg_path_t * path)
{
	if(path)
	{
		if(--path->ref == 0)
		{
			if(path->elements.data)
				free(path->elements.data);
			if(path->points.data)
				free(path->points.data);
			free(path);
		}
	}
}

struct cg_path_t * cg_path_reference(struct cg_path_t * path)
{
	if(path)
	{
		++path->ref;
		return path;
	}
	return NULL;
}

static inline void cg_path_get_current_point(struct cg_path_t * path, double * x, double * y)
{
	if(path->points.size == 0)
	{
		*x = 0.0;
		*y = 0.0;
	}
	else
	{
		*x = path->points.data[path->points.size - 1].x;
		*y = path->points.data[path->points.size - 1].y;
	}
}

static void cg_path_move_to(struct cg_path_t * path, double x, double y)
{
	cg_array_ensure(path->elements, 1);
	cg_array_ensure(path->points, 1);

	path->elements.data[path->elements.size] = CG_PATH_ELEMENT_MOVE_TO;
	path->elements.size += 1;
	path->contours += 1;
	path->points.data[path->points.size].x = x;
	path->points.data[path->points.size].y = y;
	path->points.size += 1;
	path->start.x = x;
	path->start.y = y;
}

static void cg_path_line_to(struct cg_path_t * path, double x, double y)
{
	cg_array_ensure(path->elements, 1);
	cg_array_ensure(path->points, 1);

	path->elements.data[path->elements.size] = CG_PATH_ELEMENT_LINE_TO;
	path->elements.size += 1;
	path->points.data[path->points.size].x = x;
	path->points.data[path->points.size].y = y;
	path->points.size += 1;
}

static void cg_path_curve_to(struct cg_path_t * path, double x1, double y1, double x2, double y2, double x3, double y3)
{
	cg_array_ensure(path->elements, 1);
	cg_array_ensure(path->points, 3);

	path->elements.data[path->elements.size] = CG_PATH_ELEMENT_CURVE_TO;
	path->elements.size += 1;
	path->points.data[path->points.size].x = x1;
	path->points.data[path->points.size].y = y1;
	path->points.size += 1;
	path->points.data[path->points.size].x = x2;
	path->points.data[path->points.size].y = y2;
	path->points.size += 1;
	path->points.data[path->points.size].x = x3;
	path->points.data[path->points.size].y = y3;
	path->points.size += 1;
}

static void cg_path_quad_to(struct cg_path_t * path, double x1, double y1, double x2, double y2)
{
	double x, y;
	cg_path_get_current_point(path, &x, &y);

	double cx = 2.0 / 3.0 * x1 + 1.0 / 3.0 * x;
	double cy = 2.0 / 3.0 * y1 + 1.0 / 3.0 * y;
	double cx1 = 2.0 / 3.0 * x1 + 1.0 / 3.0 * x2;
	double cy1 = 2.0 / 3.0 * y1 + 1.0 / 3.0 * y2;
	cg_path_curve_to(path, cx, cy, cx1, cy1, x2, y2);
}

static void cg_path_close(struct cg_path_t * path)
{
	if(path->elements.size == 0)
		return;
	if(path->elements.data[path->elements.size - 1] == CG_PATH_ELEMENT_CLOSE)
		return;
	cg_array_ensure(path->elements, 1);
	cg_array_ensure(path->points, 1);
	path->elements.data[path->elements.size] = CG_PATH_ELEMENT_CLOSE;
	path->elements.size += 1;
	path->points.data[path->points.size].x = path->start.x;
	path->points.data[path->points.size].y = path->start.y;
	path->points.size += 1;
}

static void cg_path_rel_move_to(struct cg_path_t * path, double dx, double dy)
{
	double x, y;
	cg_path_get_current_point(path, &x, &y);
	cg_path_move_to(path, dx + x, dy + y);
}

static void cg_path_rel_line_to(struct cg_path_t * path, double dx, double dy)
{
	double x, y;
	cg_path_get_current_point(path, &x, &y);
	cg_path_line_to(path, dx + x, dy + y);
}

static void cg_path_rel_curve_to(struct cg_path_t * path, double dx1, double dy1, double dx2, double dy2, double dx3, double dy3)
{
	double x, y;
	cg_path_get_current_point(path, &x, &y);
	cg_path_curve_to(path, dx1 + x, dy1 + y, dx2 + x, dy2 + y, dx3 + x, dy3 + y);
}

static void cg_path_rel_quad_to(struct cg_path_t * path, double dx1, double dy1, double dx2, double dy2)
{
	double x, y;
	cg_path_get_current_point(path, &x, &y);
	cg_path_quad_to(path, dx1 + x, dy1 + y, dx2 + x, dy2 + y);
}

static inline void cg_path_add_rectangle(struct cg_path_t * path, double x, double y, double w, double h)
{
	cg_path_move_to(path, x, y);
	cg_path_line_to(path, x + w, y);
	cg_path_line_to(path, x + w, y + h);
	cg_path_line_to(path, x, y + h);
	cg_path_line_to(path, x, y);
	cg_path_close(path);
}

static inline void cg_path_add_round_rectangle(struct cg_path_t * path, double x, double y, double w, double h, double rx, double ry)
{
	rx = min(rx, w * 0.5);
	ry = min(ry, h * 0.5);

	double right = x + w;
	double bottom = y + h;
	double cpx = rx * 0.55228474983079339840;
	double cpy = ry * 0.55228474983079339840;

	cg_path_move_to(path, x, y + ry);
	cg_path_curve_to(path, x, y + ry - cpy, x + rx - cpx, y, x + rx, y);
	cg_path_line_to(path, right - rx, y);
	cg_path_curve_to(path, right - rx + cpx, y, right, y + ry - cpy, right, y + ry);
	cg_path_line_to(path, right, bottom - ry);
	cg_path_curve_to(path, right, bottom - ry + cpy, right - rx + cpx, bottom, right - rx, bottom);
	cg_path_line_to(path, x + rx, bottom);
	cg_path_curve_to(path, x + rx - cpx, bottom, x, bottom - ry + cpy, x, bottom - ry);
	cg_path_line_to(path, x, y + ry);
	cg_path_close(path);
}

static void cg_path_add_ellipse(struct cg_path_t * path, double cx, double cy, double rx, double ry)
{
	double left = cx - rx;
	double top = cy - ry;
	double right = cx + rx;
	double bottom = cy + ry;
	double cpx = rx * 0.55228474983079339840;
	double cpy = ry * 0.55228474983079339840;

	cg_path_move_to(path, cx, top);
	cg_path_curve_to(path, cx + cpx, top, right, cy - cpy, right, cy);
	cg_path_curve_to(path, right, cy + cpy, cx + cpx, bottom, cx, bottom);
	cg_path_curve_to(path, cx - cpx, bottom, left, cy + cpy, left, cy);
	cg_path_curve_to(path, left, cy - cpy, cx - cpx, top, cx, top);
	cg_path_close(path);
}

static void cg_path_add_arc(struct cg_path_t * path, double cx, double cy, double r, double a0, double a1, int ccw)
{
	double da = a1 - a0;
	if(ccw == 0)
	{
		if(fabs(da) >= M_PI * 2)
		{
			da = M_PI * 2;
		}
		else
		{
			while(da < 0.0)
				da += M_PI * 2;
		}
	}
	else
	{
		if(fabs(da) >= M_PI * 2)
		{
			da = -M_PI * 2;
		}
		else
		{
			while(da > 0.0)
				da -= M_PI * 2;
		}
	}
	int ndivs = max(1, min((int)(fabs(da) / (M_PI * 0.5) + 0.5), 5));
	double hda = (da / (double)ndivs) / 2.0;
	double kappa = fabs(4.0 / 3.0 * (1.0 - cos(hda)) / sin(hda));
	if(ccw == 1)
		kappa = -kappa;
	double px = 0, py = 0;
	double ptanx = 0, ptany = 0;
	for(int i = 0; i <= ndivs; i++)
	{
		double a = a0 + da * (i / (double)ndivs);
		double dx = cos(a);
		double dy = sin(a);
		double x = cx + dx * r;
		double y = cy + dy * r;
		double tanx = -dy * r * kappa;
		double tany = dx * r * kappa;
		if(i == 0)
		{
			if(path->elements.size == 0)
				cg_path_move_to(path, x, y);
			else
				cg_path_line_to(path, x, y);
			if(da == 0)
				break;
		}
		else
		{
			cg_path_curve_to(path, px + ptanx, py + ptany, x - tanx, y - tany, x, y);
		}
		px = x;
		py = y;
		ptanx = tanx;
		ptany = tany;
	}
}

static inline void cg_path_clear(struct cg_path_t * path)
{
	path->elements.size = 0;
	path->points.size = 0;
	path->contours = 0;
	path->start.x = 0.0;
	path->start.y = 0.0;
}

struct cg_bezier_t {
	double x1; double y1;
	double x2; double y2;
	double x3; double y3;
	double x4; double y4;
};

static inline void split(struct cg_bezier_t * b, struct cg_bezier_t * first, struct cg_bezier_t * second)
{
	double c = (b->x2 + b->x3) * 0.5;
	first->x2 = (b->x1 + b->x2) * 0.5;
	second->x3 = (b->x3 + b->x4) * 0.5;
	first->x1 = b->x1;
	second->x4 = b->x4;
	first->x3 = (first->x2 + c) * 0.5;
	second->x2 = (second->x3 + c) * 0.5;
	first->x4 = second->x1 = (first->x3 + second->x2) * 0.5;

	c = (b->y2 + b->y3) * 0.5;
	first->y2 = (b->y1 + b->y2) * 0.5;
	second->y3 = (b->y3 + b->y4) * 0.5;
	first->y1 = b->y1;
	second->y4 = b->y4;
	first->y3 = (first->y2 + c) * 0.5;
	second->y2 = (second->y3 + c) * 0.5;
	first->y4 = second->y1 = (first->y3 + second->y2) * 0.5;
}

static inline void flatten(struct cg_path_t * path, struct cg_point_t * p0, struct cg_point_t * p1, struct cg_point_t * p2, struct cg_point_t * p3)
{
	struct cg_bezier_t beziers[32];
	struct cg_bezier_t * b = beziers;

	beziers[0].x1 = p0->x;
	beziers[0].y1 = p0->y;
	beziers[0].x2 = p1->x;
	beziers[0].y2 = p1->y;
	beziers[0].x3 = p2->x;
	beziers[0].y3 = p2->y;
	beziers[0].x4 = p3->x;
	beziers[0].y4 = p3->y;
	while(b >= beziers)
	{
		double y4y1 = b->y4 - b->y1;
		double x4x1 = b->x4 - b->x1;
		double l = fabs(x4x1) + fabs(y4y1);
		double d;
		if(l > 1.0)
		{
			d = fabs((x4x1) * (b->y1 - b->y2) - (y4y1) * (b->x1 - b->x2)) + fabs((x4x1) * (b->y1 - b->y3) - (y4y1) * (b->x1 - b->x3));
		}
		else
		{
			d = fabs(b->x1 - b->x2) + fabs(b->y1 - b->y2) + fabs(b->x1 - b->x3) + fabs(b->y1 - b->y3);
			l = 1.0;
		}
		if((d < l * 0.25) || (b == beziers + 31))
		{
			cg_path_line_to(path, b->x4, b->y4);
			--b;
		}
		else
		{
			split(b, b + 1, b);
			++b;
		}
	}
}

static inline struct cg_path_t * cg_path_clone_flat(struct cg_path_t * path)
{
	struct cg_point_t * points = path->points.data;
	struct cg_path_t * result = cg_path_create();
	struct cg_point_t p0;

	cg_array_ensure(result->elements, path->elements.size);
	cg_array_ensure(result->points, path->points.size);
	for(int i = 0; i < path->elements.size; i++)
	{
		switch(path->elements.data[i])
		{
		case CG_PATH_ELEMENT_MOVE_TO:
			cg_path_move_to(result, points[0].x, points[0].y);
			points += 1;
			break;
		case CG_PATH_ELEMENT_LINE_TO:
			cg_path_line_to(result, points[0].x, points[0].y);
			points += 1;
			break;
		case CG_PATH_ELEMENT_CURVE_TO:
			cg_path_get_current_point(result, &p0.x, &p0.y);
			flatten(result, &p0, points, points + 1, points + 2);
			points += 3;
			break;
		case CG_PATH_ELEMENT_CLOSE:
			cg_path_line_to(result, points[0].x, points[0].y);
			points += 1;
			break;
		default:
			break;
		}
	}
	return result;
}

static struct cg_dash_t * cg_dash_create(double * dashes, int ndash, double offset)
{
	if(dashes && (ndash > 0))
	{
		struct cg_dash_t * dash = malloc(sizeof(struct cg_dash_t));
		dash->offset = offset;
		dash->data = malloc((size_t)ndash * sizeof(double));
		dash->size = ndash;
		memcpy(dash->data, dashes, (size_t)ndash * sizeof(double));
		return dash;
	}
	return NULL;
}

static struct cg_dash_t * cg_dash_clone(struct cg_dash_t * dash)
{
	if(dash)
		return cg_dash_create(dash->data, dash->size, dash->offset);
	return NULL;
}

static void cg_dash_destroy(struct cg_dash_t * dash)
{
	if(dash)
	{
		free(dash->data);
		free(dash);
	}
}

static inline struct cg_path_t * cg_dash_path(struct cg_dash_t * dash, struct cg_path_t * path)
{
	struct cg_path_t * flat = cg_path_clone_flat(path);
	struct cg_path_t * result = cg_path_create();
	cg_array_ensure(result->elements, flat->elements.size);
	cg_array_ensure(result->points, flat->points.size);

	int toggle = 1;
	int offset = 0;
	double phase = dash->offset;
	while(phase >= dash->data[offset])
	{
		toggle = !toggle;
		phase -= dash->data[offset];
		offset += 1;
		if(offset == dash->size)
			offset = 0;
	}
	enum cg_path_element_t * elements = flat->elements.data;
	enum cg_path_element_t * end = elements + flat->elements.size;
	struct cg_point_t * points = flat->points.data;
	while(elements < end)
	{
		int itoggle = toggle;
		int ioffset = offset;
		double iphase = phase;
		double x0 = points->x;
		double y0 = points->y;
		if(itoggle)
			cg_path_move_to(result, x0, y0);
		++elements;
		++points;
		while((elements < end) && (*elements == CG_PATH_ELEMENT_LINE_TO))
		{
			double dx = points->x - x0;
			double dy = points->y - y0;
			double dist0 = sqrt(dx * dx + dy * dy);
			double dist1 = 0;
			while(dist0 - dist1 > dash->data[ioffset] - iphase)
			{
				dist1 += dash->data[ioffset] - iphase;
				double a = dist1 / dist0;
				double x = x0 + a * dx;
				double y = y0 + a * dy;
				if(itoggle)
					cg_path_line_to(result, x, y);
				else
					cg_path_move_to(result, x, y);
				itoggle = !itoggle;
				iphase = 0;
				ioffset += 1;
				if(ioffset == dash->size)
					ioffset = 0;
			}
			iphase += dist0 - dist1;
			x0 = points->x;
			y0 = points->y;
			if(itoggle)
				cg_path_line_to(result, x0, y0);
			++elements;
			++points;
		}
	}
	cg_path_destroy(flat);
	return result;
}

static SW_FT_Outline * sw_ft_outline_create(int points, int contours)
{
	SW_FT_Outline * ft = malloc(sizeof(SW_FT_Outline));
	ft->points = malloc((size_t)(points + contours) * sizeof(SW_FT_Vector));
	ft->tags = malloc((size_t)(points + contours) * sizeof(char));
	ft->contours = malloc((size_t)contours * sizeof(short));
	ft->contours_flag = malloc((size_t)contours * sizeof(char));
	ft->n_points = ft->n_contours = 0;
	ft->flags = 0x0;
	return ft;
}

static void sw_ft_outline_destroy(SW_FT_Outline * ft)
{
	free(ft->points);
	free(ft->tags);
	free(ft->contours);
	free(ft->contours_flag);
	free(ft);
}

#define FT_COORD(x)	(SW_FT_Pos)((x) * 64)
static void sw_ft_outline_move_to(SW_FT_Outline * ft, double x, double y)
{
	ft->points[ft->n_points].x = FT_COORD(x);
	ft->points[ft->n_points].y = FT_COORD(y);
	ft->tags[ft->n_points] = SW_FT_CURVE_TAG_ON;
	if(ft->n_points)
	{
		ft->contours[ft->n_contours] = ft->n_points - 1;
		ft->n_contours++;
	}
	ft->contours_flag[ft->n_contours] = 1;
	ft->n_points++;
}

static void sw_ft_outline_line_to(SW_FT_Outline * ft, double x, double y)
{
	ft->points[ft->n_points].x = FT_COORD(x);
	ft->points[ft->n_points].y = FT_COORD(y);
	ft->tags[ft->n_points] = SW_FT_CURVE_TAG_ON;
	ft->n_points++;
}

static void sw_ft_outline_curve_to(SW_FT_Outline * ft, double x1, double y1, double x2, double y2, double x3, double y3)
{
	ft->points[ft->n_points].x = FT_COORD(x1);
	ft->points[ft->n_points].y = FT_COORD(y1);
	ft->tags[ft->n_points] = SW_FT_CURVE_TAG_CUBIC;
	ft->n_points++;

	ft->points[ft->n_points].x = FT_COORD(x2);
	ft->points[ft->n_points].y = FT_COORD(y2);
	ft->tags[ft->n_points] = SW_FT_CURVE_TAG_CUBIC;
	ft->n_points++;

	ft->points[ft->n_points].x = FT_COORD(x3);
	ft->points[ft->n_points].y = FT_COORD(y3);
	ft->tags[ft->n_points] = SW_FT_CURVE_TAG_ON;
	ft->n_points++;
}

static void sw_ft_outline_close(SW_FT_Outline * ft)
{
	ft->contours_flag[ft->n_contours] = 0;
	int index = ft->n_contours ? ft->contours[ft->n_contours - 1] + 1 : 0;
	if(index == ft->n_points)
		return;

	ft->points[ft->n_points].x = ft->points[index].x;
	ft->points[ft->n_points].y = ft->points[index].y;
	ft->tags[ft->n_points] = SW_FT_CURVE_TAG_ON;
	ft->n_points++;
}

static void sw_ft_outline_end(SW_FT_Outline * ft)
{
	if(ft->n_points)
	{
		ft->contours[ft->n_contours] = ft->n_points - 1;
		ft->n_contours++;
	}
}

static SW_FT_Outline * sw_ft_outline_convert(struct cg_path_t * path, struct cg_matrix_t *  m)
{
	SW_FT_Outline * outline = sw_ft_outline_create(path->points.size, path->contours);
	enum cg_path_element_t * elements = path->elements.data;
	struct cg_point_t * points = path->points.data;
	struct cg_point_t p[3];
	for(int i = 0; i < path->elements.size; i++)
	{
		switch(elements[i])
		{
		case CG_PATH_ELEMENT_MOVE_TO:
			cg_matrix_map_point(m, &points[0], &p[0]);
			sw_ft_outline_move_to(outline, p[0].x, p[0].y);
			points += 1;
			break;
		case CG_PATH_ELEMENT_LINE_TO:
			cg_matrix_map_point(m, &points[0], &p[0]);
			sw_ft_outline_line_to(outline, p[0].x, p[0].y);
			points += 1;
			break;
		case CG_PATH_ELEMENT_CURVE_TO:
			cg_matrix_map_point(m, &points[0], &p[0]);
			cg_matrix_map_point(m, &points[1], &p[1]);
			cg_matrix_map_point(m, &points[2], &p[2]);
			sw_ft_outline_curve_to(outline, p[0].x, p[0].y, p[1].x, p[1].y, p[2].x, p[2].y);
			points += 3;
			break;
		case CG_PATH_ELEMENT_CLOSE:
			sw_ft_outline_close(outline);
			points += 1;
			break;
		}
	}
	sw_ft_outline_end(outline);
	return outline;
}

static SW_FT_Outline * sw_ft_outline_convert_dash(struct cg_path_t * path, struct cg_matrix_t * m, struct cg_dash_t * dash)
{
	struct cg_path_t * dashed = cg_dash_path(dash, path);
	SW_FT_Outline * outline = sw_ft_outline_convert(dashed, m);
	cg_path_destroy(dashed);
	return outline;
}

static void generation_callback(int count, const SW_FT_Span * spans, void * user)
{
	struct cg_rle_t * rle = user;
	cg_array_ensure(rle->spans, count);
	struct cg_span_t * data = rle->spans.data + rle->spans.size;
	memcpy(data, spans, (size_t)count * sizeof(struct cg_span_t));
	rle->spans.size += count;
}

static void bbox_callback(int x, int y, int w, int h, void * user)
{
	struct cg_rle_t * rle = user;
	rle->x = x;
	rle->y = y;
	rle->w = w;
	rle->h = h;
}

static struct cg_rle_t * cg_rle_create(void)
{
	struct cg_rle_t * rle = malloc(sizeof(struct cg_rle_t));
	cg_array_init(rle->spans);
	rle->x = 0;
	rle->y = 0;
	rle->w = 0;
	rle->h = 0;
	return rle;
}

static void cg_rle_destroy(struct cg_rle_t * rle)
{
	if(rle)
	{
		free(rle->spans.data);
		free(rle);
	}
}

static void cg_rle_rasterize(struct cg_rle_t * rle, struct cg_path_t * path, struct cg_matrix_t * m, struct cg_rect_t * clip, struct cg_stroke_data_t * stroke, enum cg_fill_rule_t winding)
{
	SW_FT_Raster_Params params;
	params.flags = SW_FT_RASTER_FLAG_DIRECT | SW_FT_RASTER_FLAG_AA;
	params.gray_spans = generation_callback;
	params.bbox_cb = bbox_callback;
	params.user = rle;

	if(clip)
	{
		params.flags |= SW_FT_RASTER_FLAG_CLIP;
		params.clip_box.xMin = (SW_FT_Pos)clip->x;
		params.clip_box.yMin = (SW_FT_Pos)clip->y;
		params.clip_box.xMax = (SW_FT_Pos)(clip->x + clip->w);
		params.clip_box.yMax = (SW_FT_Pos)(clip->y + clip->h);
	}
	if(stroke)
	{
		SW_FT_Stroker_LineCap ftCap;
		SW_FT_Stroker_LineJoin ftJoin;
		SW_FT_Fixed ftWidth;
		SW_FT_Fixed ftMiterLimit;

		struct cg_point_t p1 = { 0, 0 };
		struct cg_point_t p2 = { M_SQRT2, M_SQRT2 };

		cg_matrix_map_point(m, &p1, &p1);
		cg_matrix_map_point(m, &p2, &p2);

		double dx = p2.x - p1.x;
		double dy = p2.y - p1.y;

		double scale = sqrt(dx * dx + dy * dy) / 2.0;
		double radius = stroke->width / 2.0;

		ftWidth = (SW_FT_Fixed)(radius * scale * (1 << 6));
		ftMiterLimit = (SW_FT_Fixed)(stroke->miterlimit * (1 << 16));

		switch(stroke->cap)
		{
		case CG_LINE_CAP_ROUND:
			ftCap = SW_FT_STROKER_LINECAP_ROUND;
			break;
		case CG_LINE_CAP_SQUARE:
			ftCap = SW_FT_STROKER_LINECAP_SQUARE;
			break;
		default:
			ftCap = SW_FT_STROKER_LINECAP_BUTT;
			break;
		}
		switch(stroke->join)
		{
		case CG_LINE_JOIN_ROUND:
			ftJoin = SW_FT_STROKER_LINEJOIN_ROUND;
			break;
		case CG_LINE_JOIN_BEVEL:
			ftJoin = SW_FT_STROKER_LINEJOIN_BEVEL;
			break;
		default:
			ftJoin = SW_FT_STROKER_LINEJOIN_MITER_FIXED;
			break;
		}
		SW_FT_Outline * outline = stroke->dash ? sw_ft_outline_convert_dash(path, m, stroke->dash) : sw_ft_outline_convert(path, m);
		SW_FT_Stroker stroker;
		SW_FT_Stroker_New(&stroker);
		SW_FT_Stroker_Set(stroker, ftWidth, ftCap, ftJoin, ftMiterLimit);
		SW_FT_Stroker_ParseOutline(stroker, outline);

		SW_FT_UInt points;
		SW_FT_UInt contours;
		SW_FT_Stroker_GetCounts(stroker, &points, &contours);

		SW_FT_Outline * strokeOutline = sw_ft_outline_create((int)points, (int)contours);
		SW_FT_Stroker_Export(stroker, strokeOutline);
		SW_FT_Stroker_Done(stroker);

		strokeOutline->flags = SW_FT_OUTLINE_NONE;
		params.source = strokeOutline;
		sw_ft_grays_raster.raster_render(NULL, &params);
		sw_ft_outline_destroy(outline);
		sw_ft_outline_destroy(strokeOutline);
	}
	else
	{
		SW_FT_Outline * outline = sw_ft_outline_convert(path, m);
		outline->flags = (winding == CG_FILL_RULE_EVEN_ODD) ? SW_FT_OUTLINE_EVEN_ODD_FILL : SW_FT_OUTLINE_NONE;
		params.source = outline;
		sw_ft_grays_raster.raster_render(NULL, &params);
		sw_ft_outline_destroy(outline);
	}
}

#define DIV255(x) (((x) + ((x) >> 8) + 0x80) >> 8)
static struct cg_rle_t * cg_rle_intersection(struct cg_rle_t * a, struct cg_rle_t * b)
{
	struct cg_rle_t * result = malloc(sizeof(struct cg_rle_t));
	cg_array_init(result->spans);
	cg_array_ensure(result->spans, max(a->spans.size, b->spans.size));

	struct cg_span_t * a_spans = a->spans.data;
	struct cg_span_t * a_end = a_spans + a->spans.size;
	struct cg_span_t * b_spans = b->spans.data;
	struct cg_span_t * b_end = b_spans + b->spans.size;
	while((a_spans < a_end) && (b_spans < b_end))
	{
		if(b_spans->y > a_spans->y)
		{
			++a_spans;
			continue;
		}
		if(a_spans->y != b_spans->y)
		{
			++b_spans;
			continue;
		}
		int ax1 = a_spans->x;
		int ax2 = ax1 + a_spans->len;
		int bx1 = b_spans->x;
		int bx2 = bx1 + b_spans->len;
		if(bx1 < ax1 && bx2 < ax1)
		{
			++b_spans;
			continue;
		}
		if(ax1 < bx1 && ax2 < bx1)
		{
			++a_spans;
			continue;
		}
		int x = max(ax1, bx1);
		int len = min(ax2, bx2) - x;
		if(len)
		{
			struct cg_span_t *span = result->spans.data + result->spans.size;
			span->x = x;
			span->len = len;
			span->y = a_spans->y;
			span->coverage = DIV255(a_spans->coverage * b_spans->coverage);
			result->spans.size += 1;
		}
		if(ax2 < bx2)
		{
			++a_spans;
		}
		else
		{
			++b_spans;
		}
	}
	if(result->spans.size == 0)
	{
		result->x = 0;
		result->y = 0;
		result->w = 0;
		result->h = 0;
		return result;
	}
	struct cg_span_t * spans = result->spans.data;
	int x1 = INT_MAX;
	int y1 = spans[0].y;
	int x2 = 0;
	int y2 = spans[result->spans.size - 1].y;
	for(int i = 0; i < result->spans.size; i++)
	{
		if(spans[i].x < x1)
			x1 = spans[i].x;
		if(spans[i].x + spans[i].len > x2)
			x2 = spans[i].x + spans[i].len;
	}
	result->x = x1;
	result->y = y1;
	result->w = x2 - x1;
	result->h = y2 - y1 + 1;

	return result;
}

static void cg_rle_intersect(struct cg_rle_t * rle, struct cg_rle_t * clip)
{
	if(rle && clip)
	{
		struct cg_rle_t * result = cg_rle_intersection(rle, clip);
		cg_array_ensure(rle->spans, result->spans.size);
		memcpy(rle->spans.data, result->spans.data, (size_t)result->spans.size * sizeof(struct cg_span_t));
		rle->spans.size = result->spans.size;
		rle->x = result->x;
		rle->y = result->y;
		rle->w = result->w;
		rle->h = result->h;
		cg_rle_destroy(result);
	}
}

static struct cg_rle_t * cg_rle_clone(struct cg_rle_t * rle)
{
	if(rle)
	{
		struct cg_rle_t * result = malloc(sizeof(struct cg_rle_t));
		cg_array_init(result->spans);
		cg_array_ensure(result->spans, rle->spans.size);
		memcpy(result->spans.data, rle->spans.data, (size_t)rle->spans.size * sizeof(struct cg_span_t));
		result->spans.size = rle->spans.size;
		result->x = rle->x;
		result->y = rle->y;
		result->w = rle->w;
		result->h = rle->h;
		return result;
	}
	return NULL;
}

static inline void cg_rle_clear(struct cg_rle_t * rle)
{
	rle->spans.size = 0;
	rle->x = 0;
	rle->y = 0;
	rle->w = 0;
	rle->h = 0;
}

struct cg_gradient_t * cg_gradient_create_linear(double x1, double y1, double x2, double y2)
{
	struct cg_gradient_t * gradient = malloc(sizeof(struct cg_gradient_t));

	gradient->ref = 1;
	gradient->type = CG_GRADIENT_TYPE_LINEAR;
	gradient->spread = CG_SPREAD_METHOD_PAD;
	gradient->opacity = 1.0;
	cg_array_init(gradient->stops);
	cg_matrix_init_identity(&gradient->matrix);
	gradient->values[0] = x1;
	gradient->values[1] = y1;
	gradient->values[2] = x2;
	gradient->values[3] = y2;
	return gradient;
}

struct cg_gradient_t * cg_gradient_create_radial(double cx, double cy, double cr, double fx, double fy, double fr)
{
	struct cg_gradient_t * gradient = malloc(sizeof(struct cg_gradient_t));

	gradient->ref = 1;
	gradient->type = CG_GRADIENT_TYPE_RADIAL;
	gradient->spread = CG_SPREAD_METHOD_PAD;
	gradient->opacity = 1.0;
	cg_array_init(gradient->stops);
	cg_matrix_init_identity(&gradient->matrix);
	gradient->values[0] = cx;
	gradient->values[1] = cy;
	gradient->values[2] = cr;
	gradient->values[3] = fx;
	gradient->values[4] = fy;
	gradient->values[5] = fr;
	return gradient;
}

void cg_gradient_destroy(struct cg_gradient_t * gradient)
{
	if(gradient)
	{
		if(--gradient->ref == 0)
		{
			free(gradient->stops.data);
			free(gradient);
		}
	}
}

struct cg_gradient_t * cg_gradient_reference(struct cg_gradient_t * gradient)
{
	if(gradient)
	{
		++gradient->ref;
		return gradient;
	}
	return NULL;
}

void cg_gradient_set_spread(struct cg_gradient_t * gradient, enum cg_spread_method_t spread)
{
	gradient->spread = spread;
}

void cg_gradient_set_matrix(struct cg_gradient_t * gradient, struct cg_matrix_t * m)
{
	memcpy(&gradient->matrix, m, sizeof(struct cg_matrix_t));
}

void cg_gradient_add_stop_rgb(struct cg_gradient_t * gradient, double offset, double r, double g, double b)
{
	cg_gradient_add_stop_rgba(gradient, offset, r, g, b, 1.0);
}

void cg_gradient_add_stop_rgba(struct cg_gradient_t * gradient, double offset, double r, double g, double b, double a)
{
	if(offset < 0.0)
		offset = 0.0;
	if(offset > 1.0)
		offset = 1.0;
	cg_array_ensure(gradient->stops, 1);
	struct cg_gradient_stop_t * stops = gradient->stops.data;
	int nstops = gradient->stops.size;
	int i;
	for(i = 0; i < nstops; i++)
	{
		if(offset < stops[i].offset)
		{
			memmove(&stops[i + 1], &stops[i], (size_t)(nstops - i) * sizeof(struct cg_gradient_stop_t));
			break;
		}
	}
	struct cg_gradient_stop_t * stop = &stops[i];
	stop->offset = offset;
	cg_color_init_rgba(&stop->color, r, g, b, a);
	gradient->stops.size += 1;
}

void cg_gradient_add_stop_color(struct cg_gradient_t * gradient, double offset, struct cg_color_t * color)
{
	cg_gradient_add_stop_rgba(gradient, offset, color->r, color->g, color->b, color->a);
}

void cg_gradient_add_stop(struct cg_gradient_t * gradient, struct cg_gradient_stop_t * stop)
{
	cg_gradient_add_stop_rgba(gradient, stop->offset, stop->color.r, stop->color.g, stop->color.b, stop->color.a);
}

void cg_gradient_clear_stops(struct cg_gradient_t * gradient)
{
	gradient->stops.size = 0;
}

void cg_gradient_set_opacity(struct cg_gradient_t * gradient, double opacity)
{
	gradient->opacity = clamp(opacity, 0.0, 1.0);
}

struct cg_texture_t * cg_texture_create(struct cg_surface_t * surface)
{
	struct cg_texture_t * texture = malloc(sizeof(struct cg_texture_t));
	texture->ref = 1;
	texture->type = CG_TEXTURE_TYPE_PLAIN;
	texture->surface = cg_surface_reference(surface);
	texture->opacity = 1.0;
	cg_matrix_init_identity(&texture->matrix);
	return texture;
}

void cg_texture_destroy(struct cg_texture_t * texture)
{
	if(texture)
	{
		if(--texture->ref == 0)
		{
			cg_surface_destroy(texture->surface);
			free(texture);
		}
	}
}

struct cg_texture_t * cg_texture_reference(struct cg_texture_t * texture)
{
	if(texture)
	{
		++texture->ref;
		return texture;
	}
	return NULL;
}

void cg_texture_set_type(struct cg_texture_t * texture, enum cg_texture_type_t type)
{
	texture->type = type;
}

void cg_texture_set_matrix(struct cg_texture_t * texture, struct cg_matrix_t * m)
{
	memcpy(&texture->matrix, m, sizeof(struct cg_matrix_t));
}

void cg_texture_set_surface(struct cg_texture_t * texture, struct cg_surface_t * surface)
{
	surface = cg_surface_reference(surface);
	cg_surface_destroy(texture->surface);
	texture->surface = surface;
}

void cg_texture_set_opacity(struct cg_texture_t * texture, double opacity)
{
	texture->opacity = clamp(opacity, 0.0, 1.0);
}

struct cg_paint_t * cg_paint_create_rgb(double r, double g, double b)
{
	return cg_paint_create_rgba(r, g, b, 1.0);
}

struct cg_paint_t * cg_paint_create_rgba(double r, double g, double b, double a)
{
	struct cg_paint_t * paint = malloc(sizeof(struct cg_paint_t));
	paint->ref = 1;
	paint->type = CG_PAINT_TYPE_COLOR;
	paint->color = malloc(sizeof(struct cg_color_t));
	cg_color_init_rgba(paint->color, r, g, b, a);
	return paint;
}

struct cg_paint_t * cg_paint_create_linear(double x1, double y1, double x2, double y2)
{
	struct cg_gradient_t * gradient = cg_gradient_create_linear(x1, y1, x2, y2);
	struct cg_paint_t * paint = cg_paint_create_gradient(gradient);
	cg_gradient_destroy(gradient);
	return paint;
}

struct cg_paint_t * cg_paint_create_radial(double cx, double cy, double cr, double fx, double fy, double fr)
{
	struct cg_gradient_t * gradient = cg_gradient_create_radial(cx, cy, cr, fx, fy, fr);
	struct cg_paint_t * paint = cg_paint_create_gradient(gradient);
	cg_gradient_destroy(gradient);
	return paint;
}

struct cg_paint_t * cg_paint_create_for_surface(struct cg_surface_t * surface)
{
	struct cg_texture_t * texture = cg_texture_create(surface);
	struct cg_paint_t * paint = cg_paint_create_texture(texture);
	cg_texture_destroy(texture);
	return paint;
}

struct cg_paint_t * cg_paint_create_color(struct cg_color_t * color)
{
	return cg_paint_create_rgba(color->r, color->g, color->b, color->a);
}

struct cg_paint_t * cg_paint_create_gradient(struct cg_gradient_t * gradient)
{
	struct cg_paint_t * paint = malloc(sizeof(struct cg_paint_t));
	paint->ref = 1;
	paint->type = CG_PAINT_TYPE_GRADIENT;
	paint->gradient = cg_gradient_reference(gradient);
	return paint;
}

struct cg_paint_t * cg_paint_create_texture(struct cg_texture_t * texture)
{
	struct cg_paint_t * paint = malloc(sizeof(struct cg_paint_t));
	paint->ref = 1;
	paint->type = CG_PAINT_TYPE_TEXTURE;
	paint->texture = cg_texture_reference(texture);
	return paint;
}

void cg_paint_destroy(struct cg_paint_t * paint)
{
	if(paint)
	{
		if(--paint->ref == 0)
		{
			switch(paint->type)
			{
			case CG_PAINT_TYPE_COLOR:
				free(paint->color);
				break;
			case CG_PAINT_TYPE_GRADIENT:
				cg_gradient_destroy(paint->gradient);
				break;
			case CG_PAINT_TYPE_TEXTURE:
				cg_texture_destroy(paint->texture);
				break;
			default:
				break;
			}
			free(paint);
		}
	}
}

struct cg_paint_t * cg_paint_reference(struct cg_paint_t * paint)
{
	if(paint)
	{
		++paint->ref;
		return paint;
	}
	return NULL;
}

enum cg_paint_type_t cg_paint_get_type(struct cg_paint_t * paint)
{
	return paint->type;
}

struct cg_color_t * cg_paint_get_color(struct cg_paint_t * paint)
{
	return (paint->type == CG_PAINT_TYPE_COLOR) ? paint->color : NULL;
}

struct cg_gradient_t * cg_paint_get_gradient(struct cg_paint_t * paint)
{
	return (paint->type == CG_PAINT_TYPE_GRADIENT) ? paint->gradient : NULL;
}

struct cg_texture_t * cg_paint_get_texture(struct cg_paint_t * paint)
{
	return (paint->type == CG_PAINT_TYPE_TEXTURE) ? paint->texture : NULL;
}

#define cg_alpha(c)		((c) >> 24)
#define cg_red(c)		(((c) >> 16) & 0xff)
#define cg_green(c)		(((c) >> 8) & 0xff)
#define cg_blue(c)		(((c) >> 0) & 0xff)

struct gradient_data_t {
	enum cg_spread_method_t spread;
	struct cg_matrix_t matrix;
	uint32_t colortable[1024];
	union {
		struct {
			double x1, y1;
			double x2, y2;
		} linear;
		struct {
			double cx, cy, cr;
			double fx, fy, fr;
		} radial;
	};
};

struct texture_data_t {
	struct cg_matrix_t matrix;
	int width;
	int height;
	int stride;
	int const_alpha;
	void * pixels;
};

struct linear_gradient_values_t {
	double dx;
	double dy;
	double l;
	double off;
};

struct radial_gradient_values_t {
	double dx;
	double dy;
	double dr;
	double sqrfr;
	double a;
	double inv2a;
	int extended;
};

static inline uint32_t premultiply_color(struct cg_color_t * color, double opacity)
{
	uint32_t alpha = (uint8_t)(color->a * opacity * 255);
	uint32_t pr = (uint8_t)(color->r * alpha);
	uint32_t pg = (uint8_t)(color->g * alpha);
	uint32_t pb = (uint8_t)(color->b * alpha);
	return (alpha << 24) | (pr << 16) | (pg << 8) | (pb);
}

static inline uint32_t combine_opacity(struct cg_color_t * color, double opacity)
{
	uint32_t a = (uint8_t)(color->a * opacity * 255);
	uint32_t r = (uint8_t)(color->r * 255);
	uint32_t g = (uint8_t)(color->g * 255);
	uint32_t b = (uint8_t)(color->b * 255);
	return (a << 24) | (r << 16) | (g << 8) | (b);
}

static inline uint32_t premultiply_pixel(uint32_t color)
{
	uint32_t a = cg_alpha(color);
	uint32_t r = cg_red(color);
	uint32_t g = cg_green(color);
	uint32_t b = cg_blue(color);
	uint32_t pr = (r * a) / 255;
	uint32_t pg = (g * a) / 255;
	uint32_t pb = (b * a) / 255;
	return (a << 24) | (pr << 16) | (pg << 8) | (pb);
}

static inline uint32_t interpolate_pixel(uint32_t x, uint32_t a, uint32_t y, uint32_t b)
{
	uint32_t t = (x & 0xff00ff) * a + (y & 0xff00ff) * b;
	t = (t + ((t >> 8) & 0xff00ff) + 0x800080) >> 8;
	t &= 0xff00ff;
	x = ((x >> 8) & 0xff00ff) * a + ((y >> 8) & 0xff00ff) * b;
	x = (x + ((x >> 8) & 0xff00ff) + 0x800080);
	x &= 0xff00ff00;
	x |= t;
	return x;
}

static inline uint32_t BYTE_MUL(uint32_t x, uint32_t a)
{
	uint32_t t = (x & 0xff00ff) * a;
	t = (t + ((t >> 8) & 0xff00ff) + 0x800080) >> 8;
	t &= 0xff00ff;
	x = ((x >> 8) & 0xff00ff) * a;
	x = (x + ((x >> 8) & 0xff00ff) + 0x800080);
	x &= 0xff00ff00;
	x |= t;
	return x;
}

static inline void memfill32(uint32_t * dest, uint32_t value, int length)
{
	for(int i = 0; i < length; i++)
		dest[i] = value;
}

static inline int gradient_clamp(struct gradient_data_t * gradient, int ipos)
{
	switch(gradient->spread)
	{
	case CG_SPREAD_METHOD_PAD:
		if(ipos < 0)
			ipos = 0;
		else if(ipos >= 1024)
			ipos = 1024 - 1;
		break;
	case CG_SPREAD_METHOD_REFLECT:
		ipos = ipos % 2048;
		ipos = ipos < 0 ? 2048 + ipos : ipos;
		ipos = ipos >= 1024 ? 2048 - 1 - ipos : ipos;
		break;
	case CG_SPREAD_METHOD_REPEAT:
		ipos = ipos % 1024;
		ipos = ipos < 0 ? 1024 + ipos : ipos;
		break;
	default:
		break;
	}
	return ipos;
}

#define FIXPT_BITS	(8)
#define FIXPT_SIZE	(1 << FIXPT_BITS)

static inline uint32_t gradient_pixel_fixed(struct gradient_data_t * gradient, int fixed_pos)
{
	int ipos = (fixed_pos + (FIXPT_SIZE / 2)) >> FIXPT_BITS;
	return gradient->colortable[gradient_clamp(gradient, ipos)];
}

static inline uint32_t gradient_pixel(struct gradient_data_t * gradient, double pos)
{
	int ipos = (int)(pos * (1024 - 1) + 0.5);
	return gradient->colortable[gradient_clamp(gradient, ipos)];
}

static void fetch_linear_gradient(uint32_t * buffer, struct linear_gradient_values_t * v, struct gradient_data_t * gradient, int y, int x, int length)
{
	double t, inc;
	double rx = 0, ry = 0;

	if(v->l == 0.0)
	{
		t = inc = 0;
	}
	else
	{
		rx = gradient->matrix.c * (y + 0.5) + gradient->matrix.a * (x + 0.5) + gradient->matrix.tx;
		ry = gradient->matrix.d * (y + 0.5) + gradient->matrix.b * (x + 0.5) + gradient->matrix.ty;
		t = v->dx * rx + v->dy * ry + v->off;
		inc = v->dx * gradient->matrix.a + v->dy * gradient->matrix.b;
		t *= (1024 - 1);
		inc *= (1024 - 1);
	}
	uint32_t * end = buffer + length;
	if((inc > -1e-5) && (inc < 1e-5))
	{
		memfill32(buffer, gradient_pixel_fixed(gradient, (int)(t * FIXPT_SIZE)), length);
	}
	else
	{
		if(t + inc * length < (double)(INT_MAX >> (FIXPT_BITS + 1)) && t + inc * length > (double)(INT_MIN >> (FIXPT_BITS + 1)))
		{
			int t_fixed = (int)(t * FIXPT_SIZE);
			int inc_fixed = (int)(inc * FIXPT_SIZE);
			while(buffer < end)
			{
				*buffer = gradient_pixel_fixed(gradient, t_fixed);
				t_fixed += inc_fixed;
				++buffer;
			}
		}
		else
		{
			while(buffer < end)
			{
				*buffer = gradient_pixel(gradient, t / 1024);
				t += inc;
				++buffer;
			}
		}
	}
}

static void fetch_radial_gradient(uint32_t * buffer, struct radial_gradient_values_t * v, struct gradient_data_t * gradient, int y, int x, int length)
{
	if(v->a == 0.0)
	{
		memfill32(buffer, 0, length);
		return;
	}

	double rx = gradient->matrix.c * (y + 0.5) + gradient->matrix.tx + gradient->matrix.a * (x + 0.5);
	double ry = gradient->matrix.d * (y + 0.5) + gradient->matrix.ty + gradient->matrix.b * (x + 0.5);
	rx -= gradient->radial.fx;
	ry -= gradient->radial.fy;

	double inv_a = 1.0 / (2.0 * v->a);
	double delta_rx = gradient->matrix.a;
	double delta_ry = gradient->matrix.b;

	double b = 2 * (v->dr * gradient->radial.fr + rx * v->dx + ry * v->dy);
	double delta_b = 2 * (delta_rx * v->dx + delta_ry * v->dy);
	double b_delta_b = 2 * b * delta_b;
	double delta_b_delta_b = 2 * delta_b * delta_b;

	double bb = b * b;
	double delta_bb = delta_b * delta_b;

	b *= inv_a;
	delta_b *= inv_a;

	double rxrxryry = rx * rx + ry * ry;
	double delta_rxrxryry = delta_rx * delta_rx + delta_ry * delta_ry;
	double rx_plus_ry = 2 * (rx * delta_rx + ry * delta_ry);
	double delta_rx_plus_ry = 2 * delta_rxrxryry;

	inv_a *= inv_a;

	double det = (bb - 4 * v->a * (v->sqrfr - rxrxryry)) * inv_a;
	double delta_det = (b_delta_b + delta_bb + 4 * v->a * (rx_plus_ry + delta_rxrxryry)) * inv_a;
	double delta_delta_det = (delta_b_delta_b + 4 * v->a * delta_rx_plus_ry) * inv_a;

	uint32_t * end = buffer + length;
	if(v->extended)
	{
		while(buffer < end)
		{
			uint32_t result = 0;
			if(det >= 0)
			{
				double w = sqrt(det) - b;
				if(gradient->radial.fr + v->dr * w >= 0)
					result = gradient_pixel(gradient, w);
			}
			*buffer = result;
			det += delta_det;
			delta_det += delta_delta_det;
			b += delta_b;
			++buffer;
		}
	}
	else
	{
		while(buffer < end)
		{
			*buffer++ = gradient_pixel(gradient, sqrt(det) - b);
			det += delta_det;
			delta_det += delta_delta_det;
			b += delta_b;
		}
	}
}

static void composition_solid_source(uint32_t * dest, int length, uint32_t color, uint32_t const_alpha)
{
	if(const_alpha == 255)
	{
		memfill32(dest, color, length);
	}
	else
	{
		uint32_t ialpha = 255 - const_alpha;
		color = BYTE_MUL(color, const_alpha);
		for(int i = 0; i < length; i++)
			dest[i] = color + BYTE_MUL(dest[i], ialpha);
	}
}

static void composition_solid_source_over(uint32_t * dest, int length, uint32_t color, uint32_t const_alpha)
{
	if(const_alpha != 255)
		color = BYTE_MUL(color, const_alpha);
	uint32_t ialpha = 255 - cg_alpha(color);
	for(int i = 0; i < length; i++)
		dest[i] = color + BYTE_MUL(dest[i], ialpha);
}

static void composition_solid_destination_in(uint32_t * dest, int length, uint32_t color, uint32_t const_alpha)
{
	uint32_t a = cg_alpha(color);
	if(const_alpha != 255)
		a = BYTE_MUL(a, const_alpha) + 255 - const_alpha;
	for(int i = 0; i < length; i++)
		dest[i] = BYTE_MUL(dest[i], a);
}

static void composition_solid_destination_out(uint32_t * dest, int length, uint32_t color, uint32_t const_alpha)
{
	uint32_t a = cg_alpha(~color);
	if(const_alpha != 255)
		a = BYTE_MUL(a, const_alpha) + 255 - const_alpha;
	for(int i = 0; i < length; i++)
		dest[i] = BYTE_MUL(dest[i], a);
}

static void composition_source(uint32_t * dest, int length, uint32_t * src, uint32_t const_alpha)
{
	if(const_alpha == 255)
	{
		memcpy(dest, src, (size_t)(length) * sizeof(uint32_t));
	}
	else
	{
		uint32_t ialpha = 255 - const_alpha;
		for(int i = 0; i < length; i++)
			dest[i] = interpolate_pixel(src[i], const_alpha, dest[i], ialpha);
	}
}

static void composition_source_over(uint32_t * dest, int length, uint32_t * src, uint32_t const_alpha)
{
	uint32_t s, sia;
	if(const_alpha == 255)
	{
		for(int i = 0; i < length; i++)
		{
			s = src[i];
			if(s >= 0xff000000)
				dest[i] = s;
			else if(s != 0)
			{
				sia = cg_alpha(~s);
				dest[i] = s + BYTE_MUL(dest[i], sia);
			}
		}
	}
	else
	{
		for(int i = 0; i < length; i++)
		{
			s = BYTE_MUL(src[i], const_alpha);
			sia = cg_alpha(~s);
			dest[i] = s + BYTE_MUL(dest[i], sia);
		}
	}
}

static void composition_destination_in(uint32_t * dest, int length, uint32_t * src, uint32_t const_alpha)
{
	if(const_alpha == 255)
	{
		for(int i = 0; i < length; i++)
			dest[i] = BYTE_MUL(dest[i], cg_alpha(src[i]));
	}
	else
	{
		uint32_t cia = 255 - const_alpha;
		uint32_t a;
		for(int i = 0; i < length; i++)
		{
			a = BYTE_MUL(cg_alpha(src[i]), const_alpha) + cia;
			dest[i] = BYTE_MUL(dest[i], a);
		}
	}
}

static void composition_destination_out(uint32_t * dest, int length, uint32_t * src, uint32_t const_alpha)
{
	if(const_alpha == 255)
	{
		for(int i = 0; i < length; i++)
			dest[i] = BYTE_MUL(dest[i], cg_alpha(~src[i]));
	}
	else
	{
		uint32_t cia = 255 - const_alpha;
		uint32_t sia;
		for(int i = 0; i < length; i++)
		{
			sia = BYTE_MUL(cg_alpha(~src[i]), const_alpha) + cia;
			dest[i] = BYTE_MUL(dest[i], sia);
		}
	}
}

typedef void (*composition_solid_function_t)(uint32_t * dest, int length, uint32_t color, uint32_t const_alpha);
typedef void (*composition_function_t)(uint32_t * dest, int length, uint32_t * src, uint32_t const_alpha);

static composition_solid_function_t composition_solid_map[] = {
	composition_solid_source,
	composition_solid_source_over,
	composition_solid_destination_in,
	composition_solid_destination_out,
};

static composition_function_t composition_map[] = {
	composition_source,
	composition_source_over,
	composition_destination_in,
	composition_destination_out,
};

static void blend_solid(struct cg_surface_t * surface, enum cg_operator_t op, struct cg_rle_t * rle, uint32_t solid)
{
	composition_solid_function_t func = composition_solid_map[op];
	int count = rle->spans.size;
	struct cg_span_t * spans = rle->spans.data;
	while(count--)
	{
		uint32_t * target = (uint32_t *)(surface->pixels + spans->y * surface->stride) + spans->x;
		func(target, spans->len, solid, spans->coverage);
		++spans;
	}
}

static void blend_linear_gradient(struct cg_surface_t * surface, enum cg_operator_t op, struct cg_rle_t * rle, struct gradient_data_t * gradient)
{
	composition_function_t func = composition_map[op];
	unsigned int buffer[1024];

	struct linear_gradient_values_t v;
	v.dx = gradient->linear.x2 - gradient->linear.x1;
	v.dy = gradient->linear.y2 - gradient->linear.y1;
	v.l = v.dx * v.dx + v.dy * v.dy;
	v.off = 0.0;
	if(v.l != 0.0)
	{
		v.dx /= v.l;
		v.dy /= v.l;
		v.off = -v.dx * gradient->linear.x1 - v.dy * gradient->linear.y1;
	}

	int count = rle->spans.size;
	struct cg_span_t * spans = rle->spans.data;
	while(count--)
	{
		int length = spans->len;
		int x = spans->x;
		while(length)
		{
			int l = min(length, 1024);
			fetch_linear_gradient(buffer, &v, gradient, spans->y, x, l);
			uint32_t * target = (uint32_t *)(surface->pixels + spans->y * surface->stride) + x;
			func(target, l, buffer, spans->coverage);
			x += l;
			length -= l;
		}
		++spans;
	}
}

static void blend_radial_gradient(struct cg_surface_t * surface, enum cg_operator_t op, struct cg_rle_t * rle, struct gradient_data_t * gradient)
{
	composition_function_t func = composition_map[op];
	unsigned int buffer[1024];

	struct radial_gradient_values_t v;
	v.dx = gradient->radial.cx - gradient->radial.fx;
	v.dy = gradient->radial.cy - gradient->radial.fy;
	v.dr = gradient->radial.cr - gradient->radial.fr;
	v.sqrfr = gradient->radial.fr * gradient->radial.fr;
	v.a = v.dr * v.dr - v.dx * v.dx - v.dy * v.dy;
	v.inv2a = 1.0 / (2.0 * v.a);
	v.extended = gradient->radial.fr != 0.0 || v.a <= 0.0;

	int count = rle->spans.size;
	struct cg_span_t * spans = rle->spans.data;
	while(count--)
	{
		int length = spans->len;
		int x = spans->x;
		while(length)
		{
			int l = min(length, 1024);
			fetch_radial_gradient(buffer, &v, gradient, spans->y, x, l);
			uint32_t * target = (uint32_t *)(surface->pixels + spans->y * surface->stride) + x;
			func(target, l, buffer, spans->coverage);
			x += l;
			length -= l;
		}
		++spans;
	}
}

#define FIXED_SCALE (1 << 16)
static void blend_untransformed_argb(struct cg_surface_t * surface, enum cg_operator_t op, struct cg_rle_t * rle, struct texture_data_t * texture)
{
	composition_function_t func = composition_map[op];

	int image_width = texture->width;
	int image_height = texture->height;
	int xoff = (int)(texture->matrix.tx);
	int yoff = (int)(texture->matrix.ty);
	int count = rle->spans.size;
	struct cg_span_t * spans = rle->spans.data;
	while(count--)
	{
		int x = spans->x;
		int length = spans->len;
		int sx = xoff + x;
		int sy = yoff + spans->y;
		if(sy >= 0 && sy < image_height && sx < image_width)
		{
			if(sx < 0)
			{
				x -= sx;
				length += sx;
				sx = 0;
			}
			if(sx + length > image_width)
				length = image_width - sx;
			if(length > 0)
			{
				int coverage = (spans->coverage * texture->const_alpha) >> 8;
				uint32_t * src = (uint32_t *)(texture->pixels + sy * texture->stride) + sx;
				uint32_t * dest = (uint32_t *)(surface->pixels + spans->y * surface->stride) + x;
				func(dest, length, src, coverage);
			}
		}
		++spans;
	}
}

static void blend_transformed_argb(struct cg_surface_t * surface, enum cg_operator_t op, struct cg_rle_t * rle, struct texture_data_t * texture)
{
	composition_function_t func = composition_map[op];
	uint32_t buffer[1024];

	int image_width = texture->width;
	int image_height = texture->height;
	int fdx = (int)(texture->matrix.a * FIXED_SCALE);
	int fdy = (int)(texture->matrix.b * FIXED_SCALE);
	int count = rle->spans.size;
	struct cg_span_t * spans = rle->spans.data;
	while(count--)
	{
		uint32_t * target = (uint32_t *)(surface->pixels + spans->y * surface->stride) + spans->x;

		double cx = spans->x + 0.5;
		double cy = spans->y + 0.5;
		int x = (int)((texture->matrix.c * cy + texture->matrix.a * cx + texture->matrix.tx) * FIXED_SCALE);
		int y = (int)((texture->matrix.d * cy + texture->matrix.b * cx + texture->matrix.ty) * FIXED_SCALE);
		int length = spans->len;
		int coverage = (spans->coverage * texture->const_alpha) >> 8;
		while(length)
		{
			int l = min(length, 1024);
			uint32_t * end = buffer + l;
			uint32_t * b = buffer;
			while(b < end)
			{
				int px = clamp(x >> 16, 0, image_width - 1);
				int py = clamp(y >> 16, 0, image_height - 1);
				*b = ((uint32_t *)(texture->pixels + py * texture->stride))[px];

				x += fdx;
				y += fdy;
				++b;
			}
			func(target, l, buffer, coverage);
			target += l;
			length -= l;
		}
		++spans;
	}
}

static void blend_untransformed_tiled_argb(struct cg_surface_t * surface, enum cg_operator_t op, struct cg_rle_t * rle, struct texture_data_t * texture)
{
	composition_function_t func = composition_map[op];

	int image_width = texture->width;
	int image_height = texture->height;
	int xoff = (int)(texture->matrix.tx) % image_width;
	int yoff = (int)(texture->matrix.ty) % image_height;
	if(xoff < 0)
		xoff += image_width;
	if(yoff < 0)
		yoff += image_height;
	int count = rle->spans.size;
	struct cg_span_t * spans = rle->spans.data;
	while(count--)
	{
		int x = spans->x;
		int length = spans->len;
		int sx = (xoff + spans->x) % image_width;
		int sy = (spans->y + yoff) % image_height;
		if(sx < 0)
			sx += image_width;
		if(sy < 0)
			sy += image_height;
		int coverage = (spans->coverage * texture->const_alpha) >> 8;
		while(length)
		{
			int l = min(image_width - sx, length);
			if(1024 < l)
				l = 1024;
			uint32_t * src = (uint32_t *)(texture->pixels + sy * texture->stride) + sx;
			uint32_t * dest = (uint32_t *)(surface->pixels + spans->y * surface->stride) + x;
			func(dest, l, src, coverage);
			x += l;
			length -= l;
			sx = 0;
		}
		++spans;
	}
}

static void blend_transformed_tiled_argb(struct cg_surface_t * surface, enum cg_operator_t op, struct cg_rle_t * rle, struct texture_data_t * texture)
{
	composition_function_t func = composition_map[op];
	uint32_t buffer[1024];

	int image_width = texture->width;
	int image_height = texture->height;
	int scanline_offset = texture->stride / 4;
	int fdx = (int)(texture->matrix.a * FIXED_SCALE);
	int fdy = (int)(texture->matrix.b * FIXED_SCALE);
	int count = rle->spans.size;
	struct cg_span_t * spans = rle->spans.data;
	while(count--)
	{
		uint32_t * target = (uint32_t *)(surface->pixels + spans->y * surface->stride) + spans->x;
		uint32_t * image_bits = (uint32_t *)texture->pixels;
		double cx = spans->x + 0.5;
		double cy = spans->y + 0.5;
		int x = (int)((texture->matrix.c * cy + texture->matrix.a * cx + texture->matrix.tx) * FIXED_SCALE);
		int y = (int)((texture->matrix.d * cy + texture->matrix.b * cx + texture->matrix.ty) * FIXED_SCALE);
		int coverage = (spans->coverage * texture->const_alpha) >> 8;
		int length = spans->len;
		while(length)
		{
			int l = min(length, 1024);
			uint32_t *end = buffer + l;
			uint32_t *b = buffer;
			int px16 = x % (image_width << 16);
			int py16 = y % (image_height << 16);
			int px_delta = fdx % (image_width << 16);
			int py_delta = fdy % (image_height << 16);
			while(b < end)
			{
				if(px16 < 0)
					px16 += image_width << 16;
				if(py16 < 0)
					py16 += image_height << 16;
				int px = px16 >> 16;
				int py = py16 >> 16;
				int y_offset = py * scanline_offset;

				*b = image_bits[y_offset + px];
				x += fdx;
				y += fdy;
				px16 += px_delta;
				if(px16 >= image_width << 16)
					px16 -= image_width << 16;
				py16 += py_delta;
				if(py16 >= image_height << 16)
					py16 -= image_height << 16;
				++b;
			}
			func(target, l, buffer, coverage);
			target += l;
			length -= l;
		}
		++spans;
	}
}

static inline void cg_blend_color(struct cg_ctx_t * ctx, struct cg_rle_t * rle, struct cg_color_t * color)
{
	if(color)
	{
		struct cg_state_t * state = ctx->state;
		uint32_t solid = premultiply_color(color, state->opacity);
		uint32_t alpha = cg_alpha(solid);
		if((alpha == 255) && (state->op == CG_OPERATOR_SRC_OVER))
			blend_solid(ctx->surface, CG_OPERATOR_SRC, rle, solid);
		else
			blend_solid(ctx->surface, state->op, rle, solid);
	}
}

static inline void cg_blend_gradient(struct cg_ctx_t * ctx, struct cg_rle_t * rle, struct cg_gradient_t * gradient)
{
	if(gradient && (gradient->stops.size > 0))
	{
		struct cg_state_t * state = ctx->state;
		struct gradient_data_t data;
		int i, pos = 0, nstop = gradient->stops.size;
		struct cg_gradient_stop_t *curr, *next, *start, *last;
		uint32_t curr_color, next_color, last_color;
		uint32_t dist, idist;
		double delta, t, incr, fpos;
		double opacity = state->opacity * gradient->opacity;

		start = gradient->stops.data;
		curr = start;
		curr_color = combine_opacity(&curr->color, opacity);

		data.colortable[pos] = premultiply_pixel(curr_color);
		++pos;
		incr = 1.0 / 1024;
		fpos = 1.5 * incr;

		while(fpos <= curr->offset)
		{
			data.colortable[pos] = data.colortable[pos - 1];
			++pos;
			fpos += incr;
		}
		for(i = 0; i < nstop - 1; i++)
		{
			curr = (start + i);
			next = (start + i + 1);
			delta = 1.0 / (next->offset - curr->offset);
			next_color = combine_opacity(&next->color, opacity);
			while(fpos < next->offset && pos < 1024)
			{
				t = (fpos - curr->offset) * delta;
				dist = (uint32_t)(255 * t);
				idist = 255 - dist;
				data.colortable[pos] = premultiply_pixel(interpolate_pixel(curr_color, idist, next_color, dist));
				++pos;
				fpos += incr;
			}
			curr_color = next_color;
		}

		last = start + nstop - 1;
		last_color = premultiply_color(&last->color, opacity);
		for(; pos < 1024; ++pos)
			data.colortable[pos] = last_color;

		data.spread = gradient->spread;
		data.matrix = gradient->matrix;
		cg_matrix_multiply(&data.matrix, &data.matrix, &state->matrix);
		cg_matrix_invert(&data.matrix);

		if(gradient->type == CG_GRADIENT_TYPE_LINEAR)
		{
			data.linear.x1 = gradient->values[0];
			data.linear.y1 = gradient->values[1];
			data.linear.x2 = gradient->values[2];
			data.linear.y2 = gradient->values[3];
			blend_linear_gradient(ctx->surface, state->op, rle, &data);
		}
		else
		{
			data.radial.cx = gradient->values[0];
			data.radial.cy = gradient->values[1];
			data.radial.cr = gradient->values[2];
			data.radial.fx = gradient->values[3];
			data.radial.fy = gradient->values[4];
			data.radial.fr = gradient->values[5];
			blend_radial_gradient(ctx->surface, state->op, rle, &data);
		}
	}
}

static inline void cg_blend_texture(struct cg_ctx_t * ctx, struct cg_rle_t * rle, struct cg_texture_t * texture)
{
	if(texture)
	{
		struct cg_state_t * state = ctx->state;
		struct texture_data_t data;
		data.width = texture->surface->width;
		data.height = texture->surface->height;
		data.stride = texture->surface->stride;
		data.const_alpha = (int)(state->opacity * texture->opacity * 256.0);
		data.pixels = texture->surface->pixels;
		data.matrix = texture->matrix;
		cg_matrix_multiply(&data.matrix, &data.matrix, &state->matrix);
		cg_matrix_invert(&data.matrix);
		struct cg_matrix_t * m = &data.matrix;
		if((m->a == 1.0) && (m->b == 0.0) && (m->c == 0.0) && (m->d == 1.0))
		{
			if(texture->type == CG_TEXTURE_TYPE_PLAIN)
				blend_untransformed_argb(ctx->surface, state->op, rle, &data);
			else
				blend_untransformed_tiled_argb(ctx->surface, state->op, rle, &data);
		}
		else
		{
			if(texture->type == CG_TEXTURE_TYPE_PLAIN)
				blend_transformed_argb(ctx->surface, state->op, rle, &data);
			else
				blend_transformed_tiled_argb(ctx->surface, state->op, rle, &data);
		}
	}
}

static void cg_blend(struct cg_ctx_t * ctx, struct cg_rle_t * rle)
{
	if(rle && (rle->spans.size > 0))
	{
		struct cg_paint_t * source = ctx->state->source;
		switch(source->type)
		{
		case CG_PAINT_TYPE_COLOR:
			cg_blend_color(ctx, rle, source->color);
			break;
		case CG_PAINT_TYPE_GRADIENT:
			cg_blend_gradient(ctx, rle, source->gradient);
			break;
		case CG_PAINT_TYPE_TEXTURE:
			cg_blend_texture(ctx, rle, source->texture);
			break;
		default:
			break;
		}
	}
}

static struct cg_state_t * cg_state_create(void)
{
	struct cg_state_t * state = malloc(sizeof(struct cg_state_t));
	state->clippath = NULL;
	state->source = cg_paint_create_rgba(0, 0, 0, 1.0);
	cg_matrix_init_identity(&state->matrix);
	state->winding = CG_FILL_RULE_NON_ZERO;
	state->stroke.width = 1.0;
	state->stroke.miterlimit = 10.0;
	state->stroke.cap = CG_LINE_CAP_BUTT;
	state->stroke.join = CG_LINE_JOIN_MITER;
	state->stroke.dash = NULL;
	state->op = CG_OPERATOR_SRC_OVER;
	state->opacity = 1.0;
	state->next = NULL;
	return state;
}

static struct cg_state_t * cg_state_clone(struct cg_state_t * state)
{
	struct cg_state_t * newstate = malloc(sizeof(struct cg_state_t));
	newstate->clippath = cg_rle_clone(state->clippath);
	newstate->source = cg_paint_reference(state->source);
	newstate->matrix = state->matrix;
	newstate->winding = state->winding;
	newstate->stroke.width = state->stroke.width;
	newstate->stroke.miterlimit = state->stroke.miterlimit;
	newstate->stroke.cap = state->stroke.cap;
	newstate->stroke.join = state->stroke.join;
	newstate->stroke.dash = cg_dash_clone(state->stroke.dash);
	newstate->op = state->op;
	newstate->opacity = state->opacity;
	newstate->next = NULL;
	return newstate;
}

static void cg_state_destroy(struct cg_state_t * state)
{
	cg_rle_destroy(state->clippath);
	cg_paint_destroy(state->source);
	cg_dash_destroy(state->stroke.dash);
	free(state);
}

struct cg_ctx_t * cg_create(struct cg_surface_t * surface)
{
	struct cg_ctx_t * ctx = malloc(sizeof(struct cg_ctx_t));
	ctx->surface = cg_surface_reference(surface);
	ctx->state = cg_state_create();
	ctx->path = cg_path_create();
	ctx->rle = cg_rle_create();
	ctx->clippath = NULL;
	cg_rect_init(&ctx->clip, 0, 0, surface->width, surface->height);
	return ctx;
}

void cg_destroy(struct cg_ctx_t * ctx)
{
	if(ctx)
	{
		while(ctx->state)
		{
			struct cg_state_t * state = ctx->state;
			ctx->state = state->next;
			cg_state_destroy(state);
		}
		cg_surface_destroy(ctx->surface);
		cg_path_destroy(ctx->path);
		cg_rle_destroy(ctx->rle);
		cg_rle_destroy(ctx->clippath);
		free(ctx);
	}
}

void cg_save(struct cg_ctx_t * ctx)
{
	struct cg_state_t * state = cg_state_clone(ctx->state);
	state->next = ctx->state;
	ctx->state = state;
}

void cg_restore(struct cg_ctx_t * ctx)
{
	struct cg_state_t * state = ctx->state;
	ctx->state = state->next;
	cg_state_destroy(state);
}

void cg_set_source_rgb(struct cg_ctx_t * ctx, double r, double g, double b)
{
	cg_set_source_rgba(ctx, r, g, b, 1.0);
}

void cg_set_source_rgba(struct cg_ctx_t * ctx, double r, double g, double b, double a)
{
	struct cg_paint_t * source = cg_paint_create_rgba(r, g, b, a);
	cg_set_source(ctx, source);
	cg_paint_destroy(source);
}

void cg_set_source_surface(struct cg_ctx_t * ctx, struct cg_surface_t * surface, double x, double y)
{
	struct cg_paint_t * source = cg_paint_create_for_surface(surface);
	struct cg_texture_t * texture = cg_paint_get_texture(source);
	struct cg_matrix_t m;
	cg_matrix_init_translate(&m, x, y);
	cg_texture_set_matrix(texture, &m);
	cg_set_source(ctx, source);
	cg_paint_destroy(source);
}

void cg_set_source_color(struct cg_ctx_t * ctx, struct cg_color_t * color)
{
	cg_set_source_rgba(ctx, color->r, color->g, color->b, color->a);
}

void cg_set_source_gradient(struct cg_ctx_t * ctx, struct cg_gradient_t * gradient)
{
	struct cg_paint_t * source = cg_paint_create_gradient(gradient);
	cg_set_source(ctx, source);
	cg_paint_destroy(source);
}

void cg_set_source_texture(struct cg_ctx_t * ctx, struct cg_texture_t * texture)
{
	struct cg_paint_t * source = cg_paint_create_texture(texture);
	cg_set_source(ctx, source);
	cg_paint_destroy(source);
}

void cg_set_source(struct cg_ctx_t * ctx, struct cg_paint_t * source)
{
	source = cg_paint_reference(source);
	cg_paint_destroy(ctx->state->source);
	ctx->state->source = source;
}

void cg_set_operator(struct cg_ctx_t * ctx, enum cg_operator_t op)
{
	ctx->state->op = op;
}

void cg_set_opacity(struct cg_ctx_t * ctx, double opacity)
{
	ctx->state->opacity = clamp(opacity, 0.0, 1.0);
}

void cg_set_fill_rule(struct cg_ctx_t * ctx, enum cg_fill_rule_t winding)
{
	ctx->state->winding = winding;
}

void cg_set_line_width(struct cg_ctx_t * ctx, double width)
{
	ctx->state->stroke.width = width;
}

void cg_set_line_cap(struct cg_ctx_t * ctx, enum cg_line_cap_t cap)
{
	ctx->state->stroke.cap = cap;
}

void cg_set_line_join(struct cg_ctx_t * ctx, enum cg_line_join_t join)
{
	ctx->state->stroke.join = join;
}

void cg_set_miter_limit(struct cg_ctx_t * ctx, double limit)
{
	ctx->state->stroke.miterlimit = limit;
}

void cg_set_dash(struct cg_ctx_t * ctx, double * dashes, int ndash, double offset)
{
	cg_dash_destroy(ctx->state->stroke.dash);
	ctx->state->stroke.dash = cg_dash_create(dashes, ndash, offset);
}

void cg_translate(struct cg_ctx_t * ctx, double tx, double ty)
{
	cg_matrix_translate(&ctx->state->matrix, tx, ty);
}

void cg_scale(struct cg_ctx_t * ctx, double sx, double sy)
{
	cg_matrix_scale(&ctx->state->matrix, sx, sy);
}

void cg_rotate(struct cg_ctx_t * ctx, double r)
{
	cg_matrix_rotate(&ctx->state->matrix, r);
}

void cg_transform(struct cg_ctx_t * ctx, struct cg_matrix_t * m)
{
	cg_matrix_multiply(&ctx->state->matrix, m, &ctx->state->matrix);
}

void cg_set_matrix(struct cg_ctx_t * ctx, struct cg_matrix_t * m)
{
	memcpy(&ctx->state->matrix, m, sizeof(struct cg_matrix_t));
}

void cg_identity_matrix(struct cg_ctx_t * ctx)
{
	cg_matrix_init_identity(&ctx->state->matrix);
}

void cg_move_to(struct cg_ctx_t * ctx, double x, double y)
{
	cg_path_move_to(ctx->path, x, y);
}

void cg_line_to(struct cg_ctx_t * ctx, double x, double y)
{
	cg_path_line_to(ctx->path, x, y);
}

void cg_quad_to(struct cg_ctx_t * ctx, double x1, double y1, double x2, double y2)
{
	cg_path_quad_to(ctx->path, x1, y1, x2, y2);
}

void cg_curve_to(struct cg_ctx_t * ctx, double x1, double y1, double x2, double y2, double x3, double y3)
{
	cg_path_curve_to(ctx->path, x1, y1, x2, y2, x3, y3);
}

void cg_rel_move_to(struct cg_ctx_t * ctx, double x, double y)
{
	cg_path_rel_move_to(ctx->path, x, y);
}

void cg_rel_line_to(struct cg_ctx_t * ctx, double x, double y)
{
	cg_path_rel_line_to(ctx->path, x, y);
}

void cg_rel_curve_to(struct cg_ctx_t * ctx, double x1, double y1, double x2, double y2, double x3, double y3)
{
	cg_path_rel_curve_to(ctx->path, x1, y1, x2, y2, x3, y3);
}

void cg_rel_quad_to(struct cg_ctx_t * ctx, double x1, double y1, double x2, double y2)
{
	cg_path_rel_quad_to(ctx->path, x1, y1, x2, y2);
}

void cg_rectangle(struct cg_ctx_t * ctx, double x, double y, double w, double h)
{
	cg_path_add_rectangle(ctx->path, x, y, w, h);
}

void cg_round_rectangle(struct cg_ctx_t * ctx, double x, double y, double w, double h, double rx, double ry)
{
	cg_path_add_round_rectangle(ctx->path, x, y, w, h, rx, ry);
}

void cg_ellipse(struct cg_ctx_t * ctx, double cx, double cy, double rx, double ry)
{
	cg_path_add_ellipse(ctx->path, cx, cy, rx, ry);
}

void cg_circle(struct cg_ctx_t * ctx, double cx, double cy, double r)
{
	cg_path_add_ellipse(ctx->path, cx, cy, r, r);
}

void cg_arc(struct cg_ctx_t * ctx, double cx, double cy, double r, double a0, double a1)
{
	cg_path_add_arc(ctx->path, cx, cy, r, a0, a1, 0);
}

void cg_arc_negative(struct cg_ctx_t * ctx, double cx, double cy, double r, double a0, double a1)
{
	cg_path_add_arc(ctx->path, cx, cy, r, a0, a1, 1);
}

void cg_new_path(struct cg_ctx_t * ctx)
{
	cg_path_clear(ctx->path);
}

void cg_close_path(struct cg_ctx_t * ctx)
{
	cg_path_close(ctx->path);
}

void cg_reset_clip(struct cg_ctx_t * ctx)
{
	cg_rle_destroy(ctx->state->clippath);
	ctx->state->clippath = NULL;
}

void cg_clip(struct cg_ctx_t * ctx)
{
	cg_clip_preserve(ctx);
	cg_path_clear(ctx->path);
}

void cg_clip_preserve(struct cg_ctx_t * ctx)
{
	struct cg_state_t * state = ctx->state;
	if(state->clippath)
	{
		cg_rle_clear(ctx->rle);
		cg_rle_rasterize(ctx->rle, ctx->path, &state->matrix, &ctx->clip, NULL, state->winding);
		cg_rle_intersect(state->clippath, ctx->rle);
	}
	else
	{
		state->clippath = cg_rle_create();
		cg_rle_rasterize(state->clippath, ctx->path, &state->matrix, &ctx->clip, NULL, state->winding);
	}
}

void cg_fill(struct cg_ctx_t * ctx)
{
	cg_fill_preserve(ctx);
	cg_path_clear(ctx->path);
}

void cg_fill_preserve(struct cg_ctx_t * ctx)
{
	struct cg_state_t * state = ctx->state;
	cg_rle_clear(ctx->rle);
	cg_rle_rasterize(ctx->rle, ctx->path, &state->matrix, &ctx->clip, NULL, state->winding);
	cg_rle_intersect(ctx->rle, state->clippath);
	cg_blend(ctx, ctx->rle);
}

void cg_stroke(struct cg_ctx_t * ctx)
{
	cg_stroke_preserve(ctx);
	cg_path_clear(ctx->path);
}

void cg_stroke_preserve(struct cg_ctx_t * ctx)
{
	struct cg_state_t * state = ctx->state;
	cg_rle_clear(ctx->rle);
	cg_rle_rasterize(ctx->rle, ctx->path, &state->matrix, &ctx->clip, &state->stroke, CG_FILL_RULE_NON_ZERO);
	cg_rle_intersect(ctx->rle, state->clippath);
	cg_blend(ctx, ctx->rle);
}

void cg_paint(struct cg_ctx_t * ctx)
{
	struct cg_state_t * state = ctx->state;
	if((state->clippath == NULL) && (ctx->clippath == NULL))
	{
		struct cg_path_t * path = cg_path_create();
		cg_path_add_rectangle(path, ctx->clip.x, ctx->clip.y, ctx->clip.w, ctx->clip.h);
		struct cg_matrix_t m;
		cg_matrix_init_identity(&m);
		ctx->clippath = cg_rle_create();
		cg_rle_rasterize(ctx->clippath, path, &m, &ctx->clip, NULL, CG_FILL_RULE_NON_ZERO);
		cg_path_destroy(path);
	}
	struct cg_rle_t * rle = state->clippath ? state->clippath : ctx->clippath;
	cg_blend(ctx, rle);
}
