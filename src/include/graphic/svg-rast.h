#ifndef __GRAPHIC_SVG_RAST_H__
#define __GRAPHIC_SVG_RAST_H__

#ifdef __cplusplus
extern "C" {
#endif

#define SVG__SUBSAMPLES		5
#define SVG__FIXSHIFT			14
#define SVG__FIX				(1 << SVG__FIXSHIFT)
#define SVG__FIXMASK			(SVG__FIX - 1)
#define SVG__MEMPAGE_SIZE		1024

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
	unsigned char flags;
};

struct svg_active_edge_t {
	int x, dx;
	float ey;
	int dir;
	struct svg_active_edge_t * next;
};

struct svg_mem_page_t {
	unsigned char mem[SVG__MEMPAGE_SIZE];
	int size;
	struct svg_mem_page_t * next;
};

struct svg_cache_paint_t {
	char type;
	char spread;
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

struct svg_rasterizer_t * nsvgCreateRasterizer();
void nsvgRasterize(struct svg_rasterizer_t * r, struct svg_image_t * image, float tx, float ty, float scale, unsigned char* dst, int w, int h, int stride);
void nsvgDeleteRasterizer(struct svg_rasterizer_t * r);

#ifdef __cplusplus
}
#endif

#endif /* __GRAPHIC_SVG_RAST_H__ */
