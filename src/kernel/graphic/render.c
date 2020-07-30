/*
 * kernel/graphic/render.c
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

#include <xboot.h>
#include <graphic/surface.h>

void * render_default_create(struct surface_t * s)
{
	return NULL;
}

void render_default_destroy(void * rctx)
{
}

static inline void blend(uint32_t * d, uint32_t * s)
{
	uint32_t dv, sv = *s;
	uint8_t da, dr, dg, db;
	uint8_t sa, sr, sg, sb;
	uint8_t a, r, g, b;
	int t;

	sa = (sv >> 24) & 0xff;
	if(sa == 255)
	{
		*d = sv;
	}
	else if(sa != 0)
	{
		sr = (sv >> 16) & 0xff;
		sg = (sv >> 8) & 0xff;
		sb = (sv >> 0) & 0xff;
		dv = *d;
		da = (dv >> 24) & 0xff;
		dr = (dv >> 16) & 0xff;
		dg = (dv >> 8) & 0xff;
		db = (dv >> 0) & 0xff;
		t = sa + (sa >> 8);
		a = (((sa + da) << 8) - da * t) >> 8;
		r = (((sr + dr) << 8) - dr * t) >> 8;
		g = (((sg + dg) << 8) - dg * t) >> 8;
		b = (((sb + db) << 8) - db * t) >> 8;
		*d = (a << 24) | (r << 16) | (g << 8) | (b << 0);
	}
}

void render_default_blit(struct surface_t * s, struct region_t * clip, struct matrix_t * m, struct surface_t * src, enum render_type_t type)
{
	struct region_t r, region;
	struct matrix_t t;
	uint32_t * p;
	uint32_t * dp = surface_get_pixels(s);
	uint32_t * sp = surface_get_pixels(src);
	int ds = surface_get_stride(s) >> 2;
	int ss = surface_get_stride(src) >> 2;
	int sw = surface_get_width(src);
	int sh = surface_get_height(src);
	int x1, y1, x2, y2, stride;
	int x, y, ox, oy;
	double fx, fy, ofx, ofy;

	region_init(&r, 0, 0, surface_get_width(s), surface_get_height(s));
	if(clip)
	{
		if(!region_intersect(&r, &r, clip))
			return;
	}
	matrix_transform_region(m, sw, sh, &region);
	if(!region_intersect(&r, &r, &region))
		return;

	x1 = r.x;
	y1 = r.y;
	x2 = r.x + r.w;
	y2 = r.y + r.h;
	stride = ds - r.w;
	p = dp + y1 * ds + x1;
	fx = x1;
	fy = y1;
	memcpy(&t, m, sizeof(struct matrix_t));
	matrix_invert(&t);
	matrix_transform_point(&t, &fx, &fy);

	for(y = y1; y < y2; ++y, fx += t.c, fy += t.d)
	{
		ofx = fx;
		ofy = fy;
		for(x = x1; x < x2; ++x, ofx += t.a, ofy += t.b)
		{
			ox = (int)ofx;
			oy = (int)ofy;
			if(ox >= 0 && ox < sw && oy >= 0 && oy < sh)
			{
				blend(p, sp + oy * ss + ox);
			}
			p++;
		}
		p += stride;
	}
}

void render_default_fill(struct surface_t * s, struct region_t * clip, struct matrix_t * m, int w, int h, struct color_t * c, enum render_type_t type)
{
	struct region_t r, region;
	struct matrix_t t;
	uint32_t * p, v;
	int ds = surface_get_stride(s) >> 2;
	int x1, y1, x2, y2, stride;
	int x, y, ox, oy;
	double fx, fy, ofx, ofy;

	region_init(&r, 0, 0, surface_get_width(s), surface_get_height(s));
	if(clip)
	{
		if(!region_intersect(&r, &r, clip))
			return;
	}
	matrix_transform_region(m, w, h, &region);
	if(!region_intersect(&r, &r, &region))
		return;

	x1 = r.x;
	y1 = r.y;
	x2 = r.x + r.w;
	y2 = r.y + r.h;
	stride = ds - r.w;
	p = (uint32_t *)surface_get_pixels(s) + y1 * ds + x1;
	v = color_get_premult(c);
	fx = x1;
	fy = y1;
	memcpy(&t, m, sizeof(struct matrix_t));
	matrix_invert(&t);
	matrix_transform_point(&t, &fx, &fy);

	for(y = y1; y < y2; ++y, fx += t.c, fy += t.d)
	{
		ofx = fx;
		ofy = fy;
		for(x = x1; x < x2; ++x, ofx += t.a, ofy += t.b)
		{
			ox = (int)ofx;
			oy = (int)ofy;
			if(ox >= 0 && ox < w && oy >= 0 && oy < h)
				*p = v;
			p++;
		}
		p += stride;
	}
}

#define XVG_SUBSAMPLES		(5)
#define XVG_FIXSHIFT		(14)
#define XVG_FIX				(1 << XVG_FIXSHIFT)
#define XVG_FIXMASK			(XVG_FIX - 1)
#define XVG_MPAGE_SIZE		(4096)
#define XVG_KAPPA90			(0.5522847493f)

enum xvg_line_join_t {
	XVG_JOIN_MITER			= 0,
	XVG_JOIN_ROUND			= 1,
	XVG_JOIN_BEVEL			= 2,
};

enum xvg_line_cap_t {
	XVG_CAP_BUTT			= 0,
	XVG_CAP_ROUND			= 1,
	XVG_CAP_SQUARE			= 2,
};

enum xvg_fill_rule_t {
	XVG_FILLRULE_NONZERO	= 0,
	XVG_FILLRULE_EVENODD	= 1,
};

enum {
	XVG_POINT_CORNER		= (1 << 0),
	XVG_POINT_BEVEL			= (1 << 1),
	XVG_POINT_LEFT			= (1 << 2),
};

struct xvg_point_t {
	float x, y;
	float dx, dy;
	float len;
	float dmx, dmy;
	int flags;
};

struct xvg_edge_t {
	float x0, y0, x1, y1;
	int dir;
	struct xvg_edge_t * next;
};

struct xvg_active_edge_t {
	int x, dx;
	float ey;
	int dir;
	struct xvg_active_edge_t * next;
};

struct xvg_mem_page_t {
	unsigned char mem[XVG_MPAGE_SIZE];
	int size;
	struct xvg_mem_page_t * next;
};

struct xvg_context_t {
	float tesstol;
	float disttol;
	struct xvg_edge_t * edges;
	int nedges;
	int cedges;
	struct xvg_point_t * points;
	int npoints;
	int cpoints;
	struct xvg_active_edge_t * freelist;
	struct xvg_mem_page_t * pages;
	struct xvg_mem_page_t * cpage;
	unsigned char * bitmap;
	int width, height, stride;
	unsigned char * scanline;
	int cscanline;
	float * pts;
	int cpts;
	int npts;
	struct region_t clip;
	struct color_t color;
	float thickness;
	float miter;
	enum xvg_line_join_t join;
	enum xvg_line_cap_t cap;
	enum xvg_fill_rule_t rule;
};

static struct xvg_mem_page_t * xvg_next_page(struct xvg_context_t * ctx, struct xvg_mem_page_t * cur)
{
	struct xvg_mem_page_t * page;

	if(cur && cur->next)
		return cur->next;
	page = malloc(sizeof(struct xvg_mem_page_t));
	if(!page)
		return NULL;
	memset(page, 0, sizeof(struct xvg_mem_page_t));
	if(cur)
		cur->next = page;
	else
		ctx->pages = page;
	return page;
}

static void xvg_reset_pool(struct xvg_context_t * ctx)
{
	struct xvg_mem_page_t * p = ctx->pages;
	while(p)
	{
		p->size = 0;
		p = p->next;
	}
	ctx->cpage = ctx->pages;
}

static unsigned char * xvg_mem_alloc(struct xvg_context_t * ctx, int size)
{
	unsigned char * buf;

	if(size > XVG_MPAGE_SIZE)
		return NULL;
	if(!ctx->cpage || (ctx->cpage->size + size > XVG_MPAGE_SIZE))
		ctx->cpage = xvg_next_page(ctx, ctx->cpage);
	buf = &ctx->cpage->mem[ctx->cpage->size];
	ctx->cpage->size += size;
	return buf;
}

static int xvg_pt_equals(float x1, float y1, float x2, float y2, float tol)
{
	float dx = x2 - x1;
	float dy = y2 - y1;
	return dx * dx + dy * dy < tol * tol;
}

static void xvg_add_path_point(struct xvg_context_t * ctx, float x, float y, int flags)
{
	struct xvg_point_t * pt;

	if(ctx->npoints > 0)
	{
		pt = &ctx->points[ctx->npoints - 1];
		if(xvg_pt_equals(pt->x, pt->y, x, y, ctx->disttol))
		{
			pt->flags |= flags;
			return;
		}
	}
	if(ctx->npoints + 1 > ctx->cpoints)
	{
		ctx->cpoints = ctx->cpoints > 0 ? ctx->cpoints * 2 : 64;
		ctx->points = realloc(ctx->points, sizeof(struct xvg_point_t) * ctx->cpoints);
		if(!ctx->points)
			return;
	}
	pt = &ctx->points[ctx->npoints];
	pt->x = x;
	pt->y = y;
	pt->flags = flags;
	ctx->npoints++;
}

static void xvg_add_edge(struct xvg_context_t * ctx, float x0, float y0, float x1, float y1)
{
	struct xvg_edge_t * e;

	if(y0 == y1)
		return;
	if(ctx->nedges + 1 > ctx->cedges)
	{
		ctx->cedges = ctx->cedges > 0 ? ctx->cedges * 2 : 64;
		ctx->edges = (struct xvg_edge_t *)realloc(ctx->edges, sizeof(struct xvg_edge_t) * ctx->cedges);
		if(!ctx->edges)
			return;
	}
	e = &ctx->edges[ctx->nedges];
	ctx->nedges++;
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

static float xvg_normalize(float * x, float * y)
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

static void xvg_flatten_cubic_bez(struct xvg_context_t * ctx, float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, int level, int type)
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
	if((d2 + d3) * (d2 + d3) < ctx->tesstol * (dx * dx + dy * dy))
	{
		xvg_add_path_point(ctx, x4, y4, type);
		return;
	}
	x234 = (x23 + x34) * 0.5f;
	y234 = (y23 + y34) * 0.5f;
	x1234 = (x123 + x234) * 0.5f;
	y1234 = (y123 + y234) * 0.5f;
	xvg_flatten_cubic_bez(ctx, x1, y1, x12, y12, x123, y123, x1234, y1234, level + 1, 0);
	xvg_flatten_cubic_bez(ctx, x1234, y1234, x234, y234, x34, y34, x4, y4, level + 1, type);
}

static void xvg_init_closed(struct xvg_point_t * left, struct xvg_point_t * right, struct xvg_point_t * p0, struct xvg_point_t * p1, float width)
{
	float w = width * 0.5f;
	float dx = p1->x - p0->x;
	float dy = p1->y - p0->y;
	float len = xvg_normalize(&dx, &dy);
	float px = p0->x + dx * len * 0.5f, py = p0->y + dy * len * 0.5f;
	float dlx = dy, dly = -dx;
	float lx = px - dlx * w, ly = py - dly * w;
	float rx = px + dlx * w, ry = py + dly * w;
	left->x = lx;
	left->y = ly;
	right->x = rx;
	right->y = ry;
}

static void xvg_butt_cap(struct xvg_context_t * ctx, struct xvg_point_t * left, struct xvg_point_t * right, struct xvg_point_t * p, float dx, float dy, float width, int connect)
{
	float w = width * 0.5f;
	float px = p->x, py = p->y;
	float dlx = dy, dly = -dx;
	float lx = px - dlx * w, ly = py - dly * w;
	float rx = px + dlx * w, ry = py + dly * w;

	xvg_add_edge(ctx, lx, ly, rx, ry);
	if(connect)
	{
		xvg_add_edge(ctx, left->x, left->y, lx, ly);
		xvg_add_edge(ctx, rx, ry, right->x, right->y);
	}
	left->x = lx;
	left->y = ly;
	right->x = rx;
	right->y = ry;
}

static void xvg_square_cap(struct xvg_context_t * ctx, struct xvg_point_t * left, struct xvg_point_t * right, struct xvg_point_t * p, float dx, float dy, float width, int connect)
{
	float w = width * 0.5f;
	float px = p->x - dx * w, py = p->y - dy * w;
	float dlx = dy, dly = -dx;
	float lx = px - dlx * w, ly = py - dly * w;
	float rx = px + dlx * w, ry = py + dly * w;

	xvg_add_edge(ctx, lx, ly, rx, ry);
	if(connect)
	{
		xvg_add_edge(ctx, left->x, left->y, lx, ly);
		xvg_add_edge(ctx, rx, ry, right->x, right->y);
	}
	left->x = lx;
	left->y = ly;
	right->x = rx;
	right->y = ry;
}

static void xvg_round_cap(struct xvg_context_t * ctx, struct xvg_point_t * left, struct xvg_point_t * right, struct xvg_point_t * p, float dx, float dy, float width, int ncap, int connect)
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
			xvg_add_edge(ctx, prevx, prevy, x, y);
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
		xvg_add_edge(ctx, left->x, left->y, lx, ly);
		xvg_add_edge(ctx, rx, ry, right->x, right->y);
	}
	left->x = lx;
	left->y = ly;
	right->x = rx;
	right->y = ry;
}

static void xvg_bevel_join(struct xvg_context_t * ctx, struct xvg_point_t * left, struct xvg_point_t * right, struct xvg_point_t * p0, struct xvg_point_t * p1, float width)
{
	float w = width * 0.5f;
	float dlx0 = p0->dy, dly0 = -p0->dx;
	float dlx1 = p1->dy, dly1 = -p1->dx;
	float lx0 = p1->x - (dlx0 * w), ly0 = p1->y - (dly0 * w);
	float rx0 = p1->x + (dlx0 * w), ry0 = p1->y + (dly0 * w);
	float lx1 = p1->x - (dlx1 * w), ly1 = p1->y - (dly1 * w);
	float rx1 = p1->x + (dlx1 * w), ry1 = p1->y + (dly1 * w);

	xvg_add_edge(ctx, lx0, ly0, left->x, left->y);
	xvg_add_edge(ctx, lx1, ly1, lx0, ly0);
	xvg_add_edge(ctx, right->x, right->y, rx0, ry0);
	xvg_add_edge(ctx, rx0, ry0, rx1, ry1);
	left->x = lx1;
	left->y = ly1;
	right->x = rx1;
	right->y = ry1;
}

static void xvg_miter_join(struct xvg_context_t * ctx, struct xvg_point_t * left, struct xvg_point_t * right, struct xvg_point_t * p0, struct xvg_point_t * p1, float width)
{
	float w = width * 0.5f;
	float dlx0 = p0->dy, dly0 = -p0->dx;
	float dlx1 = p1->dy, dly1 = -p1->dx;
	float lx0, rx0, lx1, rx1;
	float ly0, ry0, ly1, ry1;

	if(p1->flags & XVG_POINT_LEFT)
	{
		lx0 = lx1 = p1->x - p1->dmx * w;
		ly0 = ly1 = p1->y - p1->dmy * w;
		xvg_add_edge(ctx, lx1, ly1, left->x, left->y);
		rx0 = p1->x + (dlx0 * w);
		ry0 = p1->y + (dly0 * w);
		rx1 = p1->x + (dlx1 * w);
		ry1 = p1->y + (dly1 * w);
		xvg_add_edge(ctx, right->x, right->y, rx0, ry0);
		xvg_add_edge(ctx, rx0, ry0, rx1, ry1);
	}
	else
	{
		lx0 = p1->x - (dlx0 * w);
		ly0 = p1->y - (dly0 * w);
		lx1 = p1->x - (dlx1 * w);
		ly1 = p1->y - (dly1 * w);
		xvg_add_edge(ctx, lx0, ly0, left->x, left->y);
		xvg_add_edge(ctx, lx1, ly1, lx0, ly0);
		rx0 = rx1 = p1->x + p1->dmx * w;
		ry0 = ry1 = p1->y + p1->dmy * w;
		xvg_add_edge(ctx, right->x, right->y, rx1, ry1);
	}
	left->x = lx1;
	left->y = ly1;
	right->x = rx1;
	right->y = ry1;
}

static void xvg_round_join(struct xvg_context_t * ctx, struct xvg_point_t * left, struct xvg_point_t * right, struct xvg_point_t * p0, struct xvg_point_t * p1, float width, int ncap)
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
		xvg_add_edge(ctx, lx1, ly1, lx, ly);
		xvg_add_edge(ctx, rx, ry, rx1, ry1);
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

static void xvg_straight_join(struct xvg_context_t * ctx, struct xvg_point_t * left, struct xvg_point_t * right, struct xvg_point_t * p1, float width)
{
	float w = width * 0.5f;
	float lx = p1->x - (p1->dmx * w), ly = p1->y - (p1->dmy * w);
	float rx = p1->x + (p1->dmx * w), ry = p1->y + (p1->dmy * w);

	xvg_add_edge(ctx, lx, ly, left->x, left->y);
	xvg_add_edge(ctx, right->x, right->y, rx, ry);
	left->x = lx;
	left->y = ly;
	right->x = rx;
	right->y = ry;
}

static int xvg_curve_divs(float ctx, float arc, float tol)
{
	float da = acosf(ctx / (ctx + tol)) * 2.0f;
	int divs = (int)ceilf(arc / da);
	if(divs < 2)
		divs = 2;
	return divs;
}

static void xvg_expand_stroke(struct xvg_context_t * ctx, struct xvg_point_t * points, int npoints, int closed, enum xvg_line_join_t join, enum xvg_line_cap_t cap, float width)
{
	struct xvg_point_t left = { 0, 0, 0, 0, 0, 0, 0, 0 }, right = { 0, 0, 0, 0, 0, 0, 0, 0 }, firstLeft = { 0, 0, 0, 0, 0, 0, 0, 0 }, firstRight = { 0, 0, 0, 0, 0, 0, 0, 0 };
	struct xvg_point_t * p0, * p1;
	int ncap = xvg_curve_divs(width * 0.5f, M_PI, ctx->tesstol);
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
		xvg_init_closed(&left, &right, p0, p1, width);
		firstLeft = left;
		firstRight = right;
	}
	else
	{
		float dx = p1->x - p0->x;
		float dy = p1->y - p0->y;
		xvg_normalize(&dx, &dy);
		if(cap == XVG_CAP_BUTT)
			xvg_butt_cap(ctx, &left, &right, p0, dx, dy, width, 0);
		else if(cap == XVG_CAP_SQUARE)
			xvg_square_cap(ctx, &left, &right, p0, dx, dy, width, 0);
		else if(cap == XVG_CAP_ROUND)
			xvg_round_cap(ctx, &left, &right, p0, dx, dy, width, ncap, 0);
	}
	for(j = s; j < e; ++j)
	{
		if(p1->flags & XVG_POINT_CORNER)
		{
			if(join == XVG_JOIN_ROUND)
				xvg_round_join(ctx, &left, &right, p0, p1, width, ncap);
			else if((join == XVG_JOIN_BEVEL) || (p1->flags & XVG_POINT_BEVEL))
				xvg_bevel_join(ctx, &left, &right, p0, p1, width);
			else
				xvg_miter_join(ctx, &left, &right, p0, p1, width);
		}
		else
		{
			xvg_straight_join(ctx, &left, &right, p1, width);
		}
		p0 = p1++;
	}
	if(closed)
	{
		xvg_add_edge(ctx, firstLeft.x, firstLeft.y, left.x, left.y);
		xvg_add_edge(ctx, right.x, right.y, firstRight.x, firstRight.y);
	}
	else
	{
		float dx = p1->x - p0->x;
		float dy = p1->y - p0->y;
		xvg_normalize(&dx, &dy);
		if(cap == XVG_CAP_BUTT)
			xvg_butt_cap(ctx, &right, &left, p1, -dx, -dy, width, 1);
		else if(cap == XVG_CAP_SQUARE)
			xvg_square_cap(ctx, &right, &left, p1, -dx, -dy, width, 1);
		else if(cap == XVG_CAP_ROUND)
			xvg_round_cap(ctx, &right, &left, p1, -dx, -dy, width, ncap, 1);
	}
}

static void xvg_prepare_stroke(struct xvg_context_t * ctx, float miter, enum xvg_line_join_t join)
{
	struct xvg_point_t * p0, * p1;
	int i, j;

	p0 = &ctx->points[ctx->npoints - 1];
	p1 = &ctx->points[0];
	for(i = 0; i < ctx->npoints; i++)
	{
		p0->dx = p1->x - p0->x;
		p0->dy = p1->y - p0->y;
		p0->len = xvg_normalize(&p0->dx, &p0->dy);
		p0 = p1++;
	}
	p0 = &ctx->points[ctx->npoints - 1];
	p1 = &ctx->points[0];
	for(j = 0; j < ctx->npoints; j++)
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
		p1->flags = (p1->flags & XVG_POINT_CORNER) ? XVG_POINT_CORNER : 0;
		cross = p1->dx * p0->dy - p0->dx * p1->dy;
		if(cross > 0.0f)
			p1->flags |= XVG_POINT_LEFT;
		if(p1->flags & XVG_POINT_CORNER)
		{
			if((dmr2 * miter * miter) < 1.0f || join == XVG_JOIN_BEVEL || join == XVG_JOIN_ROUND)
			{
				p1->flags |= XVG_POINT_BEVEL;
			}
		}
		p0 = p1++;
	}
}

static int xvg_cmp_edge(const void * p, const void * q)
{
	const struct xvg_edge_t * a = (const struct xvg_edge_t *)p;
	const struct xvg_edge_t * b = (const struct xvg_edge_t *)q;
	if(a->y0 < b->y0)
		return -1;
	if(a->y0 > b->y0)
		return 1;
	return 0;
}

static struct xvg_active_edge_t * xvg_add_active(struct xvg_context_t * ctx, struct xvg_edge_t * e, float startPoint)
{
	struct xvg_active_edge_t * z;

	if(ctx->freelist)
	{
		z = ctx->freelist;
		ctx->freelist = z->next;
	}
	else
	{
		z = (struct xvg_active_edge_t *)xvg_mem_alloc(ctx, sizeof(struct xvg_active_edge_t));
		if(!z)
			return NULL;
	}
	float dxdy = (e->x1 - e->x0) / (e->y1 - e->y0);
	if(dxdy < 0)
		z->dx = (int)(-floorf(XVG_FIX * -dxdy));
	else
		z->dx = (int)floorf(XVG_FIX * dxdy);
	z->x = (int)floorf(XVG_FIX * (e->x0 + dxdy * (startPoint - e->y0)));
	z->ey = e->y1;
	z->next = 0;
	z->dir = e->dir;

	return z;
}

static void xvg_free_active(struct xvg_context_t * ctx, struct xvg_active_edge_t * z)
{
	z->next = ctx->freelist;
	ctx->freelist = z;
}

static void xvg_fill_scanline(unsigned char * scanline, int len, int x0, int x1, int maxweight, int * xmin, int * xmax)
{
	int i = x0 >> XVG_FIXSHIFT;
	int j = x1 >> XVG_FIXSHIFT;
	if(i < *xmin)
		*xmin = i;
	if(j > *xmax)
		*xmax = j;
	if(i < len && j >= 0)
	{
		if(i == j)
		{
			scanline[i] = (unsigned char)(scanline[i] + ((x1 - x0) * maxweight >> XVG_FIXSHIFT));
		}
		else
		{
			if(i >= 0)
				scanline[i] = (unsigned char)(scanline[i] + (((XVG_FIX - (x0 & XVG_FIXMASK)) * maxweight) >> XVG_FIXSHIFT));
			else
				i = -1;
			if(j < len)
				scanline[j] = (unsigned char)(scanline[j] + (((x1 & XVG_FIXMASK) * maxweight) >> XVG_FIXSHIFT));
			else
				j = len;
			for(++i; i < j; ++i)
				scanline[i] = (unsigned char)(scanline[i] + maxweight);
		}
	}
}

static void xvg_fill_active_edges(unsigned char * scanline, int len, struct xvg_active_edge_t * e, int maxweight, int * xmin, int * xmax, enum xvg_fill_rule_t rule)
{
	int x0 = 0, w = 0;

	if(rule == XVG_FILLRULE_NONZERO)
	{
		while(e)
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
					xvg_fill_scanline(scanline, len, x0, x1, maxweight, xmin, xmax);
			}
			e = e->next;
		}
	}
	else if(rule == XVG_FILLRULE_EVENODD)
	{
		while(e)
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
				xvg_fill_scanline(scanline, len, x0, x1, maxweight, xmin, xmax);
			}
			e = e->next;
		}
	}
}

static void xvg_scanline_solid(unsigned char * dst, int count, unsigned char * cover, int x, int y, struct color_t * c)
{
	int cb = c->b;
	int cg = c->g;
	int cr = c->r;
	int ca = c->a;
	int i;

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

static void xvg_rasterize_sorted_edges(struct xvg_context_t * ctx, struct color_t * c, enum xvg_fill_rule_t rule)
{
	struct xvg_active_edge_t * active = NULL;
	int e = 0;
	int maxweight = (255 / XVG_SUBSAMPLES);
	int x0 = ctx->clip.x;
	int y0 = ctx->clip.y;
	int x1 = x0 + ctx->clip.w - 1;
	int y1 = y0 + ctx->clip.h - 1;
	int xmin, xmax;
	int y, s;

	for(y = y0; y <= y1; y++)
	{
		memset(ctx->scanline, 0, ctx->width);
		xmin = ctx->width;
		xmax = 0;
		for(s = 0; s < XVG_SUBSAMPLES; ++s)
		{
			float scany = (float)(y * XVG_SUBSAMPLES + s) + 0.5f;
			struct xvg_active_edge_t ** step = &active;
			while(*step)
			{
				struct xvg_active_edge_t * z = *step;
				if(z->ey <= scany)
				{
					*step = z->next;
					xvg_free_active(ctx, z);
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
						struct xvg_active_edge_t * t = *step;
						struct xvg_active_edge_t * q = t->next;
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
			while(e < ctx->nedges && ctx->edges[e].y0 <= scany)
			{
				if(ctx->edges[e].y1 > scany)
				{
					struct xvg_active_edge_t * z = xvg_add_active(ctx, &ctx->edges[e], scany);
					if(!z)
						break;
					if(!active)
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
						struct xvg_active_edge_t* p = active;
						while(p->next && p->next->x < z->x)
							p = p->next;
						z->next = p->next;
						p->next = z;
					}
				}
				e++;
			}
			if(active)
				xvg_fill_active_edges(ctx->scanline, ctx->width, active, maxweight, &xmin, &xmax, rule);
		}
		if(xmin < x0)
			xmin = x0;
		if(xmax > x1)
			xmax = x1;
		if(xmin <= xmax)
			xvg_scanline_solid(&ctx->bitmap[y * ctx->stride] + xmin * 4, xmax - xmin + 1, &ctx->scanline[xmin], xmin, y, c);
	}
}

static void xvg_reset(struct xvg_context_t * ctx)
{
	ctx->npts = 0;
}

static void xvg_add_point(struct xvg_context_t * ctx, float x, float y)
{
	if(ctx->npts + 1 > ctx->cpts)
	{
		ctx->cpts = ctx->cpts ? ctx->cpts * 2 : 8;
		ctx->pts = realloc(ctx->pts, ctx->cpts * 2 * sizeof(float));
		if(!ctx->pts)
			return;
	}
	ctx->pts[ctx->npts * 2 + 0] = x;
	ctx->pts[ctx->npts * 2 + 1] = y;
	ctx->npts++;
}

static void xvg_move_to(struct xvg_context_t * ctx, float x, float y)
{
	if(ctx->npts > 0)
	{
		ctx->pts[(ctx->npts - 1) * 2 + 0] = x;
		ctx->pts[(ctx->npts - 1) * 2 + 1] = y;
	}
	else
	{
		xvg_add_point(ctx, x, y);
	}
}

static void xvg_line_to(struct xvg_context_t * ctx, float x, float y)
{
	float px, py, dx, dy;
	if(ctx->npts > 0)
	{
		px = ctx->pts[(ctx->npts - 1) * 2 + 0];
		py = ctx->pts[(ctx->npts - 1) * 2 + 1];
		dx = x - px;
		dy = y - py;
		xvg_add_point(ctx, px + dx / 3.0f, py + dy / 3.0f);
		xvg_add_point(ctx, x - dx / 3.0f, y - dy / 3.0f);
		xvg_add_point(ctx, x, y);
	}
}

static void xvg_cubic_bezto(struct xvg_context_t * ctx, float cpx1, float cpy1, float cpx2, float cpy2, float x, float y)
{
	xvg_add_point(ctx, cpx1, cpy1);
	xvg_add_point(ctx, cpx2, cpy2);
	xvg_add_point(ctx, x, y);
}

static void xvg_fill(struct xvg_context_t * ctx)
{
	struct xvg_edge_t * e;
	float * p;
	int i, j;

	xvg_reset_pool(ctx);
	ctx->freelist = NULL;
	ctx->nedges = 0;
	ctx->npoints = 0;
	xvg_add_path_point(ctx, ctx->pts[0], ctx->pts[1], 0);
	for(i = 0; i < ctx->npts - 1; i += 3)
	{
		p = &ctx->pts[i * 2];
		xvg_flatten_cubic_bez(ctx, p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7], 0, 0);
	}
	xvg_add_path_point(ctx, ctx->pts[0], ctx->pts[1], 0);
	for(i = 0, j = ctx->npoints - 1; i < ctx->npoints; j = i++)
		xvg_add_edge(ctx, ctx->points[j].x, ctx->points[j].y, ctx->points[i].x, ctx->points[i].y);
	for(i = 0; i < ctx->nedges; i++)
	{
		e = &ctx->edges[i];
		e->y0 = e->y0 * XVG_SUBSAMPLES;
		e->y1 = e->y1 * XVG_SUBSAMPLES;
	}
	qsort(ctx->edges, ctx->nedges, sizeof(struct xvg_edge_t), xvg_cmp_edge);
	xvg_rasterize_sorted_edges(ctx, &ctx->color, ctx->rule);
}

static void xvg_stroke(struct xvg_context_t * ctx)
{
	struct xvg_edge_t * e;
	struct xvg_point_t * p0, * p1;
	float * p;
	int i, closed;

	xvg_reset_pool(ctx);
	ctx->freelist = NULL;
	ctx->nedges = 0;
	ctx->npoints = 0;
	xvg_add_path_point(ctx, ctx->pts[0], ctx->pts[1], XVG_POINT_CORNER);
	for(i = 0; i < ctx->npts - 1; i += 3)
	{
		p = &ctx->pts[i * 2];
		xvg_flatten_cubic_bez(ctx, p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7], 0, XVG_POINT_CORNER);
	}
	if(ctx->npoints < 2)
		return;
	closed = 0;
	p0 = &ctx->points[ctx->npoints - 1];
	p1 = &ctx->points[0];
	if(xvg_pt_equals(p0->x, p0->y, p1->x, p1->y, ctx->disttol))
	{
		ctx->npoints--;
		p0 = &ctx->points[ctx->npoints - 1];
		closed = 1;
	}
	xvg_prepare_stroke(ctx, ctx->miter, ctx->join);
	xvg_expand_stroke(ctx, ctx->points, ctx->npoints, closed, ctx->join, ctx->cap, ctx->thickness);
	for(i = 0; i < ctx->nedges; i++)
	{
		e = &ctx->edges[i];
		e->y0 = e->y0 * XVG_SUBSAMPLES;
		e->y1 = e->y1 * XVG_SUBSAMPLES;
	}
	qsort(ctx->edges, ctx->nedges, sizeof(struct xvg_edge_t), xvg_cmp_edge);
	xvg_rasterize_sorted_edges(ctx, &ctx->color, ctx->rule);
}

static void xvg_init(struct xvg_context_t * ctx, struct surface_t * s, struct region_t * clip, int thickness, struct color_t * c)
{
	ctx->tesstol = 0.25;
	ctx->disttol = 0.01;
	ctx->edges = NULL;
	ctx->nedges = 0;
	ctx->cedges = 0;
	ctx->points = NULL;
	ctx->npoints = 0;
	ctx->cpoints = 0;
	ctx->freelist = NULL;
	ctx->pages = NULL;
	ctx->cpage = NULL;
	ctx->bitmap = s->pixels;
	ctx->width = s->width;
	ctx->height = s->height;
	ctx->stride = s->stride;
	ctx->cscanline = ctx->width;
	ctx->scanline = malloc(ctx->cscanline);
	ctx->pts = NULL;
	ctx->cpts = 0;
	ctx->npts = 0;
	if(clip)
		memcpy(&ctx->clip, clip, sizeof(struct region_t));
	else
		region_init(&ctx->clip, 0, 0, s->width, s->height);
	if(c)
		memcpy(&ctx->color, c, sizeof(struct color_t));
	else
		color_init(&ctx->color, 0, 0, 0, 255);
	ctx->thickness = thickness > 0 ? thickness : 1;
	ctx->miter = 4;
	ctx->join = XVG_JOIN_MITER;
	ctx->cap = XVG_CAP_BUTT;
	ctx->rule = XVG_FILLRULE_NONZERO;
}

static void xvg_exit(struct xvg_context_t * ctx)
{
	struct xvg_mem_page_t * p, * n;

	if(ctx)
	{
		p = ctx->pages;
		while(p)
		{
			n = p->next;
			free(p);
			p = n;
		}
		if(ctx->edges)
			free(ctx->edges);
		if(ctx->points)
			free(ctx->points);
		if(ctx->scanline)
			free(ctx->scanline);
	}
}

void render_default_shape_line(struct surface_t * s, struct region_t * clip, struct point_t * p0, struct point_t * p1, int thickness, struct color_t * c)
{
	struct xvg_context_t ctx;
	struct region_t r;

	region_init(&r, 0, 0, surface_get_width(s), surface_get_height(s));
	if(clip)
	{
		if(!region_intersect(&r, &r, clip))
			return;
	}
	xvg_init(&ctx, s, &r, thickness, c);
	xvg_move_to(&ctx, p0->x, p0->y);
	xvg_line_to(&ctx, p1->x, p1->y);
	xvg_stroke(&ctx);
	xvg_exit(&ctx);
}

void render_default_shape_polyline(struct surface_t * s, struct region_t * clip, struct point_t * p, int n, int thickness, struct color_t * c)
{
	struct xvg_context_t ctx;
	struct region_t r;
	int i;

	if(p && (n > 0))
	{
		region_init(&r, 0, 0, surface_get_width(s), surface_get_height(s));
		if(clip)
		{
			if(!region_intersect(&r, &r, clip))
				return;
		}
		xvg_init(&ctx, s, &r, thickness, c);
		xvg_reset(&ctx);
		xvg_move_to(&ctx, p[0].x, p[0].y);
		for(i = 1; i < n; i++)
			xvg_line_to(&ctx, p[i].x, p[i].y);
		xvg_stroke(&ctx);
		xvg_exit(&ctx);
	}
}

void render_default_shape_curve(struct surface_t * s, struct region_t * clip, struct point_t * p, int n, int thickness, struct color_t * c)
{
	struct xvg_context_t ctx;
	struct region_t r;
	int i;

	if(p && (n >= 4))
	{
		region_init(&r, 0, 0, surface_get_width(s), surface_get_height(s));
		if(clip)
		{
			if(!region_intersect(&r, &r, clip))
				return;
		}
		xvg_init(&ctx, s, &r, thickness, c);
		xvg_reset(&ctx);
		xvg_move_to(&ctx, p[0].x, p[0].y);
		for(i = 1; i <= n - 3; i += 3)
			xvg_cubic_bezto(&ctx, p[i].x, p[i].y, p[i + 1].x, p[i + 1].y, p[i + 2].x, p[i + 2].y);
		xvg_stroke(&ctx);
		xvg_exit(&ctx);
	}
}

void render_default_shape_triangle(struct surface_t * s, struct region_t * clip, struct point_t * p0, struct point_t * p1, struct point_t * p2, int thickness, struct color_t * c)
{
	struct xvg_context_t ctx;
	struct region_t r;

	region_init(&r, 0, 0, surface_get_width(s), surface_get_height(s));
	if(clip)
	{
		if(!region_intersect(&r, &r, clip))
			return;
	}
	xvg_init(&ctx, s, &r, thickness, c);
	xvg_reset(&ctx);
	xvg_move_to(&ctx, p0->x, p0->y);
	xvg_line_to(&ctx, p1->x, p1->y);
	xvg_line_to(&ctx, p2->x, p2->y);
	xvg_line_to(&ctx, p0->x, p0->y);
	if(thickness > 0)
		xvg_stroke(&ctx);
	else
		xvg_fill(&ctx);
	xvg_exit(&ctx);
}

void render_default_shape_rectangle(struct surface_t * s, struct region_t * clip, int x, int y, int w, int h, int radius, int thickness, struct color_t * c)
{
	struct xvg_context_t ctx;
	struct region_t r;
	int corner;

	region_init(&r, 0, 0, surface_get_width(s), surface_get_height(s));
	if(clip)
	{
		if(!region_intersect(&r, &r, clip))
			return;
	}
	xvg_init(&ctx, s, &r, thickness, c);
	xvg_reset(&ctx);
	corner = (radius >> 16) & 0xf;
	radius &= 0xffff;
	if(radius > 0)
	{
		xvg_move_to(&ctx, x + radius, y);
		xvg_line_to(&ctx, x + w - radius, y);
		if(corner & (1 << 1))
		{
			xvg_line_to(&ctx, x + w, y);
			xvg_line_to(&ctx, x + w, y + radius);
		}
		else
		{
			xvg_cubic_bezto(&ctx, x + w - radius * (1 - XVG_KAPPA90), y, x + w, y + radius * (1 - XVG_KAPPA90), x + w, y + radius);
		}
		xvg_line_to(&ctx, x + w, y + h - radius);
		if(corner & (1 << 2))
		{
			xvg_line_to(&ctx, x + w, y + h);
			xvg_line_to(&ctx, w - radius, y + h);
		}
		else
		{
			xvg_cubic_bezto(&ctx, x + w, y + h - radius * (1 - XVG_KAPPA90), x + w - radius * (1 - XVG_KAPPA90), y + h, x + w - radius, y + h);
		}
		xvg_line_to(&ctx, x + radius, y + h);
		if(corner & (1 << 3))
		{
			xvg_line_to(&ctx, x, y + h);
		}
		else
		{
			xvg_cubic_bezto(&ctx, x + radius * (1 - XVG_KAPPA90), y + h, x, y + h - radius * (1 - XVG_KAPPA90), x, y + h - radius);
		}
		xvg_line_to(&ctx, x, y + radius);
		if(corner & (1 << 0))
		{
			xvg_line_to(&ctx, x, y);
			xvg_line_to(&ctx, x + radius, y);
		}
		else
		{
			xvg_cubic_bezto(&ctx, x, y + radius * (1 - XVG_KAPPA90), x + radius * (1 - XVG_KAPPA90), y, x + radius, y);
		}
	}
	else
	{
		xvg_move_to(&ctx, x, y);
		xvg_line_to(&ctx, x + w, y);
		xvg_line_to(&ctx, x + w, y + h);
		xvg_line_to(&ctx, x, y + h);
	}
	xvg_line_to(&ctx, ctx.pts[0], ctx.pts[1]);
	if(thickness > 0)
		xvg_stroke(&ctx);
	else
		xvg_fill(&ctx);
	xvg_exit(&ctx);
}

void render_default_shape_polygon(struct surface_t * s, struct region_t * clip, struct point_t * p, int n, int thickness, struct color_t * c)
{
	struct xvg_context_t ctx;
	struct region_t r;
	int i;

	if(p && (n > 0))
	{
		region_init(&r, 0, 0, surface_get_width(s), surface_get_height(s));
		if(clip)
		{
			if(!region_intersect(&r, &r, clip))
				return;
		}
		xvg_init(&ctx, s, &r, thickness, c);
		xvg_reset(&ctx);
		xvg_move_to(&ctx, p[0].x, p[0].y);
		for(i = 1; i < n; i++)
			xvg_line_to(&ctx, p[i].x, p[i].y);
		xvg_line_to(&ctx, p[0].x, p[0].y);
		if(thickness > 0)
			xvg_stroke(&ctx);
		else
			xvg_fill(&ctx);
		xvg_exit(&ctx);
	}
}

void render_default_shape_circle(struct surface_t * s, struct region_t * clip, int x, int y, int radius, int thickness, struct color_t * c)
{
	struct xvg_context_t ctx;
	struct region_t r;

	if(radius > 0)
	{
		region_init(&r, 0, 0, surface_get_width(s), surface_get_height(s));
		if(clip)
		{
			if(!region_intersect(&r, &r, clip))
				return;
		}
		xvg_init(&ctx, s, &r, thickness, c);
		xvg_reset(&ctx);
		xvg_move_to(&ctx, x + radius, y);
		xvg_cubic_bezto(&ctx, x + radius, y + radius * XVG_KAPPA90, x + radius * XVG_KAPPA90, y + radius, x, y + radius);
		xvg_cubic_bezto(&ctx, x - radius * XVG_KAPPA90, y + radius, x - radius, y + radius * XVG_KAPPA90, x - radius, y);
		xvg_cubic_bezto(&ctx, x - radius, y - radius * XVG_KAPPA90, x - radius * XVG_KAPPA90, y - radius, x, y - radius);
		xvg_cubic_bezto(&ctx, x + radius * XVG_KAPPA90, y - radius, x + radius, y - radius * XVG_KAPPA90, x + radius, y);
		xvg_line_to(&ctx, ctx.pts[0], ctx.pts[1]);
		if(thickness > 0)
			xvg_stroke(&ctx);
		else
			xvg_fill(&ctx);
		xvg_exit(&ctx);
	}
}

void render_default_shape_ellipse(struct surface_t * s, struct region_t * clip, int x, int y, int w, int h, int thickness, struct color_t * c)
{
	struct xvg_context_t ctx;
	struct region_t r;

	if((w > 0) && (h > 0))
	{
		region_init(&r, 0, 0, surface_get_width(s), surface_get_height(s));
		if(clip)
		{
			if(!region_intersect(&r, &r, clip))
				return;
		}
		xvg_init(&ctx, s, &r, thickness, c);
		xvg_reset(&ctx);
		xvg_move_to(&ctx, x + w, y);
		xvg_cubic_bezto(&ctx, x + w, y + h * XVG_KAPPA90, x + w * XVG_KAPPA90, y + h, x, y + h);
		xvg_cubic_bezto(&ctx, x - w * XVG_KAPPA90, y + h, x - w, y + h * XVG_KAPPA90, x - w, y);
		xvg_cubic_bezto(&ctx, x - w, y - h * XVG_KAPPA90, x - w * XVG_KAPPA90, y - h, x, y - h);
		xvg_cubic_bezto(&ctx, x + w * XVG_KAPPA90, y - h, x + w, y - h * XVG_KAPPA90, x + w, y);
		xvg_line_to(&ctx, ctx.pts[0], ctx.pts[1]);
		if(thickness > 0)
			xvg_stroke(&ctx);
		else
			xvg_fill(&ctx);
		xvg_exit(&ctx);
	}
}

static inline void arcto_bezier(float x, float y, float radius, float start, float sweep, float * cp)
{
	float sweep_2 = sweep / 2.0;
	float x0 = cosf(sweep_2);
	float y0 = sinf(sweep_2);
	float sn = sinf(start + sweep_2);
	float cs = cosf(start + sweep_2);
	float tx = (1.0 - x0) * 4.0 / 3.0;
	float ty = y0 - tx * x0 / y0;
	float px[4];
	float py[4];
	int i;

	px[0] = x0;
	py[0] = -y0;
	px[1] = x0 + tx;
	py[1] = -ty;
	px[2] = x0 + tx;
	py[2] = ty;
	px[3] = x0;
	py[3] = y0;
	for(i = 0; i < 4; i++)
	{
		cp[i * 2] = x + radius * (px[i] * cs - py[i] * sn);
		cp[i * 2 + 1] = y + radius * (px[i] * sn + py[i] * cs);
	}
}

void render_default_shape_arc(struct surface_t * s, struct region_t * clip, int x, int y, int radius, int a1, int a2, int thickness, struct color_t * c)
{
	struct xvg_context_t ctx;
	struct region_t r;
	float angle1, angle2;
	float start, sweep, step;
	float cp[8];
	int i, n;

	if(radius > 0)
	{
		region_init(&r, 0, 0, surface_get_width(s), surface_get_height(s));
		if(clip)
		{
			if(!region_intersect(&r, &r, clip))
				return;
		}
		xvg_init(&ctx, s, &r, thickness, c);
		xvg_reset(&ctx);
		angle1 = a1 * (M_PI / 180.0);
		angle2 = a2 * (M_PI / 180.0);
		if(angle2 < angle1)
		{
			angle2 = fmod(angle2 - angle1, 2 * M_PI);
			if(angle2 < 0)
				angle2 += 2 * M_PI;
			angle2 += angle1;
		}
		start = angle1;
		sweep = angle2 - angle1;
		n = 1 + (int)(sweep / M_PI_2);
		step = sweep / n;
		for(i = 0; i < n; i++, start += step)
		{
			arcto_bezier(x, y, radius, start, step, cp);
			if(i == 0)
				xvg_move_to(&ctx, cp[0], cp[1]);
			xvg_cubic_bezto(&ctx, cp[2], cp[3], cp[4], cp[5], cp[6], cp[7]);
		}
		if(thickness > 0)
			xvg_stroke(&ctx);
		else
			xvg_fill(&ctx);
		xvg_exit(&ctx);
	}
}

void render_default_shape_gradient(struct surface_t * s, struct region_t * clip, int x, int y, int w, int h, struct color_t * lt, struct color_t * rt, struct color_t * rb, struct color_t * lb)
{
	struct region_t region, r;
	struct color_t cl, cr;
	unsigned char * p, * q;
	int stride;
	int x0, y0, x1, y1;
	int i, j, u, v, t;
	uint8_t da, dr, dg, db;
	uint8_t sa, sr, sg, sb;

	region_init(&r, 0, 0, surface_get_width(s), surface_get_height(s));
	if(clip)
	{
		if(!region_intersect(&r, &r, clip))
			return;
	}
	region_init(&region, x, y, w, h);
	if(!region_intersect(&r, &r, &region))
		return;
	stride = surface_get_stride(s);
	q = (unsigned char *)surface_get_pixels(s) + y * stride + (x << 2);
	x0 = r.x - x;
	y0 = r.y - y;
	x1 = x0 + r.w;
	y1 = y0 + r.h;
	for(j = 0; j < h; j++, q += stride)
	{
		if((j >= y0) && (j < y1))
		{
			if(h > 1)
				u = (j << 8) / (h - 1);
			else
				u = 0;
			v = 256 - u;
			cl.b = (lt->b * v + lb->b * u) >> 8;
			cl.g = (lt->g * v + lb->g * u) >> 8;
			cl.r = (lt->r * v + lb->r * u) >> 8;
			cl.a = (lt->a * v + lb->a * u) >> 8;
			cr.b = (rt->b * v + rb->b * u) >> 8;
			cr.g = (rt->g * v + rb->g * u) >> 8;
			cr.r = (rt->r * v + rb->r * u) >> 8;
			cr.a = (rt->a * v + rb->a * u) >> 8;
			for(i = 0, p = q; i < w; i++, p += 4)
			{
				if((i >= x0) && (i < x1))
				{
					if(w > 1)
						u = (i << 8) / (w - 1);
					else
						u = 0;
					v = 256 - u;
					sa = (cl.a * v + cr.a * u) >> 8;
					if(sa != 0)
					{
						if(sa == 255)
						{
							p[0] = (cl.b * v + cr.b * u) >> 8;
							p[1] = (cl.g * v + cr.g * u) >> 8;
							p[2] = (cl.r * v + cr.r * u) >> 8;
							p[3] = sa;
						}
						else
						{
							sr = idiv255(((cl.r * v + cr.r * u) >> 8) * sa);
							sg = idiv255(((cl.g * v + cr.g * u) >> 8) * sa);
							sb = idiv255(((cl.b * v + cr.b * u) >> 8) * sa);
							db = p[0];
							dg = p[1];
							dr = p[2];
							da = p[3];
							t = sa + (sa >> 8);
							p[3] = (((sa + da) << 8) - da * t) >> 8;
							p[2] = (((sr + dr) << 8) - dr * t) >> 8;
							p[1] = (((sg + dg) << 8) - dg * t) >> 8;
							p[0] = (((sb + db) << 8) - db * t) >> 8;
						}
					}
				}
			}
		}
	}
}

void render_default_shape_checkerboard(struct surface_t * s, struct region_t * clip, int x, int y, int w, int h)
{
	struct region_t region, r;
	uint32_t * q, * p;
	int x1, y1, x2, y2;
	int i, j, l;

	region_init(&r, 0, 0, surface_get_width(s), surface_get_height(s));
	if(clip)
	{
		if(!region_intersect(&r, &r, clip))
			return;
	}
	region_init(&region, x, y, w, h);
	if(!region_intersect(&r, &r, &region))
		return;

	x1 = r.x;
	y1 = r.y;
	x2 = r.x + r.w;
	y2 = r.y + r.h;
	l = s->stride >> 2;
	q = (uint32_t *)s->pixels + y1 * l + x1;

	for(j = y1; j < y2; j++, q += l)
	{
		for(i = x1, p = q; i < x2; i++, p++)
		{
			if((i ^ j) & (1 << 3))
				*p = 0xffabb9bd;
			else
				*p = 0xff899598;
		}
	}
}

void render_default_filter_grayscale(struct surface_t * s)
{
	int i, len = surface_get_width(s) * surface_get_height(s);
	unsigned char * p = surface_get_pixels(s);
	unsigned char gray;

	for(i = 0; i < len; i++, p += 4)
	{
		if(p[3] != 0)
		{
			gray = (p[2] * 19595L + p[1] * 38469L + p[0] * 7472L) >> 16;
			p[0] = gray;
			p[1] = gray;
			p[2] = gray;
		}
	}
}

void render_default_filter_sepia(struct surface_t * s)
{
	int i, len = surface_get_width(s) * surface_get_height(s);
	unsigned char * p = surface_get_pixels(s);
	int r, g, b;

	for(i = 0; i < len; i++, p += 4)
	{
		if(p[3] != 0)
		{
			b = (p[2] * 17826L + p[1] * 34996L + p[0] * 8585L) >> 16;
			g = (p[2] * 22872L + p[1] * 44958L + p[0] * 11010L) >> 16;
			r = (p[2] * 25756L + p[1] * 50397L + p[0] * 12386L) >> 16;
			p[0] = min(b, 255);
			p[1] = min(g, 255);
			p[2] = min(r, 255);
		}
	}
}

void render_default_filter_invert(struct surface_t * s)
{
	int i, len = surface_get_width(s) * surface_get_height(s);
	unsigned char * p = surface_get_pixels(s);

	for(i = 0; i < len; i++, p += 4)
	{
		if(p[3] != 0)
		{
			p[0] = p[3] - p[0];
			p[1] = p[3] - p[1];
			p[2] = p[3] - p[2];
		}
	}
}

void render_default_filter_threshold(struct surface_t * s, int threshold, const char * type)
{
	int len = surface_get_width(s) * surface_get_height(s);
	unsigned char * p = surface_get_pixels(s);
	float w0, w1, u0, u1;
	float t, variance = 0;
	int minpos = 0, maxpos = 255;
	int histogram[256];
	int i, j, k;

	if((threshold < 0) || (threshold > 255))
	{
		threshold = 128;
		memset(histogram, 0, sizeof(histogram));
		for(i = 0, p = surface_get_pixels(s); i < len; i++, p += 4)
		{
			if(p[3] != 0)
			{
				if(p[3] == 255)
					histogram[p[0]]++;
				else
					histogram[p[0] * 255 / p[3]]++;
			}
		}
		for(i = 0; i < 255; i++)
		{
			if(histogram[i] != 0)
			{
				minpos = i;
				break;
			}
		}
		for(i = 255; i > 0; i--)
		{
			if(histogram[i] != 0)
			{
				maxpos = i;
				break;
			}
		}
		for(i = minpos; i <= maxpos; i++)
		{
			w1 = 0;
			u1 = 0;
			w0 = 0;
			u0 = 0;
			for(j = 0; j <= i; j++)
			{
				w1 += histogram[j];
				u1 += j * histogram[j];
			}
			if(w1 == 0)
				break;
			u1 = u1 / w1;
			w1 = w1 / len;
			for(k = i + 1; k < 255; k++)
			{
				w0 += histogram[k];
				u0 += k * histogram[k];
			}
			if(w0 == 0)
				break;
			u0 = u0 / w0;
			w0 = w0 / len;
			t = w0 * w1 * (u1 - u0) * (u1 - u0);
			if(variance < t)
			{
				variance = t;
				threshold = i;
			}
		}
	}
	switch(shash(type))
	{
	case 0xf4229cca: /* "binary" */
		for(i = 0, p = surface_get_pixels(s); i < len; i++, p += 4)
		{
			if(p[3] != 0)
			{
				if(p[3] == 255)
					p[2] = p[1] = p[0] = (p[0] > threshold) ? 255 : 0;
				else
					p[2] = p[1] = p[0] = (p[0] * 255 / p[3] > threshold) ? p[3] : 0;
			}
		}
		break;
	case 0xc880666f: /* "binary-invert" */
		for(i = 0, p = surface_get_pixels(s); i < len; i++, p += 4)
		{
			if(p[3] != 0)
			{
				if(p[3] == 255)
					p[2] = p[1] = p[0] = (p[0] > threshold) ? 0: 255;
				else
					p[2] = p[1] = p[0] = (p[0] * 255 / p[3] > threshold) ? 0 : p[3];
			}
		}
		break;
	case 0x1e92b0a8: /* "tozero" */
		for(i = 0, p = surface_get_pixels(s); i < len; i++, p += 4)
		{
			if(p[3] != 0)
			{
				if(p[3] == 255)
					p[2] = p[1] = p[0] = (p[0] > threshold) ? p[0] : 0;
				else
					p[2] = p[1] = p[0] = (p[0] * 255 / p[3] > threshold) ? p[0] : 0;
			}
		}
		break;
	case 0x98d3b48d: /* "tozero-invert" */
		for(i = 0, p = surface_get_pixels(s); i < len; i++, p += 4)
		{
			if(p[3] != 0)
			{
				if(p[3] == 255)
					p[2] = p[1] = p[0] = (p[0] > threshold) ? 0 : p[0];
				else
					p[2] = p[1] = p[0] = (p[0] * 255 / p[3] > threshold) ? 0 : p[0];
			}
		}
		break;
	case 0xe9e0dc6b: /* "truncate" */
		for(i = 0, p = surface_get_pixels(s); i < len; i++, p += 4)
		{
			if(p[3] != 0)
			{
				if(p[3] == 255)
					p[2] = p[1] = p[0] = (p[0] > threshold) ? threshold : p[0];
				else
					p[2] = p[1] = p[0] = (p[0] * 255 / p[3] > threshold) ? idiv255(threshold * p[3]) : p[3];
			}
		}
		break;
	default:
		break;
	}
}

