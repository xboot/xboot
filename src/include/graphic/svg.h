#ifndef __GRAPHIC_SVG_H__
#define __GRAPHIC_SVG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <graphic/color.h>
#include <xfs/xfs.h>

enum svg_paint_type_t {
	SVG_PAINT_NONE				= 0,
	SVG_PAINT_COLOR				= 1,
	SVG_PAINT_LINEAR_GRADIENT	= 2,
	SVG_PAINT_RADIAL_GRADIENT	= 3,
};

enum svg_spread_type_t {
	SVG_SPREAD_PAD				= 0,
	SVG_SPREAD_REFLECT			= 1,
	SVG_SPREAD_REPEAT			= 2,
};

enum svg_line_join_t {
	SVG_JOIN_MITER				= 0,
	SVG_JOIN_ROUND				= 1,
	SVG_JOIN_BEVEL				= 2,
};

enum svg_line_cap_t {
	SVG_CAP_BUTT				= 0,
	SVG_CAP_ROUND				= 1,
	SVG_CAP_SQUARE				= 2,
};

enum svg_fill_rule_t {
	SVG_FILLRULE_NONZERO		= 0,
	SVG_FILLRULE_EVENODD		= 1,
};

struct svg_gradient_stop_t {
	struct color_t color;
	float offset;
};

struct svg_gradient_t {
	float xform[6];
	enum svg_spread_type_t spread;
	float fx, fy;
	int nstops;
	struct svg_gradient_stop_t stops[1];
};

struct svg_paint_t {
	enum svg_paint_type_t type;
	union {
		struct color_t color;
		struct svg_gradient_t * gradient;
	};
};

struct svg_path_t {
	float * pts;
	int npts;
	int closed;
	float bounds[4];
	struct svg_path_t * next;
};

struct svg_shape_t {
	char id[64];
	struct svg_paint_t fill;
	struct svg_paint_t stroke;
	float opacity;
	float stroke_width;
	float stroke_dash_offset;
	float stroke_dash_array[8];
	int stroke_dash_count;
	enum svg_line_join_t stroke_line_join;
	enum svg_line_cap_t stroke_line_cap;
	float miter_limit;
	enum svg_fill_rule_t fill_rule;
	int visible;
	float bounds[4];
	struct svg_path_t * paths;
	struct svg_shape_t * next;
};

struct svg_t {
	float width;
	float height;
	struct svg_shape_t * shapes;
};

struct svg_t * svg_alloc(char * svgstr);
struct svg_t * svg_alloc_from_xfs(struct xfs_context_t * ctx, const char * filename);
void svg_free(struct svg_t * svg);

#ifdef __cplusplus
}
#endif

#endif /* __GRAPHIC_SVG_H__ */
