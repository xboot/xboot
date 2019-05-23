#ifndef __FRAMEWORK_CORE_L_WINDOW_H__
#define __FRAMEWORK_CORE_L_WINDOW_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <framework/luahelper.h>

#define MT_WINDOW	"__mt_window__"

int luaopen_window(lua_State * L);

#ifdef __cplusplus
}
#endif

#endif /* __FRAMEWORK_CORE_L_WINDOW_H__ */
