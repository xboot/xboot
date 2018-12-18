#ifndef __FRAMEWORK_GRAPHIC_L_GRAPHIC_H__
#define __FRAMEWORK_GRAPHIC_L_GRAPHIC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <list.h>
#include <cairo.h>
#include <cairoint.h>
#include <cairo-ft.h>
#include <framework/luahelper.h>

enum alignment_t {
	ALIGN_NONE				= 0,
	ALIGN_LEFT				= 1,
	ALIGN_TOP				= 2,
	ALIGN_RIGHT				= 3,
	ALIGN_BOTTOM			= 4,
	ALIGN_LEFT_TOP			= 5,
	ALIGN_RIGHT_TOP			= 6,
	ALIGN_LEFT_BOTTOM		= 7,
	ALIGN_RIGHT_BOTTOM		= 8,
	ALIGN_LEFT_CENTER		= 9,
	ALIGN_TOP_CENTER		= 10,
	ALIGN_RIGHT_CENTER		= 11,
	ALIGN_BOTTOM_CENTER		= 12,
	ALIGN_HORIZONTAL_CENTER	= 13,
	ALIGN_VERTICAL_CENTER	= 14,
	ALIGN_CENTER			= 15,
	ALIGN_LEFT_FILL			= 16,
	ALIGN_TOP_FILL			= 17,
	ALIGN_RIGHT_FILL		= 18,
	ALIGN_BOTTOM_FILL		= 19,
	ALIGN_HORIZONTAL_FILL	= 20,
	ALIGN_VERTICAL_FILL		= 21,
	ALIGN_CENTER_FILL		= 22,
};

struct ldobject_t {
	struct ldobject_t * parent;
	struct list_head entry;
	struct list_head children;

	double width, height;
	double x, y;
	double rotation;
	double scalex, scaley;
	double anchorx, anchory;
	double alpha;
	enum alignment_t alignment;
	int visible;
	int touchable;
	int mflag;
	cairo_matrix_t local_matrix;
	cairo_matrix_t global_matrix;
	cairo_matrix_t global_matrix_invert;

	void (*draw)(lua_State * L, struct ldobject_t * o);
	void * priv;
};

struct limage_t {
	cairo_surface_t * surface;
};

struct lninepatch_t {
	int width, height;
	int left, top, right, bottom;
	cairo_surface_t * lt;
	cairo_surface_t * mt;
	cairo_surface_t * rt;
	cairo_surface_t * lm;
	cairo_surface_t * mm;
	cairo_surface_t * rm;
	cairo_surface_t * lb;
	cairo_surface_t * mb;
	cairo_surface_t * rb;
	double __w, __h;
	double __sx, __sy;
};

struct lpattern_t {
	cairo_pattern_t * pattern;
};

#define	MT_DISPLAY		"__mt_display__"
#define	MT_DOBJECT		"__mt_dobject__"
#define	MT_EASING		"__mt_easing__"
#define	MT_FONT			"__mt_font__"
#define	MT_IMAGE		"__mt_image__"
#define	MT_MATRIX		"__mt_matrix__"
#define	MT_NINEPATCH	"__mt_ninepatch__"
#define	MT_PATTERN		"__mt_pattern__"
#define	MT_SHAPE		"__mt_shape__"

int luaopen_display(lua_State * L);
int luaopen_dobject(lua_State * L);
int luaopen_easing(lua_State * L);
int luaopen_font(lua_State * L);
int luaopen_image(lua_State * L);
int luaopen_matrix(lua_State * L);
int luaopen_ninepatch(lua_State * L);
int luaopen_pattern(lua_State * L);
int luaopen_shape(lua_State * L);

#ifdef __cplusplus
}
#endif

#endif /* __FRAMEWORK_GRAPHIC_L_GRAPHIC_H__ */
