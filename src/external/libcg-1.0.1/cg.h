#ifndef __CG_H__
#define __CG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <swft.h>

struct cg_point_t {
	double x;
	double y;
};

struct cg_rect_t {
	double x;
	double y;
	double w;
	double h;
};

struct cg_matrix_t {
	double a; double b;
	double c; double d;
	double tx; double ty;
};

struct cg_color_t {
	double r;
	double g;
	double b;
	double a;
};

struct cg_gradient_stop_t {
	double offset;
	struct cg_color_t color;
};

enum cg_path_element_t {
	CG_PATH_ELEMENT_MOVE_TO		= 0,
	CG_PATH_ELEMENT_LINE_TO		= 1,
	CG_PATH_ELEMENT_CURVE_TO	= 2,
	CG_PATH_ELEMENT_CLOSE		= 3,
};

enum cg_spread_method_t {
	CG_SPREAD_METHOD_PAD		= 0,
	CG_SPREAD_METHOD_REFLECT	= 1,
	CG_SPREAD_METHOD_REPEAT		= 2,
};

enum cg_gradient_type_t {
	CG_GRADIENT_TYPE_LINEAR		= 0,
	CG_GRADIENT_TYPE_RADIAL		= 1,
};

enum cg_texture_type_t {
	CG_TEXTURE_TYPE_PLAIN		= 0,
	CG_TEXTURE_TYPE_TILED		= 1,
};

enum cg_line_cap_t {
	CG_LINE_CAP_BUTT			= 0,
	CG_LINE_CAP_ROUND			= 1,
	CG_LINE_CAP_SQUARE			= 2,
};

enum cg_line_join_t {
	CG_LINE_JOIN_MITER			= 0,
	CG_LINE_JOIN_ROUND			= 1,
	CG_LINE_JOIN_BEVEL			= 2,
};

enum cg_fill_rule_t {
	CG_FILL_RULE_NON_ZERO		= 0,
	CG_FILL_RULE_EVEN_ODD		= 1,
};

enum cg_paint_type_t {
	CG_PAINT_TYPE_COLOR			= 0,
	CG_PAINT_TYPE_GRADIENT		= 1,
	CG_PAINT_TYPE_TEXTURE		= 2,
};

enum cg_operator_t {
	CG_OPERATOR_SRC				= 0, /* r = s * ca + d * cia */
	CG_OPERATOR_SRC_OVER		= 1, /* r = (s + d * sia) * ca + d * cia */
	CG_OPERATOR_DST_IN			= 2, /* r = d * sa * ca + d * cia */
	CG_OPERATOR_DST_OUT			= 3, /* r = d * sia * ca + d * cia */
};

struct cg_surface_t {
	int ref;
	int width;
	int height;
	int stride;
	int owndata;
	void * pixels;
};

struct cg_path_t {
	int ref;
	int contours;
	struct cg_point_t start;
	struct {
		enum cg_path_element_t * data;
		int size;
		int capacity;
	} elements;
	struct {
		struct cg_point_t * data;
		int size;
		int capacity;
	} points;
};

struct cg_gradient_t {
	int ref;
	enum cg_gradient_type_t type;
	enum cg_spread_method_t spread;
	struct cg_matrix_t matrix;
	double values[6];
	double opacity;
	struct {
		struct cg_gradient_stop_t * data;
		int size;
		int capacity;
	} stops;
};

struct cg_texture_t {
	int ref;
	enum cg_texture_type_t type;
	struct cg_surface_t * surface;
	struct cg_matrix_t matrix;
	double opacity;
};

struct cg_paint_t {
	int ref;
	enum cg_paint_type_t type;
	union {
		struct cg_color_t * color;
		struct cg_gradient_t * gradient;
		struct cg_texture_t * texture;
	};
};

struct cg_span_t {
	short x;
	short y;
	unsigned short len;
	unsigned char coverage;
};

