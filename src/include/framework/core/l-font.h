#ifndef __FRAMEWORK_CORE_L_FONT_H__
#define __FRAMEWORK_CORE_L_FONT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <framework/luahelper.h>

#define	MT_FONT		"__mt_font__"

int luaopen_font(lua_State * L);

#ifdef __cplusplus
}
#endif

#endif /* __FRAMEWORK_CORE_L_FONT_H__ */
