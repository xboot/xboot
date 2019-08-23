/*
 * kernel/graphic/svg-rast.c
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

#include <ctype.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <shash.h>
#include <graphic/svg.h>
#include <graphic/svg-rast.h>

struct svg_rasterizer_t * nsvgCreateRasterizer()
{
	struct svg_rasterizer_t * r = (struct svg_rasterizer_t *)malloc(sizeof(struct svg_rasterizer_t));
	if (r == NULL) goto error;
	memset(r, 0, sizeof(struct svg_rasterizer_t));

	r->tessTol = 0.25f;
	r->distTol = 0.01f;

	return r;

error:
	nsvgDeleteRasterizer(r);
	return NULL;
}

void nsvgDeleteRasterizer(struct svg_rasterizer_t * r)
{
	struct svg_mem_page_t* p;

	if (r == NULL) return;

	p = r->pages;
	while (p != NULL) {
		struct svg_mem_page_t* next = p->next;
		free(p);
		p = next;
	}

	if (r->edges) free(r->edges);
	if (r->points) free(r->points);
	if (r->points2) free(r->points2);
	if (r->scanline) free(r->scanline);

	free(r);
}

static struct svg_mem_page_t * nsvg__nextPage(struct svg_rasterizer_t * r, struct svg_mem_page_t* cur)
{
	struct svg_mem_page_t *newp;

	// If using existing chain, return the next page in chain
	if (cur != NULL && cur->next != NULL) {
		return cur->next;
	}

	// Alloc new page
	newp = (struct svg_mem_page_t*)malloc(sizeof(struct svg_mem_page_t));
	if (newp == NULL) return NULL;
	memset(newp, 0, sizeof(struct svg_mem_page_t));

	// Add to linked list
	if (cur != NULL)
		cur->next = newp;
	else
		r->pages = newp;

	return newp;
}

static void nsvg__resetPool(struct svg_rasterizer_t * r)
{
	struct svg_mem_page_t* p = r->pages;
	while (p != NULL) {
		p->size = 0;
		p = p->next;
	}
	r->curpage = r->pages;
}

static unsigned char* nsvg__alloc(struct svg_rasterizer_t * r, int size)
{
	unsigned char* buf;
	if (size > SVG__MEMPAGE_SIZE) return NULL;
	if (r->curpage == NULL || r->curpage->size+size > SVG__MEMPAGE_SIZE) {
		r->curpage = nsvg__nextPage(r, r->curpage);
	}
	buf = &r->curpage->mem[r->curpage->size];
	r->curpage->size += size;
	return buf;
}

static int nsvg__ptEquals(float x1, float y1, float x2, float y2, float tol)
{
	float dx = x2 - x1;
	float dy = y2 - y1;
	return dx*dx + dy*dy < tol*tol;
}

static void nsvg__addPathPoint(struct svg_rasterizer_t * r, float x, float y, int flags)
{
	struct svg_point_t* pt;

	if (r->npoints > 0) {
		pt = &r->points[r->npoints-1];
		if (nsvg__ptEquals(pt->x,pt->y, x,y, r->distTol)) {
			pt->flags = (unsigned char)(pt->flags | flags);
			return;
		}
	}

	if (r->npoints+1 > r->cpoints) {
		r->cpoints = r->cpoints > 0 ? r->cpoints * 2 : 64;
		r->points = (struct svg_point_t*)realloc(r->points, sizeof(struct svg_point_t) * r->cpoints);
		if (r->points == NULL) return;
	}

	pt = &r->points[r->npoints];
	pt->x = x;
	pt->y = y;
	pt->flags = (unsigned char)flags;
	r->npoints++;
}

static void nsvg__appendPathPoint(struct svg_rasterizer_t * r, struct svg_point_t pt)
{
	if (r->npoints+1 > r->cpoints) {
		r->cpoints = r->cpoints > 0 ? r->cpoints * 2 : 64;
		r->points = (struct svg_point_t*)realloc(r->points, sizeof(struct svg_point_t) * r->cpoints);
		if (r->points == NULL) return;
	}
	r->points[r->npoints] = pt;
	r->npoints++;
}

static void nsvg__duplicatePoints(struct svg_rasterizer_t * r)
{
	if (r->npoints > r->cpoints2) {
		r->cpoints2 = r->npoints;
		r->points2 = (struct svg_point_t*)realloc(r->points2, sizeof(struct svg_point_t) * r->cpoints2);
		if (r->points2 == NULL) return;
	}

	memcpy(r->points2, r->points, sizeof(struct svg_point_t) * r->npoints);
	r->npoints2 = r->npoints;
}

static void nsvg__addEdge(struct svg_rasterizer_t * r, float x0, float y0, float x1, float y1)
{
	struct svg_edge_t* e;

	// Skip horizontal edges
	if (y0 == y1)
		return;

	if (r->nedges+1 > r->cedges) {
		r->cedges = r->cedges > 0 ? r->cedges * 2 : 64;
		r->edges = (struct svg_edge_t*)realloc(r->edges, sizeof(struct svg_edge_t) * r->cedges);
		if (r->edges == NULL) return;
	}

	e = &r->edges[r->nedges];
	r->nedges++;

	if (y0 < y1) {
		e->x0 = x0;
		e->y0 = y0;
		e->x1 = x1;
		e->y1 = y1;
		e->dir = 1;
	} else {
		e->x0 = x1;
		e->y0 = y1;
		e->x1 = x0;
		e->y1 = y0;
		e->dir = -1;
	}
}

static float nsvg__normalize(float *x, float* y)
{
	float d = sqrtf((*x)*(*x) + (*y)*(*y));
	if (d > 1e-6f) {
		float id = 1.0f / d;
		*x *= id;
		*y *= id;
	}
	return d;
}

static float nsvg__absf(float x) { return x < 0 ? -x : x; }

static void nsvg__flattenCubicBez(struct svg_rasterizer_t * r,
								  float x1, float y1, float x2, float y2,
								  float x3, float y3, float x4, float y4,
								  int level, int type)
{
	float x12,y12,x23,y23,x34,y34,x123,y123,x234,y234,x1234,y1234;
	float dx,dy,d2,d3;

	if (level > 10) return;

	x12 = (x1+x2)*0.5f;
	y12 = (y1+y2)*0.5f;
	x23 = (x2+x3)*0.5f;
	y23 = (y2+y3)*0.5f;
	x34 = (x3+x4)*0.5f;
	y34 = (y3+y4)*0.5f;
	x123 = (x12+x23)*0.5f;
	y123 = (y12+y23)*0.5f;

	dx = x4 - x1;
	dy = y4 - y1;
	d2 = nsvg__absf(((x2 - x4) * dy - (y2 - y4) * dx));
	d3 = nsvg__absf(((x3 - x4) * dy - (y3 - y4) * dx));

	if ((d2 + d3)*(d2 + d3) < r->tessTol * (dx*dx + dy*dy)) {
		nsvg__addPathPoint(r, x4, y4, type);
		return;
	}

	x234 = (x23+x34)*0.5f;
	y234 = (y23+y34)*0.5f;
	x1234 = (x123+x234)*0.5f;
	y1234 = (y123+y234)*0.5f;

	nsvg__flattenCubicBez(r, x1,y1, x12,y12, x123,y123, x1234,y1234, level+1, 0);
	nsvg__flattenCubicBez(r, x1234,y1234, x234,y234, x34,y34, x4,y4, level+1, type);
}

static void nsvg__flattenShape(struct svg_rasterizer_t * r, struct svg_shape_t* shape, float scale)
{
	int i, j;
	struct svg_path_t* path;

	for (path = shape->paths; path != NULL; path = path->next) {
		r->npoints = 0;
		// Flatten path
		nsvg__addPathPoint(r, path->pts[0]*scale, path->pts[1]*scale, 0);
		for (i = 0; i < path->npts-1; i += 3) {
			float* p = &path->pts[i*2];
			nsvg__flattenCubicBez(r, p[0]*scale,p[1]*scale, p[2]*scale,p[3]*scale, p[4]*scale,p[5]*scale, p[6]*scale,p[7]*scale, 0, 0);
		}
		// Close path
		nsvg__addPathPoint(r, path->pts[0]*scale, path->pts[1]*scale, 0);
		// Build edges
		for (i = 0, j = r->npoints-1; i < r->npoints; j = i++)
			nsvg__addEdge(r, r->points[j].x, r->points[j].y, r->points[i].x, r->points[i].y);
	}
}

enum NSVGpointFlags
{
	SVG_PT_CORNER = 0x01,
	SVG_PT_BEVEL = 0x02,
	SVG_PT_LEFT = 0x04
};

static void nsvg__initClosed(struct svg_point_t* left, struct svg_point_t* right, struct svg_point_t* p0, struct svg_point_t* p1, float lineWidth)
{
	float w = lineWidth * 0.5f;
	float dx = p1->x - p0->x;
	float dy = p1->y - p0->y;
	float len = nsvg__normalize(&dx, &dy);
	float px = p0->x + dx*len*0.5f, py = p0->y + dy*len*0.5f;
	float dlx = dy, dly = -dx;
	float lx = px - dlx*w, ly = py - dly*w;
	float rx = px + dlx*w, ry = py + dly*w;
	left->x = lx; left->y = ly;
	right->x = rx; right->y = ry;
}

static void nsvg__buttCap(struct svg_rasterizer_t * r, struct svg_point_t* left, struct svg_point_t* right, struct svg_point_t* p, float dx, float dy, float lineWidth, int connect)
{
	float w = lineWidth * 0.5f;
	float px = p->x, py = p->y;
	float dlx = dy, dly = -dx;
	float lx = px - dlx*w, ly = py - dly*w;
	float rx = px + dlx*w, ry = py + dly*w;

	nsvg__addEdge(r, lx, ly, rx, ry);

	if (connect) {
		nsvg__addEdge(r, left->x, left->y, lx, ly);
		nsvg__addEdge(r, rx, ry, right->x, right->y);
	}
	left->x = lx; left->y = ly;
	right->x = rx; right->y = ry;
}

static void nsvg__squareCap(struct svg_rasterizer_t * r, struct svg_point_t* left, struct svg_point_t* right, struct svg_point_t* p, float dx, float dy, float lineWidth, int connect)
{
	float w = lineWidth * 0.5f;
	float px = p->x - dx*w, py = p->y - dy*w;
	float dlx = dy, dly = -dx;
	float lx = px - dlx*w, ly = py - dly*w;
	float rx = px + dlx*w, ry = py + dly*w;

	nsvg__addEdge(r, lx, ly, rx, ry);

	if (connect) {
		nsvg__addEdge(r, left->x, left->y, lx, ly);
		nsvg__addEdge(r, rx, ry, right->x, right->y);
	}
	left->x = lx; left->y = ly;
	right->x = rx; right->y = ry;
}

#ifndef SVG_PI
#define SVG_PI (3.14159265358979323846264338327f)
#endif

static void nsvg__roundCap(struct svg_rasterizer_t * r, struct svg_point_t* left, struct svg_point_t* right, struct svg_point_t* p, float dx, float dy, float lineWidth, int ncap, int connect)
{
	int i;
	float w = lineWidth * 0.5f;
	float px = p->x, py = p->y;
	float dlx = dy, dly = -dx;
	float lx = 0, ly = 0, rx = 0, ry = 0, prevx = 0, prevy = 0;

	for (i = 0; i < ncap; i++) {
		float a = (float)i/(float)(ncap-1)*SVG_PI;
		float ax = cosf(a) * w, ay = sinf(a) * w;
		float x = px - dlx*ax - dx*ay;
		float y = py - dly*ax - dy*ay;

		if (i > 0)
			nsvg__addEdge(r, prevx, prevy, x, y);

		prevx = x;
		prevy = y;

		if (i == 0) {
			lx = x; ly = y;
		} else if (i == ncap-1) {
			rx = x; ry = y;
		}
	}

	if (connect) {
		nsvg__addEdge(r, left->x, left->y, lx, ly);
		nsvg__addEdge(r, rx, ry, right->x, right->y);
	}

	left->x = lx; left->y = ly;
	right->x = rx; right->y = ry;
}

static void nsvg__bevelJoin(struct svg_rasterizer_t * r, struct svg_point_t* left, struct svg_point_t* right, struct svg_point_t* p0, struct svg_point_t* p1, float lineWidth)
{
	float w = lineWidth * 0.5f;
	float dlx0 = p0->dy, dly0 = -p0->dx;
	float dlx1 = p1->dy, dly1 = -p1->dx;
	float lx0 = p1->x - (dlx0 * w), ly0 = p1->y - (dly0 * w);
	float rx0 = p1->x + (dlx0 * w), ry0 = p1->y + (dly0 * w);
	float lx1 = p1->x - (dlx1 * w), ly1 = p1->y - (dly1 * w);
	float rx1 = p1->x + (dlx1 * w), ry1 = p1->y + (dly1 * w);

	nsvg__addEdge(r, lx0, ly0, left->x, left->y);
	nsvg__addEdge(r, lx1, ly1, lx0, ly0);

	nsvg__addEdge(r, right->x, right->y, rx0, ry0);
	nsvg__addEdge(r, rx0, ry0, rx1, ry1);

	left->x = lx1; left->y = ly1;
	right->x = rx1; right->y = ry1;
}

static void nsvg__miterJoin(struct svg_rasterizer_t * r, struct svg_point_t* left, struct svg_point_t* right, struct svg_point_t* p0, struct svg_point_t* p1, float lineWidth)
{
	float w = lineWidth * 0.5f;
	float dlx0 = p0->dy, dly0 = -p0->dx;
	float dlx1 = p1->dy, dly1 = -p1->dx;
	float lx0, rx0, lx1, rx1;
	float ly0, ry0, ly1, ry1;

	if (p1->flags & SVG_PT_LEFT) {
		lx0 = lx1 = p1->x - p1->dmx * w;
		ly0 = ly1 = p1->y - p1->dmy * w;
		nsvg__addEdge(r, lx1, ly1, left->x, left->y);

		rx0 = p1->x + (dlx0 * w);
		ry0 = p1->y + (dly0 * w);
		rx1 = p1->x + (dlx1 * w);
		ry1 = p1->y + (dly1 * w);
		nsvg__addEdge(r, right->x, right->y, rx0, ry0);
		nsvg__addEdge(r, rx0, ry0, rx1, ry1);
	} else {
		lx0 = p1->x - (dlx0 * w);
		ly0 = p1->y - (dly0 * w);
		lx1 = p1->x - (dlx1 * w);
		ly1 = p1->y - (dly1 * w);
		nsvg__addEdge(r, lx0, ly0, left->x, left->y);
		nsvg__addEdge(r, lx1, ly1, lx0, ly0);

		rx0 = rx1 = p1->x + p1->dmx * w;
		ry0 = ry1 = p1->y + p1->dmy * w;
		nsvg__addEdge(r, right->x, right->y, rx1, ry1);
	}

	left->x = lx1; left->y = ly1;
	right->x = rx1; right->y = ry1;
}

static void nsvg__roundJoin(struct svg_rasterizer_t * r, struct svg_point_t* left, struct svg_point_t* right, struct svg_point_t* p0, struct svg_point_t* p1, float lineWidth, int ncap)
{
	int i, n;
	float w = lineWidth * 0.5f;
	float dlx0 = p0->dy, dly0 = -p0->dx;
	float dlx1 = p1->dy, dly1 = -p1->dx;
	float a0 = atan2f(dly0, dlx0);
	float a1 = atan2f(dly1, dlx1);
	float da = a1 - a0;
	float lx, ly, rx, ry;

	if (da < SVG_PI) da += SVG_PI*2;
	if (da > SVG_PI) da -= SVG_PI*2;

	n = (int)ceilf((nsvg__absf(da) / SVG_PI) * (float)ncap);
	if (n < 2) n = 2;
	if (n > ncap) n = ncap;

	lx = left->x;
	ly = left->y;
	rx = right->x;
	ry = right->y;

	for (i = 0; i < n; i++) {
		float u = (float)i/(float)(n-1);
		float a = a0 + u*da;
		float ax = cosf(a) * w, ay = sinf(a) * w;
		float lx1 = p1->x - ax, ly1 = p1->y - ay;
		float rx1 = p1->x + ax, ry1 = p1->y + ay;

		nsvg__addEdge(r, lx1, ly1, lx, ly);
		nsvg__addEdge(r, rx, ry, rx1, ry1);

		lx = lx1; ly = ly1;
		rx = rx1; ry = ry1;
	}

	left->x = lx; left->y = ly;
	right->x = rx; right->y = ry;
}

static void nsvg__straightJoin(struct svg_rasterizer_t * r, struct svg_point_t* left, struct svg_point_t* right, struct svg_point_t* p1, float lineWidth)
{
	float w = lineWidth * 0.5f;
	float lx = p1->x - (p1->dmx * w), ly = p1->y - (p1->dmy * w);
	float rx = p1->x + (p1->dmx * w), ry = p1->y + (p1->dmy * w);

	nsvg__addEdge(r, lx, ly, left->x, left->y);
	nsvg__addEdge(r, right->x, right->y, rx, ry);

	left->x = lx; left->y = ly;
	right->x = rx; right->y = ry;
}

static int nsvg__curveDivs(float r, float arc, float tol)
{
	float da = acosf(r / (r + tol)) * 2.0f;
	int divs = (int)ceilf(arc / da);
	if (divs < 2) divs = 2;
	return divs;
}

static void nsvg__expandStroke(struct svg_rasterizer_t * r, struct svg_point_t* points, int npoints, int closed, int lineJoin, int lineCap, float lineWidth)
{
	int ncap = nsvg__curveDivs(lineWidth*0.5f, SVG_PI, r->tessTol);	// Calculate divisions per half circle.
	struct svg_point_t left = {0,0,0,0,0,0,0,0}, right = {0,0,0,0,0,0,0,0}, firstLeft = {0,0,0,0,0,0,0,0}, firstRight = {0,0,0,0,0,0,0,0};
	struct svg_point_t* p0, *p1;
	int j, s, e;

	// Build stroke edges
	if (closed) {
		// Looping
		p0 = &points[npoints-1];
		p1 = &points[0];
		s = 0;
		e = npoints;
	} else {
		// Add cap
		p0 = &points[0];
		p1 = &points[1];
		s = 1;
		e = npoints-1;
	}

	if (closed) {
		nsvg__initClosed(&left, &right, p0, p1, lineWidth);
		firstLeft = left;
		firstRight = right;
	} else {
		// Add cap
		float dx = p1->x - p0->x;
		float dy = p1->y - p0->y;
		nsvg__normalize(&dx, &dy);
		if (lineCap == SVG_CAP_BUTT)
			nsvg__buttCap(r, &left, &right, p0, dx, dy, lineWidth, 0);
		else if (lineCap == SVG_CAP_SQUARE)
			nsvg__squareCap(r, &left, &right, p0, dx, dy, lineWidth, 0);
		else if (lineCap == SVG_CAP_ROUND)
			nsvg__roundCap(r, &left, &right, p0, dx, dy, lineWidth, ncap, 0);
	}

	for (j = s; j < e; ++j) {
		if (p1->flags & SVG_PT_CORNER) {
			if (lineJoin == SVG_JOIN_ROUND)
				nsvg__roundJoin(r, &left, &right, p0, p1, lineWidth, ncap);
			else if (lineJoin == SVG_JOIN_BEVEL || (p1->flags & SVG_PT_BEVEL))
				nsvg__bevelJoin(r, &left, &right, p0, p1, lineWidth);
			else
				nsvg__miterJoin(r, &left, &right, p0, p1, lineWidth);
		} else {
			nsvg__straightJoin(r, &left, &right, p1, lineWidth);
		}
		p0 = p1++;
	}

	if (closed) {
		// Loop it
		nsvg__addEdge(r, firstLeft.x, firstLeft.y, left.x, left.y);
		nsvg__addEdge(r, right.x, right.y, firstRight.x, firstRight.y);
	} else {
		// Add cap
		float dx = p1->x - p0->x;
		float dy = p1->y - p0->y;
		nsvg__normalize(&dx, &dy);
		if (lineCap == SVG_CAP_BUTT)
			nsvg__buttCap(r, &right, &left, p1, -dx, -dy, lineWidth, 1);
		else if (lineCap == SVG_CAP_SQUARE)
			nsvg__squareCap(r, &right, &left, p1, -dx, -dy, lineWidth, 1);
		else if (lineCap == SVG_CAP_ROUND)
			nsvg__roundCap(r, &right, &left, p1, -dx, -dy, lineWidth, ncap, 1);
	}
}

static void nsvg__prepareStroke(struct svg_rasterizer_t * r, float miter_limit, int lineJoin)
{
	int i, j;
	struct svg_point_t* p0, *p1;

	p0 = &r->points[r->npoints-1];
	p1 = &r->points[0];
	for (i = 0; i < r->npoints; i++) {
		// Calculate segment direction and length
		p0->dx = p1->x - p0->x;
		p0->dy = p1->y - p0->y;
		p0->len = nsvg__normalize(&p0->dx, &p0->dy);
		// Advance
		p0 = p1++;
	}

	// calculate joins
	p0 = &r->points[r->npoints-1];
	p1 = &r->points[0];
	for (j = 0; j < r->npoints; j++) {
		float dlx0, dly0, dlx1, dly1, dmr2, cross;
		dlx0 = p0->dy;
		dly0 = -p0->dx;
		dlx1 = p1->dy;
		dly1 = -p1->dx;
		// Calculate extrusions
		p1->dmx = (dlx0 + dlx1) * 0.5f;
		p1->dmy = (dly0 + dly1) * 0.5f;
		dmr2 = p1->dmx*p1->dmx + p1->dmy*p1->dmy;
		if (dmr2 > 0.000001f) {
			float s2 = 1.0f / dmr2;
			if (s2 > 600.0f) {
				s2 = 600.0f;
			}
			p1->dmx *= s2;
			p1->dmy *= s2;
		}

		// Clear flags, but keep the corner.
		p1->flags = (p1->flags & SVG_PT_CORNER) ? SVG_PT_CORNER : 0;

		// Keep track of left turns.
		cross = p1->dx * p0->dy - p0->dx * p1->dy;
		if (cross > 0.0f)
			p1->flags |= SVG_PT_LEFT;

		// Check to see if the corner needs to be beveled.
		if (p1->flags & SVG_PT_CORNER) {
			if ((dmr2 * miter_limit*miter_limit) < 1.0f || lineJoin == SVG_JOIN_BEVEL || lineJoin == SVG_JOIN_ROUND) {
				p1->flags |= SVG_PT_BEVEL;
			}
		}

		p0 = p1++;
	}
}

static void nsvg__flattenShapeStroke(struct svg_rasterizer_t * r, struct svg_shape_t* shape, float scale)
{
	int i, j, closed;
	struct svg_path_t* path;
	struct svg_point_t* p0, *p1;
	float miter_limit = shape->miter_limit;
	int lineJoin = shape->stroke_line_join;
	int lineCap = shape->stroke_line_cap;
	float lineWidth = shape->stroke_width * scale;

	for (path = shape->paths; path != NULL; path = path->next) {
		// Flatten path
		r->npoints = 0;
		nsvg__addPathPoint(r, path->pts[0]*scale, path->pts[1]*scale, SVG_PT_CORNER);
		for (i = 0; i < path->npts-1; i += 3) {
			float* p = &path->pts[i*2];
			nsvg__flattenCubicBez(r, p[0]*scale,p[1]*scale, p[2]*scale,p[3]*scale, p[4]*scale,p[5]*scale, p[6]*scale,p[7]*scale, 0, SVG_PT_CORNER);
		}
		if (r->npoints < 2)
			continue;

		closed = path->closed;

		// If the first and last points are the same, remove the last, mark as closed path.
		p0 = &r->points[r->npoints-1];
		p1 = &r->points[0];
		if (nsvg__ptEquals(p0->x,p0->y, p1->x,p1->y, r->distTol)) {
			r->npoints--;
			p0 = &r->points[r->npoints-1];
			closed = 1;
		}

		if (shape->stroke_dash_count > 0) {
			int idash = 0, dashState = 1;
			float totalDist = 0, dashLen, allDashLen, dashOffset;
			struct svg_point_t cur;

			if (closed)
				nsvg__appendPathPoint(r, r->points[0]);

			// Duplicate points -> points2.
			nsvg__duplicatePoints(r);

			r->npoints = 0;
 			cur = r->points2[0];
			nsvg__appendPathPoint(r, cur);

			// Figure out dash offset.
			allDashLen = 0;
			for (j = 0; j < shape->stroke_dash_count; j++)
				allDashLen += shape->stroke_dash_array[j];
			if (shape->stroke_dash_count & 1)
				allDashLen *= 2.0f;
			// Find location inside pattern
			dashOffset = fmodf(shape->stroke_dash_offset, allDashLen);
			if (dashOffset < 0.0f)
				dashOffset += allDashLen;

			while (dashOffset > shape->stroke_dash_array[idash]) {
				dashOffset -= shape->stroke_dash_array[idash];
				idash = (idash + 1) % shape->stroke_dash_count;
			}
			dashLen = (shape->stroke_dash_array[idash] - dashOffset) * scale;

			for (j = 1; j < r->npoints2; ) {
				float dx = r->points2[j].x - cur.x;
				float dy = r->points2[j].y - cur.y;
				float dist = sqrtf(dx*dx + dy*dy);

				if ((totalDist + dist) > dashLen) {
					// Calculate intermediate point
					float d = (dashLen - totalDist) / dist;
					float x = cur.x + dx * d;
					float y = cur.y + dy * d;
					nsvg__addPathPoint(r, x, y, SVG_PT_CORNER);

					// Stroke
					if (r->npoints > 1 && dashState) {
						nsvg__prepareStroke(r, miter_limit, lineJoin);
						nsvg__expandStroke(r, r->points, r->npoints, 0, lineJoin, lineCap, lineWidth);
					}
					// Advance dash pattern
					dashState = !dashState;
					idash = (idash+1) % shape->stroke_dash_count;
					dashLen = shape->stroke_dash_array[idash] * scale;
					// Restart
					cur.x = x;
					cur.y = y;
					cur.flags = SVG_PT_CORNER;
					totalDist = 0.0f;
					r->npoints = 0;
					nsvg__appendPathPoint(r, cur);
				} else {
					totalDist += dist;
					cur = r->points2[j];
					nsvg__appendPathPoint(r, cur);
					j++;
				}
			}
			// Stroke any leftover path
			if (r->npoints > 1 && dashState)
				nsvg__expandStroke(r, r->points, r->npoints, 0, lineJoin, lineCap, lineWidth);
		} else {
			nsvg__prepareStroke(r, miter_limit, lineJoin);
			nsvg__expandStroke(r, r->points, r->npoints, closed, lineJoin, lineCap, lineWidth);
		}
	}
}

static int nsvg__cmpEdge(const void *p, const void *q)
{
	const struct svg_edge_t* a = (const struct svg_edge_t*)p;
	const struct svg_edge_t* b = (const struct svg_edge_t*)q;

	if (a->y0 < b->y0) return -1;
	if (a->y0 > b->y0) return  1;
	return 0;
}


static struct svg_active_edge_t* nsvg__addActive(struct svg_rasterizer_t * r, struct svg_edge_t* e, float startPoint)
{
	 struct svg_active_edge_t* z;

	if (r->freelist != NULL) {
		// Restore from freelist.
		z = r->freelist;
		r->freelist = z->next;
	} else {
		// Alloc new edge.
		z = (struct svg_active_edge_t*)nsvg__alloc(r, sizeof(struct svg_active_edge_t));
		if (z == NULL) return NULL;
	}

	float dxdy = (e->x1 - e->x0) / (e->y1 - e->y0);
//	STBTT_assert(e->y0 <= start_point);
	// round dx down to avoid going too far
	if (dxdy < 0)
		z->dx = (int)(-floorf(SVG__FIX * -dxdy));
	else
		z->dx = (int)floorf(SVG__FIX * dxdy);
	z->x = (int)floorf(SVG__FIX * (e->x0 + dxdy * (startPoint - e->y0)));
//	z->x -= off_x * FIX;
	z->ey = e->y1;
	z->next = 0;
	z->dir = e->dir;

	return z;
}

static void nsvg__freeActive(struct svg_rasterizer_t * r, struct svg_active_edge_t* z)
{
	z->next = r->freelist;
	r->freelist = z;
}

static void nsvg__fillScanline(unsigned char* scanline, int len, int x0, int x1, int maxWeight, int* xmin, int* xmax)
{
	int i = x0 >> SVG__FIXSHIFT;
	int j = x1 >> SVG__FIXSHIFT;
	if (i < *xmin) *xmin = i;
	if (j > *xmax) *xmax = j;
	if (i < len && j >= 0) {
		if (i == j) {
			// x0,x1 are the same pixel, so compute combined coverage
			scanline[i] = (unsigned char)(scanline[i] + ((x1 - x0) * maxWeight >> SVG__FIXSHIFT));
		} else {
			if (i >= 0) // add antialiasing for x0
				scanline[i] = (unsigned char)(scanline[i] + (((SVG__FIX - (x0 & SVG__FIXMASK)) * maxWeight) >> SVG__FIXSHIFT));
			else
				i = -1; // clip

			if (j < len) // add antialiasing for x1
				scanline[j] = (unsigned char)(scanline[j] + (((x1 & SVG__FIXMASK) * maxWeight) >> SVG__FIXSHIFT));
			else
				j = len; // clip

			for (++i; i < j; ++i) // fill pixels between x0 and x1
				scanline[i] = (unsigned char)(scanline[i] + maxWeight);
		}
	}
}

// note: this routine clips fills that extend off the edges... ideally this
// wouldn't happen, but it could happen if the truetype glyph bounding boxes
// are wrong, or if the user supplies a too-small bitmap
static void nsvg__fillActiveEdges(unsigned char* scanline, int len, struct svg_active_edge_t* e, int maxWeight, int* xmin, int* xmax, char fill_rule)
{
	// non-zero winding fill
	int x0 = 0, w = 0;

	if (fill_rule == SVG_FILLRULE_NONZERO) {
		// Non-zero
		while (e != NULL) {
			if (w == 0) {
				// if we're currently at zero, we need to record the edge start point
				x0 = e->x; w += e->dir;
			} else {
				int x1 = e->x; w += e->dir;
				// if we went to zero, we need to draw
				if (w == 0)
					nsvg__fillScanline(scanline, len, x0, x1, maxWeight, xmin, xmax);
			}
			e = e->next;
		}
	} else if (fill_rule == SVG_FILLRULE_EVENODD) {
		// Even-odd
		while (e != NULL) {
			if (w == 0) {
				// if we're currently at zero, we need to record the edge start point
				x0 = e->x; w = 1;
			} else {
				int x1 = e->x; w = 0;
				nsvg__fillScanline(scanline, len, x0, x1, maxWeight, xmin, xmax);
			}
			e = e->next;
		}
	}
}

static float nsvg__clampf(float a, float mn, float mx) { return a < mn ? mn : (a > mx ? mx : a); }

static unsigned int nsvg__RGBA(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	return (r) | (g << 8) | (b << 16) | (a << 24);
}

static unsigned int nsvg__lerpRGBA(unsigned int c0, unsigned int c1, float u)
{
	int iu = (int)(nsvg__clampf(u, 0.0f, 1.0f) * 256.0f);
	int r = (((c0) & 0xff)*(256-iu) + (((c1) & 0xff)*iu)) >> 8;
	int g = (((c0>>8) & 0xff)*(256-iu) + (((c1>>8) & 0xff)*iu)) >> 8;
	int b = (((c0>>16) & 0xff)*(256-iu) + (((c1>>16) & 0xff)*iu)) >> 8;
	int a = (((c0>>24) & 0xff)*(256-iu) + (((c1>>24) & 0xff)*iu)) >> 8;
	return nsvg__RGBA((unsigned char)r, (unsigned char)g, (unsigned char)b, (unsigned char)a);
}

static unsigned int nsvg__applyOpacity(unsigned int c, float u)
{
	int iu = (int)(nsvg__clampf(u, 0.0f, 1.0f) * 256.0f);
	int r = (c) & 0xff;
	int g = (c>>8) & 0xff;
	int b = (c>>16) & 0xff;
	int a = (((c>>24) & 0xff)*iu) >> 8;
	return nsvg__RGBA((unsigned char)r, (unsigned char)g, (unsigned char)b, (unsigned char)a);
}

static inline int nsvg__div255(int x)
{
    return ((x+1) * 257) >> 16;
}

static void nsvg__scanlineSolid(unsigned char* dst, int count, unsigned char* cover, int x, int y,
								float tx, float ty, float scale, struct svg_cache_paint_t* cache)
{

	if (cache->type == SVG_PAINT_COLOR) {
		int i, cr, cg, cb, ca;
		cr = cache->colors[0] & 0xff;
		cg = (cache->colors[0] >> 8) & 0xff;
		cb = (cache->colors[0] >> 16) & 0xff;
		ca = (cache->colors[0] >> 24) & 0xff;

		for (i = 0; i < count; i++) {
			int r,g,b;
			int a = nsvg__div255((int)cover[0] * ca);
			int ia = 255 - a;
			// Premultiply
			r = nsvg__div255(cr * a);
			g = nsvg__div255(cg * a);
			b = nsvg__div255(cb * a);

			// Blend over
			r += nsvg__div255(ia * (int)dst[0]);
			g += nsvg__div255(ia * (int)dst[1]);
			b += nsvg__div255(ia * (int)dst[2]);
			a += nsvg__div255(ia * (int)dst[3]);

			dst[0] = (unsigned char)r;
			dst[1] = (unsigned char)g;
			dst[2] = (unsigned char)b;
			dst[3] = (unsigned char)a;

			cover++;
			dst += 4;
		}
	} else if (cache->type == SVG_PAINT_LINEAR_GRADIENT) {
		// TODO: spread modes.
		// TODO: plenty of opportunities to optimize.
		float fx, fy, dx, gy;
		float* t = cache->xform;
		int i, cr, cg, cb, ca;
		unsigned int c;

		fx = ((float)x - tx) / scale;
		fy = ((float)y - ty) / scale;
		dx = 1.0f / scale;

		for (i = 0; i < count; i++) {
			int r,g,b,a,ia;
			gy = fx*t[1] + fy*t[3] + t[5];
			c = cache->colors[(int)nsvg__clampf(gy*255.0f, 0, 255.0f)];
			cr = (c) & 0xff;
			cg = (c >> 8) & 0xff;
			cb = (c >> 16) & 0xff;
			ca = (c >> 24) & 0xff;

			a = nsvg__div255((int)cover[0] * ca);
			ia = 255 - a;

			// Premultiply
			r = nsvg__div255(cr * a);
			g = nsvg__div255(cg * a);
			b = nsvg__div255(cb * a);

			// Blend over
			r += nsvg__div255(ia * (int)dst[0]);
			g += nsvg__div255(ia * (int)dst[1]);
			b += nsvg__div255(ia * (int)dst[2]);
			a += nsvg__div255(ia * (int)dst[3]);

			dst[0] = (unsigned char)r;
			dst[1] = (unsigned char)g;
			dst[2] = (unsigned char)b;
			dst[3] = (unsigned char)a;

			cover++;
			dst += 4;
			fx += dx;
		}
	} else if (cache->type == SVG_PAINT_RADIAL_GRADIENT) {
		// TODO: spread modes.
		// TODO: plenty of opportunities to optimize.
		// TODO: focus (fx,fy)
		float fx, fy, dx, gx, gy, gd;
		float* t = cache->xform;
		int i, cr, cg, cb, ca;
		unsigned int c;

		fx = ((float)x - tx) / scale;
		fy = ((float)y - ty) / scale;
		dx = 1.0f / scale;

		for (i = 0; i < count; i++) {
			int r,g,b,a,ia;
			gx = fx*t[0] + fy*t[2] + t[4];
			gy = fx*t[1] + fy*t[3] + t[5];
			gd = sqrtf(gx*gx + gy*gy);
			c = cache->colors[(int)nsvg__clampf(gd*255.0f, 0, 255.0f)];
			cr = (c) & 0xff;
			cg = (c >> 8) & 0xff;
			cb = (c >> 16) & 0xff;
			ca = (c >> 24) & 0xff;

			a = nsvg__div255((int)cover[0] * ca);
			ia = 255 - a;

			// Premultiply
			r = nsvg__div255(cr * a);
			g = nsvg__div255(cg * a);
			b = nsvg__div255(cb * a);

			// Blend over
			r += nsvg__div255(ia * (int)dst[0]);
			g += nsvg__div255(ia * (int)dst[1]);
			b += nsvg__div255(ia * (int)dst[2]);
			a += nsvg__div255(ia * (int)dst[3]);

			dst[0] = (unsigned char)r;
			dst[1] = (unsigned char)g;
			dst[2] = (unsigned char)b;
			dst[3] = (unsigned char)a;

			cover++;
			dst += 4;
			fx += dx;
		}
	}
}

static void nsvg__rasterizeSortedEdges(struct svg_rasterizer_t * r, float tx, float ty, float scale, struct svg_cache_paint_t* cache, char fill_rule)
{
	struct svg_active_edge_t *active = NULL;
	int y, s;
	int e = 0;
	int maxWeight = (255 / SVG__SUBSAMPLES);  // weight per vertical scanline
	int xmin, xmax;

	for (y = 0; y < r->height; y++) {
		memset(r->scanline, 0, r->width);
		xmin = r->width;
		xmax = 0;
		for (s = 0; s < SVG__SUBSAMPLES; ++s) {
			// find center of pixel for this scanline
			float scany = (float)(y*SVG__SUBSAMPLES + s) + 0.5f;
			struct svg_active_edge_t **step = &active;

			// update all active edges;
			// remove all active edges that terminate before the center of this scanline
			while (*step) {
				struct svg_active_edge_t *z = *step;
				if (z->ey <= scany) {
					*step = z->next; // delete from list
//					SVG__assert(z->valid);
					nsvg__freeActive(r, z);
				} else {
					z->x += z->dx; // advance to position for current scanline
					step = &((*step)->next); // advance through list
				}
			}

			// resort the list if needed
			for (;;) {
				int changed = 0;
				step = &active;
				while (*step && (*step)->next) {
					if ((*step)->x > (*step)->next->x) {
						struct svg_active_edge_t* t = *step;
						struct svg_active_edge_t* q = t->next;
						t->next = q->next;
						q->next = t;
						*step = q;
						changed = 1;
					}
					step = &(*step)->next;
				}
				if (!changed) break;
			}

			// insert all edges that start before the center of this scanline -- omit ones that also end on this scanline
			while (e < r->nedges && r->edges[e].y0 <= scany) {
				if (r->edges[e].y1 > scany) {
					struct svg_active_edge_t* z = nsvg__addActive(r, &r->edges[e], scany);
					if (z == NULL) break;
					// find insertion point
					if (active == NULL) {
						active = z;
					} else if (z->x < active->x) {
						// insert at front
						z->next = active;
						active = z;
					} else {
						// find thing to insert AFTER
						struct svg_active_edge_t* p = active;
						while (p->next && p->next->x < z->x)
							p = p->next;
						// at this point, p->next->x is NOT < z->x
						z->next = p->next;
						p->next = z;
					}
				}
				e++;
			}

			// now process all active edges in non-zero fashion
			if (active != NULL)
				nsvg__fillActiveEdges(r->scanline, r->width, active, maxWeight, &xmin, &xmax, fill_rule);
		}
		// Blit
		if (xmin < 0) xmin = 0;
		if (xmax > r->width-1) xmax = r->width-1;
		if (xmin <= xmax) {
			nsvg__scanlineSolid(&r->bitmap[y * r->stride] + xmin*4, xmax-xmin+1, &r->scanline[xmin], xmin, y, tx,ty, scale, cache);
		}
	}

}

static void nsvg__unpremultiplyAlpha(unsigned char* image, int w, int h, int stride)
{
	int x,y;

	// Unpremultiply
	for (y = 0; y < h; y++) {
		unsigned char *row = &image[y*stride];
		for (x = 0; x < w; x++) {
			int r = row[0], g = row[1], b = row[2], a = row[3];
			if (a != 0) {
				row[0] = (unsigned char)(r*255/a);
				row[1] = (unsigned char)(g*255/a);
				row[2] = (unsigned char)(b*255/a);
			}
			row += 4;
		}
	}

	// Defringe
	for (y = 0; y < h; y++) {
		unsigned char *row = &image[y*stride];
		for (x = 0; x < w; x++) {
			int r = 0, g = 0, b = 0, a = row[3], n = 0;
			if (a == 0) {
				if (x-1 > 0 && row[-1] != 0) {
					r += row[-4];
					g += row[-3];
					b += row[-2];
					n++;
				}
				if (x+1 < w && row[7] != 0) {
					r += row[4];
					g += row[5];
					b += row[6];
					n++;
				}
				if (y-1 > 0 && row[-stride+3] != 0) {
					r += row[-stride];
					g += row[-stride+1];
					b += row[-stride+2];
					n++;
				}
				if (y+1 < h && row[stride+3] != 0) {
					r += row[stride];
					g += row[stride+1];
					b += row[stride+2];
					n++;
				}
				if (n > 0) {
					row[0] = (unsigned char)(r/n);
					row[1] = (unsigned char)(g/n);
					row[2] = (unsigned char)(b/n);
				}
			}
			row += 4;
		}
	}
}


static void nsvg__initPaint(struct svg_cache_paint_t* cache, struct svg_paint_t* paint, float opacity)
{
	int i, j;
	struct svg_gradient_t* grad;

	cache->type = paint->type;

	if (paint->type == SVG_PAINT_COLOR) {
		cache->colors[0] = nsvg__applyOpacity(paint->color, opacity);
		return;
	}

	grad = paint->gradient;

	cache->spread = grad->spread;
	memcpy(cache->xform, grad->xform, sizeof(float)*6);

	if (grad->nstops == 0) {
		for (i = 0; i < 256; i++)
			cache->colors[i] = 0;
	} if (grad->nstops == 1) {
		for (i = 0; i < 256; i++)
			cache->colors[i] = nsvg__applyOpacity(grad->stops[i].color, opacity);
	} else {
		unsigned int ca, cb = 0;
		float ua, ub, du, u;
		int ia, ib, count;

		ca = nsvg__applyOpacity(grad->stops[0].color, opacity);
		ua = nsvg__clampf(grad->stops[0].offset, 0, 1);
		ub = nsvg__clampf(grad->stops[grad->nstops-1].offset, ua, 1);
		ia = (int)(ua * 255.0f);
		ib = (int)(ub * 255.0f);
		for (i = 0; i < ia; i++) {
			cache->colors[i] = ca;
		}

		for (i = 0; i < grad->nstops-1; i++) {
			ca = nsvg__applyOpacity(grad->stops[i].color, opacity);
			cb = nsvg__applyOpacity(grad->stops[i+1].color, opacity);
			ua = nsvg__clampf(grad->stops[i].offset, 0, 1);
			ub = nsvg__clampf(grad->stops[i+1].offset, 0, 1);
			ia = (int)(ua * 255.0f);
			ib = (int)(ub * 255.0f);
			count = ib - ia;
			if (count <= 0) continue;
			u = 0;
			du = 1.0f / (float)count;
			for (j = 0; j < count; j++) {
				cache->colors[ia+j] = nsvg__lerpRGBA(ca,cb,u);
				u += du;
			}
		}

		for (i = ib; i < 256; i++)
			cache->colors[i] = cb;
	}

}

/*
static void dumpEdges(struct svg_rasterizer_t * r, const char* name)
{
	float xmin = 0, xmax = 0, ymin = 0, ymax = 0;
	struct svg_edge_t *e = NULL;
	int i;
	if (r->nedges == 0) return;
	FILE* fp = fopen(name, "w");
	if (fp == NULL) return;

	xmin = xmax = r->edges[0].x0;
	ymin = ymax = r->edges[0].y0;
	for (i = 0; i < r->nedges; i++) {
		e = &r->edges[i];
		xmin = nsvg__minf(xmin, e->x0);
		xmin = nsvg__minf(xmin, e->x1);
		xmax = nsvg__maxf(xmax, e->x0);
		xmax = nsvg__maxf(xmax, e->x1);
		ymin = nsvg__minf(ymin, e->y0);
		ymin = nsvg__minf(ymin, e->y1);
		ymax = nsvg__maxf(ymax, e->y0);
		ymax = nsvg__maxf(ymax, e->y1);
	}

	fprintf(fp, "<svg viewBox=\"%f %f %f %f\" xmlns=\"http://www.w3.org/2000/svg\">", xmin, ymin, (xmax - xmin), (ymax - ymin));

	for (i = 0; i < r->nedges; i++) {
		e = &r->edges[i];
		fprintf(fp ,"<line x1=\"%f\" y1=\"%f\" x2=\"%f\" y2=\"%f\" style=\"stroke:#000;\" />", e->x0,e->y0, e->x1,e->y1);
	}

	for (i = 0; i < r->npoints; i++) {
		if (i+1 < r->npoints)
			fprintf(fp ,"<line x1=\"%f\" y1=\"%f\" x2=\"%f\" y2=\"%f\" style=\"stroke:#f00;\" />", r->points[i].x, r->points[i].y, r->points[i+1].x, r->points[i+1].y);
		fprintf(fp ,"<circle cx=\"%f\" cy=\"%f\" r=\"1\" style=\"fill:%s;\" />", r->points[i].x, r->points[i].y, r->points[i].flags == 0 ? "#f00" : "#0f0");
	}

	fprintf(fp, "</svg>");
	fclose(fp);
}
*/