struct cg_rle_t {
	struct {
		struct cg_span_t * data;
		int size;
		int capacity;
	} spans;
	int x;
	int y;
	int w;
	int h;
};

struct cg_dash_t {
	double offset;
	double * data;
	int size;
};

struct cg_stroke_data_t {
	double width;
	double miterlimit;
	enum cg_line_cap_t cap;
	enum cg_line_join_t join;
	struct cg_dash_t * dash;
};

struct cg_state_t {
	struct cg_rle_t * clippath;
	struct cg_paint_t * source;
	struct cg_matrix_t matrix;
	enum cg_fill_rule_t winding;
	struct cg_stroke_data_t stroke;
	enum cg_operator_t op;
	double opacity;
	struct cg_state_t * next;
};

struct cg_ctx_t {
	struct cg_surface_t * surface;
	struct cg_state_t * state;
	struct cg_path_t * path;
	struct cg_rle_t * rle;
	struct cg_rle_t * clippath;
	struct cg_rect_t clip;
};

#ifndef CG_MIN
#define CG_MIN(a, b)		({typeof(a) _amin = (a); typeof(b) _bmin = (b); (void)(&_amin == &_bmin); _amin < _bmin ? _amin : _bmin;})
#endif
#ifndef CG_MAX
#define CG_MAX(a, b)		({typeof(a) _amax = (a); typeof(b) _bmax = (b); (void)(&_amax == &_bmax); _amax > _bmax ? _amax : _bmax;})
#endif
#ifndef CG_CLAMP
#define CG_CLAMP(v, a, b)	CG_MIN(CG_MAX(a, v), b)
#endif
#ifndef CG_ALPHA
#define CG_ALPHA(c)			((c) >> 24)
#endif
#ifndef CG_DIV255
#define CG_DIV255(x)		(((x) + ((x) >> 8) + 0x80) >> 8)
#endif
#ifndef CG_BYTE_MUL
#define CG_BYTE_MUL(x, a)	((((((x) >> 8) & 0x00ff00ff) * (a)) & 0xff00ff00) + (((((x) & 0x00ff00ff) * (a)) >> 8) & 0x00ff00ff))
#endif

void cg_memfill32(uint32_t * dst, uint32_t val, int len);
void cg_comp_solid_source(uint32_t * dst, int len, uint32_t color, uint32_t alpha);
void cg_comp_solid_source_over(uint32_t * dst, int len, uint32_t color, uint32_t alpha);
void cg_comp_solid_destination_in(uint32_t * dst, int len, uint32_t color, uint32_t alpha);
void cg_comp_solid_destination_out(uint32_t * dst, int len, uint32_t color, uint32_t alpha);
void cg_comp_source(uint32_t * dst, int len, uint32_t * src, uint32_t alpha);
void cg_comp_source_over(uint32_t * dst, int len, uint32_t * src, uint32_t alpha);
void cg_comp_destination_in(uint32_t * dst, int len, uint32_t * src, uint32_t alpha);
void cg_comp_destination_out(uint32_t * dst, int len, uint32_t * src, uint32_t alpha);

void cg_matrix_init(struct cg_matrix_t * m, double a, double b, double c, double d, double tx, double ty);
void cg_matrix_init_identity(struct cg_matrix_t * m);
void cg_matrix_init_translate(struct cg_matrix_t * m, double tx, double ty);
void cg_matrix_init_scale(struct cg_matrix_t * m, double sx, double sy);
void cg_matrix_init_rotate(struct cg_matrix_t * m, double r);
void cg_matrix_translate(struct cg_matrix_t * m, double tx, double ty);
void cg_matrix_scale(struct cg_matrix_t * m, double sx, double sy);
void cg_matrix_rotate(struct cg_matrix_t * m, double r);
void cg_matrix_multiply(struct cg_matrix_t * m, struct cg_matrix_t * m1, struct cg_matrix_t * m2);
void cg_matrix_invert(struct cg_matrix_t * m);
void cg_matrix_map_point(struct cg_matrix_t * m, struct cg_point_t * p1, struct cg_point_t * p2);

