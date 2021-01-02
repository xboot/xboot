/*
 * kernel/graphic/svg-raster.c
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

#include <xboot.h>
#include <graphic/surface.h>

#define SVG_SUBSAMPLES	(5)
#define SVG_FIXSHIFT	(14)
#define SVG_FIX			(1 << SVG_FIXSHIFT)
#define SVG_FIXMASK		(SVG_FIX - 1)
#define SVG_MPAGE_SIZE	(4096)

enum svg_point_flags_t {
	SVG_POINT_CORNER	= (1 << 0),
	SVG_POINT_BEVEL		= (1 << 1),
	SVG_POINT_LEFT		= (1 << 2),
};

struct svg_edge_t {
	float x0, y0, x1, y1;
	int dir;
	struct svg_edge_t * next;
};

struct svg_point_t {
	float x, y;
	float dx, dy;
	float len;
	float dmx, dmy;
	int flags;
};

struct svg_active_edge_t {
	int x, dx;
	float ey;
	int dir;
	struct svg_active_edge_t * next;
};

struct svg_mem_page_t {
	unsigned char mem[SVG_MPAGE_SIZE];
	int size;
	struct svg_mem_page_t * next;
};

struct svg_cache_paint_t {
	enum svg_paint_type_t type;
	enum svg_spread_type_t spread;
	float xform[6];
	struct color_t colors[256];
};

struct svg_rasterizer_t {
	float px, py;
	float tesstol;
	float disttol;
	struct svg_edge_t * edges;
	int nedges;
	int cedges;
	struct svg_point_t * points;
	int npoints;
	int cpoints;
	struct svg_point_t * points2;
	int npoints2;
	int cpoints2;
	struct svg_active_edge_t * freelist;
	struct svg_mem_page_t * pages;
	struct svg_mem_page_t * curpage;
	unsigned char * bitmap;
	int width, height, stride;
	unsigned char * scanline;
	int cscanline;
};

static struct svg_mem_page_t * svg_next_page(struct svg_rasterizer_t * r, struct svg_mem_page_t * cur)
{
	struct svg_mem_page_t * page;

	if(cur != NULL && cur->next != NULL)
		return cur->next;

	page = malloc(sizeof(struct svg_mem_page_t));
	if(!page)
		return NULL;
	memset(page, 0, sizeof(struct svg_mem_page_t));

	if(cur != NULL)
		cur->next = page;
	else
		r->pages = page;
	return page;
}

static void svg_reset_pool(struct svg_rasterizer_t * r)
{
	struct svg_mem_page_t * p = r->pages;
	while(p)
	{
		p->size = 0;
		p = p->next;
	}
	r->curpage = r->pages;
}

static unsigned char * svg_mem_alloc(struct svg_rasterizer_t * r, int size)
{
	unsigned char * buf;

	if(size > SVG_MPAGE_SIZE)
		return NULL;
	if(r->curpage == NULL || r->curpage->size + size > SVG_MPAGE_SIZE)
		r->curpage = svg_next_page(r, r->curpage);
	buf = &r->curpage->mem[r->curpage->size];
	r->curpage->size += size;
	return buf;
}

static int svg_pt_equals(float x1, float y1, float x2, float y2, float tol)
{
	float dx = x2 - x1;
	float dy = y2 - y1;
	return dx * dx + dy * dy < tol * tol;
}

static void svg_add_path_point(struct svg_rasterizer_t * r, float x, float y, int flags)
{
	struct svg_point_t * pt;

	if(r->npoints > 0)
	{
		pt = &r->points[r->npoints - 1];
		if(svg_pt_equals(pt->x, pt->y, x, y, r->disttol))
		{
			pt->flags |= flags;
			return;
		}
	}
	if(r->npoints + 1 > r->cpoints)
	{
		r->cpoints = r->cpoints > 0 ? r->cpoints * 2 : 64;
		r->points = realloc(r->points, sizeof(struct svg_point_t) * r->cpoints);
		if(r->points == NULL)
			return;
	}
	pt = &r->points[r->npoints];
	pt->x = x;
	pt->y = y;
	pt->flags = flags;
	r->npoints++;
}

static void svg_append_path_point(struct svg_rasterizer_t * r, struct svg_point_t pt)
{
	if(r->npoints + 1 > r->cpoints)
	{
		r->cpoints = r->cpoints > 0 ? r->cpoints * 2 : 64;
		r->points = realloc(r->points, sizeof(struct svg_point_t) * r->cpoints);
		if(!r->points)
			return;
	}
	r->points[r->npoints] = pt;
	r->npoints++;
}

static void svg_duplicate_points(struct svg_rasterizer_t * r)
{
	if(r->npoints > r->cpoints2)
	{
		r->cpoints2 = r->npoints;
		r->points2 = realloc(r->points2, sizeof(struct svg_point_t) * r->cpoints2);
		if(!r->points2)
			return;
	}
	memcpy(r->points2, r->points, sizeof(struct svg_point_t) * r->npoints);
	r->npoints2 = r->npoints;
}

static void svg_add_edge(struct svg_rasterizer_t * r, float x0, float y0, float x1, float y1)
{
	struct svg_edge_t * e;

	if(y0 == y1)
		return;
	if(r->nedges + 1 > r->cedges)
	{
		r->cedges = r->cedges > 0 ? r->cedges * 2 : 64;
		r->edges = (struct svg_edge_t *)realloc(r->edges, sizeof(struct svg_edge_t) * r->cedges);
		if(r->edges == NULL)
			return;
	}
	e = &r->edges[r->nedges];
	r->nedges++;
	if(y0 < y1)
	{
		e->x0 = x0;
		e->y0 = y0;
		e->x1 = x1;
		e->y1 = y1;
		e->dir = 1;
	}
	else
	{
		e->x0 = x1;
		e->y0 = y1;
		e->x1 = x0;
		e->y1 = y0;
		e->dir = -1;
	}
}

static float svg_normalize(float * x, float * y)
{
	float d = sqrtf((*x) * (*x) + (*y) * (*y));
	if(d > 1e-6f)
	{
		float id = 1.0f / d;
		*x *= id;
		*y *= id;
	}
	return d;
}

static void svg_flatten_cubic_bez(struct svg_rasterizer_t * r, float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, int level, int type)
{
	float x12, y12, x23, y23, x34, y34, x123, y123, x234, y234, x1234, y1234;
	float dx, dy, d2, d3;

	if(level > 10)
		return;
	x12 = (x1 + x2) * 0.5f;
	y12 = (y1 + y2) * 0.5f;
	x23 = (x2 + x3) * 0.5f;
	y23 = (y2 + y3) * 0.5f;
	x34 = (x3 + x4) * 0.5f;
	y34 = (y3 + y4) * 0.5f;
	x123 = (x12 + x23) * 0.5f;
	y123 = (y12 + y23) * 0.5f;

	dx = x4 - x1;
	dy = y4 - y1;
	d2 = fabsf(((x2 - x4) * dy - (y2 - y4) * dx));
	d3 = fabsf(((x3 - x4) * dy - (y3 - y4) * dx));

	if((d2 + d3) * (d2 + d3) < r->tesstol * (dx * dx + dy * dy))
	{
		svg_add_path_point(r, x4, y4, type);
		return;
	}
	x234 = (x23 + x34) * 0.5f;
	y234 = (y23 + y34) * 0.5f;
	x1234 = (x123 + x234) * 0.5f;
	y1234 = (y123 + y234) * 0.5f;

	svg_flatten_cubic_bez(r, x1, y1, x12, y12, x123, y123, x1234, y1234, level + 1, 0);
	svg_flatten_cubic_bez(r, x1234, y1234, x234, y234, x34, y34, x4, y4, level + 1, type);
}

static void svg_flatten_shape(struct svg_rasterizer_t * r, struct svg_shape_t * shape, float sx, float sy)
{
	struct svg_path_t * path;
	int i, j;

	for(path = shape->paths; path != NULL; path = path->next)
	{
		r->npoints = 0;
		svg_add_path_point(r, path->pts[0] * sx, path->pts[1] * sy, 0);
		for(i = 0; i < path->npts - 1; i += 3)
		{
			float * p = &path->pts[i * 2];
			svg_flatten_cubic_bez(r, p[0] * sx, p[1] * sy, p[2] * sx, p[3] * sy, p[4] * sx, p[5] * sy, p[6] * sx, p[7] * sy, 0, 0);
		}
		svg_add_path_point(r, path->pts[0] * sx, path->pts[1] * sy, 0);
		for(i = 0, j = r->npoints - 1; i < r->npoints; j = i++)
			svg_add_edge(r, r->points[j].x, r->points[j].y, r->points[i].x, r->points[i].y);
	}
}

static void svg_init_closed(struct svg_point_t * left, struct svg_point_t * right, struct svg_point_t * p0, struct svg_point_t * p1, float width)
{
	float w = width * 0.5f;
	float dx = p1->x - p0->x;
	float dy = p1->y - p0->y;
	float len = svg_normalize(&dx, &dy);
	float px = p0->x + dx * len * 0.5f, py = p0->y + dy * len * 0.5f;
	float dlx = dy, dly = -dx;
	float lx = px - dlx * w, ly = py - dly * w;
	float rx = px + dlx * w, ry = py + dly * w;
	left->x = lx;
	left->y = ly;
	right->x = rx;
	right->y = ry;
}

static void svg_butt_cap(struct svg_rasterizer_t * r, struct svg_point_t * left, struct svg_point_t * right, struct svg_point_t * p, float dx, float dy, float width, int connect)
{
	float w = width * 0.5f;
	float px = p->x, py = p->y;
	float dlx = dy, dly = -dx;
	float lx = px - dlx * w, ly = py - dly * w;
	float rx = px + dlx * w, ry = py + dly * w;

	svg_add_edge(r, lx, ly, rx, ry);
	if(connect)
	{
		svg_add_edge(r, left->x, left->y, lx, ly);
		svg_add_edge(r, rx, ry, right->x, right->y);
	}
	left->x = lx;
	left->y = ly;
	right->x = rx;
	right->y = ry;
}

static void svg_square_cap(struct svg_rasterizer_t * r, struct svg_point_t * left, struct svg_point_t * right, struct svg_point_t * p, float dx, float dy, float width, int connect)
{
	float w = width * 0.5f;
	float px = p->x - dx * w, py = p->y - dy * w;
	float dlx = dy, dly = -dx;
	float lx = px - dlx * w, ly = py - dly * w;
	float rx = px + dlx * w, ry = py + dly * w;

	svg_add_edge(r, lx, ly, rx, ry);
	if(connect)
	{
		svg_add_edge(r, left->x, left->y, lx, ly);
		svg_add_edge(r, rx, ry, right->x, right->y);
	}
	left->x = lx;
	left->y = ly;
	right->x = rx;
	right->y = ry;
}

static void svg_round_cap(struct svg_rasterizer_t * r, struct svg_point_t * left, struct svg_point_t * right, struct svg_point_t * p, float dx, float dy, float width, int ncap, int connect)
{
	float w = width * 0.5f;
	float px = p->x, py = p->y;
	float dlx = dy, dly = -dx;
	float lx = 0, ly = 0, rx = 0, ry = 0, prevx = 0, prevy = 0;
	int i;

	for(i = 0; i < ncap; i++)
	{
		float a = (float)i / (float)(ncap - 1) * M_PI;
		float ax = cosf(a) * w, ay = sinf(a) * w;
		float x = px - dlx * ax - dx * ay;
		float y = py - dly * ax - dy * ay;
		if(i > 0)
			svg_add_edge(r, prevx, prevy, x, y);
		prevx = x;
		prevy = y;
		if(i == 0)
		{
			lx = x;
			ly = y;
		}
		else if(i == ncap - 1)
		{
			rx = x;
			ry = y;
		}
	}
	if(connect)
	{
		svg_add_edge(r, left->x, left->y, lx, ly);
		svg_add_edge(r, rx, ry, right->x, right->y);
	}
	left->x = lx;
	left->y = ly;
	right->x = rx;
	right->y = ry;
}

static void svg_bevel_join(struct svg_rasterizer_t * r, struct svg_point_t * left, struct svg_point_t * right, struct svg_point_t * p0, struct svg_point_t * p1, float width)
{
	float w = width * 0.5f;
	float dlx0 = p0->dy, dly0 = -p0->dx;
	float dlx1 = p1->dy, dly1 = -p1->dx;
	float lx0 = p1->x - (dlx0 * w), ly0 = p1->y - (dly0 * w);
	float rx0 = p1->x + (dlx0 * w), ry0 = p1->y + (dly0 * w);
	float lx1 = p1->x - (dlx1 * w), ly1 = p1->y - (dly1 * w);
	float rx1 = p1->x + (dlx1 * w), ry1 = p1->y + (dly1 * w);

	svg_add_edge(r, lx0, ly0, left->x, left->y);
	svg_add_edge(r, lx1, ly1, lx0, ly0);
	svg_add_edge(r, right->x, right->y, rx0, ry0);
	svg_add_edge(r, rx0, ry0, rx1, ry1);
	left->x = lx1;
	left->y = ly1;
	right->x = rx1;
	right->y = ry1;
}

static void svg_miter_join(struct svg_rasterizer_t * r, struct svg_point_t * left, struct svg_point_t * right, struct svg_point_t * p0, struct svg_point_t * p1, float width)
{
	float w = width * 0.5f;
	float dlx0 = p0->dy, dly0 = -p0->dx;
	float dlx1 = p1->dy, dly1 = -p1->dx;
	float lx0, rx0, lx1, rx1;
	float ly0, ry0, ly1, ry1;

	if(p1->flags & SVG_POINT_LEFT)
	{
		lx0 = lx1 = p1->x - p1->dmx * w;
		ly0 = ly1 = p1->y - p1->dmy * w;
		svg_add_edge(r, lx1, ly1, left->x, left->y);
		rx0 = p1->x + (dlx0 * w);
		ry0 = p1->y + (dly0 * w);
		rx1 = p1->x + (dlx1 * w);
		ry1 = p1->y + (dly1 * w);
		svg_add_edge(r, right->x, right->y, rx0, ry0);
		svg_add_edge(r, rx0, ry0, rx1, ry1);
	}
	else
	{
		lx0 = p1->x - (dlx0 * w);
		ly0 = p1->y - (dly0 * w);
		lx1 = p1->x - (dlx1 * w);
		ly1 = p1->y - (dly1 * w);
		svg_add_edge(r, lx0, ly0, left->x, left->y);
		svg_add_edge(r, lx1, ly1, lx0, ly0);
		rx0 = rx1 = p1->x + p1->dmx * w;
		ry0 = ry1 = p1->y + p1->dmy * w;
		svg_add_edge(r, right->x, right->y, rx1, ry1);
	}
	left->x = lx1;
	left->y = ly1;
	right->x = rx1;
	right->y = ry1;
}

static void svg_round_join(struct svg_rasterizer_t * r, struct svg_point_t * left, struct svg_point_t * right, struct svg_point_t * p0, struct svg_point_t * p1, float width, int ncap)
{
	float w = width * 0.5f;
	float dlx0 = p0->dy, dly0 = -p0->dx;
	float dlx1 = p1->dy, dly1 = -p1->dx;
	float a0 = atan2f(dly0, dlx0);
	float a1 = atan2f(dly1, dlx1);
	float da = a1 - a0;
	float lx, ly, rx, ry;
	int i, n;

	if(da < M_PI)
		da += M_PI * 2;
	if(da > M_PI)
		da -= M_PI * 2;

	n = (int)ceilf((fabsf(da) / M_PI) * (float)ncap);
	if(n < 2)
		n = 2;
	if(n > ncap)
		n = ncap;
	lx = left->x;
	ly = left->y;
	rx = right->x;
	ry = right->y;

	for(i = 0; i < n; i++)
	{
		float u = (float)i / (float)(n - 1);
		float a = a0 + u * da;
		float ax = cosf(a) * w, ay = sinf(a) * w;
		float lx1 = p1->x - ax, ly1 = p1->y - ay;
		float rx1 = p1->x + ax, ry1 = p1->y + ay;
		svg_add_edge(r, lx1, ly1, lx, ly);
		svg_add_edge(r, rx, ry, rx1, ry1);
		lx = lx1;
		ly = ly1;
		rx = rx1;
		ry = ry1;
	}
	left->x = lx;
	left->y = ly;
	right->x = rx;
	right->y = ry;
}

static void svg_straight_join(struct svg_rasterizer_t * r, struct svg_point_t * left, struct svg_point_t * right, struct svg_point_t * p1, float width)
{
	float w = width * 0.5f;
	float lx = p1->x - (p1->dmx * w), ly = p1->y - (p1->dmy * w);
	float rx = p1->x + (p1->dmx * w), ry = p1->y + (p1->dmy * w);

	svg_add_edge(r, lx, ly, left->x, left->y);
	svg_add_edge(r, right->x, right->y, rx, ry);
	left->x = lx;
	left->y = ly;
	right->x = rx;
	right->y = ry;
}

static int svg_curve_divs(float r, float arc, float tol)
{
	float da = acosf(r / (r + tol)) * 2.0f;
	int divs = (int)ceilf(arc / da);
	if(divs < 2)
		divs = 2;
	return divs;
}

static void svg_expand_stroke(struct svg_rasterizer_t * r, struct svg_point_t * points, int npoints, int closed, enum svg_line_join_t join, enum svg_line_cap_t cap, float width)
{
	struct svg_point_t left = { 0, 0, 0, 0, 0, 0, 0, 0 }, right = { 0, 0, 0, 0, 0, 0, 0, 0 }, firstLeft = { 0, 0, 0, 0, 0, 0, 0, 0 }, firstRight = { 0, 0, 0, 0, 0, 0, 0, 0 };
	struct svg_point_t * p0, * p1;
	int ncap = svg_curve_divs(width * 0.5f, M_PI, r->tesstol);
	int j, s, e;

	if(closed)
	{
		p0 = &points[npoints - 1];
		p1 = &points[0];
		s = 0;
		e = npoints;
	}
	else
	{
		p0 = &points[0];
		p1 = &points[1];
		s = 1;
		e = npoints - 1;
	}
	if(closed)
	{
		svg_init_closed(&left, &right, p0, p1, width);
		firstLeft = left;
		firstRight = right;
	}
	else
	{
		float dx = p1->x - p0->x;
		float dy = p1->y - p0->y;
		svg_normalize(&dx, &dy);
		if(cap == SVG_CAP_BUTT)
			svg_butt_cap(r, &left, &right, p0, dx, dy, width, 0);
		else if(cap == SVG_CAP_SQUARE)
			svg_square_cap(r, &left, &right, p0, dx, dy, width, 0);
		else if(cap == SVG_CAP_ROUND)
			svg_round_cap(r, &left, &right, p0, dx, dy, width, ncap, 0);
	}

	for(j = s; j < e; ++j)
	{
		if(p1->flags & SVG_POINT_CORNER)
		{
			if(join == SVG_JOIN_ROUND)
				svg_round_join(r, &left, &right, p0, p1, width, ncap);
			else if((join == SVG_JOIN_BEVEL) || (p1->flags & SVG_POINT_BEVEL))
				svg_bevel_join(r, &left, &right, p0, p1, width);
			else
				svg_miter_join(r, &left, &right, p0, p1, width);
		}
		else
		{
			svg_straight_join(r, &left, &right, p1, width);
		}
		p0 = p1++;
	}
	if(closed)
	{
		svg_add_edge(r, firstLeft.x, firstLeft.y, left.x, left.y);
		svg_add_edge(r, right.x, right.y, firstRight.x, firstRight.y);
	}
	else
	{
		float dx = p1->x - p0->x;
		float dy = p1->y - p0->y;
		svg_normalize(&dx, &dy);
		if(cap == SVG_CAP_BUTT)
			svg_butt_cap(r, &right, &left, p1, -dx, -dy, width, 1);
		else if(cap == SVG_CAP_SQUARE)
			svg_square_cap(r, &right, &left, p1, -dx, -dy, width, 1);
		else if(cap == SVG_CAP_ROUND)
			svg_round_cap(r, &right, &left, p1, -dx, -dy, width, ncap, 1);
	}
}

static void svg_prepare_stroke(struct svg_rasterizer_t * r, float miter_limit, enum svg_line_join_t join)
{
	struct svg_point_t * p0, * p1;
	int i, j;

	p0 = &r->points[r->npoints - 1];
	p1 = &r->points[0];
	for(i = 0; i < r->npoints; i++)
	{
		p0->dx = p1->x - p0->x;
		p0->dy = p1->y - p0->y;
		p0->len = svg_normalize(&p0->dx, &p0->dy);
		p0 = p1++;
	}

	p0 = &r->points[r->npoints - 1];
	p1 = &r->points[0];
	for(j = 0; j < r->npoints; j++)
	{
		float dlx0, dly0, dlx1, dly1, dmr2, cross;
		dlx0 = p0->dy;
		dly0 = -p0->dx;
		dlx1 = p1->dy;
		dly1 = -p1->dx;
		p1->dmx = (dlx0 + dlx1) * 0.5f;
		p1->dmy = (dly0 + dly1) * 0.5f;
		dmr2 = p1->dmx * p1->dmx + p1->dmy * p1->dmy;
		if(dmr2 > 0.000001f)
		{
			float s2 = 1.0f / dmr2;
			if(s2 > 600.0f)
			{
				s2 = 600.0f;
			}
			p1->dmx *= s2;
			p1->dmy *= s2;
		}
		p1->flags = (p1->flags & SVG_POINT_CORNER) ? SVG_POINT_CORNER : 0;
		cross = p1->dx * p0->dy - p0->dx * p1->dy;
		if(cross > 0.0f)
			p1->flags |= SVG_POINT_LEFT;
		if(p1->flags & SVG_POINT_CORNER)
		{
			if((dmr2 * miter_limit * miter_limit) < 1.0f || join == SVG_JOIN_BEVEL || join == SVG_JOIN_ROUND)
			{
				p1->flags |= SVG_POINT_BEVEL;
			}
		}
		p0 = p1++;
	}
}

static void svg_flatten_shape_stroke(struct svg_rasterizer_t * r, struct svg_shape_t * shape, float sx, float sy)
{
	struct svg_path_t * path;
	struct svg_point_t * p0, * p1;
	float miter_limit = shape->miter_limit;
	enum svg_line_join_t join = shape->stroke_line_join;
	enum svg_line_cap_t cap = shape->stroke_line_cap;
	float sw = (sx + sy) * 0.5f;
	float width = shape->stroke_width * sw;
	int i, j, closed;

	for(path = shape->paths; path != NULL; path = path->next)
	{
		r->npoints = 0;
		svg_add_path_point(r, path->pts[0] * sx, path->pts[1] * sy, SVG_POINT_CORNER);
		for(i = 0; i < path->npts - 1; i += 3)
		{
			float* p = &path->pts[i * 2];
			svg_flatten_cubic_bez(r, p[0] * sx, p[1] * sy, p[2] * sx, p[3] * sy, p[4] * sx, p[5] * sy, p[6] * sx, p[7] * sy, 0, SVG_POINT_CORNER);
		}
		if(r->npoints < 2)
			continue;
		closed = path->closed;

		p0 = &r->points[r->npoints - 1];
		p1 = &r->points[0];
		if(svg_pt_equals(p0->x, p0->y, p1->x, p1->y, r->disttol))
		{
			r->npoints--;
			p0 = &r->points[r->npoints - 1];
			closed = 1;
		}
		if(shape->stroke_dash_count > 0)
		{
			int idash = 0, dashState = 1;
			float totalDist = 0, dashLen, allDashLen, dashOffset;
			struct svg_point_t cur;

			if(closed)
				svg_append_path_point(r, r->points[0]);
			svg_duplicate_points(r);

			r->npoints = 0;
			cur = r->points2[0];
			svg_append_path_point(r, cur);

			allDashLen = 0;
			for(j = 0; j < shape->stroke_dash_count; j++)
				allDashLen += shape->stroke_dash_array[j];
			if(shape->stroke_dash_count & 1)
				allDashLen *= 2.0f;
			dashOffset = fmodf(shape->stroke_dash_offset, allDashLen);
			if(dashOffset < 0.0f)
				dashOffset += allDashLen;

			while(dashOffset > shape->stroke_dash_array[idash])
			{
				dashOffset -= shape->stroke_dash_array[idash];
				idash = (idash + 1) % shape->stroke_dash_count;
			}
			dashLen = (shape->stroke_dash_array[idash] - dashOffset) * sw;

			for(j = 1; j < r->npoints2;)
			{
				float dx = r->points2[j].x - cur.x;
				float dy = r->points2[j].y - cur.y;
				float dist = sqrtf(dx * dx + dy * dy);

				if((totalDist + dist) > dashLen)
				{
					float d = (dashLen - totalDist) / dist;
					float x = cur.x + dx * d;
					float y = cur.y + dy * d;
					svg_add_path_point(r, x, y, SVG_POINT_CORNER);

					if(r->npoints > 1 && dashState)
					{
						svg_prepare_stroke(r, miter_limit, join);
						svg_expand_stroke(r, r->points, r->npoints, 0, join, cap, width);
					}
					dashState = !dashState;
					idash = (idash + 1) % shape->stroke_dash_count;
					dashLen = shape->stroke_dash_array[idash] * sw;
					cur.x = x;
					cur.y = y;
					cur.flags = SVG_POINT_CORNER;
					totalDist = 0.0f;
					r->npoints = 0;
					svg_append_path_point(r, cur);
				}
				else
				{
					totalDist += dist;
					cur = r->points2[j];
					svg_append_path_point(r, cur);
					j++;
				}
			}
			if(r->npoints > 1 && dashState)
				svg_expand_stroke(r, r->points, r->npoints, 0, join, cap, width);
		}
		else
		{
			svg_prepare_stroke(r, miter_limit, join);
			svg_expand_stroke(r, r->points, r->npoints, closed, join, cap, width);
		}
	}
}

static int svg_cmp_edge(const void * p, const void * q)
{
	const struct svg_edge_t * a = (const struct svg_edge_t *)p;
	const struct svg_edge_t * b = (const struct svg_edge_t *)q;

	if(a->y0 < b->y0)
		return -1;
	if(a->y0 > b->y0)
		return 1;
	return 0;
}

static struct svg_active_edge_t * svg_add_active(struct svg_rasterizer_t * r, struct svg_edge_t * e, float startPoint)
{
	struct svg_active_edge_t * z;

	if(r->freelist != NULL)
	{
		z = r->freelist;
		r->freelist = z->next;
	}
	else
	{
		z = (struct svg_active_edge_t *)svg_mem_alloc(r, sizeof(struct svg_active_edge_t));
		if(!z)
			return NULL;
	}

	float dxdy = (e->x1 - e->x0) / (e->y1 - e->y0);
	if(dxdy < 0)
		z->dx = (int)(-floorf(SVG_FIX * -dxdy));
	else
		z->dx = (int)floorf(SVG_FIX * dxdy);
	z->x = (int)floorf(SVG_FIX * (e->x0 + dxdy * (startPoint - e->y0)));
	z->ey = e->y1;
	z->next = 0;
	z->dir = e->dir;

	return z;
}

static void svg_free_active(struct svg_rasterizer_t * r, struct svg_active_edge_t * z)
{
	z->next = r->freelist;
	r->freelist = z;
}

static void svg_fill_scanline(unsigned char * scanline, int len, int x0, int x1, int maxWeight, int * xmin, int * xmax)
{
	int i = x0 >> SVG_FIXSHIFT;
	int j = x1 >> SVG_FIXSHIFT;
	if(i < *xmin)
		*xmin = i;
	if(j > *xmax)
		*xmax = j;
	if(i < len && j >= 0)
	{
		if(i == j)
		{
			scanline[i] = (unsigned char)(scanline[i] + ((x1 - x0) * maxWeight >> SVG_FIXSHIFT));
		}
		else
		{
			if(i >= 0)
				scanline[i] = (unsigned char)(scanline[i] + (((SVG_FIX - (x0 & SVG_FIXMASK)) * maxWeight) >> SVG_FIXSHIFT));
			else
				i = -1;
			if(j < len)
				scanline[j] = (unsigned char)(scanline[j] + (((x1 & SVG_FIXMASK) * maxWeight) >> SVG_FIXSHIFT));
			else
				j = len;
			for(++i; i < j; ++i)
				scanline[i] = (unsigned char)(scanline[i] + maxWeight);
		}
	}
}

static void svg_fill_active_edges(unsigned char * scanline, int len, struct svg_active_edge_t * e, int maxWeight, int * xmin, int * xmax, enum svg_fill_rule_t fill_rule)
{
	int x0 = 0, w = 0;

	if(fill_rule == SVG_FILLRULE_NONZERO)
	{
		while(e != NULL)
		{
			if(w == 0)
			{
				x0 = e->x;
				w += e->dir;
			}
			else
			{
				int x1 = e->x;
				w += e->dir;
				if(w == 0)
					svg_fill_scanline(scanline, len, x0, x1, maxWeight, xmin, xmax);
			}
			e = e->next;
		}
	}
	else if(fill_rule == SVG_FILLRULE_EVENODD)
	{
		while(e != NULL)
		{
			if(w == 0)
			{
				x0 = e->x;
				w = 1;
			}
			else
			{
				int x1 = e->x;
				w = 0;
				svg_fill_scanline(scanline, len, x0, x1, maxWeight, xmin, xmax);
			}
			e = e->next;
		}
	}
}

static void svg_lerp_rgba(struct color_t * c, struct color_t * c0, struct color_t * c1, float u)
{
	int iu = (int)(clamp(u, 0.0f, 1.0f) * 256.0f);
	int b = (c0->b * (256 - iu) + (c1->b * iu)) >> 8;
	int g = (c0->g * (256 - iu) + (c1->g * iu)) >> 8;
	int r = (c0->r * (256 - iu) + (c1->r * iu)) >> 8;
	int a = (c0->a * (256 - iu) + (c1->a * iu)) >> 8;
	color_init(c, r, g, b, a);
}

static void svg_apply_opacity(struct color_t * c, struct color_t * o, float u)
{
	int iu = (int)(clamp(u, 0.0f, 1.0f) * 256.0f);
	color_init(c, o->r, o->g, o->b, (o->a * iu) >> 8);
}

static void svg_scanline_solid(unsigned char * dst, int count, unsigned char * cover, int x, int y, float tx, float ty, float sx, float sy, struct svg_cache_paint_t * cache)
{
	if(cache->type == SVG_PAINT_COLOR)
	{
		int i, cb, cg, cr, ca;
		cb = cache->colors[0].b;
		cg = cache->colors[0].g;
		cr = cache->colors[0].r;
		ca = cache->colors[0].a;

		for(i = 0; i < count; i++)
		{
			int b, g, r;
			int a = idiv255((int)cover[0] * ca);
			int ia = 255 - a;
			b = idiv255(cb * a);
			g = idiv255(cg * a);
			r = idiv255(cr * a);

			b += idiv255(ia * (int)dst[0]);
			g += idiv255(ia * (int)dst[1]);
			r += idiv255(ia * (int)dst[2]);
			a += idiv255(ia * (int)dst[3]);

			dst[0] = (unsigned char)b;
			dst[1] = (unsigned char)g;
			dst[2] = (unsigned char)r;
			dst[3] = (unsigned char)a;

			cover++;
			dst += 4;
		}
	}
	else if(cache->type == SVG_PAINT_LINEAR_GRADIENT)
	{
		float fx, fy, dx, gy;
		float * t = cache->xform;
		int i, cb, cg, cr, ca;
		struct color_t * c;

		fx = ((float)x - tx) / sx;
		fy = ((float)y - ty) / sy;
		dx = 1.0f / sx;

		for(i = 0; i < count; i++)
		{
			int b, g, r, a, ia;
			gy = fx * t[1] + fy * t[3] + t[5];
			c = &cache->colors[(int)clamp(gy * 255.0f, 0.0f, 255.0f)];
			cb = c->b;
			cg = c->g;
			cr = c->r;
			ca = c->a;

			a = idiv255((int)cover[0] * ca);
			ia = 255 - a;

			b = idiv255(cb * a);
			g = idiv255(cg * a);
			r = idiv255(cr * a);

			b += idiv255(ia * (int)dst[0]);
			g += idiv255(ia * (int)dst[1]);
			r += idiv255(ia * (int)dst[2]);
			a += idiv255(ia * (int)dst[3]);

			dst[0] = (unsigned char)b;
			dst[1] = (unsigned char)g;
			dst[2] = (unsigned char)r;
			dst[3] = (unsigned char)a;

			cover++;
			dst += 4;
			fx += dx;
		}
	}
	else if(cache->type == SVG_PAINT_RADIAL_GRADIENT)
	{
		float fx, fy, dx, gx, gy, gd;
		float* t = cache->xform;
		int i, cb, cg, cr, ca;
		struct color_t * c;

		fx = ((float)x - tx) / sx;
		fy = ((float)y - ty) / sy;
		dx = 1.0f / sx;

		for(i = 0; i < count; i++)
		{
			int b, g, r, a, ia;
			gx = fx * t[0] + fy * t[2] + t[4];
			gy = fx * t[1] + fy * t[3] + t[5];
			gd = sqrtf(gx * gx + gy * gy);
			c = &cache->colors[(int)clamp(gd * 255.0f, 0.0f, 255.0f)];
			cb = c->b;
			cg = c->g;
			cr = c->r;
			ca = c->a;

			a = idiv255((int)cover[0] * ca);
			ia = 255 - a;

			b = idiv255(cb * a);
			g = idiv255(cg * a);
			r = idiv255(cr * a);

			b += idiv255(ia * (int)dst[0]);
			g += idiv255(ia * (int)dst[1]);
			r += idiv255(ia * (int)dst[2]);
			a += idiv255(ia * (int)dst[3]);

			dst[0] = (unsigned char)b;
			dst[1] = (unsigned char)g;
			dst[2] = (unsigned char)r;
			dst[3] = (unsigned char)a;

			cover++;
			dst += 4;
			fx += dx;
		}
	}
}

static void svg_rasterize_sorted_edges(struct svg_rasterizer_t * r, float tx, float ty, float sx, float sy, struct svg_cache_paint_t * cache, enum svg_fill_rule_t fill_rule)
{
	struct svg_active_edge_t * active = NULL;
	int y, s;
	int e = 0;
	int maxWeight = (255 / SVG_SUBSAMPLES);
	int xmin, xmax;

	for(y = 0; y < r->height; y++)
	{
		memset(r->scanline, 0, r->width);
		xmin = r->width;
		xmax = 0;
		for(s = 0; s < SVG_SUBSAMPLES; ++s)
		{
			float scany = (float)(y * SVG_SUBSAMPLES + s) + 0.5f;
			struct svg_active_edge_t ** step = &active;

			while(*step)
			{
				struct svg_active_edge_t * z = *step;
				if(z->ey <= scany)
				{
					*step = z->next;
					svg_free_active(r, z);
				}
				else
				{
					z->x += z->dx;
					step = &((*step)->next);
				}
			}

			for(;;)
			{
				int changed = 0;
				step = &active;
				while(*step && (*step)->next)
				{
					if((*step)->x > (*step)->next->x)
					{
						struct svg_active_edge_t * t = *step;
						struct svg_active_edge_t * q = t->next;
						t->next = q->next;
						q->next = t;
						*step = q;
						changed = 1;
					}
					step = &(*step)->next;
				}
				if(!changed)
					break;
			}

			while(e < r->nedges && r->edges[e].y0 <= scany)
			{
				if(r->edges[e].y1 > scany)
				{
					struct svg_active_edge_t * z = svg_add_active(r, &r->edges[e], scany);
					if(z == NULL)
						break;
					if(active == NULL)
					{
						active = z;
					}
					else if(z->x < active->x)
					{
						z->next = active;
						active = z;
					}
					else
					{
						struct svg_active_edge_t* p = active;
						while(p->next && p->next->x < z->x)
							p = p->next;
						z->next = p->next;
						p->next = z;
					}
				}
				e++;
			}
			if(active != NULL)
				svg_fill_active_edges(r->scanline, r->width, active, maxWeight, &xmin, &xmax, fill_rule);
		}
		if(xmin < 0)
			xmin = 0;
		if(xmax > r->width - 1)
			xmax = r->width - 1;
		if(xmin <= xmax)
			svg_scanline_solid(&r->bitmap[y * r->stride] + xmin * 4, xmax - xmin + 1, &r->scanline[xmin], xmin, y, tx, ty, sx, sy, cache);
	}
}

static void svg_init_paint(struct svg_cache_paint_t * cache, struct svg_paint_t * paint, float opacity)
{
	struct svg_gradient_t * grad;
	int i, j;

	cache->type = paint->type;
	if(paint->type == SVG_PAINT_COLOR)
	{
		svg_apply_opacity(&cache->colors[0], &paint->color, opacity);
		return;
	}
	grad = paint->gradient;
	cache->spread = grad->spread;
	memcpy(cache->xform, grad->xform, sizeof(float) * 6);

	if(grad->nstops == 0)
	{
		for(i = 0; i < 256; i++)
			memset(&cache->colors[i], 0, sizeof(struct color_t));
	}
	if(grad->nstops == 1)
	{
		for(i = 0; i < 256; i++)
			svg_apply_opacity(&cache->colors[i], &grad->stops[i].color, opacity);
	}
	else
	{
		struct color_t ca, cb;
		float ua, ub, du, u;
		int ia, ib, count;

		svg_apply_opacity(&ca, &grad->stops[0].color, opacity);
		ua = clamp(grad->stops[0].offset, 0.0f, 1.0f);
		ub = clamp(grad->stops[grad->nstops - 1].offset, ua, 1.0f);
		ia = (int)(ua * 255.0f);
		ib = (int)(ub * 255.0f);
		for(i = 0; i < ia; i++)
		{
			memcpy(&cache->colors[i], &ca, sizeof(struct color_t));
		}
		for(i = 0; i < grad->nstops - 1; i++)
		{
			svg_apply_opacity(&ca, &grad->stops[i].color, opacity);
			svg_apply_opacity(&cb, &grad->stops[i + 1].color, opacity);
			ua = clamp(grad->stops[i].offset, 0.0f, 1.0f);
			ub = clamp(grad->stops[i + 1].offset, 0.0f, 1.0f);
			ia = (int)(ua * 255.0f);
			ib = (int)(ub * 255.0f);
			count = ib - ia;
			if(count <= 0)
				continue;
			u = 0;
			du = 1.0f / (float)count;
			for(j = 0; j < count; j++)
			{
				svg_lerp_rgba(&cache->colors[ia + j], &ca, &cb, u);
				u += du;
			}
		}
		for(i = ib; i < 256; i++)
			memcpy(&cache->colors[i], &cb, sizeof(struct color_t));
	}
}

static void svg_unpremultiply_alpha(unsigned char * img, int w, int h, int stride)
{
	int x, y;

	for(y = 0; y < h; y++)
	{
		unsigned char * row = &img[y * stride];
		for(x = 0; x < w; x++)
		{
			int b = row[0], g = row[1], r = row[2], a = row[3];
			if(a != 0)
			{
				row[0] = (unsigned char)(b * 255 / a);
				row[1] = (unsigned char)(g * 255 / a);
				row[2] = (unsigned char)(r * 255 / a);
			}
			row += 4;
		}
	}
	for(y = 0; y < h; y++)
	{
		unsigned char * row = &img[y * stride];
		for(x = 0; x < w; x++)
		{
			int b = 0, g = 0, r = 0, a = row[3], n = 0;
			if(a == 0)
			{
				if(x - 1 > 0 && row[-1] != 0)
				{
					b += row[-4];
					g += row[-3];
					r += row[-2];
					n++;
				}
				if(x + 1 < w && row[7] != 0)
				{
					b += row[4];
					g += row[5];
					r += row[6];
					n++;
				}
				if(y - 1 > 0 && row[-stride + 3] != 0)
				{
					b += row[-stride];
					g += row[-stride + 1];
					r += row[-stride + 2];
					n++;
				}
				if(y + 1 < h && row[stride + 3] != 0)
				{
					b += row[stride];
					g += row[stride + 1];
					r += row[stride + 2];
					n++;
				}
				if(n > 0)
				{
					row[0] = (unsigned char)(b / n);
					row[1] = (unsigned char)(g / n);
					row[2] = (unsigned char)(r / n);
				}
			}
			row += 4;
		}
	}
}

void render_default_shape_raster(struct surface_t * s, struct svg_t * svg, float tx, float ty, float sx, float sy)
{
	struct svg_rasterizer_t r;
	struct svg_cache_paint_t cache;
	struct svg_mem_page_t * p, * n;
	struct svg_shape_t * shape;
	struct svg_edge_t * e;
	float sw;
	int i;

	if(s)
	{
		sw = (sx + sy) * 0.5f;
		r.px = 0;
		r.py = 0;
		r.tesstol = 0.25;
		r.disttol = 0.01;
		r.edges = NULL;
		r.nedges = 0;
		r.cedges = 0;
		r.points = NULL;
		r.npoints = 0;
		r.cpoints = 0;
		r.points2 = NULL;
		r.npoints2 = 0;
		r.cpoints2 = 0;
		r.freelist = NULL;
		r.pages = 0;
		r.curpage = 0;
		r.bitmap = surface_get_pixels(s);
		r.width = surface_get_width(s);
		r.height = surface_get_height(s);
		r.stride = surface_get_stride(s);
		r.cscanline = r.width;
		r.scanline = malloc(r.cscanline);
		if(!r.scanline)
			return;

		for(shape = svg->shapes; shape != NULL; shape = shape->next)
		{
			if(!shape->visible)
				continue;
			if(shape->fill.type != SVG_PAINT_NONE)
			{
				svg_reset_pool(&r);
				r.freelist = NULL;
				r.nedges = 0;
				svg_flatten_shape(&r, shape, sx, sy);
				for(i = 0; i < r.nedges; i++)
				{
					e = &r.edges[i];
					e->x0 = tx + e->x0;
					e->y0 = (ty + e->y0) * SVG_SUBSAMPLES;
					e->x1 = tx + e->x1;
					e->y1 = (ty + e->y1) * SVG_SUBSAMPLES;
				}
				qsort(r.edges, r.nedges, sizeof(struct svg_edge_t), svg_cmp_edge);
				svg_init_paint(&cache, &shape->fill, shape->opacity);
				svg_rasterize_sorted_edges(&r, tx, ty, sx, sy, &cache, shape->fill_rule);
			}
			if((shape->stroke.type != SVG_PAINT_NONE) && (shape->stroke_width * sw > 0.01f))
			{
				svg_reset_pool(&r);
				r.freelist = NULL;
				r.nedges = 0;
				svg_flatten_shape_stroke(&r, shape, sx, sy);
				for(i = 0; i < r.nedges; i++)
				{
					e = &r.edges[i];
					e->x0 = tx + e->x0;
					e->y0 = (ty + e->y0) * SVG_SUBSAMPLES;
					e->x1 = tx + e->x1;
					e->y1 = (ty + e->y1) * SVG_SUBSAMPLES;
				}
				qsort(r.edges, r.nedges, sizeof(struct svg_edge_t), svg_cmp_edge);
				svg_init_paint(&cache, &shape->stroke, shape->opacity);
				svg_rasterize_sorted_edges(&r, tx, ty, sx, sy, &cache, SVG_FILLRULE_NONZERO);
			}
		}
		svg_unpremultiply_alpha(r.bitmap, r.width, r.height, r.stride);

		p = r.pages;
		while(p)
		{
			n = p->next;
			free(p);
			p = n;
		}
		if(r.edges)
			free(r.edges);
		if(r.points)
			free(r.points);
		if(r.points2)
			free(r.points2);
		if(r.scanline)
			free(r.scanline);
	}
}