static const unsigned char colormap_parula[256][3] = {
	{ 0x44, 0x08, 0x9d }, { 0x43, 0x0c, 0x9e }, { 0x42, 0x0f, 0x9f }, { 0x42, 0x12, 0xa0 }, { 0x41, 0x15, 0xa1 }, { 0x40, 0x17, 0xa2 }, { 0x40, 0x1a, 0xa2 }, { 0x3f, 0x1c, 0xa3 },
	{ 0x3e, 0x1f, 0xa3 }, { 0x3d, 0x21, 0xa3 }, { 0x3c, 0x23, 0xa4 }, { 0x3b, 0x25, 0xa4 }, { 0x3a, 0x28, 0xa4 }, { 0x39, 0x2a, 0xa4 }, { 0x38, 0x2c, 0xa4 }, { 0x37, 0x2e, 0xa3 },
	{ 0x35, 0x30, 0xa3 }, { 0x34, 0x32, 0xa3 }, { 0x33, 0x34, 0xa2 }, { 0x32, 0x35, 0xa2 }, { 0x31, 0x37, 0xa1 }, { 0x30, 0x39, 0xa0 }, { 0x2f, 0x3b, 0x9f }, { 0x2d, 0x3c, 0x9f },
	{ 0x2c, 0x3e, 0x9e }, { 0x2b, 0x3f, 0x9d }, { 0x2a, 0x41, 0x9c }, { 0x29, 0x42, 0x9b }, { 0x29, 0x44, 0x9a }, { 0x28, 0x45, 0x99 }, { 0x27, 0x47, 0x98 }, { 0x26, 0x48, 0x97 },
	{ 0x26, 0x49, 0x96 }, { 0x25, 0x4a, 0x95 }, { 0x25, 0x4c, 0x94 }, { 0x24, 0x4d, 0x94 }, { 0x24, 0x4e, 0x93 }, { 0x24, 0x4f, 0x92 }, { 0x23, 0x50, 0x91 }, { 0x23, 0x51, 0x90 },
	{ 0x23, 0x52, 0x8f }, { 0x23, 0x53, 0x8e }, { 0x23, 0x54, 0x8e }, { 0x23, 0x55, 0x8d }, { 0x23, 0x56, 0x8c }, { 0x23, 0x57, 0x8b }, { 0x23, 0x58, 0x8b }, { 0x23, 0x59, 0x8a },
	{ 0x24, 0x5a, 0x89 }, { 0x24, 0x5b, 0x89 }, { 0x24, 0x5c, 0x88 }, { 0x25, 0x5d, 0x88 }, { 0x25, 0x5e, 0x87 }, { 0x25, 0x5f, 0x86 }, { 0x26, 0x60, 0x86 }, { 0x26, 0x61, 0x85 },
	{ 0x26, 0x61, 0x85 }, { 0x27, 0x62, 0x84 }, { 0x27, 0x63, 0x84 }, { 0x28, 0x64, 0x83 }, { 0x28, 0x65, 0x83 }, { 0x29, 0x66, 0x83 }, { 0x29, 0x66, 0x82 }, { 0x2a, 0x67, 0x82 },
	{ 0x2a, 0x68, 0x81 }, { 0x2b, 0x69, 0x81 }, { 0x2b, 0x6a, 0x81 }, { 0x2b, 0x6b, 0x80 }, { 0x2c, 0x6b, 0x80 }, { 0x2c, 0x6c, 0x80 }, { 0x2d, 0x6d, 0x80 }, { 0x2d, 0x6e, 0x7f },
	{ 0x2d, 0x6f, 0x7f }, { 0x2e, 0x70, 0x7f }, { 0x2e, 0x70, 0x7e }, { 0x2e, 0x71, 0x7e }, { 0x2f, 0x72, 0x7e }, { 0x2f, 0x73, 0x7e }, { 0x2f, 0x74, 0x7d }, { 0x30, 0x75, 0x7d },
	{ 0x30, 0x75, 0x7d }, { 0x30, 0x76, 0x7d }, { 0x30, 0x77, 0x7d }, { 0x30, 0x78, 0x7c }, { 0x30, 0x79, 0x7c }, { 0x31, 0x7a, 0x7c }, { 0x31, 0x7a, 0x7c }, { 0x31, 0x7b, 0x7b },
	{ 0x31, 0x7c, 0x7b }, { 0x31, 0x7d, 0x7b }, { 0x31, 0x7e, 0x7b }, { 0x31, 0x7f, 0x7a }, { 0x31, 0x80, 0x7a }, { 0x31, 0x81, 0x7a }, { 0x31, 0x81, 0x79 }, { 0x30, 0x82, 0x79 },
	{ 0x30, 0x83, 0x79 }, { 0x30, 0x84, 0x79 }, { 0x30, 0x85, 0x78 }, { 0x30, 0x86, 0x78 }, { 0x30, 0x87, 0x77 }, { 0x2f, 0x88, 0x77 }, { 0x2f, 0x89, 0x77 }, { 0x2f, 0x89, 0x76 },
	{ 0x2f, 0x8a, 0x76 }, { 0x2e, 0x8b, 0x75 }, { 0x2e, 0x8c, 0x75 }, { 0x2e, 0x8d, 0x74 }, { 0x2d, 0x8e, 0x74 }, { 0x2d, 0x8f, 0x73 }, { 0x2d, 0x90, 0x73 }, { 0x2c, 0x91, 0x72 },
	{ 0x2c, 0x92, 0x72 }, { 0x2c, 0x93, 0x71 }, { 0x2b, 0x94, 0x70 }, { 0x2b, 0x95, 0x70 }, { 0x2b, 0x95, 0x6f }, { 0x2b, 0x96, 0x6e }, { 0x2a, 0x97, 0x6d }, { 0x2a, 0x98, 0x6d },
	{ 0x2a, 0x99, 0x6c }, { 0x2a, 0x9a, 0x6b }, { 0x29, 0x9b, 0x6a }, { 0x29, 0x9c, 0x69 }, { 0x29, 0x9d, 0x68 }, { 0x29, 0x9e, 0x67 }, { 0x29, 0x9f, 0x66 }, { 0x29, 0xa0, 0x65 },
	{ 0x29, 0xa1, 0x64 }, { 0x2a, 0xa1, 0x63 }, { 0x2a, 0xa2, 0x62 }, { 0x2a, 0xa3, 0x61 }, { 0x2b, 0xa4, 0x60 }, { 0x2b, 0xa5, 0x5e }, { 0x2c, 0xa6, 0x5d }, { 0x2c, 0xa7, 0x5c },
	{ 0x2d, 0xa8, 0x5a }, { 0x2e, 0xa8, 0x59 }, { 0x2f, 0xa9, 0x58 }, { 0x30, 0xaa, 0x56 }, { 0x31, 0xab, 0x55 }, { 0x33, 0xac, 0x53 }, { 0x34, 0xad, 0x52 }, { 0x35, 0xad, 0x50 },
	{ 0x37, 0xae, 0x4e }, { 0x39, 0xaf, 0x4d }, { 0x3b, 0xb0, 0x4b }, { 0x3c, 0xb1, 0x49 }, { 0x3e, 0xb1, 0x47 }, { 0x41, 0xb2, 0x46 }, { 0x43, 0xb3, 0x44 }, { 0x45, 0xb3, 0x42 },
	{ 0x48, 0xb4, 0x40 }, { 0x4a, 0xb5, 0x3e }, { 0x4d, 0xb5, 0x3c }, { 0x4f, 0xb6, 0x3a }, { 0x52, 0xb6, 0x37 }, { 0x55, 0xb7, 0x35 }, { 0x58, 0xb7, 0x33 }, { 0x5b, 0xb8, 0x31 },
	{ 0x5e, 0xb8, 0x2f }, { 0x61, 0xb9, 0x2d }, { 0x64, 0xb9, 0x2b }, { 0x67, 0xba, 0x29 }, { 0x6b, 0xba, 0x27 }, { 0x6e, 0xba, 0x25 }, { 0x71, 0xbb, 0x23 }, { 0x74, 0xbb, 0x22 },
	{ 0x77, 0xbb, 0x20 }, { 0x7b, 0xbc, 0x1f }, { 0x7e, 0xbc, 0x1e }, { 0x81, 0xbc, 0x1d }, { 0x84, 0xbc, 0x1c }, { 0x87, 0xbd, 0x1c }, { 0x8a, 0xbd, 0x1b }, { 0x8d, 0xbd, 0x1b },
	{ 0x90, 0xbd, 0x1b }, { 0x93, 0xbd, 0x1b }, { 0x95, 0xbe, 0x1c }, { 0x98, 0xbe, 0x1c }, { 0x9b, 0xbe, 0x1d }, { 0x9e, 0xbe, 0x1e }, { 0xa0, 0xbe, 0x1f }, { 0xa3, 0xbf, 0x20 },
	{ 0xa5, 0xbf, 0x21 }, { 0xa8, 0xbf, 0x22 }, { 0xaa, 0xbf, 0x23 }, { 0xad, 0xbf, 0x24 }, { 0xaf, 0xc0, 0x25 }, { 0xb2, 0xc0, 0x27 }, { 0xb4, 0xc0, 0x28 }, { 0xb7, 0xc0, 0x29 },
	{ 0xb9, 0xc1, 0x2b }, { 0xbb, 0xc1, 0x2c }, { 0xbe, 0xc1, 0x2d }, { 0xc0, 0xc1, 0x2f }, { 0xc2, 0xc1, 0x30 }, { 0xc4, 0xc2, 0x32 }, { 0xc7, 0xc2, 0x33 }, { 0xc9, 0xc2, 0x34 },
	{ 0xcb, 0xc2, 0x36 }, { 0xcd, 0xc2, 0x37 }, { 0xcf, 0xc3, 0x39 }, { 0xd2, 0xc3, 0x3a }, { 0xd4, 0xc3, 0x3b }, { 0xd6, 0xc3, 0x3d }, { 0xd8, 0xc4, 0x3e }, { 0xda, 0xc4, 0x40 },
	{ 0xdc, 0xc4, 0x41 }, { 0xde, 0xc4, 0x42 }, { 0xe1, 0xc4, 0x44 }, { 0xe3, 0xc5, 0x45 }, { 0xe5, 0xc5, 0x46 }, { 0xe7, 0xc5, 0x48 }, { 0xe9, 0xc5, 0x49 }, { 0xeb, 0xc5, 0x4a },
	{ 0xed, 0xc6, 0x4c }, { 0xef, 0xc6, 0x4d }, { 0xf1, 0xc6, 0x4e }, { 0xf3, 0xc6, 0x50 }, { 0xf6, 0xc6, 0x51 }, { 0xf8, 0xc7, 0x52 }, { 0xfa, 0xc7, 0x53 }, { 0xfc, 0xc7, 0x53 },
	{ 0xfd, 0xc8, 0x51 }, { 0xfd, 0xc9, 0x50 }, { 0xfe, 0xca, 0x4f }, { 0xfe, 0xcc, 0x4e }, { 0xfe, 0xcd, 0x4d }, { 0xfe, 0xce, 0x4c }, { 0xfe, 0xd0, 0x4b }, { 0xfe, 0xd1, 0x4a },
	{ 0xfe, 0xd3, 0x49 }, { 0xfe, 0xd4, 0x48 }, { 0xfe, 0xd5, 0x47 }, { 0xfe, 0xd7, 0x46 }, { 0xfe, 0xd8, 0x45 }, { 0xfe, 0xda, 0x44 }, { 0xfe, 0xdb, 0x43 }, { 0xfe, 0xdc, 0x41 },
	{ 0xfe, 0xde, 0x40 }, { 0xfd, 0xdf, 0x3f }, { 0xfd, 0xe1, 0x3d }, { 0xfd, 0xe2, 0x3c }, { 0xfd, 0xe3, 0x3a }, { 0xfd, 0xe5, 0x38 }, { 0xfd, 0xe6, 0x37 }, { 0xfd, 0xe8, 0x35 },
	{ 0xfd, 0xe9, 0x33 }, { 0xfc, 0xeb, 0x31 }, { 0xfc, 0xec, 0x2e }, { 0xfc, 0xee, 0x2c }, { 0xfc, 0xef, 0x29 }, { 0xfc, 0xf1, 0x27 }, { 0xfb, 0xf2, 0x23 }, { 0xfb, 0xf4, 0x20 },
};