struct cg_surface_t * cg_surface_create(int width, int height);
struct cg_surface_t * cg_surface_create_for_data(int width, int height, void * pixels);
void cg_surface_destroy(struct cg_surface_t * surface);
struct cg_surface_t * cg_surface_reference(struct cg_surface_t * surface);

struct cg_path_t * cg_path_create(void);
void cg_path_destroy(struct cg_path_t * path);
struct cg_path_t * cg_path_reference(struct cg_path_t * path);

struct cg_gradient_t * cg_gradient_create_linear(double x1, double y1, double x2, double y2);
struct cg_gradient_t * cg_gradient_create_radial(double cx, double cy, double cr, double fx, double fy, double fr);
void cg_gradient_destroy(struct cg_gradient_t * gradient);
struct cg_gradient_t * cg_gradient_reference(struct cg_gradient_t * gradient);
void cg_gradient_set_spread(struct cg_gradient_t * gradient, enum cg_spread_method_t spread);
void cg_gradient_set_matrix(struct cg_gradient_t * gradient, struct cg_matrix_t * m);
void cg_gradient_add_stop_rgb(struct cg_gradient_t * gradient, double offset, double r, double g, double b);
void cg_gradient_add_stop_rgba(struct cg_gradient_t * gradient, double offset, double r, double g, double b, double a);
void cg_gradient_add_stop_color(struct cg_gradient_t * gradient, double offset, struct cg_color_t * color);
void cg_gradient_add_stop(struct cg_gradient_t * gradient, struct cg_gradient_stop_t * stop);
void cg_gradient_clear_stops(struct cg_gradient_t * gradient);
void cg_gradient_set_opacity(struct cg_gradient_t * paint, double opacity);

struct cg_texture_t * cg_texture_create(struct cg_surface_t * surface);
void cg_texture_destroy(struct cg_texture_t * texture);
struct cg_texture_t * cg_texture_reference(struct cg_texture_t * texture);
void cg_texture_set_type(struct cg_texture_t * texture, enum cg_texture_type_t type);
void cg_texture_set_matrix(struct cg_texture_t * texture, struct cg_matrix_t * m);
void cg_texture_set_surface(struct cg_texture_t * texture, struct cg_surface_t * surface);
void cg_texture_set_opacity(struct cg_texture_t * texture, double opacity);

struct cg_paint_t * cg_paint_create_rgb(double r, double g, double b);
struct cg_paint_t * cg_paint_create_rgba(double r, double g, double b, double a);
struct cg_paint_t * cg_paint_create_linear(double x1, double y1, double x2, double y2);
struct cg_paint_t * cg_paint_create_radial(double cx, double cy, double cr, double fx, double fy, double fr);
struct cg_paint_t * cg_paint_create_for_surface(struct cg_surface_t * surface);
struct cg_paint_t * cg_paint_create_color(struct cg_color_t * color);
struct cg_paint_t * cg_paint_create_gradient(struct cg_gradient_t * gradient);
struct cg_paint_t * cg_paint_create_texture(struct cg_texture_t * texture);
void cg_paint_destroy(struct cg_paint_t * paint);
struct cg_paint_t * cg_paint_reference(struct cg_paint_t * paint);
enum cg_paint_type_t cg_paint_get_type(struct cg_paint_t * paint);
struct cg_color_t * cg_paint_get_color(struct cg_paint_t * paint);
struct cg_gradient_t * cg_paint_get_gradient(struct cg_paint_t * paint);
struct cg_texture_t * cg_paint_get_texture(struct cg_paint_t * paint);

