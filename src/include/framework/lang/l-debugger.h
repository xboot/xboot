#ifndef __FRAMEWORK_L_DEBUGGER_H__
#define __FRAMEWORK_L_DEBUGGER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <framework/luahelper.h>

int luaopen_debugger(lua_State * L);

#ifdef __cplusplus
}
#endif

#endif /* __FRAMEWORK_L_DEBUGGER_H__ */