static const unsigned char colormap_jet[256][3] = {
	{ 0x00, 0x00, 0x7f }, { 0x00, 0x00, 0x83 }, { 0x00, 0x00, 0x87 }, { 0x00, 0x00, 0x8b }, { 0x00, 0x00, 0x8f }, { 0x00, 0x00, 0x93 }, { 0x00, 0x00, 0x97 }, { 0x00, 0x00, 0x9b },
	{ 0x00, 0x00, 0x9f }, { 0x00, 0x00, 0xa3 }, { 0x00, 0x00, 0xa7 }, { 0x00, 0x00, 0xab }, { 0x00, 0x00, 0xaf }, { 0x00, 0x00, 0xb3 }, { 0x00, 0x00, 0xb7 }, { 0x00, 0x00, 0xbb },
	{ 0x00, 0x00, 0xbf }, { 0x00, 0x00, 0xc3 }, { 0x00, 0x00, 0xc7 }, { 0x00, 0x00, 0xcb }, { 0x00, 0x00, 0xcf }, { 0x00, 0x00, 0xd3 }, { 0x00, 0x00, 0xd7 }, { 0x00, 0x00, 0xdb },
	{ 0x00, 0x00, 0xdf }, { 0x00, 0x00, 0xe3 }, { 0x00, 0x00, 0xe7 }, { 0x00, 0x00, 0xeb }, { 0x00, 0x00, 0xef }, { 0x00, 0x00, 0xf3 }, { 0x00, 0x00, 0xf7 }, { 0x00, 0x00, 0xfb },
	{ 0x00, 0x00, 0xff }, { 0x00, 0x04, 0xff }, { 0x00, 0x08, 0xff }, { 0x00, 0x0c, 0xff }, { 0x00, 0x10, 0xff }, { 0x00, 0x14, 0xff }, { 0x00, 0x18, 0xff }, { 0x00, 0x1c, 0xff },
	{ 0x00, 0x20, 0xff }, { 0x00, 0x24, 0xff }, { 0x00, 0x28, 0xff }, { 0x00, 0x2c, 0xff }, { 0x00, 0x30, 0xff }, { 0x00, 0x34, 0xff }, { 0x00, 0x38, 0xff }, { 0x00, 0x3c, 0xff },
	{ 0x00, 0x40, 0xff }, { 0x00, 0x44, 0xff }, { 0x00, 0x48, 0xff }, { 0x00, 0x4c, 0xff }, { 0x00, 0x50, 0xff }, { 0x00, 0x54, 0xff }, { 0x00, 0x58, 0xff }, { 0x00, 0x5c, 0xff },
	{ 0x00, 0x60, 0xff }, { 0x00, 0x64, 0xff }, { 0x00, 0x68, 0xff }, { 0x00, 0x6c, 0xff }, { 0x00, 0x70, 0xff }, { 0x00, 0x74, 0xff }, { 0x00, 0x78, 0xff }, { 0x00, 0x7c, 0xff },
	{ 0x00, 0x80, 0xff }, { 0x00, 0x84, 0xff }, { 0x00, 0x88, 0xff }, { 0x00, 0x8c, 0xff }, { 0x00, 0x90, 0xff }, { 0x00, 0x94, 0xff }, { 0x00, 0x98, 0xff }, { 0x00, 0x9c, 0xff },
	{ 0x00, 0xa0, 0xff }, { 0x00, 0xa4, 0xff }, { 0x00, 0xa8, 0xff }, { 0x00, 0xac, 0xff }, { 0x00, 0xb0, 0xff }, { 0x00, 0xb4, 0xff }, { 0x00, 0xb8, 0xff }, { 0x00, 0xbc, 0xff },
	{ 0x00, 0xc0, 0xff }, { 0x00, 0xc4, 0xff }, { 0x00, 0xc8, 0xff }, { 0x00, 0xcc, 0xff }, { 0x00, 0xd0, 0xff }, { 0x00, 0xd4, 0xff }, { 0x00, 0xd8, 0xff }, { 0x00, 0xdc, 0xff },
	{ 0x00, 0xe0, 0xff }, { 0x00, 0xe4, 0xff }, { 0x00, 0xe8, 0xff }, { 0x00, 0xec, 0xff }, { 0x00, 0xf0, 0xff }, { 0x00, 0xf4, 0xff }, { 0x00, 0xf8, 0xff }, { 0x00, 0xfc, 0xff },
	{ 0x01, 0xff, 0xfd }, { 0x05, 0xff, 0xf9 }, { 0x09, 0xff, 0xf5 }, { 0x0d, 0xff, 0xf1 }, { 0x11, 0xff, 0xed }, { 0x15, 0xff, 0xe9 }, { 0x19, 0xff, 0xe5 }, { 0x1d, 0xff, 0xe1 },
	{ 0x21, 0xff, 0xdd }, { 0x25, 0xff, 0xd9 }, { 0x29, 0xff, 0xd5 }, { 0x2d, 0xff, 0xd1 }, { 0x31, 0xff, 0xcd }, { 0x35, 0xff, 0xc9 }, { 0x39, 0xff, 0xc5 }, { 0x3d, 0xff, 0xc1 },
	{ 0x41, 0xff, 0xbd }, { 0x45, 0xff, 0xb9 }, { 0x49, 0xff, 0xb5 }, { 0x4d, 0xff, 0xb1 }, { 0x51, 0xff, 0xad }, { 0x55, 0xff, 0xa9 }, { 0x59, 0xff, 0xa5 }, { 0x5d, 0xff, 0xa1 },
	{ 0x61, 0xff, 0x9d }, { 0x65, 0xff, 0x99 }, { 0x69, 0xff, 0x95 }, { 0x6d, 0xff, 0x91 }, { 0x71, 0xff, 0x8d }, { 0x75, 0xff, 0x89 }, { 0x79, 0xff, 0x85 }, { 0x7d, 0xff, 0x81 },
	{ 0x81, 0xff, 0x7d }, { 0x85, 0xff, 0x79 }, { 0x89, 0xff, 0x75 }, { 0x8d, 0xff, 0x71 }, { 0x91, 0xff, 0x6d }, { 0x95, 0xff, 0x69 }, { 0x99, 0xff, 0x65 }, { 0x9d, 0xff, 0x61 },
	{ 0xa1, 0xff, 0x5d }, { 0xa5, 0xff, 0x59 }, { 0xa9, 0xff, 0x55 }, { 0xad, 0xff, 0x51 }, { 0xb1, 0xff, 0x4d }, { 0xb5, 0xff, 0x49 }, { 0xb9, 0xff, 0x45 }, { 0xbd, 0xff, 0x41 },
	{ 0xc1, 0xff, 0x3d }, { 0xc5, 0xff, 0x39 }, { 0xc9, 0xff, 0x35 }, { 0xcd, 0xff, 0x31 }, { 0xd1, 0xff, 0x2d }, { 0xd5, 0xff, 0x29 }, { 0xd9, 0xff, 0x25 }, { 0xdd, 0xff, 0x21 },
	{ 0xe1, 0xff, 0x1d }, { 0xe5, 0xff, 0x19 }, { 0xe9, 0xff, 0x15 }, { 0xed, 0xff, 0x11 }, { 0xf1, 0xff, 0x0d }, { 0xf5, 0xff, 0x09 }, { 0xf9, 0xff, 0x05 }, { 0xfd, 0xff, 0x01 },
	{ 0xff, 0xfc, 0x00 }, { 0xff, 0xf8, 0x00 }, { 0xff, 0xf4, 0x00 }, { 0xff, 0xf0, 0x00 }, { 0xff, 0xec, 0x00 }, { 0xff, 0xe8, 0x00 }, { 0xff, 0xe4, 0x00 }, { 0xff, 0xe0, 0x00 },
	{ 0xff, 0xdc, 0x00 }, { 0xff, 0xd8, 0x00 }, { 0xff, 0xd4, 0x00 }, { 0xff, 0xd0, 0x00 }, { 0xff, 0xcc, 0x00 }, { 0xff, 0xc8, 0x00 }, { 0xff, 0xc4, 0x00 }, { 0xff, 0xc0, 0x00 },
	{ 0xff, 0xbc, 0x00 }, { 0xff, 0xb8, 0x00 }, { 0xff, 0xb4, 0x00 }, { 0xff, 0xb0, 0x00 }, { 0xff, 0xac, 0x00 }, { 0xff, 0xa8, 0x00 }, { 0xff, 0xa4, 0x00 }, { 0xff, 0xa0, 0x00 },
	{ 0xff, 0x9c, 0x00 }, { 0xff, 0x98, 0x00 }, { 0xff, 0x94, 0x00 }, { 0xff, 0x90, 0x00 }, { 0xff, 0x8c, 0x00 }, { 0xff, 0x88, 0x00 }, { 0xff, 0x84, 0x00 }, { 0xff, 0x80, 0x00 },
	{ 0xff, 0x7c, 0x00 }, { 0xff, 0x78, 0x00 }, { 0xff, 0x74, 0x00 }, { 0xff, 0x70, 0x00 }, { 0xff, 0x6c, 0x00 }, { 0xff, 0x68, 0x00 }, { 0xff, 0x64, 0x00 }, { 0xff, 0x60, 0x00 },
	{ 0xff, 0x5c, 0x00 }, { 0xff, 0x58, 0x00 }, { 0xff, 0x54, 0x00 }, { 0xff, 0x50, 0x00 }, { 0xff, 0x4c, 0x00 }, { 0xff, 0x48, 0x00 }, { 0xff, 0x44, 0x00 }, { 0xff, 0x40, 0x00 },
	{ 0xff, 0x3c, 0x00 }, { 0xff, 0x38, 0x00 }, { 0xff, 0x34, 0x00 }, { 0xff, 0x30, 0x00 }, { 0xff, 0x2c, 0x00 }, { 0xff, 0x28, 0x00 }, { 0xff, 0x24, 0x00 }, { 0xff, 0x20, 0x00 },
	{ 0xff, 0x1c, 0x00 }, { 0xff, 0x18, 0x00 }, { 0xff, 0x14, 0x00 }, { 0xff, 0x10, 0x00 }, { 0xff, 0x0c, 0x00 }, { 0xff, 0x08, 0x00 }, { 0xff, 0x04, 0x00 }, { 0xff, 0x00, 0x00 },
	{ 0xfb, 0x00, 0x00 }, { 0xf7, 0x00, 0x00 }, { 0xf3, 0x00, 0x00 }, { 0xef, 0x00, 0x00 }, { 0xeb, 0x00, 0x00 }, { 0xe7, 0x00, 0x00 }, { 0xe3, 0x00, 0x00 }, { 0xdf, 0x00, 0x00 },
	{ 0xdb, 0x00, 0x00 }, { 0xd7, 0x00, 0x00 }, { 0xd3, 0x00, 0x00 }, { 0xcf, 0x00, 0x00 }, { 0xcb, 0x00, 0x00 }, { 0xc7, 0x00, 0x00 }, { 0xc3, 0x00, 0x00 }, { 0xbf, 0x00, 0x00 },
	{ 0xbb, 0x00, 0x00 }, { 0xb7, 0x00, 0x00 }, { 0xb3, 0x00, 0x00 }, { 0xaf, 0x00, 0x00 }, { 0xab, 0x00, 0x00 }, { 0xa7, 0x00, 0x00 }, { 0xa3, 0x00, 0x00 }, { 0x9f, 0x00, 0x00 },
	{ 0x9b, 0x00, 0x00 }, { 0x97, 0x00, 0x00 }, { 0x93, 0x00, 0x00 }, { 0x8f, 0x00, 0x00 }, { 0x8b, 0x00, 0x00 }, { 0x87, 0x00, 0x00 }, { 0x83, 0x00, 0x00 }, { 0x7f, 0x00, 0x00 },
};

