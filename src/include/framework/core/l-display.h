#ifndef __FRAMEWORK_CORE_L_DISPLAY_H__
#define __FRAMEWORK_CORE_L_DISPLAY_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <framework/luahelper.h>

#define MT_DISPLAY	"__mt_display__"

int luaopen_display(lua_State * L);

#ifdef __cplusplus
}
#endif

#endif /* __FRAMEWORK_CORE_L_DISPLAY_H__ */
