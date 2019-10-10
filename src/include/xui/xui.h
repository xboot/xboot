#ifndef __XUI_H__
#define __XUI_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot/window.h>
#include <graphic/point.h>
#include <graphic/region.h>
#include <graphic/color.h>
#include <graphic/font.h>

struct xui_context_t {
	struct window_t * w;
	struct font_context_t * f;
	struct region_t clip;
	struct color_t bgcolor;

	struct fifo_t * cmdfifo;
};

enum xui_command_type_t {
	XUI_COMMAND_TYPE_CLIP,
	XUI_COMMAND_TYPE_LINE,
	XUI_COMMAND_TYPE_TRIANGLE,
	XUI_COMMAND_TYPE_RECTANGLE,
	XUI_COMMAND_TYPE_CIRCLE,
	XUI_COMMAND_TYPE_ELLIPSE,
	XUI_COMMAND_TYPE_ARC,
};

struct xui_command_t {
	enum xui_command_type_t type;

	union {
		struct {
			struct region_t r;
		} clip;

		struct {
			struct point_t p0;
			struct point_t p1;
			int thickness;
			struct color_t c;
		} line;

		struct {
			struct point_t p0;
			struct point_t p1;
			struct point_t p2;
			int thickness;
			struct color_t c;
		} triangle;

		struct {
			int x;
			int y;
			int w;
			int h;
			int radius;
			int thickness;
			struct color_t c;
		} rectangle;

		struct {
			int x;
			int y;
			int radius;
			int thickness;
			struct color_t c;
		} circle;

		struct {
			int x;
			int y;
			int w;
			int h;
			int thickness;
			struct color_t c;
		} ellipse;

		struct {
			int x;
			int y;
			int radius;
			int a1;
			int a2;
			int thickness;
			struct color_t c;
		} arc;
	} u;
};

struct xui_context_t * xui_context_alloc(const char * fb, const char * input, struct color_t * c);
void xui_context_free(struct xui_context_t * ctx);
void xui_context_clear(struct xui_context_t * ctx);

void xui_push_clip(struct xui_context_t * ctx, int x, int y, int w, int h);
void xui_push_line(struct xui_context_t * ctx, int x0, int y0, int x1, int y1, int thickness, struct color_t * c);
void xui_push_triangle(struct xui_context_t * ctx, int x0, int y0, int x1, int y1, int x2, int y2, int thickness, struct color_t * c);
void xui_push_rectangle(struct xui_context_t * ctx, int x, int y, int w, int h, int radius, int thickness, struct color_t * c);
void xui_push_circle(struct xui_context_t * ctx, int x, int y, int radius, int thickness, struct color_t * c);
void xui_push_ellipse(struct xui_context_t * ctx, int x, int y, int w, int h, int thickness, struct color_t * c);
void xui_push_arc(struct xui_context_t * ctx, int x, int y, int radius, int a1, int a2, int thickness, struct color_t * c);

void xui_loop(struct xui_context_t * ctx, void (*func)(struct xui_context_t *));

#ifdef __cplusplus
}
#endif

#endif /* __XUI_H__ */
