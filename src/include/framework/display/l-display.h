#ifndef __FRAMEWORK_L_DISPLAY_H__
#define __FRAMEWORK_L_DISPLAY_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <cairo.h>
#include <framework/luahelper.h>

#define	MT_NAME_DISPLAY		"mt_name_display"
#define	MT_NAME_MATRIX		"mt_name_matrix"
#define MT_NAME_EASING		"mt_name_easing"
#define	MT_NAME_FONT		"mt_name_font"
#define	MT_NAME_PARTTERN	"mt_name_parttern"
#define	MT_NAME_TEXTURE		"mt_name_texture"
#define MT_NAME_SHAPE		"mt_name_shape"

struct ltexture_t {
	cairo_surface_t * surface;

	/* Nine patch */
	struct {
		int valid;

		struct {
			int a, b, c, d;
		} stretch;

		struct {
			int a, b, c, d;
		} content;

		cairo_surface_t * tl;
		cairo_surface_t * tm;
		cairo_surface_t * tr;
		cairo_surface_t * ml;
		cairo_surface_t * mm;
		cairo_surface_t * mr;
		cairo_surface_t * bl;
		cairo_surface_t * bm;
		cairo_surface_t * br;
	} patch;
};

int luaopen_display(lua_State * L);
int luaopen_matrix(lua_State * L);
int luaopen_easing(lua_State * L);
int luaopen_font(lua_State * L);
int luaopen_parttern(lua_State * L);
int luaopen_texture(lua_State * L);
int luaopen_shape(lua_State * L);

#ifdef __cplusplus
}
#endif

#endif /* __FRAMEWORK_L_DISPLAY_H__ */
