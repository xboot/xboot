/*
 * kernel/graphic/render.c
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

void render_default_filter_gray(struct surface_t * s)
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
