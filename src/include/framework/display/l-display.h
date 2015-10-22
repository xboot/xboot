#ifndef __FRAMEWORK_L_DISPLAY_H__
#define __FRAMEWORK_L_DISPLAY_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <cairo.h>
#include <framework/luahelper.h>

#define	MT_MATRIX		"mt_matrix"
#define MT_EASING		"mt_easing"
#define	MT_OBJECT		"mt_object"
#define	MT_PATTERN		"mt_pattern"
#define	MT_TEXTURE		"mt_texture"
#define	MT_NINEPATCH	"mt_ninepatch"
#define MT_SHAPE		"mt_shape"
#define	MT_FONT			"mt_font"
#define	MT_DISPLAY		"mt_display"

enum alignment_t {
	ALIGN_NONE					= 0,
	ALIGN_LEFT					= 1,
	ALIGN_TOP					= 2,
	ALIGN_RIGHT					= 3,
	ALIGN_BOTTOM				= 4,
	ALIGN_LEFT_TOP				= 5,
	ALIGN_RIGHT_TOP				= 6,
	ALIGN_LEFT_BOTTOM			= 7,
	ALIGN_RIGHT_BOTTOM			= 8,
	ALIGN_LEFT_CENTER			= 9,
	ALIGN_TOP_CENTER			= 10,
	ALIGN_RIGHT_CENTER			= 11,
	ALIGN_BOTTOM_CENTER			= 12,
	ALIGN_HORIZONTAL_CENTER		= 13,
	ALIGN_VERTICAL_CENTER		= 14,
	ALIGN_CENTER				= 15,
	ALIGN_LEFT_FILL				= 16,
	ALIGN_TOP_FILL				= 17,
	ALIGN_RIGHT_FILL			= 18,
	ALIGN_BOTTOM_FILL			= 19,
	ALIGN_HORIZONTAL_FILL		= 20,
	ALIGN_VERTICAL_FILL			= 21,
	ALIGN_CENTER_FILL			= 22,
};

struct lobject_t {
	double width, height;
	double x, y;
	double rotation;
	double scalex, scaley;
	double anchorx, anchory;
	double alpha;
	enum alignment_t alignment;
	int visible;
	int touchable;

	int __translate;
	int __rotate;
	int __scale;
	int __anchor;

	int __obj_matrix_valid;
	cairo_matrix_t __obj_matrix;
	cairo_matrix_t __transform_matrix;
};

struct ltexture_t {
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

int luaopen_matrix(lua_State * L);
int luaopen_easing(lua_State * L);
int luaopen_object(lua_State * L);
int luaopen_pattern(lua_State * L);
int luaopen_texture(lua_State * L);
int luaopen_ninepatch(lua_State * L);
int luaopen_shape(lua_State * L);
int luaopen_font(lua_State * L);
int luaopen_display(lua_State * L);

#ifdef __cplusplus
}
#endif

#endif /* __FRAMEWORK_L_DISPLAY_H__ */