static const unsigned char colormap_rainbow[256][3] = {
	{ 0x26, 0x00, 0x32 }, { 0x26, 0x00, 0x33 }, { 0x26, 0x00, 0x33 }, { 0x26, 0x00, 0x33 }, { 0x27, 0x00, 0x34 }, { 0x27, 0x00, 0x34 }, { 0x27, 0x00, 0x35 }, { 0x28, 0x00, 0x36 },
	{ 0x29, 0x00, 0x37 }, { 0x29, 0x00, 0x39 }, { 0x2a, 0x00, 0x3a }, { 0x2b, 0x00, 0x3c }, { 0x2c, 0x00, 0x3d }, { 0x2d, 0x00, 0x3f }, { 0x2e, 0x00, 0x41 }, { 0x2f, 0x00, 0x43 },
	{ 0x30, 0x00, 0x46 }, { 0x32, 0x00, 0x48 }, { 0x33, 0x00, 0x4b }, { 0x34, 0x00, 0x4d }, { 0x35, 0x00, 0x50 }, { 0x36, 0x00, 0x53 }, { 0x38, 0x00, 0x56 }, { 0x39, 0x00, 0x59 },
	{ 0x3a, 0x00, 0x5c }, { 0x3b, 0x00, 0x60 }, { 0x3c, 0x00, 0x63 }, { 0x3d, 0x00, 0x67 }, { 0x3e, 0x00, 0x6a }, { 0x3f, 0x00, 0x6e }, { 0x40, 0x00, 0x72 }, { 0x41, 0x00, 0x75 },
	{ 0x41, 0x00, 0x79 }, { 0x42, 0x00, 0x7d }, { 0x42, 0x00, 0x81 }, { 0x42, 0x00, 0x85 }, { 0x42, 0x00, 0x89 }, { 0x42, 0x00, 0x8d }, { 0x42, 0x00, 0x91 }, { 0x41, 0x00, 0x96 },
	{ 0x41, 0x00, 0x9a }, { 0x40, 0x00, 0x9e }, { 0x3f, 0x00, 0xa2 }, { 0x3e, 0x00, 0xa6 }, { 0x3c, 0x00, 0xaa }, { 0x3b, 0x00, 0xae }, { 0x39, 0x00, 0xb3 }, { 0x37, 0x00, 0xb7 },
	{ 0x34, 0x00, 0xbb }, { 0x32, 0x00, 0xbf }, { 0x2f, 0x00, 0xc3 }, { 0x2c, 0x00, 0xc6 }, { 0x29, 0x00, 0xca }, { 0x26, 0x00, 0xce }, { 0x22, 0x00, 0xd2 }, { 0x1e, 0x00, 0xd5 },
	{ 0x1a, 0x00, 0xd9 }, { 0x15, 0x00, 0xdc }, { 0x11, 0x00, 0xdf }, { 0x0c, 0x00, 0xe2 }, { 0x07, 0x00, 0xe5 }, { 0x02, 0x00, 0xe8 }, { 0x00, 0x03, 0xeb }, { 0x00, 0x08, 0xed },
	{ 0x00, 0x0e, 0xf0 }, { 0x00, 0x14, 0xf2 }, { 0x00, 0x1a, 0xf4 }, { 0x00, 0x21, 0xf6 }, { 0x00, 0x27, 0xf7 }, { 0x00, 0x2e, 0xf9 }, { 0x00, 0x34, 0xfa }, { 0x00, 0x3b, 0xfb },
	{ 0x00, 0x42, 0xfc }, { 0x00, 0x49, 0xfd }, { 0x00, 0x50, 0xfd }, { 0x00, 0x56, 0xfd }, { 0x00, 0x5d, 0xfd }, { 0x00, 0x64, 0xfd }, { 0x00, 0x6b, 0xfd }, { 0x00, 0x72, 0xfd },
	{ 0x00, 0x79, 0xfd }, { 0x00, 0x80, 0xfd }, { 0x00, 0x88, 0xfc }, { 0x00, 0x8f, 0xfc }, { 0x00, 0x96, 0xfc }, { 0x00, 0x9d, 0xfc }, { 0x00, 0xa5, 0xfc }, { 0x00, 0xac, 0xfc },
	{ 0x00, 0xb4, 0xfc }, { 0x00, 0xbb, 0xfc }, { 0x00, 0xc2, 0xfb }, { 0x00, 0xca, 0xfb }, { 0x00, 0xd2, 0xfb }, { 0x00, 0xd9, 0xfb }, { 0x00, 0xe1, 0xfa }, { 0x00, 0xe8, 0xfa },
	{ 0x00, 0xf0, 0xfa }, { 0x00, 0xf7, 0xfa }, { 0x00, 0xf9, 0xf4 }, { 0x00, 0xf9, 0xeb }, { 0x00, 0xf8, 0xe3 }, { 0x00, 0xf8, 0xdb }, { 0x00, 0xf7, 0xd2 }, { 0x00, 0xf7, 0xca },
	{ 0x00, 0xf6, 0xc1 }, { 0x00, 0xf5, 0xb9 }, { 0x00, 0xf5, 0xb0 }, { 0x00, 0xf4, 0xa7 }, { 0x00, 0xf3, 0x9f }, { 0x00, 0xf1, 0x96 }, { 0x00, 0xf0, 0x8d }, { 0x00, 0xee, 0x84 },
	{ 0x00, 0xed, 0x7b }, { 0x00, 0xeb, 0x72 }, { 0x00, 0xe8, 0x69 }, { 0x00, 0xe5, 0x60 }, { 0x00, 0xe2, 0x57 }, { 0x00, 0xde, 0x4e }, { 0x00, 0xd9, 0x45 }, { 0x00, 0xd4, 0x3d },
	{ 0x00, 0xce, 0x34 }, { 0x00, 0xc6, 0x2b }, { 0x00, 0xbe, 0x23 }, { 0x00, 0xb5, 0x1b }, { 0x00, 0xac, 0x14 }, { 0x00, 0xa3, 0x0d }, { 0x00, 0x9d, 0x07 }, { 0x00, 0x99, 0x02 },
	{ 0x02, 0x99, 0x00 }, { 0x07, 0x9d, 0x00 }, { 0x0d, 0xa3, 0x00 }, { 0x14, 0xac, 0x00 }, { 0x1b, 0xb5, 0x00 }, { 0x23, 0xbe, 0x00 }, { 0x2b, 0xc6, 0x00 }, { 0x34, 0xce, 0x00 },
	{ 0x3d, 0xd4, 0x00 }, { 0x45, 0xd9, 0x00 }, { 0x4e, 0xde, 0x00 }, { 0x57, 0xe2, 0x00 }, { 0x60, 0xe5, 0x00 }, { 0x69, 0xe8, 0x00 }, { 0x72, 0xeb, 0x00 }, { 0x7b, 0xed, 0x00 },
	{ 0x84, 0xee, 0x00 }, { 0x8d, 0xf0, 0x00 }, { 0x96, 0xf1, 0x00 }, { 0x9f, 0xf3, 0x00 }, { 0xa7, 0xf4, 0x00 }, { 0xb0, 0xf5, 0x00 }, { 0xb9, 0xf5, 0x00 }, { 0xc1, 0xf6, 0x00 },
	{ 0xca, 0xf7, 0x00 }, { 0xd2, 0xf7, 0x00 }, { 0xdb, 0xf8, 0x00 }, { 0xe3, 0xf8, 0x00 }, { 0xeb, 0xf9, 0x00 }, { 0xf4, 0xf9, 0x00 }, { 0xfa, 0xf7, 0x00 }, { 0xfa, 0xf0, 0x00 },
	{ 0xfa, 0xe8, 0x00 }, { 0xfa, 0xe1, 0x00 }, { 0xfb, 0xd9, 0x00 }, { 0xfb, 0xd2, 0x00 }, { 0xfb, 0xca, 0x00 }, { 0xfb, 0xc2, 0x00 }, { 0xfc, 0xbb, 0x00 }, { 0xfc, 0xb4, 0x00 },
	{ 0xfc, 0xac, 0x00 }, { 0xfc, 0xa5, 0x00 }, { 0xfc, 0x9d, 0x00 }, { 0xfc, 0x96, 0x00 }, { 0xfc, 0x8f, 0x00 }, { 0xfc, 0x88, 0x00 }, { 0xfd, 0x80, 0x00 }, { 0xfd, 0x79, 0x00 },
	{ 0xfd, 0x72, 0x00 }, { 0xfd, 0x6b, 0x00 }, { 0xfd, 0x64, 0x00 }, { 0xfd, 0x5d, 0x00 }, { 0xfd, 0x56, 0x00 }, { 0xfd, 0x50, 0x00 }, { 0xfd, 0x49, 0x00 }, { 0xfc, 0x42, 0x00 },
	{ 0xfb, 0x3b, 0x00 }, { 0xfa, 0x34, 0x00 }, { 0xf9, 0x2e, 0x00 }, { 0xf7, 0x27, 0x00 }, { 0xf6, 0x21, 0x00 }, { 0xf4, 0x1a, 0x00 }, { 0xf2, 0x14, 0x00 }, { 0xf0, 0x0e, 0x00 },
	{ 0xed, 0x08, 0x00 }, { 0xeb, 0x03, 0x00 }, { 0xe8, 0x00, 0x02 }, { 0xe5, 0x00, 0x07 }, { 0xe2, 0x00, 0x0c }, { 0xdf, 0x00, 0x11 }, { 0xdc, 0x00, 0x15 }, { 0xd9, 0x00, 0x1a },
	{ 0xd5, 0x00, 0x1e }, { 0xd2, 0x00, 0x22 }, { 0xce, 0x00, 0x26 }, { 0xca, 0x00, 0x29 }, { 0xc6, 0x00, 0x2c }, { 0xc3, 0x00, 0x2f }, { 0xbf, 0x00, 0x32 }, { 0xbb, 0x00, 0x34 },
	{ 0xb7, 0x00, 0x37 }, { 0xb3, 0x00, 0x39 }, { 0xae, 0x00, 0x3b }, { 0xaa, 0x00, 0x3c }, { 0xa6, 0x00, 0x3e }, { 0xa2, 0x00, 0x3f }, { 0x9e, 0x00, 0x40 }, { 0x9a, 0x00, 0x41 },
	{ 0x96, 0x00, 0x41 }, { 0x91, 0x00, 0x42 }, { 0x8d, 0x00, 0x42 }, { 0x89, 0x00, 0x42 }, { 0x85, 0x00, 0x42 }, { 0x81, 0x00, 0x42 }, { 0x7d, 0x00, 0x42 }, { 0x79, 0x00, 0x41 },
	{ 0x75, 0x00, 0x41 }, { 0x72, 0x00, 0x40 }, { 0x6e, 0x00, 0x3f }, { 0x6a, 0x00, 0x3e }, { 0x67, 0x00, 0x3d }, { 0x63, 0x00, 0x3c }, { 0x60, 0x00, 0x3b }, { 0x5c, 0x00, 0x3a },
	{ 0x59, 0x00, 0x39 }, { 0x56, 0x00, 0x38 }, { 0x53, 0x00, 0x36 }, { 0x50, 0x00, 0x35 }, { 0x4d, 0x00, 0x34 }, { 0x4b, 0x00, 0x33 }, { 0x48, 0x00, 0x32 }, { 0x46, 0x00, 0x30 },
	{ 0x43, 0x00, 0x2f }, { 0x41, 0x00, 0x2e }, { 0x3f, 0x00, 0x2d }, { 0x3d, 0x00, 0x2c }, { 0x3c, 0x00, 0x2b }, { 0x3a, 0x00, 0x2a }, { 0x39, 0x00, 0x29 }, { 0x37, 0x00, 0x29 },
	{ 0x36, 0x00, 0x28 }, { 0x35, 0x00, 0x27 }, { 0x34, 0x00, 0x27 }, { 0x34, 0x00, 0x27 }, { 0x33, 0x00, 0x26 }, { 0x33, 0x00, 0x26 }, { 0x33, 0x00, 0x26 }, { 0x32, 0x00, 0x26 },
};

