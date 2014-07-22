#ifndef __FRAMEWORK_L_DISPLAY_H__
#define __FRAMEWORK_L_DISPLAY_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <cairo.h>
#include <framework/luahelper.h>

#define	MT_NAME_MATRIX		"mt_name_matrix"
#define MT_NAME_EASING		"mt_name_easing"
#define	MT_NAME_OBJECT		"mt_name_object"
#define	MT_NAME_PARTTERN	"mt_name_parttern"
#define	MT_NAME_TEXTURE		"mt_name_texture"
#define	MT_NAME_NINEPATCH	"mt_name_ninepatch"
#define MT_NAME_SHAPE		"mt_name_shape"
#define	MT_NAME_FONT		"mt_name_font"
#define	MT_NAME_DISPLAY		"mt_name_display"

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
	double width, height;

	struct {
		int a, b, c, d;
	} stretch;

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
};

int luaopen_matrix(lua_State * L);
int luaopen_easing(lua_State * L);
int luaopen_object(lua_State * L);
int luaopen_parttern(lua_State * L);
int luaopen_texture(lua_State * L);
int luaopen_ninepatch(lua_State * L);
int luaopen_shape(lua_State * L);
int luaopen_font(lua_State * L);
int luaopen_display(lua_State * L);

#ifdef __cplusplus
}
#endif

#endif /* __FRAMEWORK_L_DISPLAY_H__ */
