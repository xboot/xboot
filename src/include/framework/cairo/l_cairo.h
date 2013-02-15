#ifndef __FRAMEWORK_L_CAIRO_H__
#define __FRAMEWORK_FRAMERATE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <framework/luahelper.h>

#define	MT_NAME_CAIRO				"mt_name_cairo"
#define	MT_NAME_CAIRO_SURFACE		"mt_name_cairo_surface"

int l_cairo_image_surface_create(lua_State * L);
extern const luaL_Reg m_cairo_surface[];

int luaopen_cairo(lua_State * L);

#ifdef __cplusplus
}
#endif

#endif /* __FRAMEWORK_L_CAIRO_H__ */
