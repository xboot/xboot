#ifndef __GRAPHIC_SVG_RAST_H__
#define __GRAPHIC_SVG_RAST_H__

#ifdef __cplusplus
extern "C" {
#endif

#define SVG_SUBSAMPLES		(5)
#define SVG_FIXSHIFT		(14)
#define SVG_FIX				(1 << SVG_FIXSHIFT)
#define SVG_FIXMASK			(SVG_FIX - 1)
#define SVG_MEMPAGE_SIZE	(1024)

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
	unsigned char mem[SVG_MEMPAGE_SIZE];
	int size;
	struct svg_mem_page_t * next;
};

struct svg_cache_paint_t {
	enum svg_paint_type_t type;
	enum svg_spread_type_t spread;
	float xform[6];
	unsigned int colors[256];
};

struct svg_rasterizer_t {
	float px, py;

	float tessTol;
	float distTol;

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

	unsigned char * scanline;
	int cscanline;

	unsigned char * bitmap;
	int width, height, stride;
};

struct svg_rasterizer_t * svg_rasterizer_alloc(void);
void svg_rasterize(struct svg_rasterizer_t * r, struct svg_image_t * image, float tx, float ty, float scalex, float scaley, unsigned char* dst, int w, int h, int stride);
void svg_rasterizer_free(struct svg_rasterizer_t * r);

#ifdef __cplusplus
}
#endif

#endif /* __GRAPHIC_SVG_RAST_H__ */