void render_default_filter_colormap(struct surface_t * s, const char * type)
{
	int i, len = surface_get_width(s) * surface_get_height(s);
	unsigned char * p = surface_get_pixels(s);
	const unsigned char (*cm)[3];
	unsigned char r, g, b;

	switch(shash(type))
	{
	case 0x143c974a: /* "parula" */
		cm = colormap_parula;
		break;
	case 0x0b888348: /* "jet" */
		cm = colormap_jet;
		break;
	case 0x351d1e97: /* "rainbow" */
		cm = colormap_rainbow;
		break;
	default:
		cm = colormap_parula;
		break;
	}
	for(i = 0; i < len; i++, p += 4)
	{
		if(p[3] != 0)
		{
			if(p[3] == 255)
			{
				p[0] = cm[p[0]][0];
				p[1] = cm[p[1]][1];
				p[2] = cm[p[2]][2];
			}
			else
			{
				b = p[0] * 255 / p[3];
				g = p[1] * 255 / p[3];
				r = p[2] * 255 / p[3];
				p[0] = idiv255(cm[b][0] * p[3]);
				p[1] = idiv255(cm[g][1] * p[3]);
				p[2] = idiv255(cm[r][2] * p[3]);
			}
		}
	}
}

void render_default_filter_coloring(struct surface_t * s, struct color_t * c)
{
	int i, len = surface_get_width(s) * surface_get_height(s);
	unsigned char * p = surface_get_pixels(s);
	unsigned char r = c->r;
	unsigned char g = c->g;
	unsigned char b = c->b;

	for(i = 0; i < len; i++, p += 4)
	{
		if(p[3] == 255)
		{
			p[0] = b;
			p[1] = g;
			p[2] = r;
		}
		else
		{
			p[0] = idiv255(b * p[3]);
			p[1] = idiv255(g * p[3]);
			p[2] = idiv255(r * p[3]);
		}
	}
}