struct cg_ctx_t * cg_create(struct cg_surface_t * surface);
void cg_destroy(struct cg_ctx_t * ctx);
void cg_save(struct cg_ctx_t * ctx);
void cg_restore(struct cg_ctx_t * ctx);
void cg_set_source_rgb(struct cg_ctx_t * ctx, double r, double g, double b);
void cg_set_source_rgba(struct cg_ctx_t * ctx, double r, double g, double b, double a);
void cg_set_source_surface(struct cg_ctx_t * ctx, struct cg_surface_t * surface, double x, double y);
void cg_set_source_color(struct cg_ctx_t * ctx, struct cg_color_t * color);
void cg_set_source_gradient(struct cg_ctx_t * ctx, struct cg_gradient_t * gradient);
void cg_set_source_texture(struct cg_ctx_t * ctx, struct cg_texture_t * texture);
void cg_set_source(struct cg_ctx_t * ctx, struct cg_paint_t * source);
void cg_set_operator(struct cg_ctx_t * ctx, enum cg_operator_t op);
void cg_set_opacity(struct cg_ctx_t * ctx, double opacity);
void cg_set_fill_rule(struct cg_ctx_t * ctx, enum cg_fill_rule_t winding);
void cg_set_line_width(struct cg_ctx_t * ctx, double width);
void cg_set_line_cap(struct cg_ctx_t * ctx, enum cg_line_cap_t cap);
void cg_set_line_join(struct cg_ctx_t * ctx, enum cg_line_join_t join);
void cg_set_miter_limit(struct cg_ctx_t * ctx, double limit);
void cg_set_dash(struct cg_ctx_t * ctx, double * dashes, int ndash, double offset);
void cg_translate(struct cg_ctx_t * ctx, double tx, double ty);
void cg_scale(struct cg_ctx_t * ctx, double sx, double sy);
void cg_rotate(struct cg_ctx_t * ctx, double r);
void cg_transform(struct cg_ctx_t * ctx, struct cg_matrix_t * m);
void cg_set_matrix(struct cg_ctx_t * ctx, struct cg_matrix_t * m);
void cg_identity_matrix(struct cg_ctx_t * ctx);
void cg_move_to(struct cg_ctx_t * ctx, double x, double y);
void cg_line_to(struct cg_ctx_t * ctx, double x, double y);
void cg_curve_to(struct cg_ctx_t * ctx, double x1, double y1, double x2, double y2, double x3, double y3);
void cg_quad_to(struct cg_ctx_t * ctx, double x1, double y1, double x2, double y2);
void cg_rel_move_to(struct cg_ctx_t * ctx, double dx, double dy);
void cg_rel_line_to(struct cg_ctx_t * ctx, double dx, double dy);
void cg_rel_curve_to(struct cg_ctx_t * ctx, double dx1, double dy1, double dx2, double dy2, double dx3, double dy3);
void cg_rel_quad_to(struct cg_ctx_t * ctx, double dx1, double dy1, double dx2, double dy2);
void cg_rectangle(struct cg_ctx_t * ctx, double x, double y, double w, double h);
void cg_round_rectangle(struct cg_ctx_t * ctx, double x, double y, double w, double h, double rx, double ry);
void cg_ellipse(struct cg_ctx_t * ctx, double cx, double cy, double rx, double ry);
void cg_circle(struct cg_ctx_t * ctx, double cx, double cy, double r);
void cg_arc(struct cg_ctx_t * ctx, double cx, double cy, double r, double a0, double a1);
void cg_arc_negative(struct cg_ctx_t * ctx, double cx, double cy, double r, double a0, double a1);
void cg_new_path(struct cg_ctx_t * ctx);
void cg_close_path(struct cg_ctx_t * ctx);
void cg_reset_clip(struct cg_ctx_t * ctx);
void cg_clip(struct cg_ctx_t * ctx);
void cg_clip_preserve(struct cg_ctx_t * ctx);
void cg_fill(struct cg_ctx_t * ctx);
void cg_fill_preserve(struct cg_ctx_t * ctx);
void cg_stroke(struct cg_ctx_t * ctx);
void cg_stroke_preserve(struct cg_ctx_t * ctx);
void cg_paint(struct cg_ctx_t * ctx);

#ifdef __cplusplus
}
#endif

#endif /* __CG_H__ */
