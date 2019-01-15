#ifndef __FRAMEWORK_GRAPHIC_L_GRAPHIC_H__
#define __FRAMEWORK_GRAPHIC_L_GRAPHIC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <cairo.h>
#include <cairoint.h>
#include <cairo-ft.h>
#include <framework/luahelper.h>

#define	MT_FONT			"__mt_font__"

int luaopen_font(lua_State * L);

#ifdef __cplusplus
}
#endif

#endif /* __FRAMEWORK_GRAPHIC_L_GRAPHIC_H__ */