void render_default_filter_hue(struct surface_t * s, int angle)
{
	int i, len = surface_get_width(s) * surface_get_height(s);
	unsigned char * p = surface_get_pixels(s);
	float av = angle * M_PI / 180.0;
	float cv = cosf(av);
	float sv = sinf(av);
	int r, g, b;
	int tr, tg, tb;
	int m[9];

	m[0] = (0.213 + cv * 0.787 - sv * 0.213) * 65536;
	m[1] = (0.715 - cv * 0.715 - sv * 0.715) * 65536;
	m[2] = (0.072 - cv * 0.072 + sv * 0.928) * 65536;
	m[3] = (0.213 - cv * 0.213 + sv * 0.143) * 65536;
	m[4] = (0.715 + cv * 0.285 + sv * 0.140) * 65536;
	m[5] = (0.072 - cv * 0.072 - sv * 0.283) * 65536;
	m[6] = (0.213 - cv * 0.213 - sv * 0.787) * 65536;
	m[7] = (0.715 - cv * 0.715 + sv * 0.715) * 65536;
	m[8] = (0.072 + cv * 0.928 + sv * 0.072) * 65536;
	for(i = 0; i < len; i++, p += 4)
	{
		if(p[3] != 0)
		{
			if(p[3] == 255)
			{
				b = p[0];
				g = p[1];
				r = p[2];
				tb = (m[6] * r + m[7] * g + m[8] * b) >> 16;
				tg = (m[3] * r + m[4] * g + m[5] * b) >> 16;
				tr = (m[0] * r + m[1] * g + m[2] * b) >> 16;
				p[0] = clamp(tb, 0, 255);
				p[1] = clamp(tg, 0, 255);
				p[2] = clamp(tr, 0, 255);
			}
			else
			{
				b = p[0] * 255 / p[3];
				g = p[1] * 255 / p[3];
				r = p[2] * 255 / p[3];
				tb = (m[6] * r + m[7] * g + m[8] * b) >> 16;
				tg = (m[3] * r + m[4] * g + m[5] * b) >> 16;
				tr = (m[0] * r + m[1] * g + m[2] * b) >> 16;
				p[0] = clamp(idiv255(tb * p[3]), 0, 255);
				p[1] = clamp(idiv255(tg * p[3]), 0, 255);
				p[2] = clamp(idiv255(tr * p[3]), 0, 255);
			}
		}
	}
}

