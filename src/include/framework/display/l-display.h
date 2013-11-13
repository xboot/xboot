#ifndef __FRAMEWORK_L_DISPLAY_H__
#define __FRAMEWORK_L_DISPLAY_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <cairo.h>
#include <framework/luahelper.h>

#define	MT_NAME_RECTANGLE	"mt_name_rectangle"
#define	MT_NAME_MATRIX		"mt_name_matrix"
#define MT_NAME_EASING		"mt_name_easing"
#define	MT_NAME_OBJECT		"mt_name_object"
#define	MT_NAME_PARTTERN	"mt_name_parttern"
#define	MT_NAME_TEXTURE		"mt_name_texture"
#define MT_NAME_SHAPE		"mt_name_shape"
#define	MT_NAME_FONT		"mt_name_font"
#define	MT_NAME_DISPLAY		"mt_name_display"

struct rectangle_t {
	double x, y;
	double w, h;
};

struct ltexture_t {
	cairo_surface_t * surface;

#if 0
	/* Nine patch */
	struct {
		int valid;

		struct {
			int a, b, c, d;
		} stretch;

		struct {
			int a, b, c, d;
		} content;

		cairo_surface_t * lt;
		cairo_surface_t * mt;
		cairo_surface_t * rt;
		cairo_surface_t * lm;
		cairo_surface_t * mm;
		cairo_surface_t * rm;
		cairo_surface_t * lb;
		cairo_surface_t * mb;
		cairo_surface_t * rb;
	} patch;
#endif
};

int luaopen_rectangle(lua_State * L);
int luaopen_matrix(lua_State * L);
int luaopen_easing(lua_State * L);
int luaopen_object(lua_State * L);
int luaopen_parttern(lua_State * L);
int luaopen_texture(lua_State * L);
int luaopen_shape(lua_State * L);
int luaopen_font(lua_State * L);
int luaopen_display(lua_State * L);

#ifdef __cplusplus
}
#endif

#endif /* __FRAMEWORK_L_DISPLAY_H__ */
