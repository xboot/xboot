#ifndef __FRAMEWORK_L_CAIRO_H__
#define __FRAMEWORK_L_CAIRO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <framework/luahelper.h>

#define	MT_NAME_CAIRO				"mt_name_cairo"
#define	MT_NAME_CAIRO_SURFACE		"mt_name_cairo_surface"
#define	MT_NAME_CAIRO_PATH			"mt_name_cairo_path"
#define	MT_NAME_CAIRO_PATTERN		"mt_name_cairo_pattern"
#define	MT_NAME_CAIRO_MATRIX		"mt_name_cairo_matrix"

int l_cairo_image_surface_create(lua_State * L);
int l_cairo_image_surface_create_from_png(lua_State * L);
int l_cairo_surface_create_similar(lua_State * L);
int l_cairo_pattern_create_rgb(lua_State * L);
int l_cairo_pattern_create_rgba(lua_State * L);
int l_cairo_pattern_create_for_surface(lua_State * L);
int l_cairo_pattern_create_linear(lua_State * L);
int l_cairo_pattern_create_radial(lua_State * L);
int l_cairo_matrix_create(lua_State * L);

extern const luaL_Reg m_cairo_surface[];
extern const luaL_Reg m_cairo_path[];
extern const luaL_Reg m_cairo_pattern[];
extern const luaL_Reg m_cairo_matrix[];

int luaopen_cairo(lua_State * L);

#ifdef __cplusplus
}
#endif

#endif /* __FRAMEWORK_L_CAIRO_H__ */