void render_default_filter_saturate(struct surface_t * s, int saturate)
{
	int i, len = surface_get_width(s) * surface_get_height(s);
	unsigned char * p = surface_get_pixels(s);
	int v = clamp(saturate, -100, 100) * 128 / 100;
	int r, g, b, vmin, vmax;
	int alpha, delta, value, lv, sv;

	for(i = 0; i < len; i++, p += 4)
	{
		if(p[3] != 0)
		{
			if(p[3] == 255)
			{
				b = p[0];
				g = p[1];
				r = p[2];
				vmin = min(min(r, g), b);
				vmax = max(max(r, g), b);
				delta = vmax - vmin;
				value = vmax + vmin;
				if(delta == 0)
					continue;
				lv = value >> 1;
				sv = lv < 128 ? (delta << 7) / value : (delta << 7) / (510 - value);
				if(v >= 0)
				{
					alpha = (v + sv >= 128) ? sv : 128 - v;
					if(alpha != 0)
						alpha = 128 * 128 / alpha - 128;
				}
				else
				{
					alpha = v;
				}
				r = r + ((r - lv) * alpha >> 7);
				g = g + ((g - lv) * alpha >> 7);
				b = b + ((b - lv) * alpha >> 7);
				p[0] = clamp(b, 0, 255);
				p[1] = clamp(g, 0, 255);
				p[2] = clamp(r, 0, 255);
			}
			else
			{
				b = p[0] * 255 / p[3];
				g = p[1] * 255 / p[3];
				r = p[2] * 255 / p[3];
				vmin = min(min(r, g), b);
				vmax = max(max(r, g), b);
				delta = vmax - vmin;
				value = vmax + vmin;
				if(delta == 0)
					continue;
				lv = value >> 1;
				sv = lv < 128 ? (delta << 7) / value : (delta << 7) / (510 - value);
				if(v >= 0)
				{
					alpha = (v + sv >= 128) ? sv : 128 - v;
					if(alpha != 0)
						alpha = 128 * 128 / alpha - 128;
				}
				else
				{
					alpha = v;
				}
				r = r + ((r - lv) * alpha >> 7);
				g = g + ((g - lv) * alpha >> 7);
				b = b + ((b - lv) * alpha >> 7);
				p[0] = clamp(idiv255(b * p[3]), 0, 255);
				p[1] = clamp(idiv255(g * p[3]), 0, 255);
				p[2] = clamp(idiv255(r * p[3]), 0, 255);
			}
		}
	}
}

void render_default_filter_brightness(struct surface_t * s, int brightness)
{
	int i, len = surface_get_width(s) * surface_get_height(s);
	unsigned char * p = surface_get_pixels(s);
	int t, v = clamp(brightness, -100, 100) * 255 / 100;

	for(i = 0; i < len; i++, p += 4)
	{
		if(p[3] != 0)
		{
			if(p[3] == 255)
			{
				p[0] = clamp(p[0] + v, 0, 255);
				p[1] = clamp(p[1] + v, 0, 255);
				p[2] = clamp(p[2] + v, 0, 255);
			}
			else
			{
				t = idiv255(v * p[3]);
				p[0] = clamp(p[0] + t, 0, 255);
				p[1] = clamp(p[1] + t, 0, 255);
				p[2] = clamp(p[2] + t, 0, 255);
			}
		}
	}
}

void render_default_filter_contrast(struct surface_t * s, int contrast)
{
	int i, len = surface_get_width(s) * surface_get_height(s);
	unsigned char * p = surface_get_pixels(s);
	int v = clamp(contrast, -100, 100) * 128 / 100;
	int r, g, b;
	int tr, tg, tb;

	for(i = 0; i < len; i++, p += 4)
	{
		if(p[3] != 0)
		{
			if(p[3] == 255)
			{
				b = p[0];
				g = p[1];
				r = p[2];
				tb = ((b << 7) + (b - 128) * v) >> 7;
				tg = ((g << 7) + (g - 128) * v) >> 7;
				tr = ((r << 7) + (r - 128) * v) >> 7;
				p[0] = clamp(tb, 0, 255);
				p[1] = clamp(tg, 0, 255);
				p[2] = clamp(tr, 0, 255);
			}
			else
			{
				b = p[0] * 255 / p[3];
				g = p[1] * 255 / p[3];
				r = p[2] * 255 / p[3];
				tb = ((b << 7) + (b - 128) * v) >> 7;
				tg = ((g << 7) + (g - 128) * v) >> 7;
				tr = ((r << 7) + (r - 128) * v) >> 7;
				p[0] = clamp(idiv255(tb * p[3]), 0, 255);
				p[1] = clamp(idiv255(tg * p[3]), 0, 255);
				p[2] = clamp(idiv255(tr * p[3]), 0, 255);
			}
		}
	}
}

void render_default_filter_opacity(struct surface_t * s, int alpha)
{
	int i, len = surface_get_width(s) * surface_get_height(s);
	unsigned char * p = surface_get_pixels(s);
	int v = clamp(alpha, 0, 100) * 256 / 100;

	switch(v)
	{
	case 0:
		memset(s->pixels, 0, s->pixlen);
		break;
	case 256:
		break;
	default:
		for(i = 0; i < len; i++, p += 4)
		{
			if(p[3] != 0)
			{
				p[0] = (p[0] * v) >> 8;
				p[1] = (p[1] * v) >> 8;
				p[2] = (p[2] * v) >> 8;
				p[3] = (p[3] * v) >> 8;
			}
		}
		break;
	}
}