void nsvgRasterize(struct svg_rasterizer_t * r,
				   struct svg_image_t* image, float tx, float ty, float scale,
				   unsigned char* dst, int w, int h, int stride)
{
	struct svg_shape_t *shape = NULL;
	struct svg_edge_t *e = NULL;
	struct svg_cache_paint_t cache;
	int i;

	r->bitmap = dst;
	r->width = w;
	r->height = h;
	r->stride = stride;

	if (w > r->cscanline) {
		r->cscanline = w;
		r->scanline = (unsigned char*)realloc(r->scanline, w);
		if (r->scanline == NULL) return;
	}

	for (i = 0; i < h; i++)
		memset(&dst[i*stride], 0, w*4);

	for (shape = image->shapes; shape != NULL; shape = shape->next) {
		if (!(shape->flags & SVG_FLAGS_VISIBLE))
			continue;

		if (shape->fill.type != SVG_PAINT_NONE) {
			nsvg__resetPool(r);
			r->freelist = NULL;
			r->nedges = 0;

			nsvg__flattenShape(r, shape, scale);

			// Scale and translate edges
			for (i = 0; i < r->nedges; i++) {
				e = &r->edges[i];
				e->x0 = tx + e->x0;
				e->y0 = (ty + e->y0) * SVG__SUBSAMPLES;
				e->x1 = tx + e->x1;
				e->y1 = (ty + e->y1) * SVG__SUBSAMPLES;
			}

			// Rasterize edges
			qsort(r->edges, r->nedges, sizeof(struct svg_edge_t), nsvg__cmpEdge);

			// now, traverse the scanlines and find the intersections on each scanline, use non-zero rule
			nsvg__initPaint(&cache, &shape->fill, shape->opacity);

			nsvg__rasterizeSortedEdges(r, tx,ty,scale, &cache, shape->fill_rule);
		}
		if (shape->stroke.type != SVG_PAINT_NONE && (shape->stroke_width * scale) > 0.01f) {
			nsvg__resetPool(r);
			r->freelist = NULL;
			r->nedges = 0;

			nsvg__flattenShapeStroke(r, shape, scale);

//			dumpEdges(r, "edge.svg");

			// Scale and translate edges
			for (i = 0; i < r->nedges; i++) {
				e = &r->edges[i];
				e->x0 = tx + e->x0;
				e->y0 = (ty + e->y0) * SVG__SUBSAMPLES;
				e->x1 = tx + e->x1;
				e->y1 = (ty + e->y1) * SVG__SUBSAMPLES;
			}

			// Rasterize edges
			qsort(r->edges, r->nedges, sizeof(struct svg_edge_t), nsvg__cmpEdge);

			// now, traverse the scanlines and find the intersections on each scanline, use non-zero rule
			nsvg__initPaint(&cache, &shape->stroke, shape->opacity);

			nsvg__rasterizeSortedEdges(r, tx,ty,scale, &cache, SVG_FILLRULE_NONZERO);
		}
	}

	nsvg__unpremultiplyAlpha(dst, w, h, stride);

	r->bitmap = NULL;
	r->width = 0;
	r->height = 0;
	r->stride = 0;
}