void render_default_filter_haldclut(struct surface_t * s, struct surface_t * clut, const char * type)
{
	int width = surface_get_width(s);
	int height = surface_get_height(s);
	int stride = surface_get_stride(s);
	unsigned char * p, * q = surface_get_pixels(s);
	int cw = surface_get_width(clut);
	int ch = surface_get_height(clut);
	unsigned char * t, * cp, * cq = surface_get_pixels(clut);
	double sum[9];
	double dr, dg, db, xdr, xdg, xdb;
	int ri, gi, bi;
	int x, y, v;
	int level, level2, level_1, level_2;

	if(cw == ch)
	{
		switch(cw)
		{
		case 8:    level =  2 *  2; break;
		case 27:   level =  3 *  3; break;
		case 64:   level =  4 *  4; break;
		case 125:  level =  5 *  5; break;
		case 216:  level =  6 *  6; break;
		case 343:  level =  7 *  7; break;
		case 512:  level =  8 *  8; break;
		case 729:  level =  9 *  9; break;
		case 1000: level = 10 * 10; break;
		case 1331: level = 11 * 11; break;
		case 1728: level = 12 * 12; break;
		case 2197: level = 13 * 13; break;
		case 2744: level = 14 * 14; break;
		case 3375: level = 15 * 15; break;
		case 4096: level = 16 * 16; break;
		default:
			return;
		}
		level2 = level * level;
		level_1 = level - 1;
		level_2 = level - 2;
		switch(shash(type))
		{
		case 0x09fa48d7: /* "nearest" */
			for(y = 0; y < height; y++, q += stride)
			{
				for(x = 0, p = q; x < width; x++, p += 4)
				{
					if(p[3] != 0)
					{
						if(p[3] == 255)
						{
							bi = idiv255(p[0] * level_1);
							if(bi > level_2)
								bi = level_2;
							gi = idiv255(p[1] * level_1);
							if(gi > level_2)
								gi = level_2;
							ri = idiv255(p[2] * level_1);
							if(ri > level_2)
								ri = level_2;
							cp = cq + ((bi * level2 + gi * level + ri) << 2);
							p[0] = cp[0];
							p[1] = cp[1];
							p[2] = cp[2];
						}
						else
						{
							bi = p[0] * level_1 / p[3];
							if(bi > level_2)
								bi = level_2;
							gi = p[1] * level_1 / p[3];
							if(gi > level_2)
								gi = level_2;
							ri = p[2] * level_1 / p[3];
							if(ri > level_2)
								ri = level_2;
							cp = cq + ((bi * level2 + gi * level + ri) << 2);
							p[0] = idiv255(cp[0] * p[3]);
							p[1] = idiv255(cp[1] * p[3]);
							p[2] = idiv255(cp[2] * p[3]);
						}
					}
				}
			}
			break;
		case 0x860ab38f: /* "trilinear" */
			for(y = 0; y < height; y++, q += stride)
			{
				for(x = 0, p = q; x < width; x++, p += 4)
				{
					if(p[3] != 0)
					{
						if(p[3] == 255)
						{
							bi = idiv255(p[0] * level_1);
							if(bi > level_2)
								bi = level_2;
							gi = idiv255(p[1] * level_1);
							if(gi > level_2)
								gi = level_2;
							ri = idiv255(p[2] * level_1);
							if(ri > level_2)
								ri = level_2;
							db = (double)p[0] * level_1 / 255 - bi;
							dg = (double)p[1] * level_1 / 255 - gi;
							dr = (double)p[2] * level_1 / 255 - ri;
							xdb = 1 - db;
							xdg = 1 - dg;
							xdr = 1 - dr;
							cp = cq + ((bi * level2 + gi * level + ri) << 2);
							t = cp;
							sum[0] = (double)t[0] * xdr;
							sum[1] = (double)t[1] * xdr;
							sum[2] = (double)t[2] * xdr;
							t += 4;
							sum[0] += (double)t[0] * dr;
							sum[1] += (double)t[1] * dr;
							sum[2] += (double)t[2] * dr;
							t = cp + (level << 2);
							sum[3] = (double)t[0] * xdr;
							sum[4] = (double)t[1] * xdr;
							sum[5] = (double)t[2] * xdr;
							t += 4;
							sum[3] += (double)t[0] * dr;
							sum[4] += (double)t[1] * dr;
							sum[5] += (double)t[2] * dr;
							sum[6] = sum[0] * xdg + sum[3] * dg;
							sum[7] = sum[1] * xdg + sum[4] * dg;
							sum[8] = sum[2] * xdg + sum[5] * dg;
							t = cp + (level2 << 2);
							sum[0] = (double)t[0] * xdr;
							sum[1] = (double)t[1] * xdr;
							sum[2] = (double)t[2] * xdr;
							t += 4;
							sum[0] += (double)t[0] * dr;
							sum[1] += (double)t[1] * dr;
							sum[2] += (double)t[2] * dr;
							t = cp + ((level2 + level) << 2);
							sum[3] = (double)t[0] * xdr;
							sum[4] = (double)t[1] * xdr;
							sum[5] = (double)t[2] * xdr;
							t += 4;
							sum[3] += (double)t[0] * dr;
							sum[4] += (double)t[1] * dr;
							sum[5] += (double)t[2] * dr;
							sum[0] = sum[0] * xdg + sum[3] * dg;
							sum[1] = sum[1] * xdg + sum[4] * dg;
							sum[2] = sum[2] * xdg + sum[5] * dg;
							v = sum[6] * xdb + sum[0] * db;
							p[0] = clamp(v, 0, 255);
							v = sum[7] * xdb + sum[1] * db;
							p[1] = clamp(v, 0, 255);
							v = sum[8] * xdb + sum[2] * db;
							p[2] = clamp(v, 0, 255);
						}
						else
						{
							bi = p[0] * level_1 / p[3];
							if(bi > level_2)
								bi = level_2;
							gi = p[1] * level_1 / p[3];
							if(gi > level_2)
								gi = level_2;
							ri = p[2] * level_1 / p[3];
							if(ri > level_2)
								ri = level_2;
							db = (double)p[0] * level_1 / p[3] - bi;
							dg = (double)p[1] * level_1 / p[3] - gi;
							dr = (double)p[2] * level_1 / p[3] - ri;
							xdb = 1 - db;
							xdg = 1 - dg;
							xdr = 1 - dr;
							cp = cq + ((bi * level2 + gi * level + ri) << 2);
							t = cp;
							sum[0] = (double)t[0] * xdr;
							sum[1] = (double)t[1] * xdr;
							sum[2] = (double)t[2] * xdr;
							t += 4;
							sum[0] += (double)t[0] * dr;
							sum[1] += (double)t[1] * dr;
							sum[2] += (double)t[2] * dr;
							t = cp + (level << 2);
							sum[3] = (double)t[0] * xdr;
							sum[4] = (double)t[1] * xdr;
							sum[5] = (double)t[2] * xdr;
							t += 4;
							sum[3] += (double)t[0] * dr;
							sum[4] += (double)t[1] * dr;
							sum[5] += (double)t[2] * dr;
							sum[6] = sum[0] * xdg + sum[3] * dg;
							sum[7] = sum[1] * xdg + sum[4] * dg;
							sum[8] = sum[2] * xdg + sum[5] * dg;
							t = cp + (level2 << 2);
							sum[0] = (double)t[0] * xdr;
							sum[1] = (double)t[1] * xdr;
							sum[2] = (double)t[2] * xdr;
							t += 4;
							sum[0] += (double)t[0] * dr;
							sum[1] += (double)t[1] * dr;
							sum[2] += (double)t[2] * dr;
							t = cp + ((level2 + level) << 2);
							sum[3] = (double)t[0] * xdr;
							sum[4] = (double)t[1] * xdr;
							sum[5] = (double)t[2] * xdr;
							t += 4;
							sum[3] += (double)t[0] * dr;
							sum[4] += (double)t[1] * dr;
							sum[5] += (double)t[2] * dr;
							sum[0] = sum[0] * xdg + sum[3] * dg;
							sum[1] = sum[1] * xdg + sum[4] * dg;
							sum[2] = sum[2] * xdg + sum[5] * dg;
							v = (sum[6] * xdb + sum[0] * db) * p[3] / 255;
							p[0] = clamp(v, 0, 255);
							v = (sum[7] * xdb + sum[1] * db) * p[3] / 255;
							p[1] = clamp(v, 0, 255);
							v = (sum[8] * xdb + sum[2] * db) * p[3] / 255;
							p[2] = clamp(v, 0, 255);
						}
					}
				}
			}
			break;
		default:
			break;
		}
	}
}

static inline void blurinner(unsigned char * p, int * zr, int * zg, int * zb, int * za, int alpha)
{
	int r, g, b;
	unsigned char a;

	b = p[0];
	g = p[1];
	r = p[2];
	a = p[3];

	*zb += (alpha * ((b << 7) - *zb)) >> 16;
	*zg += (alpha * ((g << 7) - *zg)) >> 16;
	*zr += (alpha * ((r << 7) - *zr)) >> 16;
	*za += (alpha * ((a << 7) - *za)) >> 16;

	p[0] = *zb >> 7;
	p[1] = *zg >> 7;
	p[2] = *zr >> 7;
	p[3] = *za >> 7;
}

static inline void blurrow(unsigned char * pixel, int width, int height, int channel, int line, int alpha)
{
	unsigned char * p = &(pixel[line * width * channel]);
	int zr, zg, zb, za;
	int i;

	zb = p[0] << 7;
	zg = p[1] << 7;
	zr = p[2] << 7;
	za = p[3] << 7;

	for(i = 0; i < width; i++)
		blurinner(&p[i * channel], &zr, &zg, &zb, &za, alpha);
	for(i = width - 2; i >= 0; i--)
		blurinner(&p[i * channel], &zr, &zg, &zb, &za, alpha);
}

static inline void blurcol(unsigned char * pixel, int width, int height, int channel, int x, int alpha)
{
	unsigned char * p = pixel;
	int zr, zg, zb, za;
	int i;

	p += x * channel;
	zb = p[0] << 7;
	zg = p[1] << 7;
	zr = p[2] << 7;
	za = p[3] << 7;

	for(i = width; i < (height - 1) * width; i += width)
		blurinner(&p[i * channel], &zr, &zg, &zb, &za, alpha);
	for(i = (height - 2) * width; i >= 0; i -= width)
		blurinner(&p[i * channel], &zr, &zg, &zb, &za, alpha);
}

static inline void expblur(unsigned char * pixel, int width, int height, int channel, int radius)
{
	int alpha = (int)((1 << 16) * (1.0 - expf(-2.3 / (radius + 1.0))));
	int row, col;

	for(row = 0; row < height; row++)
		blurrow(pixel, width, height, channel, row, alpha);
	for(col = 0; col < width; col++)
		blurcol(pixel, width, height, channel, col, alpha);
}

void render_default_filter_blur(struct surface_t * s, int radius)
{
	int width = surface_get_width(s);
	int height = surface_get_height(s);
	unsigned char * pixels = surface_get_pixels(s);

	if(radius > 0)
		expblur(pixels, width, height, 4, radius);
}

void render_default_filter_erode(struct surface_t * s, int times)
{
	int width = surface_get_width(s);
	int height = surface_get_height(s);
	int stride = surface_get_stride(s);
	int len = width * height;
	unsigned char * l, * t, * r, * b;
	unsigned char * lt, * rt, * lb, * rb;
	unsigned char * p, * q;
	unsigned char red, green, blue;
	void * pixels;
	int x, y, u, v;
	int i;

	pixels = memalign(4, s->pixlen);
	if(pixels)
	{
		while(times-- > 0)
		{
			for(i = 0, p = surface_get_pixels(s), q = pixels; i < len; i++, p += 4, q += 4)
			{
				if((p[3] != 0) && (p[3] != 255))
				{
					q[0] = p[0] * 255 / p[3];
					q[1] = p[1] * 255 / p[3];
					q[2] = p[2] * 255 / p[3];
					q[3] = p[3];
				}
				else
				{
					*((uint32_t *)q) = *((uint32_t *)p);
				}
			}
			for(y = 0, p = surface_get_pixels(s), q = pixels; y < height; y++)
			{
				for(x = 0; x < width; x++, p += 4, q += 4)
				{
					if(q[3] != 0)
					{
						u = (x - 1 > 0 ? 1 : x) << 2;
						v = (x + 1 < width ? 1 : width - x - 1) << 2;
						l = q - u;
						t = q - ((y - 1 > 0 ? 1 : y) * stride);
						r = q + v;
						b = q + ((y + 1 < height ? 1 : height - y - 1) * stride);
						lt = t - u;
						rt = t + v;
						lb = b - u;
						rb = b + v;
						blue = q[0];
						if(l[0] < blue)
							blue = l[0];
						if(t[0] < blue)
							blue = t[0];
						if(r[0] < blue)
							blue = r[0];
						if(b[0] < blue)
							blue = b[0];
						if(lt[0] < blue)
							blue = lt[0];
						if(rt[0] < blue)
							blue = rt[0];
						if(lb[0] < blue)
							blue = lb[0];
						if(rb[0] < blue)
							blue = rb[0];
						green = q[1];
						if(l[1] < green)
							green = l[1];
						if(t[1] < green)
							green = t[1];
						if(r[1] < green)
							green = r[1];
						if(b[1] < green)
							green = b[1];
						if(lt[1] < green)
							green = lt[1];
						if(rt[1] < green)
							green = rt[1];
						if(lb[1] < green)
							green = lb[1];
						if(rb[1] < green)
							green = rb[1];
						red = q[2];
						if(l[2] < red)
							red = l[2];
						if(t[2] < red)
							red = t[2];
						if(r[2] < red)
							red = r[2];
						if(b[2] < red)
							red = b[2];
						if(lt[2] < red)
							red = lt[2];
						if(rt[2] < red)
							red = rt[2];
						if(lb[2] < red)
							red = lb[2];
						if(rb[2] < red)
							red = rb[2];
						if(q[3] == 255)
						{
							p[0] = blue;
							p[1] = green;
							p[2] = red;
						}
						else
						{
							p[0] = idiv255(blue * q[3]);
							p[1] = idiv255(green * q[3]);
							p[2] = idiv255(red * q[3]);
						}
					}
				}
			}
		}
		free(pixels);
	}
}

void render_default_filter_dilate(struct surface_t * s, int times)
{
	int width = surface_get_width(s);
	int height = surface_get_height(s);
	int stride = surface_get_stride(s);
	int len = width * height;
	unsigned char * l, * t, * r, * b;
	unsigned char * lt, * rt, * lb, * rb;
	unsigned char * p, * q;
	unsigned char red, green, blue;
	void * pixels;
	int x, y, u, v;
	int i;

	pixels = memalign(4, s->pixlen);
	if(pixels)
	{
		while(times-- > 0)
		{
			for(i = 0, p = surface_get_pixels(s), q = pixels; i < len; i++, p += 4, q += 4)
			{
				if((p[3] != 0) && (p[3] != 255))
				{
					q[0] = p[0] * 255 / p[3];
					q[1] = p[1] * 255 / p[3];
					q[2] = p[2] * 255 / p[3];
					q[3] = p[3];
				}
				else
				{
					*((uint32_t *)q) = *((uint32_t *)p);
				}
			}
			for(y = 0, p = surface_get_pixels(s), q = pixels; y < height; y++)
			{
				for(x = 0; x < width; x++, p += 4, q += 4)
				{
					if(q[3] != 0)
					{
						u = (x - 1 > 0 ? 1 : x) << 2;
						v = (x + 1 < width ? 1 : width - x - 1) << 2;
						l = q - u;
						t = q - ((y - 1 > 0 ? 1 : y) * stride);
						r = q + v;
						b = q + ((y + 1 < height ? 1 : height - y - 1) * stride);
						lt = t - u;
						rt = t + v;
						lb = b - u;
						rb = b + v;
						blue = q[0];
						if(l[0] > blue)
							blue = l[0];
						if(t[0] > blue)
							blue = t[0];
						if(r[0] > blue)
							blue = r[0];
						if(b[0] > blue)
							blue = b[0];
						if(lt[0] > blue)
							blue = lt[0];
						if(rt[0] > blue)
							blue = rt[0];
						if(lb[0] > blue)
							blue = lb[0];
						if(rb[0] > blue)
							blue = rb[0];
						green = q[1];
						if(l[1] > green)
							green = l[1];
						if(t[1] > green)
							green = t[1];
						if(r[1] > green)
							green = r[1];
						if(b[1] > green)
							green = b[1];
						if(lt[1] > green)
							green = lt[1];
						if(rt[1] > green)
							green = rt[1];
						if(lb[1] > green)
							green = lb[1];
						if(rb[1] > green)
							green = rb[1];
						red = q[2];
						if(l[2] > red)
							red = l[2];
						if(t[2] > red)
							red = t[2];
						if(r[2] > red)
							red = r[2];
						if(b[2] > red)
							red = b[2];
						if(lt[2] > red)
							red = lt[2];
						if(rt[2] > red)
							red = rt[2];
						if(lb[2] > red)
							red = lb[2];
						if(rb[2] > red)
							red = rb[2];
						if(q[3] == 255)
						{
							p[0] = blue;
							p[1] = green;
							p[2] = red;
						}
						else
						{
							p[0] = idiv255(blue * q[3]);
							p[1] = idiv255(green * q[3]);
							p[2] = idiv255(red * q[3]);
						}
					}
				}
			}
		}
		free(pixels);
	}
}
